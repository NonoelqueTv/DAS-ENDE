// Modelo_Final\arduino\TM_Serial_Demo/TM_Serial_Demo.ino
// VERSIÓN PRO: NO BLOQUEANTE (SIN DELAY)
// Permite seguir leyendo sensores/serial mientras el motor se mueve.

#define IN1 11
#define IN2 10
#define ENA 9
#define PLAY_PIN 7

// Variables para la lectura Serial
String inLine;

// --- VARIABLES PARA LA MÁQUINA DE ESTADOS (MILLIS) ---
// Definimos los nombres de los estados para que sea fácil de leer
const int ESTADO_QUIETO = 0;
const int ESTADO_MOVIENDO_DERECHA = 1;
const int ESTADO_PAUSA_INTERMEDIA = 2;
const int ESTADO_MOVIENDO_IZQUIERDA = 3;

int estadoMotor = ESTADO_QUIETO;       // Estado actual
unsigned long tiempoInicioEstado = 0;  // Para guardar "la hora" en que empezó una acción

void setup() {
  Serial.begin(115200);

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(ENA, OUTPUT);

  pinMode(PLAY_PIN, OUTPUT);
  digitalWrite(PLAY_PIN, LOW);

  apagarMotorHardware(); // Asegurar motor apagado al inicio
}

void loop() {
  // 1. LEER EL PUERTO SERIAL (Siempre activo)
  while (Serial.available()) {
    char c = Serial.read();
    if (c == '\n') {
      manejarComando(inLine);
      inLine = "";
    } else if (c != '\r') {
      inLine += c;
    }
  }

  // 2. GESTIONAR EL MOTOR (Se ejecuta miles de veces por segundo)
  gestionarMaquinaDeEstados();
}

void manejarComando(const String& s) {
  if (s.startsWith("CLASE:")) {
    int clase = s.substring(6).toInt();
    // Procesamos la orden
    procesarLogica(clase);
    // Feedback opcional
    // Serial.print("RECIBIDO: "); Serial.println(clase);
  }
}

void procesarLogica(int c) {
  // --- c == 0 o c == 1 → ACTIVAR SECUENCIA MOTOR ---
  if (c == 0 || c == 1) {
    // Solo iniciamos si el motor está QUIETO.
    // Esto evita que se reinicie el ciclo si la cámara te detecta muchas veces seguidas.
    if (estadoMotor == ESTADO_QUIETO) {
      iniciarSecuenciaMotor();
    }
    return;
  }

  // --- c == 2 o c == 3 → ISD1820 ---
  if (c == 2 || c == 3) {
    reproducirAudio();
    return;
  }
}

// --- LÓGICA DE TIEMPO (MÁQUINA DE ESTADOS) ---

void iniciarSecuenciaMotor() {
  estadoMotor = ESTADO_MOVIENDO_DERECHA;
  tiempoInicioEstado = millis(); // Guardamos "la hora" actual
  
  // Accion inmediata: Mover derecha
  analogWrite(ENA, 255);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  
  Serial.println("Motor: Iniciando Derecha (2s)");
}

void gestionarMaquinaDeEstados() {
  // Si está quieto, no hay nada que calcular
  if (estadoMotor == ESTADO_QUIETO) return;

  unsigned long tiempoActual = millis();

  switch (estadoMotor) {
    
    case ESTADO_MOVIENDO_DERECHA:
      // ¿Pasaron ya 2000ms (2 seg)?
      if (tiempoActual - tiempoInicioEstado >= 2000) {
        // Fin de fase derecha -> Pasar a Pausa
        apagarMotorHardware();
        estadoMotor = ESTADO_PAUSA_INTERMEDIA;
        tiempoInicioEstado = tiempoActual; // Reiniciamos el cronómetro
        Serial.println("Motor: Pausa (1s)");
      }
      break;

    case ESTADO_PAUSA_INTERMEDIA:
      // ¿Pasaron ya 1000ms (1 seg)?
      if (tiempoActual - tiempoInicioEstado >= 1000) {
        // Fin de pausa -> Pasar a Izquierda
        analogWrite(ENA, 255);
        digitalWrite(IN1, LOW);
        digitalWrite(IN2, HIGH);
        
        estadoMotor = ESTADO_MOVIENDO_IZQUIERDA;
        tiempoInicioEstado = tiempoActual;
        Serial.println("Motor: Volviendo Izquierda (2s)");
      }
      break;

    case ESTADO_MOVIENDO_IZQUIERDA:
      // ¿Pasaron ya 2000ms (2 seg)?
      if (tiempoActual - tiempoInicioEstado >= 2000) {
        // Fin de ciclo -> Volver a Quieto
        apagarMotorHardware();
        estadoMotor = ESTADO_QUIETO;
        Serial.println("Motor: Ciclo Terminado. Listo para nueva orden.");
      }
      break;
  }
}

// --- FUNCIONES DE HARDWARE ---

void apagarMotorHardware() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  analogWrite(ENA, 0);
}

void reproducirAudio() {
  // El pulso de audio es muy corto (200ms), 
  // por simplicidad podemos dejar este pequeño delay.
  // No afectará gravemente al flujo del programa.
  digitalWrite(PLAY_PIN, HIGH);
  delay(200); 
  digitalWrite(PLAY_PIN, LOW);
}
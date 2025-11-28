// Modelo_Final\arduino\TM_Serial_Demo/TM_Serial_Demo.ino
// PINES: IN1=9, IN2=5, ENA=11 (Basado en tu último código pegado)
// SECUENCIA: Derecha (2s) -> PAUSA (2s) -> Izquierda (2s)

#define IN1 9   // Dirección A
#define IN2 5   // Dirección B (Nota: En tu código anterior era 10, aquí pusiste 5. Lo dejo en 5).
#define ENA 11  // Velocidad (PWM)

#define PLAY_PIN 7

String inLine;

// --- MÁQUINA DE ESTADOS (TIMING) ---
const int ESTADO_QUIETO = 0;
const int ESTADO_MOVIENDO_DERECHA = 1;
const int ESTADO_PAUSA_INTERMEDIA = 2;
const int ESTADO_MOVIENDO_IZQUIERDA = 3;

int estadoMotor = ESTADO_QUIETO;
unsigned long tiempoInicioEstado = 0;

void setup() {
  Serial.begin(115200);

  // Configuramos los pines del motor
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(ENA, OUTPUT);

  // Configuramos el audio
  pinMode(PLAY_PIN, OUTPUT);
  digitalWrite(PLAY_PIN, LOW);

  apagarMotorHardware();
}

void loop() {
  // 1. Lectura Serial
  while (Serial.available()) {
    char c = Serial.read();
    if (c == '\n') {
      manejarComando(inLine);
      inLine = "";
    } else if (c != '\r') {
      inLine += c;
    }
  }

  // 2. Control de Tiempos del Motor
  gestionarMaquinaDeEstados();
}

void manejarComando(const String& s) {
  if (s.startsWith("CLASE:")) {
    int clase = s.substring(6).toInt();
    procesarLogica(clase);
  }
}

void procesarLogica(int c) {
  // --- GRUPO MOTOR: Vale(0), Alvaro(2), Benja(3) ---
  if (c == 0 || c == 2 || c == 3) {
    // Solo inicia si el motor está totalmente quieto
    if (estadoMotor == ESTADO_QUIETO) {
      iniciarSecuenciaMotor();
    }
    return;
  }

  // --- GRUPO AUDIO: NADA(1) ---
  if (c == 1) {
    reproducirAudio();
    return;
  }
}

// --- LÓGICA DE SECUENCIA ---

void iniciarSecuenciaMotor() {
  estadoMotor = ESTADO_MOVIENDO_DERECHA;
  tiempoInicioEstado = millis();
  
  // ACCIÓN: MOVER DERECHA (ABRIR)
  analogWrite(ENA, 255);    
  digitalWrite(IN1, HIGH);  
  digitalWrite(IN2, LOW);   
  
  Serial.println("Motor: Abriendo (2s)");
}

void gestionarMaquinaDeEstados() {
  if (estadoMotor == ESTADO_QUIETO) return;

  unsigned long tiempoActual = millis();

  switch (estadoMotor) {
    
    // FASE 1: DERECHA (ABRIENDO)
    case ESTADO_MOVIENDO_DERECHA:
      if (tiempoActual - tiempoInicioEstado >= 5000) { // 2 segundos moviendo
        apagarMotorHardware(); // Frenamos
        estadoMotor = ESTADO_PAUSA_INTERMEDIA;
        tiempoInicioEstado = tiempoActual;
        Serial.println("Motor: Abierto - Esperando (2s)");
      }
      break;

    // FASE 2: PAUSA (PORTÓN ABIERTO) - AQUÍ ESTÁ EL CAMBIO
    case ESTADO_PAUSA_INTERMEDIA:
      // CAMBIO: Aumentado de 1000 a 2000 (2 segundos de espera)
      if (tiempoActual - tiempoInicioEstado >= 2500) { 
        
        // ACCIÓN: MOVER IZQUIERDA (CERRAR)
        analogWrite(ENA, 255);   // Potencia ON
        digitalWrite(IN1, LOW);  // Invertimos
        digitalWrite(IN2, HIGH); // Invertimos
        
        estadoMotor = ESTADO_MOVIENDO_IZQUIERDA;
        tiempoInicioEstado = tiempoActual;
        Serial.println("Motor: Cerrando (2s)");
      }
      break;

    // FASE 3: IZQUIERDA (CERRANDO)
    case ESTADO_MOVIENDO_IZQUIERDA:
      if (tiempoActual - tiempoInicioEstado >= 1000) { // 2 segundos cerrando
        apagarMotorHardware(); // Apagado final
        estadoMotor = ESTADO_QUIETO;
        Serial.println("Ciclo Terminado. Listo.");
      }
      break;
  }
}

// --- FUNCIONES HARDWARE ---

void apagarMotorHardware() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  analogWrite(ENA, 0);
}

void reproducirAudio() {
  digitalWrite(PLAY_PIN, HIGH);
  delay(200); 
  digitalWrite(PLAY_PIN, LOW);
  delay(300); 
}
// --------------------------
// CONFIGURACIÓN DE PINES
// --------------------------
#define REC_PIN 6
#define PLAY_PIN 7

#define MOTOR_A 11   // izquierda
#define MOTOR_B 10   // derecha

String inLine;

// Temporizador de ingreso (30 s)
unsigned long startTime;
bool ventanaActiva = true;

void setup() {
  Serial.begin(115200);

  pinMode(REC_PIN, OUTPUT);
  pinMode(PLAY_PIN, OUTPUT);

  pinMode(MOTOR_A, OUTPUT);
  pinMode(MOTOR_B, OUTPUT);

  apagarMotor();
  digitalWrite(REC_PIN, LOW);
  digitalWrite(PLAY_PIN, LOW);

  startTime = millis();

  // -----------------------------------------
  // PRIMEROS 5 SEGUNDOS: GRABACIÓN DEL ISD1820
  // -----------------------------------------
  digitalWrite(REC_PIN, HIGH);
  delay(5000);
  digitalWrite(REC_PIN, LOW);
}

void loop() {

  // Actualizar ventana de tiempo (30 segundos)
  if (millis() - startTime > 30000) {
    ventanaActiva = false;
  }

  // Leer comandos del serial
  while (Serial.available()) {
    char c = Serial.read();
    if (c == '\n') {
      manejarComando(inLine);
      inLine = "";
    } else if (c != '\r') {
      inLine += c;
    }
  }
}

// -----------------------------------------
// PROCESA LÍNEAS COMO "CLASE:0" "CLASE:1"
// -----------------------------------------
void manejarComando(const String& s) {
  if (s.startsWith("CLASE:")) {
    int clase = s.substring(6).toInt();
    aplicarClase(clase);
    Serial.print("OK "); Serial.println(clase);
  }
}

// -----------------------------------------
// LÓGICA SEGÚN LA CLASE Y TEMPORIZADOR
// -----------------------------------------
void aplicarClase(int clase) {

  if (ventanaActiva) {

    // Solo abrir para Vale (0), Benja (1), Alvaro (2)
    if (clase == 0 || clase == 1 || clase == 2) {
      abrirPorton();
    }

    // Si clase == 3 (NADA), no hace nada
  }
  else {
    // Ventana cerrada: reproducir audio SIEMPRE
    reproducirAdvertencia();
  }
}

// -----------------------------------------
// FUNCIÓN: ABRIR - ESPERAR - CERRAR PORTÓN
// -----------------------------------------
void abrirPorton() {

  // Abrir (izquierda)
  digitalWrite(MOTOR_A, HIGH);
  digitalWrite(MOTOR_B, LOW);
  delay(5000);

  apagarMotor();
  delay(3000);

  // Cerrar (derecha)
  digitalWrite(MOTOR_A, LOW);
  digitalWrite(MOTOR_B, HIGH);
  delay(5000);

  apagarMotor();
}

// -----------------------------------------
// FUNCIÓN: REPRODUCIR MENSAJE DENEGADO
// -----------------------------------------
void reproducirAdvertencia() {
  digitalWrite(PLAY_PIN, HIGH);
  delay(500);   
  digitalWrite(PLAY_PIN, LOW);
  delay(2000);  // para que no lo repita muy rápido
}

// -----------------------------------------
// SEGURIDAD: APAGAR EL MOTOR
// -----------------------------------------
void apagarMotor() {
  digitalWrite(MOTOR_A, LOW);
  digitalWrite(MOTOR_B, LOW);
}
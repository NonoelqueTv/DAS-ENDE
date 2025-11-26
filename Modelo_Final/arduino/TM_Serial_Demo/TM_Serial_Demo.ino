// CONTROL TM + ISD1820 + PORTÓN
// Requerimientos aplicados según pedido

#define REC_PIN   6
#define PLAY_PIN  7

#define MOTOR_D   10   // derecha
#define MOTOR_I   11   // izquierda

String inLine;

// Estado de registro
bool usuarioRegistrado = false;

void setup() {
  Serial.begin(115200);

  pinMode(REC_PIN, OUTPUT);
  pinMode(PLAY_PIN, OUTPUT);

  pinMode(MOTOR_D, OUTPUT);
  pinMode(MOTOR_I, OUTPUT);

  digitalWrite(REC_PIN, LOW);
  digitalWrite(PLAY_PIN, LOW);
  apagarMotor();

  // Grabación inicial 5 s
  Serial.println("Grabando 5 segundos...");
  digitalWrite(REC_PIN, HIGH);
  delay(5000);
  digitalWrite(REC_PIN, LOW);
  Serial.println("Grabación finalizada.");
}

void loop() {
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

void manejarComando(const String& s) {
  if (s.startsWith("CLASE:")) {
    int clase = s.substring(6).toInt();
    aplicarClase(clase);
    Serial.print("OK ");
    Serial.println(clase);
  }
}

void aplicarClase(int c) {

  // --- CLASE 3: solo acciona ISD1820 ---
  if (c == 3) {
    reproducirAudio();
    return;
  }

  // --- CLASES 0,1,2 ---
  if (c == 0 || c == 1 || c == 2) {

    // Si ya usaron el portón antes
    if (usuarioRegistrado) {
      Serial.println("Usuario ya registrado");
      reproducirAudio();
      return;
    }

    // Primera vez: abrir y cerrar portón
    usuarioRegistrado = true;
    moverPorton();
  }
}

void moverPorton() {

  // 2.5 s a la derecha
  digitalWrite(MOTOR_D, HIGH);
  digitalWrite(MOTOR_I, LOW);
  delay(2500);

  apagarMotor();
  delay(4000);

  // 2.5 s a la izquierda
  digitalWrite(MOTOR_D, LOW);
  digitalWrite(MOTOR_I, HIGH);
  delay(2500);

  apagarMotor();
}

void reproducirAudio() {
  digitalWrite(PLAY_PIN, HIGH);
  delay(200);
  digitalWrite(PLAY_PIN, LOW);
}

void apagarMotor() {
  digitalWrite(MOTOR_D, LOW);
  digitalWrite(MOTOR_I, LOW);
}

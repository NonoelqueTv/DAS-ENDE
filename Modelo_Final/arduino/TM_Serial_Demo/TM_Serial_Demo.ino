// TM_Serial_Demo + ISD1820
// Al inicio graba 5 segundos.
// Luego reproduce si llega CLASE:0, CLASE:1 o CLASE:2.

#define pinREC  6   // Pin REC del ISD1820
#define pinPLAY 7   // Pin PLAY del ISD1820

String inLine;

void setup() {
  Serial.begin(115200);

  pinMode(pinREC, OUTPUT);
  pinMode(pinPLAY, OUTPUT);

  // Asegurar que arranca apagado
  digitalWrite(pinREC, LOW);
  digitalWrite(pinPLAY, LOW);

  // --- GRABAR AL INICIO ---
  Serial.println("Grabando 5 segundos...");
  digitalWrite(pinREC, HIGH);
  delay(5000);
  digitalWrite(pinREC, LOW);
  Serial.println("Grabación finalizada.");
}

void loop() {
  while (Serial.available()) {
    char c = Serial.read();
    if (c == '\n') { manejarComando(inLine); inLine = ""; }
    else if (c != '\r') inLine += c;
  }
}

void manejarComando(const String& s) {
  if (s.startsWith("CLASE:")) {
    int clase = s.substring(6).toInt();
    aplicarClase(clase);
    Serial.print("OK "); Serial.println(clase);
  }
}

void aplicarClase(int c) {
  // Si la clase es 0, 1 o 2 → reproducir
  if (c == 0 || c == 1 || c == 2) {
    reproducirAudio();
  }
}

void reproducirAudio() {
  // Pulso PLAY corto para reproducir una vez
  digitalWrite(pinPLAY, HIGH);
  delay(200);
  digitalWrite(pinPLAY, LOW);
}

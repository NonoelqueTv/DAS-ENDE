// TM_Serial_Demo.ino - Arduino ejemplo simple
// Lee líneas como "CLASE:0", "CLASE:1" o "CLASE:2" y enciende LEDs.
// Ajustá pines/acciones a tu proyecto.

#define LED0 3
#define LED1 5
#define LED2 6

String inLine;

void setup() {
  Serial.begin(115200);
  pinMode(LED0, OUTPUT);
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  apagarTodo();
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
  apagarTodo();
  if (c == 0) digitalWrite(LED0, HIGH);
  else if (c == 1) digitalWrite(LED1, HIGH);
  else if (c == 2) digitalWrite(LED2, HIGH);
}

void apagarTodo() {
  digitalWrite(LED0, LOW);
  digitalWrite(LED1, LOW);
  digitalWrite(LED2, LOW);
}

/* Liberar ou negar acesso com LEDS por meio da comunicação serial */
int ledVerde = 9;
int ledVermelho = 8;

void setup() {
  Serial.begin(9600);
  pinMode(ledVerde, OUTPUT);
  pinMode(ledVermelho, OUTPUT);

}

void loop() {
  digitalWrite(ledVermelho, HIGH);
  if(Serial.available()) {
    char caractere = Serial.read();
    if( caractere == '1') {
      digitalWrite(ledVermelho, LOW);
      digitalWrite(ledVerde, HIGH);
      delay(5000);
      digitalWrite(ledVerde, LOW);
      digitalWrite(ledVermelho, HIGH);
    }
  }
}

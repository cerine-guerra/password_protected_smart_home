int pumpPin = 9;

void setup() {
  pinMode(pumpPin , OUTPUT);
  digitalWrite(pumpPin, LOW);

}

void loop() {
  digitalWrite(pumpPin, HIGH);
  delay(1000);

  digitalWrite(pumpPin, LOW);
  delay(1000);
}

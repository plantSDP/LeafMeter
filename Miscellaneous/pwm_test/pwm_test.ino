void setup() {
  pinMode(3, OUTPUT);
  analogWriteFrequency(3,500);
}

void loop() {
  analogWrite(3,255);
  delay(2000);
  analogWrite(3,220);
  delay(2000);
  analogWrite(3,200);
  delay(2000);
  analogWrite(3,180);
  delay(2000);
  analogWrite(3,150);
}

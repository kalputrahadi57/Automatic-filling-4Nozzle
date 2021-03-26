unsigned long oldTimer, countLed;
void timerLed() {
  unsigned long tTimer = millis();
  if (tTimer - oldTimer > 100) {
    oldTimer = tTimer;
    countLed++;
    if (countLed % (5 * setSt) == 0) {
      digitalWrite(ledPin, HIGH);
    } else digitalWrite(ledPin, LOW);

    if (countLed % 10 == 0) {
      tunda--;
      if (tunda == 0) {
        setSt = 0;
        digitalWrite(ledPin, LOW);
//        Serial.print("tundaNol ");
//        Serial.println(tunda);
      }
    }
  }
}

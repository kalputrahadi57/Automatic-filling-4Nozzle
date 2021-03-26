//volatile int flow_frequency; // Measures flow sensor pulses
unsigned int lpji; // Calculated litres/hour
float lpjf; // Calculated litres/hour
float lpm; // Calculated litres/hour
float lpd; // Calculated litres/hour
unsigned long currentTime;
unsigned long cloopTime;

void flow () {
  pulses++;
}

void setup1() {
  pinMode(encoder_pin, INPUT);
  digitalWrite(encoder_pin, HIGH);
  attachInterrupt(0, flow, RISING); // Setup Interrupt
  sei(); // Enable interrupts
  currentTime = millis();
  cloopTime = currentTime;
}
void LP () {
  currentTime = millis();
  if (currentTime >= (cloopTime + 1000)) {
    cloopTime = currentTime; // Updates cloopTime
    // Pulse frequency (Hz) = 7.5Q, Q is flow rate in L/min.

    lpji = (pulses * 60 / 7.5); // (Pulse frequency x 60 min) / 7.5Q = flowrate in L/hour
    lpjf = (pulses * 60 / 7.5);
    lpm = (pulses / 7.5);
    lpd = (pulses / (7.5 * 60));

    Serial.print(pulses); // Print litres/hour
    Serial.println(" Pulses");
    Serial.print(lpji, DEC); // Print litres/hour
    Serial.println(" L/hour I");
    Serial.print(lpjf, 2); // Print litres/hour
    Serial.println(" L/hour F");
    Serial.print(lpm, 2); // Print litres/hour
    Serial.println(" L/m F");
    Serial.print(lpd, 2); // Print litres/hour
    Serial.println(" L/d F");

    Serial.println();
    pulses = 0; // Reset Counter
  }
}

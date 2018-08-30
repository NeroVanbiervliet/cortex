#define TIME_STROBE_ON 5000 // milliseconds

void stormSetup() {
    pinMode(RELAIS_STROBOSCOPE, OUTPUT); 
    digitalWrite(RELAIS_STROBOSCOPE, LOW);
}

void performStorm(boolean silent=false) {
  if (!silent) makeSound(SND_STORM); 
  digitalWrite(RELAIS_STROBOSCOPE, HIGH); 
  delay(TIME_STROBE_ON); // TODO blocks program, use timer2
  digitalWrite(RELAIS_STROBOSCOPE, LOW); 
}

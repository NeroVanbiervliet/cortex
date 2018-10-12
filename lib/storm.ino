#include <MsTimer2.h>

#define TIME_STROBE_ON 5000 // milliseconds


void stormSetup() {
    pinMode(RELAIS_STROBOSCOPE, OUTPUT); 
    digitalWrite(RELAIS_STROBOSCOPE, LOW);
    MsTimer2::set(TIME_STROBE_ON, endStorm);
}

void performStorm(boolean silent=false) {
  Serial.println("performing Storm"); 
  if (!silent) makeSound(SND_STORM); 
  digitalWrite(RELAIS_STROBOSCOPE, HIGH); 

  // timer will end the storm
  MsTimer2::start();
}

void endStorm() {
  MsTimer2::stop();
  digitalWrite(RELAIS_STROBOSCOPE, LOW); 
}

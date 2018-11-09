#include <MsTimer2.h>

#define TIME_STROBE_ON 2500 // milliseconds


void stormSetup() {
    pinMode(RELAIS_STROBOSCOPE, OUTPUT); 
    digitalWrite(RELAIS_STROBOSCOPE, LOW);
    MsTimer2::set(TIME_STROBE_ON, endStorm);
}

void performStorm() {
  Serial.println("performing Storm"); 
  digitalWrite(RELAIS_STROBOSCOPE, HIGH); 

  // timer will end the storm
  MsTimer2::start();
}

void endStorm() {
  MsTimer2::stop();
  digitalWrite(RELAIS_STROBOSCOPE, LOW); 
}

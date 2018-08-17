#include "HX711.h" // scale library

// constants
// GND and 5V are also connected
#define SCALE_DOUT CONTROLLINO_D11
#define SCALE_CLK CONTROLLINO_D10
#define CALIBRATION_FACTOR -799650 // obtained with other sketch
#define EXPECTED_WEIGHT 42 // [grams]
#define TOLERANCE 2 // tolerated fault above and below [grams]
HX711 scale(SCALE_DOUT, SCALE_CLK);

// indicates wether correct weight was measured
bool correctWeight = false;

void scaleSetup() {
  // library initialisation 
  scale.set_scale(CALIBRATION_FACTOR);
  scale.tare(); // reset to zero kg
}

int getWeight() {
  return (int) scale.get_units()*1000; 
}

// weighs ingredients in kettle and checks with expected value
void weighKettle() {
  int weight = getWeight(); 
  if (weight > EXPECTED_WEIGHT-TOLERANCE && weight < EXPECTED_WEIGHT+TOLERANCE) correctWeight = true;
  launchStateTimer(TIME_BETWEEN_WEIGHINGS);
}

// getter needed because of scope
bool isCorrectWeight() {
  return correctWeight; 
}


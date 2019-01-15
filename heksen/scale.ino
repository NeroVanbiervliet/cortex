#include "HX711.h" // scale library

// constants
// GND and 5V are also connected
#define SCALE_DOUT CONTROLLINO_D11
#define SCALE_CLK CONTROLLINO_D10
#define CALIBRATION_FACTOR -99650 // obtained with other sketch
#define EXPECTED_WEIGHT 545 // [grams] mushroom (172) + mandrake (144) + water flask (229)
#define TOLERANCE 30 // tolerated fault above and below [grams]
HX711 scale(SCALE_DOUT, SCALE_CLK);

// indicates how many times in a row the correct weight was measured
int correctWeight = 0;

void scaleSetup() {
  // library initialisation 
  scale.set_scale(CALIBRATION_FACTOR);
  scale.tare(); // reset to zero kg
}

int getWeight() {
  return scale.get_units()*1000; 
}

bool isWithinTolerance(int weight) {
  return weight > EXPECTED_WEIGHT-TOLERANCE && weight < EXPECTED_WEIGHT+TOLERANCE;
}

// weighs ingredients in kettle and checks with expected value
void weighKettle() {
  int weight = getWeight();
  Serial.print("Weight: ");
  Serial.println(weight); 
  
  if (isWithinTolerance(weight)) correctWeight += 1;
  else correctWeight = 0;
  Serial.print("Correctweight count: ");
  Serial.println(correctWeight);
  
  launchStateTimerDs(TIME_BETWEEN_WEIGHINGS);
}

// getter needed because of scope
bool isCorrectWeight() {
  return correctWeight >= REQUIRED_CORRECT_SAMPLES; 
}

void debugWeight() {
  char weightStr[3];
  sprintf(weightStr, "%03d", getWeight());
  setMessageToSend(weightStr);
}

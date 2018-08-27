#include <TimerOne.h>
#include <Controllino.h> 

// timing constants [seconds]
#define TIME_STROBE_ON 5
#define TIME_BETWEEN_WEIGHINGS 1

// state constants
#define STATE_INIT 0
#define STATE_VICTORY 1

// victory button constants
#define BUTTON_SUPPLY CONTROLLINO_D0
#define BUTTON_INT CONTROLLINO_IN0
#define BUTTON_DEBOUNCING_TIME 50 // [milliseconds]

// sound constants
#define SND_STORM "T1"
#define SND_VICTORY "T2"

// for testing purpose NEED remove
#define RELAIS_LAMP_TEST CONTROLLINO_R9

// state management
int state = STATE_INIT;

// debouncing
unsigned long lastButtonPress = 42; // default value :) 

void setup() {
  // pinmodes
  pinMode(BUTTON_SUPPLY, OUTPUT);
  pinMode(BUTTON_INT, INPUT);
  pinMode(RELAIS_LAMP_TEST, OUTPUT); 

  // default states of outputs
  digitalWrite(BUTTON_SUPPLY, HIGH); 
  digitalWrite(RELAIS_LAMP_TEST, LOW); // NEED remove

  // interrupts
  attachInterrupt(digitalPinToInterrupt(BUTTON_INT), buttonIsr, CHANGE);

  // setup needed for common file and scale file
  commonSetup(); 
}

void loop() {
  handleEthernet(); // NEED delay nodig?
}

void buttonIsr() {
  if (digitalRead(BUTTON_INT) == HIGH) {
    lastButtonPress = millis(); 
  }
  else {
    // if debouncing passes, STATE_VICTORY is reached
    if (millis() - lastButtonPress > BUTTON_DEBOUNCING_TIME && state == STATE_INIT) nextState();
  }
}

// implements state diagram
void nextState() {
  int nextState; // next state to be set
  if (state == STATE_INIT) nextState = STATE_VICTORY;  
  
  Serial.println("state: " + String(nextState)); 
  state = nextState; 
  performState(); 
}

// performs state actions
void performState() {
  switch (state) {
    case STATE_VICTORY:
    makeSound(SND_VICTORY); 
    // NEED send signal to stop timer (houdini)
    // NEED strobe
    break; 
  }
}

// handles ethernet api request
void handleApiRequest(String apiPath) {
  Serial.println("api request received at path: " + apiPath); 
  if (apiPath == "/storm/on") {
    digitalWrite(RELAIS_LAMP_TEST, HIGH);
  }
  else if (apiPath == "/storm/off") {
    digitalWrite(RELAIS_LAMP_TEST, LOW); 
  }
  else if (apiPath == "storm-on_NEXT_ERSCOMMAND_") { 
    setMessageToSend("@Action.ServerTest.ers"); 
  }
}


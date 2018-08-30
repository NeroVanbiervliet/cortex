#include <TimerOne.h>
#include <Controllino.h> 

// timing constants [seconds]
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

// required by storm.lib
#define RELAIS_STROBOSCOPE CONTROLLINO_R5
void performStorm(boolean silent=false); // prototype

// state management
int state = STATE_INIT;

// debouncing
unsigned long lastButtonPress = 42; // default value :) 

void setup() {
  // pinmodes
  pinMode(BUTTON_SUPPLY, OUTPUT);
  pinMode(BUTTON_INT, INPUT);

  // default states of outputs
  digitalWrite(BUTTON_SUPPLY, HIGH); 

  // interrupts
  attachInterrupt(digitalPinToInterrupt(BUTTON_INT), buttonIsr, CHANGE);

  // setup needed for common file and scale file
  commonSetup(); 
  stormSetup(); 
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
    int deltaTime = millis() - lastButtonPress;
    if (deltaTime > BUTTON_DEBOUNCING_TIME && state == STATE_INIT) nextState();
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
    setMessageToSend("@Action.victory.ers"); 
    performStorm(true);
    break; 
  }
}

// handles ethernet api request
void handleApiRequest(String apiPath) {
  Serial.println("api request received at path: " + apiPath); 
  if (apiPath == "/storm") {
    performStorm();   
  }
}



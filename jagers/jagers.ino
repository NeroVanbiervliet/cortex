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

// computer action constants
#define ACTION_VICTORY "@{6eefb755-bf7f-46cb-b43b-deb75d65e28f}"
#define ACTION_CONTROLLINO_CHECK "@{4326b3f3-3d1c-40ec-8804-7a4e8e6a593c}"

// required by storm.lib
#define RELAIS_STROBOSCOPE CONTROLLINO_R5

// cross magnet
#define RELAIS_MAGNET_CROSS CONTROLLINO_R9

// blacklight
#define RELAIS_BLACKLIGHT CONTROLLINO_R3

// state management
int state = STATE_INIT;

// debouncing
unsigned long lastButtonPress = 42; // default value :) 

void setup() {
  // pinmodes
  pinMode(BUTTON_SUPPLY, OUTPUT);
  pinMode(BUTTON_INT, INPUT);
  pinMode(RELAIS_MAGNET_CROSS, OUTPUT); 
  pinMode(RELAIS_BLACKLIGHT, OUTPUT);

  // default states of outputs
  digitalWrite(BUTTON_SUPPLY, HIGH); 
  digitalWrite(RELAIS_MAGNET_CROSS, HIGH); 
  digitalWrite(RELAIS_BLACKLIGHT, HIGH);

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
    setMessageToSend(ACTION_VICTORY);
    performStorm();
    break; 
  }
}

// handles ethernet api request
void handleApiRequest(String apiPath) {
  Serial.println("api request received at path: " + apiPath); 
  if (apiPath == "/storm") performStorm();
  else if (apiPath == "/cross/magnet/off") digitalWrite(RELAIS_MAGNET_CROSS, LOW); 
  else if (apiPath == "/blacklight/on") digitalWrite(RELAIS_BLACKLIGHT, HIGH);
  else if (apiPath == "/blacklight/off") digitalWrite(RELAIS_BLACKLIGHT, LOW);
}

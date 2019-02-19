#include <TimerOne.h>
#include <Controllino.h> 

// scale constants
#define TIME_BETWEEN_WEIGHINGS 5 // [deciseconds]: 1 ds = 0.1 seconds
#define REQUIRED_CORRECT_SAMPLES 3

// state constants
#define STATE_WEIGH_KETTLE 2
#define STATE_UNLOCK_COMPARTMENT 3
#define STATE_VICTORY 4

// victory button constants
#define BUTTON_SUPPLY CONTROLLINO_D0
#define BUTTON_INT CONTROLLINO_IN0
#define BUTTON_DEBOUNCING_TIME 50 // [milliseconds]

// computer action constants
#define ACTION_VICTORY "@{8308bbc1-5ca7-4230-9673-9b59e7120aa6}"
#define ACTION_CONTROLLINO_CHECK "@{c27eabf2-20bd-4c24-b0e7-610f604310a9}"

// magnet constants
#define RELAIS_MAGNET_DOOR CONTROLLINO_R6
#define RELAIS_MAGNET_COMPARTMENT CONTROLLINO_R7

// required by storm.lib
#define RELAIS_STROBOSCOPE CONTROLLINO_R5

// state management
int state = STATE_WEIGH_KETTLE;

// debouncing
unsigned long lastButtonPress = 42; // default value :)

void setup() {
  // pinmodes
  pinMode(BUTTON_SUPPLY, OUTPUT);
  pinMode(BUTTON_INT, INPUT);
  pinMode(RELAIS_MAGNET_DOOR, OUTPUT); 
  pinMode(RELAIS_MAGNET_COMPARTMENT, OUTPUT); 

  // default states of outputs
  digitalWrite(BUTTON_SUPPLY, HIGH); 
  digitalWrite(RELAIS_MAGNET_DOOR, LOW);
  digitalWrite(RELAIS_MAGNET_COMPARTMENT, HIGH);

  // interrupts
  attachInterrupt(digitalPinToInterrupt(BUTTON_INT), buttonIsr, CHANGE);

  // setup needed for common file and scale file
  commonSetup(); 
  scaleSetup(); 
  stormSetup(); 

  // NEED remove
  performState();
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
    if (deltaTime > BUTTON_DEBOUNCING_TIME && state == STATE_UNLOCK_COMPARTMENT) nextState();
  }
}

// implements state diagram
void nextState() {
  int nextState; // next state to be set
  if (state == STATE_WEIGH_KETTLE && !isCorrectWeight()) nextState = STATE_WEIGH_KETTLE; 
  if (state == STATE_WEIGH_KETTLE && isCorrectWeight()) nextState = STATE_UNLOCK_COMPARTMENT; 
  if (state == STATE_UNLOCK_COMPARTMENT) nextState = STATE_VICTORY; 
  
  Serial.println("state: " + String(nextState)); 
  state = nextState; 
  performState(); 
}

// performs state actions
void performState() {
  switch (state) {
    case STATE_WEIGH_KETTLE:
    weighKettle(); // will trigger nextState()
    break;

    case STATE_UNLOCK_COMPARTMENT:
    digitalWrite(RELAIS_MAGNET_COMPARTMENT, LOW);
    break;

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
  else if (apiPath == "/door/magnet/off") digitalWrite(RELAIS_MAGNET_DOOR, LOW); 
  else if (apiPath == "/door/magnet/on") digitalWrite(RELAIS_MAGNET_DOOR, HIGH); 
  else if (apiPath == "/compartment/magnet/off") digitalWrite(RELAIS_MAGNET_COMPARTMENT, LOW);
  else if (apiPath == "/compartment/magnet/on") digitalWrite(RELAIS_MAGNET_COMPARTMENT, HIGH);
  else if (apiPath == "/kettle/weight") debugWeight();
}

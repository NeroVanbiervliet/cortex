#include <TimerOne.h>
#include <Controllino.h> 

// scale constants
#define TIME_BETWEEN_WEIGHINGS 5 // [deciseconds]: 1 ds = 0.1 seconds
#define REQUIRED_CORRECT_SAMPLES 3

// state constants
#define STATE_INIT 0
#define STATE_UNLOCK_DOOR 1
#define STATE_WEIGH_KETTLE 2
#define STATE_UNLOCK_COMPARTMENT 3
#define STATE_VICTORY 4

// victory button constants
#define BUTTON_SUPPLY CONTROLLINO_D0
#define BUTTON_INT CONTROLLINO_IN0
#define BUTTON_DEBOUNCING_TIME 50 // [milliseconds]

// stone pile constants
// 12 V and GND are also connected
#define STONE_PILE_INT CONTROLLINO_IN1
#define STONE_PILE_DEBOUCING_TIME 50 // [milliseconds]s

// magnet constants
#define RELAIS_MAGNET_DOOR CONTROLLINO_R6
#define RELAIS_MAGNET_COMPARTMENT CONTROLLINO_R7

// sound constants
#define SND_DOOR_OPEN "T2"
#define SND_COMPARTMENT_OPEN "T3"
#define SND_VICTORY "T4"

// required by storm.lib
#define RELAIS_STROBOSCOPE CONTROLLINO_R5

// state management
int state = STATE_WEIGH_KETTLE;

// debouncing
unsigned long lastButtonPress = 42; // default value :) 
unsigned long lastStonePilePress = 42; 

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
  attachInterrupt(digitalPinToInterrupt(STONE_PILE_INT), stonePileIsr, CHANGE); 

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
    if (millis() - lastButtonPress > BUTTON_DEBOUNCING_TIME && state == STATE_UNLOCK_COMPARTMENT) nextState();
  }
}
 
void stonePileIsr() {
  if (digitalRead(STONE_PILE_INT) == HIGH) {
    lastStonePilePress = millis(); 
  }
  else {
    // if debouncing passes, STATE_UNLOCK_DOOR is reached
    if (millis() - lastStonePilePress > STONE_PILE_DEBOUCING_TIME && state == STATE_INIT) nextState();
  }
}

// implements state diagram
void nextState() {
  int nextState; // next state to be set
  if (state == STATE_INIT) nextState = STATE_UNLOCK_DOOR; 
  if (state == STATE_UNLOCK_DOOR) nextState = STATE_WEIGH_KETTLE; 
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
    case STATE_UNLOCK_DOOR:
    digitalWrite(RELAIS_MAGNET_DOOR, LOW); 
    makeSound(SND_DOOR_OPEN); 
    nextState();
    break; 

    case STATE_WEIGH_KETTLE:
    weighKettle(); // will trigger nextState()
    break;

    case STATE_UNLOCK_COMPARTMENT:
    digitalWrite(RELAIS_MAGNET_COMPARTMENT, LOW); 
    //makeSound(SND_COMPARTMENT_OPEN);
    break;

    case STATE_VICTORY:
    setMessageToSend("@Action.victory.ers");
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

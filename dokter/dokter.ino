#include <TimerOne.h>
#include <Controllino.h> 

// timing constants [seconds]
#define TIME_SMOKE_ON 4
#define TIME_WAIT 30

// state constants
#define STATE_INIT 0
#define STATE_ACTIVE 1
#define STATE_PLAY_AUDIO 2
#define STATE_SMOKE 3
#define STATE_SMOKE_OFF 4

// computer action constants
#define SND_KEYPRESS "@{e347e304-184b-43bf-a543-0ccb3a3df18f}"
#define SND_GOODCODE "@{07d3aa47-3bdc-40a9-8211-249a215b2039}"
#define SND_BADCODE "@{1a97e4ef-75cf-4342-af3e-81a6bb917141}"
#define SND_ALARM "@{2f53229c-84a1-4154-8faa-9dd57e6d2fa0}"
#define ACTION_CONTROLLINO_CHECK "@{}" // not used for dokter


// keypad constants
#define KEYPAD_SUPPLY CONTROLLINO_D0
#define KEYPAD_INT CONTROLLINO_IN0
#define KEYPAD_DEBOUNCING_TIME 50 // [milliseconds]
int KEYPAD_COLUMN_PINS[] = {CONTROLLINO_A0, CONTROLLINO_A1};
#define KEYPAD_STATUS_LED CONTROLLINO_D7

// other constants
#define SUPPLY_ALARM_LIGHT CONTROLLINO_D11 // red flashing light
#define RELAIS_SMOKE CONTROLLINO_R8
#define RELAIS_LASERS CONTROLLINO_R9
#define RELAIS_MAIN_LIGHT CONTROLLINO_R5

// debouncing
unsigned long lastKeyPress;
int lastDigit; // last registered digit of keypad

// state management
int state = STATE_INIT; // default state before correct keypad code is entered
bool firstActivation = true; 

// keypad code
int keypadCodeTruth[4] = {1,4,7,4};
int keypadCodeAttempt[4];
int codeIndex = 0;

// keypad status light (evenly spaced perceptually)
int statusLightBrightness[4] = {0, 40, 125, 255};

void setup() {
  // pinmodes
  pinMode(KEYPAD_SUPPLY, OUTPUT);
  pinMode(KEYPAD_INT, INPUT);
  for (int i=0; i<2; i++) {
    pinMode(KEYPAD_COLUMN_PINS[i], INPUT); 
  }
  pinMode(KEYPAD_STATUS_LED, OUTPUT);
  pinMode(SUPPLY_ALARM_LIGHT, OUTPUT); 
  pinMode(RELAIS_LASERS, OUTPUT); 
  pinMode(RELAIS_SMOKE, OUTPUT); 
  pinMode(RELAIS_MAIN_LIGHT, OUTPUT); 

  // default states of outputs 
  digitalWrite(KEYPAD_SUPPLY, HIGH);
  analogWrite(KEYPAD_STATUS_LED, 0); 
  digitalWrite(SUPPLY_ALARM_LIGHT, LOW);  
  digitalWrite(RELAIS_LASERS, LOW); 
  digitalWrite(RELAIS_SMOKE, LOW); 
  digitalWrite(RELAIS_MAIN_LIGHT, HIGH);

  // interrupts
  attachInterrupt(digitalPinToInterrupt(KEYPAD_INT), keypadIsr, CHANGE);

  // setup needed for common file
  commonSetup(); 
}


void loop() {
  handleEthernet(); // NEED delay nodig?
}

void keypadIsr() {
  if (digitalRead(KEYPAD_INT) == HIGH) {
    lastKeyPress = millis(); 
    readKeypad(); 
  }
  else {
    if (millis() - lastKeyPress > KEYPAD_DEBOUNCING_TIME) registerLastDigit();
  }
}

// if deboucing passes, register the last digit
void registerLastDigit() {
  keypadCodeAttempt[codeIndex] = lastDigit;
  Serial.println(lastDigit); 
  codeIndex++;
  setMessageToSend(SND_KEYPRESS);
  setStatusLight(codeIndex); 

  // check code
  if (codeIndex == 4) {
    codeIndex = 0;
    if (checkKeyPadCode()) { setMessageToSend(SND_GOODCODE); nextState(); }
    else setMessageToSend(SND_BADCODE);
  }
}

void setStatusLight(int codeIndex) {
  analogWrite(KEYPAD_STATUS_LED, statusLightBrightness[codeIndex]);
}

// check if the attempted keypad code is correct
bool checkKeyPadCode() {
  bool match = true;
  for (int i=0; i<4; i++) {
    match = match && ((keypadCodeAttempt[i] == keypadCodeTruth[i]) || ((i==2) && (keypadCodeAttempt[i] == 10)));
  }
  return match;
}

// reads the keypad
void readKeypad() {
  // give voltage time to rise, interrupt is already triggered @9V
  delayMicroseconds(100); 
  
  // check what column caused interrupt
  for(int column=0; column<2; column++) {
    float voltage = analogRead(KEYPAD_COLUMN_PINS[column]); 
    voltage = map(voltage, 0, 1023, 0, 160)/float(10); // 16 blijkbaar omdat voltage controllino tot 16V kan meten
    if (voltage > 6) {
      int row = voltageToRow(voltage); 
      lastDigit = indicesToDigit(row, column); 
    }
  }
}

// converts analog measured voltage to the row of the keypad
// OUT: row 0-3, 0 is top row
int voltageToRow(float voltage) {
  if (voltage < 9.69) return 0;
  if (voltage >= 9.69 && voltage < 10.62) return 1; 
  if (voltage >= 10.62) return 2; 
}

// converts row and column indices of keypad to the digit
int indicesToDigit(int row, int column) {
  return (row*3) + column + 1; 
}

// implements state diagram
void nextState() {
  int nextState; // next state to be set
  if (state == STATE_INIT) nextState = STATE_ACTIVE; 
  if (state == STATE_ACTIVE && !firstActivation) nextState = STATE_SMOKE;
  if (state == STATE_ACTIVE && firstActivation) { nextState = STATE_PLAY_AUDIO; firstActivation = false; }
  if (state == STATE_PLAY_AUDIO) nextState = STATE_SMOKE; 
  if (state == STATE_SMOKE) nextState = STATE_SMOKE_OFF; 
  if (state == STATE_SMOKE_OFF) nextState = STATE_INIT; 
  
  Serial.println("state: " + String(nextState)); 
  state = nextState; 
  performState(); 
}

// performs state actions
void performState() {
  Serial.print("STATE =");
  Serial.println(state);
  switch (state) {
    case STATE_ACTIVE:
    digitalWrite(RELAIS_LASERS, HIGH); 
    digitalWrite(SUPPLY_ALARM_LIGHT, HIGH); 
    digitalWrite(RELAIS_MAIN_LIGHT, LOW); 
    nextState(); 
    break;
    
    case STATE_PLAY_AUDIO:
    setMessageToSend(SND_ALARM); 
    nextState(); 
    break; 

    case STATE_SMOKE:
    digitalWrite(RELAIS_SMOKE, HIGH); 
    launchStateTimer(TIME_SMOKE_ON); 
    break; 

    case STATE_SMOKE_OFF:
    digitalWrite(RELAIS_SMOKE, LOW); 
    nextState();
    break;
  }
}

// handles ethernet api request
void handleApiRequest(String apiPath) {
  Serial.println("api request received at path: " + apiPath); 
  // empty
}

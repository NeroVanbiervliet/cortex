#include <Controllino.h> 

// keypad constants
#define KEYPAD_SUPPLY CONTROLLINO_D0
#define KEYPAD_INT CONTROLLINO_IN0
#define KEYPAD_DEBOUNCING_TIME 50 // [milliseconds]
int KEYPAD_COLUMN_PINS[] = {CONTROLLINO_A0, CONTROLLINO_A1, CONTROLLINO_A2};

// sound constants
#define SND_KEY_PRESS 0
#define SND_BAD_CODE 1
#define SND_GOOD_CODE 2

// other constants
#define RELAIS_ALARM_LIGHT CONTROLLINO_R2

// debouncing
unsigned long lastKeyPress;
int lastDigit; // last registered digit of keypad

// keypad code
int keypadCodeTruth[4] = {1,4,7,4};
int keypadCodeAttempt[4];
int codeIndex = 0;

void setup() {
  // pinmodes
  pinMode(KEYPAD_SUPPLY, OUTPUT);
  pinMode(KEYPAD_INT, INPUT);
  for (int i=0; i<3; i++) {
    pinMode(KEYPAD_COLUMN_PINS[i], INPUT); 
  }
  pinMode(RELAIS_ALARM_LIGHT, OUTPUT); 

  // default states of outputs 
  digitalWrite(KEYPAD_SUPPLY, HIGH); 
  digitalWrite(RELAIS_ALARM_LIGHT, LOW);  

  // interrupts
  attachInterrupt(digitalPinToInterrupt(KEYPAD_INT), keypadIsr, CHANGE);

  // pc serial for debugging
  Serial.begin(9600); 
  Serial.println("serial established"); 

  // mp3 serial
  Serial2.begin(38400); 
}


void loop() {
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
  codeIndex++;
  makeSound(SND_KEY_PRESS);

  // check code
  if (codeIndex == 4) {
    codeIndex = 0;
    if (checkKeyPadCode()) makeSound(SND_GOOD_CODE); 
    else makeSound(SND_BAD_CODE); 
  }
}

// check if the attempted keypad code is correct
bool checkKeyPadCode() {
  bool match = true;
  for (int i=0; i<4; i++) {
    match = match && (keypadCodeAttempt[i] == keypadCodeTruth[i]);
  }
  return match;
}

// reads the keypad
void readKeypad() {
  // give voltage time to rise, interrupt is already triggered @9V
  delayMicroseconds(100); 
  
  // check what column caused interrupt
  for(int column=0; column<3; column++) {
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
  if (voltage >= 10.62 && voltage < 11.54) return 2; 
  if (voltage >= 11.54) return 3; 
}

// converts row and column indices of keypad to the digit
int indicesToDigit(int row, int column) {
  return (row*3) + column + 1; 
}

void makeSound(int sound) {
  switch (sound) {
    case SND_KEY_PRESS:
    Serial2.println("T2"); // mp3 trigger command
    break;
    
    case SND_GOOD_CODE:
    Serial2.println("T5"); 
    break;

    case SND_BAD_CODE:
    Serial2.println("T3");   
  }
}


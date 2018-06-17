#include <Controllino.h> 

#define KEYPAD_SUPPLY CONTROLLINO_D0
#define KEYPAD_INT CONTROLLINO_IN0
int KEYPAD_COLUMN_PINS[] = {CONTROLLINO_A0, CONTROLLINO_A1, CONTROLLINO_A2};

void setup() {
  pinMode(KEYPAD_SUPPLY, OUTPUT);
  digitalWrite(KEYPAD_SUPPLY, HIGH); 
  
  pinMode(KEYPAD_INT, INPUT); 
  attachInterrupt(digitalPinToInterrupt(KEYPAD_INT), keypadIsr, RISING);

  for (int i=0; i<3; i++) {
    pinMode(KEYPAD_COLUMN_PINS[i], INPUT); 
  }

  Serial.begin(9600); 
  Serial.println("serial established"); 
}


void loop() {
}

void keypadIsr() {
  // check what column caused interrupt
  for(int column=0; column<3; column++) {
    float voltage = analogRead(KEYPAD_COLUMN_PINS[column]); 
    voltage = map(voltage, 0, 1023, 0, 160)/float(10); // 16 blijkbaar omdat voltage controllino tot 16V kan meten
    if (voltage > 6) {
      int row = voltageToRow(voltage); 
      int digit = indicesToDigit(row, column); 
      // NEED verwerk digit (toevoegen aan codeTry) 
      Serial.println(digit); 
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



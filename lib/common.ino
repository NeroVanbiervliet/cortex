// timer
int timerRemainingCount; // remaining deciseconds in timer

// code that is called during setup()
void commonSetup() {
  // timer, cannot be initialised outside setup()!
  Timer1.initialize(100000); // 0.1 second period
  Timer1.attachInterrupt(timerIsr); 
  Timer1.stop(); 

  // pc serial for debugging
  Serial.begin(9600); 
  Serial.println("serial established"); 

  // mp3 serial
  Serial2.begin(38400); 
}

void makeSound(String soundName) {
  // convert String to char array
  char charBuf[50];
  ("@Action.sound-" + soundName + ".ers").toCharArray(charBuf, 50);
  
  // send action to pc
  setMessageToSend(charBuf);
}

// launch timer until next state
void launchStateTimer(int seconds) {
  launchStateTimerDs(seconds*10);  
}

// launch timer until next state
void launchStateTimerDs(int deciseconds) {
  timerRemainingCount = deciseconds;
  Timer1.start(); 
}

// timer interrupt subrouting
void timerIsr() {
  timerRemainingCount--; 
  Serial.println("remcount: " + String(timerRemainingCount)); 
  if (timerRemainingCount <= 0) {
    Timer1.stop(); 
    nextState(); 
  }
}

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

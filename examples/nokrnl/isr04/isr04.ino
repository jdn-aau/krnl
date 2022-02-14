const byte ledPin = 13;
const byte interruptPin = 2;

volatile byte state = LOW; // to avoid register optimization

#define DEBOUNCETIME 100

volatile unsigned int cnt = 0;
volatile unsigned long lastRunOfISR = 0;

void blink() {
  cnt++;   
  if ( timer0_overflow_count - lastRunOfISR  < DEBOUNCETIME )
  {/* do nothin - too early */
      
  }
  else {
    state = !state;
    lastRunOfISR = timer0_overflow_count;
  }
}

void setup() {
  pinMode(ledPin, OUTPUT);
  pinMode(interruptPin, INPUT_PULLUP);
  Serial.begin(115200);
  attachInterrupt(digitalPinToInterrupt(interruptPin), blink, CHANGE);
}
 

void loop() {
  digitalWrite(ledPin, state);
  Serial.println(cnt);
}

 

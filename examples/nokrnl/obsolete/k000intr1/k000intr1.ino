const byte ledPin = 13;
const byte interruptPin = 2;

volatile byte state = LOW; // to avoid register optimization

void setup() {
  pinMode(ledPin, OUTPUT);
  pinMode(interruptPin, INPUT_PULLUP);
  Serial.begin(115200);
  attachInterrupt(digitalPinToInterrupt(interruptPin), blink, CHANGE);
}
unsigned int cnt = 0;

void loop() {
  digitalWrite(ledPin, state);
  Serial.println(cnt);
}

// solve bouncing
unsigned long lastRunOfISR = 0;

void blink() {
  cnt++;
  if ( millis() - lastRunOfISR  < 100 )
  {}
  else {
    state = !state;
    lastRunOfISR = millis();
  }
}

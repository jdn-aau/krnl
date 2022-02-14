const byte ledPin = 13;
const byte interruptPin = 2;

volatile byte state = LOW;    // to avoid register optimization

volatile unsigned int cnt = 0;

void blink ()
{
    cnt++;  // interrupt is dsiabled when running this code
}

void setup ()
{
    pinMode (ledPin, OUTPUT);
    pinMode (interruptPin, INPUT_PULLUP);
    
    Serial.begin (115200);
    
    attachInterrupt (digitalPinToInterrupt (interruptPin), blink, CHANGE);
}

void loop ()
{
    digitalWrite (ledPin, state);
    Serial.println (cnt);
}
 

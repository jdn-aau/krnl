const byte ledPin = 13;
const byte interruptPin0 = 2;
const byte interruptPin1 = 3;

volatile byte state = LOW;    // to avoid register optimization

volatile boolean ledON = false;
volatile int cnt0 = 0, cnt1 = 0;

void onnn ()
{
  cnt0++;
  ledON = true;  // interrupt is dsiabled when running this code
}

void offf ()
{
  cnt1++;
  ledON = false;
}


void setup()
{
  pinMode (interruptPin0, INPUT_PULLUP);
  pinMode (interruptPin1, INPUT_PULLUP);

  Serial.begin (115200);

  attachInterrupt (digitalPinToInterrupt (interruptPin0), onnn, CHANGE);
  attachInterrupt (digitalPinToInterrupt (interruptPin1), offf, CHANGE);
}

void loop ()
{
  digitalWrite (ledPin, ledON);
  Serial.println("");
  Serial.print(cnt0); Serial.print(" "); Serial.println(cnt1);
}

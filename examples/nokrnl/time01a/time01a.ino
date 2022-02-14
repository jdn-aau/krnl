// measure duratino of code by scope / dig analysator
const unsigned long sampleInterval = 10;  // 10 msec == 100 Hz
unsigned long  tNow, t;

void setup() {
  Serial.begin(115200);
  pinMode(13, OUTPUT);
  tNow = sampleInterval;
}

void runMyCode()
{
  delay(3); // to simulate that code takes time
}

loop() {
	
  if (tNow <= millis() ) {
    digitalWrite(13, HIGH);
    // too expensive in time ?! Serial.println(millis());
    tNow += sampleInterval;
    runMyCode();
    digitalWrite(13, LOW);
  }
}

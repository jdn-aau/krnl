
// time01  /JDN
const unsigned long sampleInterval = 200;  // 200 msec == 5 Hz
unsigned long  tNow;               

void setup() {
    Serial.begin(115200);
  pinMode(13, OUTPUT);
  tNow = sampleInterval; 
}

void runMyCode()
{
  digitalWrite(13, !digitalRead(13) ); // togle LED
  // LED is on for 200msec and off for 200msec
} 

void loop() {
 
  if (tNow <= millis() ) {
    Serial.println(millis());  // normally dont print hre - taske time
    tNow += sampleInterval;
    runMyCode();
  }
}

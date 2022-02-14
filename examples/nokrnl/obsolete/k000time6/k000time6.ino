// k000time06 /JDN

// no check - no mercy

void setup() {
  Serial.begin(115200); // so high to speed up transmission
}

const unsigned long tPer1 = 300, 
                    tPer2 = 500;  // sampling period
volatile unsigned long nextTimeToRun1 = 0, 
                       nextTimeToRun2 = 0;

void controlCode1()
{
  // your code instead of delay which simulate code execution
  delay(100);
}

void controlCode2()
{
  // your code instead of delay which simulate code execution
  delay(90);
}
 
void loop() {

  if (millis() >= nextTimeToRun1) {
    controlCode1();
    nextTimeToRun1 += tPer1;
  }

  if (millis() >= nextTimeToRun2) {
    controlCode2();
    nextTimeToRun2 += tPer2;
  }
}

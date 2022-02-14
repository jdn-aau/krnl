// k000time07/JDN

// no check - no mercy

void setup() {
  Serial.begin(115200); // so high to speed up transmission
}

const unsigned long tPer1 = 300, 
                    tPer2 = 500;  // sampling period
volatile unsigned long nextTimeToRun1 = 0, 
                       nextTimeToRun2 = 0;

extern unsigned long timer0_overflow_count;
void controlCode1()
{
    Serial.print( millis()); Serial.println(" 1"); // debug
  // your code instead of delay which simulate code execution
  delay(100);
}

void controlCode2()
{
    Serial.print( millis()); Serial.println(" 2"); // debug
  // your code instead of delay which simulate code execution
  delay(90);
}

 
int kicker( void (*f)(), unsigned long *t, unsigned long per)
{
  // is it time to run ?
//  if (millis() >= *t) {
  if (*t < timer0_overflow_count) {
     (*f)();
    *t += per;  // update next time to run
    return 1;  // did run code
  }
  return 0; //did not run code - not time for it
}

void loop() {

  kicker(controlCode1,&nextTimeToRun1, tPer1);
  kicker(controlCode2,&nextTimeToRun2, tPer2);
}

// k000time05 /JDN

// testing if there is suifficient gap to next tim(testCloseToLimit)

void setup() {
  Serial.begin(115200); // so high to speed up transmission
}

volatile unsigned long nextTimeToRun1 = 0, nextTimeToRun2 = 0;

const unsigned long tPer1 = 300, tPer2 = 500;  // sampling period
const unsigned int  tGap1 = 200, tGap2 = 15;   // suifficent time ?

int tLow1 = 10,
    tHigh1 = 150,
    tLow2 = 10,
    tHigh2 = 400;
void controlCode1()
{
  Serial.print(" id 1 - ");
  delay( random(tLow1, tHigh1) ) ; // to mimic code takes time
}

void controlCode2()
{
  Serial.print(" id 2 - ");
  delay( random(tLow2, tHigh2)  ) ; // to mimic code takes time
}

int testCloseToLimit(int id, unsigned int nTime, unsigned long limit)
{
  unsigned int tm;

  tm = millis();

  // did we finish after start time for next sample ?
  if (nTime < tm) {
    Serial.print(" ");
    Serial.print(tm - nTime); Serial.println("  Toolate tomatoo -  ");
    return -1;
  }

  tm  = (nTime - tm);
  Serial.print(" "); Serial.print(tm);

  Serial.print(" "); Serial.print(limit);

  // Did we finish after our safety deadline ?
  if (tm  < limit) {
    Serial.println(" below safety limit  ");
    return 1;
  }
  else  {
    Serial.println(" ok ");
    return 0; // ok
  }
}

int kicker( void (*f)(), unsigned long *t, unsigned long per)
{
  // is it time to run ?
  if (millis() >= *t) {
    Serial.print( millis());
    (*f)();
    *t += per;  // update next time to run
    return 1;  // did run code
  }
  return 0; //did not run code - not time for it
}


void loop() {

  if (kicker(controlCode1, &nextTimeToRun1, tPer1) == 1)
    testCloseToLimit(1, nextTimeToRun1, tGap1);

  if (kicker(controlCode2, &nextTimeToRun2, tPer2) == 1)
    testCloseToLimit(2, nextTimeToRun2, tGap2);
}

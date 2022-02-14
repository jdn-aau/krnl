// k000time02 /JDN

void setup() {
  Serial.begin(115200); // so high to speed up transmission
}

const unsigned long Tper = 100;  // sampling period

int tMin=10, tMax = 95;

volatile unsigned long nextTimeToRun = 0;


void controlCode()
{
  delay( random (tMin, tMax ) ) ; // to mimic code takes time
}

int kicker( void (*f)(), unsigned long *t, unsigned long per)
{
  // is it time to run ?
  if (millis() >= *t) {
    Serial.println( millis() );
    (*f)();
    *t += per;  // update next time to run
    return 1;  // did run code
  }
  return 0; //did not run code - not time for it
}


void loop() {
  kicker(controlCode, &nextTimeToRun, Tper);
}

/*
  9686 - 1 approaching limit - limit 30 actual 1
  9700
  9800
  9900
  10000
  10071 - 1 approaching limit - limit 30 actual 1
  10100
  10200
  10300
  10378 - 1 approaching limit - limit 30 actual 1
  10400
  10500
  10588 - 1 approaching limit - limit 30 actual 1
  10600
  10700
  10787 - 1 approaching limit - limit 30 actual 1
  10800
  10900
  10974 - 1 approaching limit - limit 30 actual 1
  11000
  11100
  11181 - 1 approaching limit - limit 30 actual 1
  11200
  11275 - 1 approaching limit - limit 30 actual 1
  11300
  11400
  11500
  11600
  11672 - 1 approaching limit - limit 30 actual 1
  11700
  11789 - 1 approaching limit - limit 30 actual 1
  11800
  11900
  12000
*/

// time06 /JDN
// do it easy - introducin a simple kicker functino

#define DEBUG

#ifdef DEBUG
#define BUGPRINTLN(x)  Serial.println(x)
#else
#define BUGPRINTLN(x) 
#endif

void setup() {
  Serial.begin(115200); // so high to speed up transmission
}

const unsigned long Tper = 100;  // sampling period

volatile unsigned long nextTimeToRun = 0, timeToDelay;

unsigned long limit = 30;

void controlCode()
{
  delay( random (10, 90 ) ) ; // to mimic code takes time
}

int testCloseToLimit(int id, unsigned int nTime, unsigned long limit)
{
unsigned int gap;
    //Is there less than limit time to next time we will run ?
  if (gap = (nTime - millis())  < limit) {
    Serial.print( millis()); Serial.print(" - ");
    Serial.print(id);
    Serial.print(" approaching limit - limit ");
    Serial.print(limit);
    Serial.print(" actual "); Serial.println(gap);
    return 1;
  }
  else {
    return 0; // ok
  }

}


int kicker( void (*f)(), unsigned long *t, unsigned long per)
{
  // is it time to run ?
  if (millis() >= *t) {
    BUGPRINTLN(millis());
    (*f)();
    *t += per;  // update next time to run
    return 1;  // did run code
  }
  return 0; //did not run code - not time for it
}


void loop() {
  if (kicker(controlCode, &nextTimeToRun, Tper) == 1) {
    testCloseToLimit(1, nextTimeToRun, limit);
  }
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

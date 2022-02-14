// time07 /JDN

// two tasks
// no check - no mercy


const unsigned long tPer1 = 300, tPer2 = 500;	// sampling period
volatile unsigned long nextTimeToRun1 = 0, nextTimeToRun2 = 0;

void
setup ()
{
    // just for debug 
	Serial.begin (115200);	// so high to speed up transmission
}
void
controlCode1 ()
{
    // your code instead of delay which simulate code execution
    delay (100);
}

void
controlCode2 ()
{
    // your code instead of delay which simulate code execution
    delay (90);
}

unsigned long t;
void
loop ()
{

    t = millis ();
	
    if (nextTimeToRun1 <= t)
      {
	  controlCode1 ();
	  nextTimeToRun1 += tPer1;
      }

    if (nextTimeToRun2 <= t)
      {
	  controlCode2 ();
	  nextTimeToRun2 += tPer2;
      }
}

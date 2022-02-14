// no leap milliseconds

void
setup ()
{
    pinMode (13, OUTPUT);
    tNow = sampleInterval;
}

void
runMyCode ()
{
    digitalWrite (13, HIGH);
    delay (3);
    digitalWrite (13, LOW);
}

extern unsigned long timer0_overflow_count;
const unsigned long sampleInterval = 10;	// 10.25  msec !!! not using millis
unsigned long tNow;

void
loop ()
{

    cli ();			// disable interrupt
    t = timer0_overflow_count;
    sei ();			// enable ...

    if (tNow <= t)
      {
	  // old: if (tNow < millis() ) {
	  tNow += sampleInterval;

	  digitalWrite (13, HIGH);	//d13 high when we run code 
	  runMyCode ();
	  digitalWrite (13, LOW);	// end of code
      }
}

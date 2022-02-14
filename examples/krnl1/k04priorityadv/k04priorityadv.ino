#include <krnl.h>
// two task loops
//  t1 change to high priority every 50o msec and back to equal pirority
// 500 msec later
// Observe what happens to t1 (blinker) when t1 has highest priority
//
// ? Why does the high priority part seems to last shorter compared to when both are running
// hint: when you are running you use all cpu time bq of k_eat_ticks
//   so when both is running the t2 uses same cpu_time/time as t1 so it takes t1 200 ticks to
// run 100 ticks code ...

// Conclusion: eat time (busy waiting) is not the best thing in the world because you dont get
// anything for the wait time

// hint tick in "Show timestamp in terminal"

// Why is stak usage go down from first loop to the following
// Which task uses most stak ?
// A: Seems to be t1 because ... we are using print !!!!!

// DEBUGGING - remove comments in sewtup and below funcctino loop)
// set LEDs from D8 to ground, D9 to ground and D10 to ground (we have three tasks. two + dummy

struct k_t *p1, *p2;
unsigned char s1[200], s2[200];


// the high priority task
void
t1 ()
{
    unsigned long tm0, tm1;
    int v = 0;

    // t1 change priority every 2 seconds to equal or higher as t2
    // in that way we might observe perf issues

    tm0 = k_millis ();
    while (1)
      {
	  v++;

	  if (v == 40)
	    {			// 2 seconds bbq k_eat_ticks(100) just below

		k_set_prio (9);	// highest prio in this testprogram

		Serial.
		    println
		    ("t1 prio set to 9 - HIGHEST - time for equal periode:");
		tm1 = k_millis ();
		Serial.println (tm1 - tm0);	// you are printing time for the equal periode
		tm0 = tm1;	//both running so double so long time
	    }

	  if (80 <= v)
	    {
		Serial.
		    println
		    ("t1 prio set to 10 - EQUAL - time for high periode(only t1 running:");
		tm1 = k_millis ();
		Serial.println (tm1 - tm0);	// you are printing time for
		tm0 = tm1;	// the high priority periode - only t1 running

		k_set_prio (10);	// equal prio
		v = 0;
	    }

	  k_eat_ticks (100);	// eat time for 1000 ticks so mimic cpu usage == 2000 msec
	  /*
	     Serial.println("stak t1 and t2");  // REMEMBER print is expensive to avoid it if possible
	     Serial.println(k_unused_stak(NULL) );  // how much stak is still unused
	     Serial.println(k_unused_stak(p2) );  // how much stak is still unused
	   */
      }
}


void
t2 ()
{
    int i = 0;
    while (1)
      {
	  digitalWrite (13, HIGH);
	  k_eat_ticks (50);
	  digitalWrite (13, LOW);
	  k_eat_ticks (50);
      }				// do we keep up in pace ?
}


void
setup ()
{
    int res;
    Serial.begin (115200);
    while (!Serial);
    pinMode (13, OUTPUT);

    // remove to set activew - used by debugging
    /*
       for (int i=8; i <13; i++)
       pinMode(i,OUTPUT);
     */

    k_init (2, 0, 0);		// init with space for one task
    //         |--- no of mg Queues (0)
    //       |----- no of semaphores (0)
    //     |------- no of tasks (2)

    // priority low number higher priority than higher number
    p1 = k_crt_task (t1, 10, 200, s1);	// t1 as task, priority 9, 100 B stak
    p2 = k_crt_task (t2, 10, 200, s2);	// t2 as task, priority 10, 100 B stak

    Serial.println ("bef start");
    res = k_start (1);		// 1 milli sec tick speed
    // you will never return from k_start
    Serial.print ("ups an error occured: ");
    Serial.println (res);
    while (1);
}

void
loop ()
{
}

/***** DEBUGGING PART - LED ON 8-12**********/
/************************ DEBUG CALLBACK BREAKOUT PART ****************/
// must be extern C ! its not mandatory to supply with these  functions - only if you need

/* remove to set active
  extern "C" {
  // FOR UNO !!!!
  // See https://jensd.dk/edu/doc/arduino/ards.html for find proper port on MEGA
  // dummy running will set D8 high and rest low
  // t1 running will set D9 high  .....
  // t2 running will set D10 high .....
  // task numbering is in creation order: dummy: 0,  first of yours 1, next 2,...
  void k_breakout() // called every task shift from dispatcher
  {
    unsigned char c;
    // you cant use led13 at same time for blinking ..
    // then you shall say PORTB = (PORTB & 0x200) | (1 << pRun->nr)
    PORTB  = (1 << pRun->nr); // arduino uno !! specific usage of PORTB
  }
  // for a MEGA you have to find another port :-)
  // port K (adc8-15) seems feasible
  // get inspired at http://kom.aau.dk/~jdn/edu/doc/arduino/ards.html
  }

*/

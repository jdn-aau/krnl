//220216
#include <krnl.h>
//
// sleppy waiting k_sleep("sleeptime in msec");
//

// Set t1 priority to  9 == highest priority
// What will happen ? What do you see on the led and the measurements
// REALTIME ?
// change priority of t1 to 9 (higher than  t2) : does it matters ?

// gives a guess how cpu time is used in krnl "dummy time eating"

struct k_t *p1, *p2;

#define SS 140
unsigned char s1[SS], s2[SS];


void t1()
{
	int v;
	
	while (1) {
		k_sleep(2000);  // eat time for 2000 ticks so mimic cpu usage == 2000 msec
	}
}

unsigned long tStart, tStop;
void t2()
{
	int i = 0;
	while (1) {
		Serial.println("start");
		
		tStart = k_millis();
		
		for (int i = 0; i < 10 ; i++) { // 10 loops of 100 milliseconds == 1 second ! ?
			digitalWrite(13, HIGH);
			k_sleep(50);
			digitalWrite(13, LOW);
			k_sleep(50);
		}
		
		tStop = k_millis();
		
		Serial.print( (tStop - tStart) / 1000.0);
		Serial.println("  did it take 1 second ?? :-)?");
	}
}

// we do sleep instead of busy waiting so others can use cpu
// added breakout so led 13 is ON when dummy task is running


void setup()
{
	int res;
	Serial.begin(115200);
	while (! Serial) ;

	pinMode(13, OUTPUT);
	/* using analyser(like analog discovery) D8 will be dummy running, D9 when first created task is running ,... */
	/* remember to change in k_breakout*/
	/* for (int i=8; i< 14; i++) { pinMode(i,OUTPUT); digitalWrite(i,LOW); } */
	
	
	k_init(2, 0, 0); // init with space for one task
	//         |--- no of mg Queues (0)
	//       |----- no of semaphores (0)
	//     |------- no of tasks (2)
	
	// priority low number higher priority than higher number
	p1 = k_crt_task(t1, 10, s1, SS); // t1 as task, priority 9, 100 B stak
	p2 = k_crt_task(t2, 10, s2, SS); // t2 as task, priority 10, 100 B stak
	
	Serial.println("bef start");
	res = k_start(); // 1 milli sec tick speed
	// you will never return from k_start
	Serial.print("ups an error occured: "); Serial.println(res);
	while (1) ;
}

void loop() {}


extern "C" {
	
	void k_breakout() // called every task shift from dispatcher
	{
		
		
		if (pRun->nr == 0)
		{
			PORTB = PORTB | B00100000;  // led13 (bit 5) on let the rest be untouched
		}
		else {
			PORTB = PORTB & B11011111;  // led off uno
		}
	}
	/* using D8-D13 use following instead of teh code above*/
	/* PORTB = (1 << pRun->nr); */
}

// on mega led is on portb bit 7 so use B10000000 and B011111111 above

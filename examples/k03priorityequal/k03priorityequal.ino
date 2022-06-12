//220216
#include <krnl.h>
//
// busy waiting all the way around - like using ya old delay
//

// Set t1 priority to  9 == highest priority
// What will happen ? What do you see on the led and the measurements
// REALTIME ?
//
// gives a guess how cpu time is used in krnl "dummy time eating"

struct k_t *p1, *p2;

#define SS 110
unsigned char s1[SS], s2[SS];


void t1()
{
	int v;
	
	while (1) {
		k_eat_msec(2000);  // eat time for 2000 ticks so mimic cpu usage == 2000 msec
	}
}

unsigned long tStart,tStop;
void t2()
{
	int i = 0;
	while (1) {    
		Serial.println("start");
		tStart = k_millis();
		for (int i=0; i < 10 ; i++) { // 10 loops of 100 milliseconds == 1 second ! ?
			digitalWrite(13, HIGH);
			k_eat_msec(50);
			digitalWrite(13, LOW);
			k_eat_msec(50);
		}
		tStop = k_millis();
		Serial.print((tStop-tStart)/1000.0);
		Serial.print(" sec ");
		Serial.println("Did it take 1 second ???- not - why ");
		// hint task t1 ies eating 50% of cputime
		// the have same priority to you share cpu 50/50
		// so it will approx take 100 msec to execut k_eat_msec(50)
	}
}


void setup()
{
	int res;
	Serial.begin(115200);
	while (! Serial) ;
	pinMode(13, OUTPUT);
	
	k_init(2, 0, 0); // init with space for one task
	//         |--- no of mg Queues (0)
	//       |----- no of semaphores (0)
	//     |------- no of tasks (2)
	
	// priority low number higher priority than higher number
	p1 = k_crt_task(t1, 10,s1,SS); // t1 as task, priority 9, 100 B stak
	p2 = k_crt_task(t2, 10, s2,SS); // t2 as task, priority 10, 100 B stak
	
	Serial.println("bef start");
	res = k_start(); // 1 milli sec tick speed
	// you will never return from k_start
	Serial.print("ups an error occured: "); Serial.println(res);
	while (1) ;
}

void loop() {}

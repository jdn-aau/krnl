//220216
#include <krnl.h>
// one task loops and blink
// k_wait on a semaphore ensures proper timing

// task2 is using partly k_eat_ticks in the loop
// Try to lower tasl2 priority to 11 and see what happens
//?: what do you observe ?
// hint k_eat_ticks mimic cpu usage so if a task uses 2000 ticks cpu time then it should not
// have high(est) priority because lower priority task will then starve: not getting cpu time
//
// If you as starting point gives all same prioriryt there is a high chance taht all will more or less get
// what the need.

// even better is to give high priority task (like sampling and control) a high priority
//
// you can mimic high cpu usage for task1 with insertion of a k_eat_ticks in the code below (is commented out)

// The commented out eat uses 90 ticks so task1 will use 90 out of 100 (the time in loop) of the CPU
// or 90% of cpu power ...
// Beware of high prority task using a lot of cpu power
// life is life

// ?: what will happen if equal priorities and the k_eat_ticks(90)  in task1 ?
// when task2 is u´ni the k_eat_ticks(2000) then it will be in competition with task1
// the have same priority so they will share cpu time (round robbin)
// So it will take 180 ticks for task1 to execute k_eat_ticks(90) and at same time task2 will et 90 ticks
//
// At the same time the krnl signals to semaphore  s1 every 100 ticks. But a loop will take 180 ticks. So
// will will come behind with 80 ticks for every loop(when task2 i in k_eat_ticks(2000)

// you can see than k_Semval will never pass 50. This is because k_crt_sem is called with 50 as max.
//
// ?: can you model it ?


struct k_t *p1, *p2 , *s1;


int pendingWait = 0, loopCnt = 0;

int er2;
void task1()
{
	
	//let krnl send a signal to the semaphore every 100 ticks
	// ensure real time capabilities
	
	k_set_sem_timer(s1, 200);
	
	while (1) {
		
		loopCnt++;
		
		
		// play with second parameter: timeout value 0, -1, pos value
		//  2nd parm, 0: wait forever,
		//            pos num: timeout
		//            neg num : we will not just only eat a signal if one is already present
		// second parameter is timeout:  0: wait forever if needed, -1 : no wait at all, pos int wat up until <value> ticks
		
		
		// Question are we behind the period : timer generated signal to semaphore did occur before we entered the wait call waiting
		// on next tick
		//
		// so return values:
		// 0: we came before signal so we has been sleeping a bit - so we did arrive before the timer generated signal too the semaphore
		//    we are NOT behind
		
		// 1: there was a signal(key) waiting on us - we did eat it - but we arrived after signal -  we were behind ...
		
		// -1: did not get a signal before timeout  (if it was  k_wait(s1,22)  : we will at most 22 ticks before timeout
		
		// -2: we will not wait and there was no signal for us:  k_wait(s1,-1)
		
		
		
		er2 = k_wait(s1, 100);
		
		
		Serial.print(loopCnt);
		
		Serial.print(" - ret from k_wait: "); Serial.println(er2);
		
		
		k_eat_msec(100);
		
	}
}


void task2()
{
	unsigned int loopNrCpy;
	unsigned long tt;
	while (1) {
		
		k_sleep(200);  // just eating time
		k_eat_msec(150); // nasty !!! yo are requesting a heavy load on the processor
	}
}


#define STK 110
char a1[STK], a2[STK];

void setup()
{
	int res;
	Serial.begin(115200);
	while (! Serial) ;
	for (int i = 8 ; i < 14; i++) {
		pinMode(i, OUTPUT);
		digitalWrite(i, LOW);
	}
	
	k_init(2, 1, 0); // init with space for two tasks and one semaphore
	//           |--- no of mg Queues (0)
	//        |----- no of semaphores (0)
	//     |------- no of tasks (2)
	
	// priority low number higher priority than higher number
	p1 = k_crt_task(task1, 10, a1, STK); // task1 as task, priority 10, 100 B stak
	p2 = k_crt_task(task2, 11, a2, STK); // task2 as task, priority 11 == lower than t1, 100 B stak
	
	s1 = k_crt_sem(0, 1); // crt sem
	
	res = k_start(); // 1 milli sec tick speed
	// you will never return from k_start
	Serial.print("ups an error occured: "); Serial.println(res);
	while (1) ;
}

// TRY TO SET PRIORITIES EQUAL
// TRY TO SET task2 higher thatn t1 (priority) stil at right time in task1 ?

void loop() {}


extern "C" {
	void k_breakout() // called every task shift from dispatcher
	{
		PORTB = (1 << pRun->nr);
		// on MEGA use PORTA
	}
}

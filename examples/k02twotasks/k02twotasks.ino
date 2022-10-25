//220216
#include <krnl.h>

const int STKSZ = 200;

struct k_t *pt1,  // pointer to hold reference
  *pt2;           // to taskdescriptor for t1 and t2

char stak1[STKSZ], stak2[STKSZ];

void sampl() {}  // for time being empty

void alg01() {}  // for time being empty

int loopNr = 0;

void aktuer01() {
  Serial.println("aktuer");
  Serial.println(loopNr);
  loopNr = loopNr + 1;
}


volatile int i = 1000;
/**
*  dette er en test
*/
void t1(void) {
  while (1) {
    k_sleep(1000);
    k_eat_msec(500);
    sampl();
    alg01();
    aktuer01();
  }
}



void t2() {
  while (1) {
   // digitalWrite(12, !digitalRead(13));
    k_sleep(300);
  }
}

void setup() {

  Serial.begin(115200);
  pinMode(13, OUTPUT);

  // init krnl so you can create 2 tasks, no semaphores and no message queues
  k_init(2, 0, 0);


  //               |------------ function used for body code for task
  //               |  |--------- priority (lower number= higher prio
  //               |  |   |----- staksize for array s1
  //                         |-- array used for stak
  pt1 = k_crt_task(t1, 11, stak1, STKSZ);
  pt2 = k_crt_task(t2, 10, stak2, STKSZ);


  // NB-1 remember an Arduino has only 2-8 kByte RAM
  // NB-2 remember that stak is used in function calls for
  //  - return address
  //  - registers stakked
  //  - local variabels in a function
  //  So having 200 Bytes of stak excludes a local variable like ...
  //    int arr[400];
  // krnl call k_unused_stak returns size of unused stak
  // Both task has same priority so krnl will shift between the
  // tasks every 10 milli second (speed set in k_start)

  k_start();  // start kernel with tick speed 1 milli seconds
}

void loop() {
  /* loop will never be called */
}



extern "C" {

	void k_breakout() // called every task shift from dispatcher
	{


		if (pRun->nr == 0)  // 0 is dummy task - the eater of excessive CPU when all user tasks are idling
		{
			PORTB = PORTB | B00100000;  // led13 (bit 5) on let the rest be untouched
		}
		else {
			PORTB = PORTB & B11011111;  // led13 off uno
		}
		/* using D8-D13 use following instead of teh code above*/
		/* PORTB = (1 << pRun->nr); */
	}

}

/*
 * README README
 * LED 13 will be ON when dummy is running and OFF when all other tasks are running
 * See k_breakout function above
 * 
 * 0) Cleanup
 * CODE for task t1 and t2 to begin with - here is the code for teh two functions and
 * k_crt_task calls in setup - if you have changed it/played with the code
 * 
 * 
 * 
 * void t1(void)
 * {
 *    while (1) {
 *        
 *        Serial.println(i++);
 *        k_eat_msec(1000); // eat 1000 msec time
 *        //k_sleep(1000);    // sleep for 1000 msec
 *        
 *    }               // lenght of ticks in millisec is specified in
 * }                 // k_start call called from setup - USE 1 msec :-)
 * 
 * void t2(void)
 * {
 *    // and task body for task 2
 *    // runs independent of task t1
 *    while (1) {
 *        k_eat_msec(1000); // simulating algorithms running for 1 sec
 *        //k_sleep(1000);    // sleep for 1 sec
 *    }
 * }
 * 
 *  and in setup:
 *  
 *     pt1 = k_crt_task(t1, 11, STKSZ, stak1);
 *    //pt2 = k_crt_task(t2, 11, STKSZ, stak2);
 *  
 * 
 ********************************
 * 
 * 1) Run code. You will see LED13 is OFF
 * Why ? because t1 is running all the time
 * 
 * 2) remove comment in front of k_sleep(1000) in t1
 * Now LED13 will be ON for 1sec and off for 1sec bq whenin k_sleep t1 is not in active Q
 * and dummy task will run
 * 
 * You will see when LED13 is going OFF there is a short blink on TX LED.
 * This is because t1 is printing an int just after comming back from k_sleep(1000) and looping arounf in the while loop
 * 
 * 3) Go back and comment out k_sleep in t1 and  remove comment in front of pt2 = k_crt_task(t2  - so you will now have two tasks
 * Both are doing a k_eat_msec and t1 is also printing.
 * LED13 will never be ON.
 * TX LED will blink every 2 sec not every 1 sec
 * Why? because we all the time has two task wanting to use the CPU
 * So eating 1000 msec takes 2000 msec because they run 1 msec each and then do round robbin
 * So we are basicly wasting more than 99% of the CPU
 * 
 * 4) In t1 and t2 comment out k_eat_msec and active (remove comment) from k_sleep
 * Now LED 13 will be ON all time (if looking in logic analyser you can see that LED 13 will be OFF very shortly when t1 is printing
 * and when t2 is leaving k_sleep and loop arouond in the while loop and again calling k_sleep)
 * you cant see it on LED13 with you eye bq it is so short in time
 * An now TX LED is blinking every 1 sec so task t1 is doing what we want it to do . print and sleep for 1 sec over and over
 * 
 * Change from k_eat_msec to k_sleep
 * What do we see
 * 
 * 5) Change task priority 
 *  Go back and comment out k_sleep in t1 and  remove comment in front of pt2 = k_vcrt_task(t2  - so you will now have two tasks
 * Both are doing a k_eat_msec and t1 is also printing.
 * Change task t2 priority to 8
 * You will see t1 stops printg == stops running
 * Why ? because t2 has highest priority 
 * 
 * 
 * ON when task t1 i in k_sleep(1000) then no load on CPU for 1sec
 * OFF when task t1 is either printing or eating CPU time in k_eat_msec(1000)
 * A guess i that printing takes less than 1 msec. so looptime will be 1+1000 [msec]
 * 
 * 2) In function setup remove comment in front of creating of task t2  (pt2 = k_crt_task(t2,...
 * 
 */
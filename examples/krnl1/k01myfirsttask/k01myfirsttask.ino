#include <krnl.h>
// one task loops and blink
// k_sleep is used for delay - and ensure no busy waiting
// if delay(...) is used then you use cpu time 

struct k_t *pTask;
unsigned char taskStak[200];

void
t1 ()
{
    int cnt=0;
    while (1)
      {				// your task loop - forever

     Serial.println(cnt++);
	  k_sleep (500);	// dont use delay because it is eating CPU time
	  digitalWrite (13, HIGH);	// slow but ...

	  k_sleep (100);
	  digitalWrite (13, LOW);
      }
}				// if you do not have an endless loop you will be terminated when 

   // leaving this task body function
   // NB you can activate an arduino alike loop in krnl.h approx line 230 by
   // changing #define KRNLZOMBIMODE 1
   // to       #define KRNLZOMBIMODE 0

void
setup ()
{
    int res;
    Serial.begin (115200);
    while (!Serial);
    Serial.println ("k01myfirsttask");

    pinMode (13, OUTPUT);
    k_init (1, 0, 0);		// init with space for one task
    //            ^------- number of message queues
    //         ^--------- number of semaphores
    //      ^------- number of tasks
    // RETURNS 0 is all went ok

    // priority low number higher priority than higher number
    pTask = k_crt_task (t1, 10, 200);	// t1 as task, priority 10, 100 B stak
    //                         ^-- array used for stak
    //                     ^------ stak size in bytes
    //                 ^---------- task priority 1 is highest 
    //              ^------------- function to be used as body for tas
    // return NULL ptr if something went wrong

    res = k_start (1);		// 1 milli sec tick speed ALWAYS Parameter is ignored
    // you will never return from k_start
    // if you come here is it bq krnl has not started due to an error
    // ret vakue is negative and is a count of initalisation errors like
    // no slot for tasks etc
    // see krnl.c approx line ++1270

    Serial.print ("ups an error occured: ");
    Serial.println (res);
    while (1);
}

void
loop ()
{
}				// is NEVER used

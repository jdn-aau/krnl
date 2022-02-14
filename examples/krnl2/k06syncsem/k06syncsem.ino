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

void task1()
{


  k_set_sem_timer(s1, 100); //let krnl send a signal to the semaphore every 100 ticks
  // ensure real time capabilities
  while (1) {

    //   k_eat_ticks(90);
    loopCnt++;
    Serial.print(loopCnt); Serial.print("  ");
    pendingWait = k_semval(s1);  // are we behind? - if semaphore value is above 1 answer is yes!
    Serial.print(pendingWait);
    if (1 < pendingWait) {
      Serial.print(" we are behind (nr of loops*100):aka ticks "); Serial.println(100 * pendingWait);
    }
    else {
      Serial.println(" right on time");
    }

    k_wait(s1, 0); //wait until  a kick comes   parm 0 means waith until signal - so no timeout

    digitalWrite(13, ! digitalRead(13));  // invert LED by reading an write inverse value
  }
}


void task2()
{
  unsigned int loopNrCpy;
  unsigned long tt;
  while (1) {

    k_sleep(1000);  // just eating time
    k_eat_ticks(1000); // nasty !!! yo are requesting a heavy load on the processor
  }
}


char a1[100],a2[100];

void setup()
{
  int res;
  Serial.begin(115200);
  while (! Serial) ;
  pinMode(13, OUTPUT);

  k_init(2, 1, 0); // init with space for two tasks and one semaphore
  //           |--- no of mg Queues (0)
  //        |----- no of semaphores (0)
  //     |------- no of tasks (2)

  // priority low number higher priority than higher number
  p1 = k_crt_task(task1, 10, 100,a1); // task1 as task, priority 10, 100 B stak
  p2 = k_crt_task(task2, 10, 100,a2); // task2 as task, priority 11 == lower than t1, 100 B stak

  s1 = k_crt_sem(0, 50); // crt sem

  res = k_start(1); // 1 milli sec tick speed
  // you will never return from k_start
  Serial.print("ups an error occured: "); Serial.println(res);
  while (1) ;
}

void loop() {}

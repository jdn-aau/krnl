
/* k09
     
     sampling with fixed frequency with timer on sempahore

*/

#include <krnl.h>

#define STKSIZE 100

#define TASKPRIO 10

char stak[STKSIZE], stak2[STKSIZE];
struct k_t * pTask, *pTask2, *sem1;

volatile char reg = 0;

void task()
{
  int res;

  k_set_sem_timer(sem1, 10); // krnl signals every 10 msec
  
  while (1) {
    res = k_wait(sem1, 0); // knock knock at sem1. timeout = 0 means forever
    k_eat_time(3);
  }
}


void setup() {
  // for debugging
  for (int i = 8; i < 14; i++)
    pinMode(i, OUTPUT);

  Serial.begin(9600);

  k_init(1, 1, 0); // 2 task, 1 semaphores, 0 messaegQueues */
  pTask = k_crt_task(task, TASKPRIO, stak, STKSIZE);

  sem1 = k_crt_sem(0, 10); // 1: start value, 10: max value (clipping)
  k_start(1); /* start krnl timer speed 1 milliseconds*/

  Serial.println("If you see this then krnl didnt start :-( ");
}

void loop() {}

/***** DEBUGGING PART - LED ON 8-12**********/
/************************ DEBUG CALLBACK BREAKOUT PART ****************/
// must be extern C ! its not mandatory to supply with these  functions - only if you need

extern "C" {

  // called when a semphore is clipping - nr is id of semaphore and i os nr of times clip has occured
  unsigned char led13;
  ;
  void k_sem_clip(unsigned char nr, int i)
  {
    return;
    if (nr == 2)
      led13 |= 0x20;
  }

  void k_sem_noclip(unsigned char nr)
  {
    return;
    if (nr == 2)
      led13 = 0x00;
  }


  /* void k_send_Q_clip(unsigned char nr, int i) {} */

  // task numbering is in creation order: dummy: 0,  first of yours 1, next 2,...
  void k_breakout() // called every task shift from dispatcher
  {
    unsigned char c;
    PORTB  = (1 << pRun->nr) | reg; // arduino uno !! specific usage of PORTB
  }
  // for a MEGA you have to find another port :-)
  // port K (adc8-15) seems feasible
  // get inspired at http://kom.aau.dk/~jdn/edu/doc/arduino/ards.html
}





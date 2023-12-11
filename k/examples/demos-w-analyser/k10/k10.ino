
/* k10

     sampling with fixed frequency with timer on sempahore
     two tasks !
*/

#include <krnl.h>

#define STKSIZE 200

#define TASKPRIO 10

char stak[STKSIZE], stak2[STKSIZE];
struct k_t * pTask, *pTask2, *sem1, *sem2;

volatile char reg = 0;

void task()
{
  int res;

  k_set_sem_timer(sem1, 15); // krnl signals every 10 msec

  while (1) {
    res = k_wait(sem1, 0); // knock knock at sem1. timeout = 0 means forever
    if (res == 0) {
      DI();
      reg = reg | 0x08; // reset crit reg pin
      PORTB = (1 << pRun->nr) | reg;
      EI();

      k_eat_msec(4);

      DI();
      reg = reg & 0xf7; // reset crit reg pin
      PORTB = (1 << pRun->nr) | reg;
      EI();
    }
  }
}

void task2()
{
  int res;

  k_set_sem_timer(sem2, 10); // krnl signals every 10 msec

  while (1) {
    res = k_wait(sem2, 0); // knock knock at sem1. timeout = 0 means forever
    if (res == 0) {
      DI();  // just for setting bit for critical region for osc
      reg =  reg | 0x10;
      PORTB = (1 << pRun->nr) | reg;
      EI();

      k_eat_msec(3);

      DI();
      reg = reg & 0xef; // reset crit reg pin
      PORTB = (1 << pRun->nr) | reg;
      EI();
    }
  }
}


void setup() {
  // for debugging
  for (int i = 8; i < 14; i++)
    pinMode(i, OUTPUT);

  Serial.begin(9600);

  k_init(2, 2, 0); // 2 task, 1 semaphores, 0 messageQueues */
  pTask = k_crt_task(task, TASKPRIO, stak, STKSIZE);
  pTask2 = k_crt_task(task2, TASKPRIO, stak2, STKSIZE);
  sem1 = k_crt_sem(0, 0); // 1: start value, 10: max value (clipping)
  sem2 = k_crt_sem(0, 0); // 1: start value, 10: max value (clipping)k_start(); /* start krnl timer speed 1 milliseconds*/

  k_start();
  Serial.println("If you see this then krnl didnt start :-( ");
}

void loop() {}

/***** DEBUGGING PART - LED ON 8-12**********/
/************************ DEBUG CALLBACK BREAKOUT PART ****************/
// must be extern C ! its not mandatory to supply with these  functions - only if you need

extern "C" {

  // called when a semphore is clipping - nr is id of semaphore and i os nr of times clip has occured
  unsigned char led13;

  void k_sem_clip(unsigned char nr, int i)
  {
    return;
    if (nr == 2)
      led13 |= 0x20;
  }

  void k_sem_unclip(unsigned char nr)
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





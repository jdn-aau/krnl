  
/* k02
    One task printing own usage of stak
*/

#include <krnl.h>

#define STKSIZE 100

#define TASKPRIO 10

char stak[STKSIZE],stak2[STKSIZE];
struct k_t * pTask,*pTask2;

void task()
{
  int unusedStak;
  while (1) {
   // k_eat_msec(10);  // consume 10 millisec of CPU time
  //  k_sleep(30); // sleep 100 ticks - replacement for delay bq k_seelp releases CPU
  }
}


void task2()
{
  int unusedStak;
  while (1) {
  //  k_eat_msec(3);  // consume 10 millisec of CPU time
  //  k_sleep(20); // sleep 100 ticks - replacement for delay bq k_seelp releases CPU
  }
}


void setup() {
  // for debugging
  for (int i = 8; i < 14; i++)
    pinMode(i, OUTPUT);

  Serial.begin(9600);
  
  k_init(2, 0, 0); // 1 task, 0 semaphores, 0 messaegQueues */
  pTask = k_crt_task(task, TASKPRIO, stak, STKSIZE);
  pTask2 = k_crt_task(task2, TASKPRIO+1, stak2, STKSIZE);
  k_start(); /* start krnl timer speed 1 milliseconds*/

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
    PORTB  = (1 << pRun->nr); // arduino uno !! specific usage of PORTB
  }
  // for a MEGA you have to find another port :-)
  // port K (adc8-15) seems feasible
  // get inspired at http://kom.aau.dk/~jdn/edu/doc/arduino/ards.html
}





/* k03
    two tasks multitasing same priority
*/

#include <krnl.h>

#define STKSIZE 100

#define TASKPRIO 10

char stak[STKSIZE], stak2[STKSIZE];
struct k_t * pTask, *pTask2;

void task()
{
  int unusedStak;
  while (1) {
    k_eat_msec(10);  // consume 10 millisec of CPU time
    k_sleep(30); // sleep 100 ticks - replacement for delay bq k_seelp releases CPU
  }
}


void task2()
{
  int unusedStak;
  while (1) {
    k_eat_msec(3);  // consume 10 millisec of CPU time
    k_sleep(20); // sleep 100 ticks - replacement for delay bq k_seelp releases CPU
  }
}


void setup() {
  // for debugging
  for (int i = 8; i < 14; i++)
    pinMode(i, OUTPUT);

  Serial.begin(9600);

  k_init(2, 0, 0); // 1 task, 0 semaphores, 0 messaegQueues */
  pTask = k_crt_task(task, TASKPRIO, stak, STKSIZE);
  pTask2 = k_crt_task(task2, TASKPRIO, stak2, STKSIZE);
  k_start(); /* start krnl timer speed 1 milliseconds*/

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
    PORTB  = (1 << pRun->nr); // arduino uno !! specific usage of PORTB
  }
  // for a MEGA you have to find another port :-)
  // port K (adc8-15) seems feasible
  // get inspired at http://kom.aau.dk/~jdn/edu/doc/arduino/ards.html
}





/* k04
 *  as k03 but now task has higher priority so it can take cpu from task2 when needed :-)
*/

#include <krnl.h>

#define STKSIZE 100

#define TASKPRIO 10

char stak[STKSIZE], stak2[STKSIZE];
struct k_t * pTask, *pTask2;

void task()
{
  int unusedStak;
  while (1) {
    k_eat_msec(10);  // consume 10 millisec of CPU time
    k_sleep(30); // sleep 100 ticks - replacement for delay bq k_seelp releases CPU
  }
}


void task2()
{
  int unusedStak;
  while (1) {
    k_eat_msec(3);  // consume 10 millisec of CPU time
    k_sleep(20); // sleep 100 ticks - replacement for delay bq k_seelp releases CPU
  }
}


void setup() {
  // for debugging
  for (int i = 8; i < 14; i++)
    pinMode(i, OUTPUT);

  Serial.begin(9600);

  k_init(2, 0, 0); // 1 task, 0 semaphores, 0 messaegQueues */
  pTask = k_crt_task(task, TASKPRIO, stak, STKSIZE);
  pTask2 = k_crt_task(task2, TASKPRIO+1, stak2, STKSIZE); // +1 == lower priority
  k_start(); /* start krnl timer speed 1 milliseconds*/

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
    PORTB  = (1 << pRun->nr); // arduino uno !! specific usage of PORTB
  }
  // for a MEGA you have to find another port :-)
  // port K (adc8-15) seems feasible
  // get inspired at http://kom.aau.dk/~jdn/edu/doc/arduino/ards.html
}





/* k05
    Critical region by semaphore
    task update data and task2 use them

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
  while (1) {
    res = k_wait(sem1, 0); // knock knock at sem1. timeout = 0 means forever
    
    DI();  // just for setting bit for critical region for osc
    reg =  0x08;
    PORTB = (1 << pRun->nr) | reg;
    EI();

    k_eat_msec(10);  // consume 10 millisec of CPU time

    DI();
    reg = 0; // reset crit reg pin
    PORTB = (1 << pRun->nr) | reg;
    EI();

    k_signal(sem1);

    k_sleep(1);
    k_eat_msec(3);
    k_sleep(30); // sleep 100 ticks - replacement for delay bq k_seelp releases CPU
  }
}

void task2()
{
  int res;
  while (1) {
    res = k_wait(sem1, 0); // knock knock at sem1. timeout = 0 menans forever
    DI();  // just for setting bit for critical region for osc
    reg =  0x10;
    PORTB = (1 << pRun->nr) | reg;
    EI();

    k_eat_msec(3);  // consume 10 millisec of CPU time

    DI();
    reg = 0;
    PORTB = (1 << pRun->nr) | reg;
    EI();

    k_signal(sem1);
    k_sleep(1);
    k_eat_msec(1);

    k_sleep(20); // sleep 100 ticks - replacement for delay bq k_seelp releases CPU
  }
}


void setup() {
  // for debugging
  for (int i = 8; i < 14; i++)
    pinMode(i, OUTPUT);

  Serial.begin(9600);

  k_init(2, 2, 0); // 2 task, 1 semaphores, 0 messaegQueues */
  pTask = k_crt_task(task, TASKPRIO, stak, STKSIZE);
  pTask2 = k_crt_task(task2, TASKPRIO, stak2, STKSIZE); // +1 == lower priority

  sem1 = k_crt_sem(1, 10); // 1: start value, 10: max value (clipping)
  k_start(); /* start krnl timer speed 1 milliseconds*/

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




 
/* k06
    Critical region by semaphore
    task update data and task2 use them
    task has higher priority than task 2

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
  while (1) {
    res = k_wait(sem1, 0); // knock knock at sem1. timeout = 0 means forever
    
    DI();  // just for setting bit for critical region for osc
    reg =  0x08;
    PORTB = (1 << pRun->nr) | reg;
    EI();

    k_eat_msec(10);  // consume 10 millisec of CPU time

    DI();
    reg = 0; // reset crit reg pin
    PORTB = (1 << pRun->nr) | reg;
    EI();

    k_signal(sem1);

    k_sleep(1);
    k_eat_msec(3);
    k_sleep(30); // sleep 100 ticks - replacement for delay bq k_seelp releases CPU
  }
}

void task2()  // lower prority than task above
{
  int res;
  while (1) {
    res = k_wait(sem1, 0); // knock knock at sem1. timeout = 0 menans forever
    DI();  // just for setting bit for critical region for osc
    reg =  0x10;
    PORTB = (1 << pRun->nr) | reg;
    EI();

    k_eat_msec(3);  // consume 10 millisec of CPU time

    DI();
    reg = 0;
    PORTB = (1 << pRun->nr) | reg;
    EI();

    k_signal(sem1);
    k_sleep(1);
    k_eat_msec(1);

    k_sleep(20); // sleep 100 ticks - replacement for delay bq k_seelp releases CPU
  }
}


void setup() {
  // for debugging
  for (int i = 8; i < 14; i++)
    pinMode(i, OUTPUT);

  Serial.begin(9600);

  k_init(2, 2, 0); // 2 task, 1 semaphores, 0 messaegQueues */
  pTask = k_crt_task(task, TASKPRIO, stak, STKSIZE);
  pTask2 = k_crt_task(task2, TASKPRIO+1, stak2, STKSIZE); // +1 == lower priority

  sem1 = k_crt_sem(1, 10); // 1: start value, 10: max value (clipping)
  k_start(); /* start krnl timer speed 1 milliseconds*/

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





/* k07
    Critical region by semaphore
    task has higher priority than task 2

*/

#include <krnl.h>

#define STKSIZE 110

#define TASKPRIO 10

char stak[STKSIZE], stak2[STKSIZE];
struct k_t * pTask, *pTask2, *sem1;

volatile char reg = 0;
 
void task()
{
  int res;
  while (1) {
    res = k_wait(sem1, 0); // knock knock at sem1. timeout = 0 means forever
    
    DI();  // just for setting bit for critical region for osc
    reg =  0x08;
    PORTB = (1 << pRun->nr) | reg;
    EI();

    k_eat_msec(10);  // consume 10 millisec of CPU time

    DI();
    reg = 0; // reset crit reg pin
    PORTB = (1 << pRun->nr) | reg;
   
    k_signal(sem1);

    k_sleep(1);
    k_eat_msec(3);
    k_sleep(30); // sleep 100 ticks - replacement for delay bq k_seelp releases CPU
  }
}

void task2()    // same priority
{
  int res;
  while (1) {
    res = k_wait(sem1, 0); // knock knock at sem1. timeout = 0 menans forever
    DI();  // just for setting bit for critical region for osc
    reg =  0x10;
    PORTB = (1 << pRun->nr) | reg;
    EI();

    k_eat_msec(3);  // consume 10 millisec of CPU time

    DI();
    reg = 0;
    PORTB = (1 << pRun->nr) | reg;

    k_signal(sem1);
    k_sleep(1);
    k_eat_msec(1);

    k_sleep(20); // sleep 100 ticks - replacement for delay bq k_seelp releases CPU
  }
}


void setup() {
  // for debugging
  for (int i = 8; i < 14; i++)
    pinMode(i, OUTPUT);

  Serial.begin(9600);

  k_init(2, 2, 0); // 2 task, 1 semaphores, 0 messaegQueues */
  pTask = k_crt_task(task, TASKPRIO, stak, STKSIZE);
  pTask2 = k_crt_task(task2, TASKPRIO, stak2, STKSIZE); // +1 == lower priority

  sem1 = k_crt_sem(1, 10); // 1: start value, 10: max value (clipping)
  k_start(); /* start krnl timer speed 1 milliseconds*/

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





/* k08
    Critical region by semaphore
    task update data and task2 use them
    task has higher priority than task 2

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
  while (1) {
    res = k_wait(sem1, 0); // knock knock at sem1. timeout = 0 means forever
    
    DI();  // just for setting bit for critical region for osc
    reg =  0x08;
    PORTB = (1 << pRun->nr) | reg;
    EI();

    k_eat_msec(10);  // consume 10 millisec of CPU time

    DI();
    reg = 0; // reset crit reg pin
    PORTB = (1 << pRun->nr) | reg;
   
    k_signal(sem1);

    k_sleep(1);
    k_eat_msec(3);
    k_sleep(30); // sleep 100 ticks - replacement for delay bq k_seelp releases CPU
  }
}

void task2()  // lower prority than task above
{
  int res;
  while (1) {
    res = k_wait(sem1, 0); // knock knock at sem1. timeout = 0 menans forever
    DI();  // just for setting bit for critical region for osc
    reg =  0x10;
    PORTB = (1 << pRun->nr) | reg;
    EI();

    k_eat_msec(3);  // consume 10 millisec of CPU time

    DI();
    reg = 0;
    PORTB = (1 << pRun->nr) | reg;

    k_signal(sem1);
    k_sleep(1);
    k_eat_msec(1);

    k_sleep(20); // sleep 100 ticks - replacement for delay bq k_seelp releases CPU
  }
}


void setup() {
  // for debugging
  for (int i = 8; i < 14; i++)
    pinMode(i, OUTPUT);

  Serial.begin(9600);

  k_init(2, 2, 0); // 2 task, 1 semaphores, 0 messaegQueues */
  pTask = k_crt_task(task, TASKPRIO, stak, STKSIZE);
  pTask2 = k_crt_task(task2, TASKPRIO+1, stak2, STKSIZE); // +1 == lower priority

  sem1 = k_crt_sem(1, 10); // 1: start value, 10: max value (clipping)
  k_start(); /* start krnl timer speed 1 milliseconds*/

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
    k_eat_msec(3);
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
  k_start(); /* start krnl timer speed 1 milliseconds*/

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
  pTask2 = k_crt_task(task2, TASKPRIO+1, stak2, STKSIZE);
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




// 220216 -
#include <krnl.h>

 
struct k_t *pTask;  // pointer to task descriptor

// stak to be used by task
#define STK 150
unsigned char taskStak[STK];

void t1 ()
{
	int cnt = 0;
	
	while (1) // your task loop - forever
	{ 
		Serial.println(cnt++);
 		k_sleep(500); 
	}
}              

void setup ()
{
	int res;
	Serial.begin (115200);
	while (!Serial);
	Serial.println ("k01myfirsttask");
	
	pinMode (13, OUTPUT);  // for debug
	
	k_init (1, 0, 0);       // init with space for one task
	//            ^------- number of message queues
	//         ^--------- number of semaphores
	//      ^------- number of tasks
	// RETURNS 0 is all went ok
	
	// priority low number higher priority than higher number
	pTask = k_crt_task (t1, 10, taskStak, STK);   // t1 as task, priority 10, 100 B stak
	//                                ^-- array used for stak
	//                                    ^--- stak size in bytes
	//                      ^---------- task priority 1 is highest
	//                  ^------------- function to be used as body for tas
	// return NULL ptr if something went wrong
	
	res = k_start (1);      // 1 milli sec tick speed ALWAYS Parameter is ignored
	// you will never return from k_start
	// if you come here is it bq krnl has not started due to an error
	// ret vakue is negative and is a count of initalisation errors like
	// no slot for tasks etc
	// see krnl.c approx line ++1270
	
	Serial.print ("ups an error occured: ");
	Serial.println (res);
	while (1);
}

void loop () {}               // is NEVER used

/*  
 * README README 
 * In task t1 we are waiting 500 msec in each loop by calling k_eat_msec
 * This is busy waiting so t1 is running on CPU all time
 * The debug k_break function below turn led13 on when dummy is running (dummey has pRun->nr  == 0)
 * But led 13 never light up - meaning 100% usage of cpu :-(
 * 
 * Changing from k_eat_msec(500)  to k_sleep(500) in t1 we still are waiting 500 msec but now in passive mode
 * So try do remove comment from k_sleep and comment out k_eat_msec
 * Now you will observe that led13 is ON all time. 
 * Why?
 * Because printing an integer takes nearly no time compared to sleeping in 500 msec
 * It you observe by a logic analyser on pin13 you will see it goes low for a very sjort amount of time and
 * then high for 500 msec. So we dont use the CPU for just waiting - 
 * 
 * 2)
 * Try to change k_eat_msec and k_sleep to (and no comment in from of no one ot them
 * k_eat_msec(500);
 * k_sleep(2000);
 * 
 * Then will observe the led13 is off foo 500 msec (when printing and running k_eat_msec) 
 * and led13 is ON for 2000 msec(when we are sleeping in k_sleep(2000)
 * 
 * You can see when your are printing becasue the led marked TX is ON when the uno is doing serial printing
 */

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
	}
}

// on MEGA led13 is on PORTB bit 7.    so use B10000000 instead of B00100000 and    B011111111 instead of B11011111
//220216
#include <krnl.h>

// A small krnl program with two independent tasks
// They run at same priority so krnl will do timeslicing between them
// Watch LED and Serial TX

// NB only one task must use print if you dont protect the serial port by a critical section

// This is a very basic rt progarm, but there might be some hidden "figures"

// ?1: observe how much stak there is used
//

#define STKSZ 200

struct k_t *pt1, // pointer to hold reference
*pt2;          // to taskdescriptor for t1 and t2

unsigned char stak1[STKSZ], stak2[STKSZ];


volatile int i = 0;
void t1(void)
{
	while (1) {
		
		Serial.println(i++);
		k_eat_msec(1000); // eat 1000 msec time
		//k_sleep(1000);    // sleep for 1000 msec
		
	}               // lenght of ticks in millisec is specified in
}                 // k_start call called from setup - USE 1 msec :-)

void t2(void)
{
	// and task body for task 2
	// runs independent of task t1
	while (1) {
		k_eat_msec(1000); // simulating algorithms running for 1 sec
		//k_sleep(1000);    // sleep for 1 sec
	}
}

void setup()
{
	Serial.begin(115200);  // for output from task 1
	while (! Serial) ;
	Serial.println("k02twotasks");
	
	pinMode(13, OUTPUT); // for debug - ON when dummy is running
	
	/* using ananlyser D8 will be dummy running, D9 when first created task is running ,... */
	/* remember to change in k_breakout*/
	/* for (int i=8; i< 14; i++) { pinMode(i,OUTPUT); digitalWrite(i,LOW); } */
	
	
	// init krnl so you can create 2 tasks, no semaphores and no message queues
	k_init(2, 0, 0);
	
	// two task are created
	//               |------------ function used for body code for task
	//               |  |--------- priority (lower number= higher prio
	//               |  |   |----- staksize for array s1
	//                         |-- array used for stak
	pt1 = k_crt_task(t1, 11, stak1, STKSZ);
	//pt2 = k_crt_task(t2, 11, stak2, STKSZ);
	
	
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
	
	k_start(); // start kernel with tick speed 1 milli seconds
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
 //220216
 #include <krnl.h>
 // one task loops and blink
 
 
 // hint using k_sleep instead og k_eat_ticks
 // k_eat_ticks eat your cpu time 
 
 // k_eat_ticks is busy waiting meaning it is using cpu time
 // See in krnl.c approx line 597
 // In contradition k_sleep deactivate the calling task for the time given as parameter
 // and is reactivated (reinserted in the activeQ) after timeout
 
 // ?: how much cputime does t2 used in one loop
 // ?: how much time does t1 use in one loop
 // ?: what will happen if you increase t1 priority to 9 ?
 // ?: why is t2 running in bursts
 // ?: Can you predict the minimum time it takes to k_eat_ticks(2000) if you do not know
 //    what other tasks are doing  (a:no)
 // ?: Can you predict minimu time it takes to k_sleep(2000);   (a: yes)
 
 struct k_t *p1, *p2;
 #define STK 110
 
 unsigned char s1[STK], s2[STK];
 
 void t1()
 {
	 int v;
	 
	 while (1) {
		 k_eat_msec(2000);   // burn of cpu time for 2000 ticks
		 k_sleep(2000);      // sleep - no cpu eat for 2000 ticks
	 }
 }
 
 void t2()
 {
	 int i = 0;
	 while (1) {
		 digitalWrite(13, HIGH);
		 k_sleep(50);
		 digitalWrite(13, LOW);
		 //k_sleep(50);
		 //or
		 k_eat_msec(50);
	 }
	 // do we keep up in pace ?
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
	 p1 = k_crt_task(t1, 10, s1,STK); // t1 as task, priority 9, 100 B stak
	 p2 = k_crt_task(t2, 10, s2,STK); // t2 as task, priority 10, 100 B stak
	 
	 Serial.println("bef start");
	 res = k_start(); // 1 milli sec tick speed
	 // you will never return from k_start
	 Serial.print("ups an error occured: "); Serial.println(res);
	 while (1) ;
 }
 
 void loop() {}
 
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
//220216
#include <krnl.h>

#define STK 110
struct k_t *p1, *p2, *sem1, *sem2;

char st1[STK], st2[STK];

// HERE WE WILL LIGNT ON LED IF OVERFLOW ON sem1 == tperiodic is behind

int x = 0;
int clipp;
void tperiodic()
{
	// simple periodic sampler/controller/...
	k_set_sem_timer(sem1, 200);
	
	while (1) {
		k_wait(sem1, 0);
		
		k_eat_msec(140);  // we eat cpu time
		
		Serial.print(x++);
		Serial.print(" nr of clip on sem1 ");
		
		DI();  // silencio disable interrupt
		clipp = sem1->clip;
		EI();
		
		Serial.println(clipp);
		
		// reset bit 5 LED13 if there has been an overflow on the semaphore
		DI();
		PORTB &= B11011111;  // reset bit 5 == led13 on uno (on mega its bit7) 0x80 or B10000000
		EI();
	}
}


// sem->clip

void tnoise()
{
	while (1) {
		k_eat_msec( 600 ); // we eat between 30 and 200 msec of time
		k_sleep(1000);          // and sleep for 500 msec
	}
}

int err;

void setup()
{
	
	for (int i = 8; i < 14; i++) {
		pinMode(i, OUTPUT);
		digitalWrite(i, LOW);
	}
	
	Serial.begin(115200);
	
	k_init(2, 1, 0); // init with space for three tasks
	
	// priority low number higher priority than higher number
	//Task 1
	p1 = k_crt_task(tperiodic, 10, st1, STK);
	
	//Task 2
	p2 = k_crt_task(tnoise, 11 , st2, STK);
	
	sem1 = k_crt_sem(0, 1);
	
	err = k_start(); // 1 milli sec tick speed
	
	Serial.print("start error ");
	Serial.print(err); // if error is in -1 to -20 its because you need to adjust in k_init
	
}


// LED13 will go ON if overflow på semaphore sem1
// YOU can see nr of clip/saturatino situation in the printotu (terminal)

// FOr tnoise:
// prio = 11  no overflow bq tperodic has highest prio
// prio =10 overflow willl occur bq tnoise is eating 600 msec and therefore share CPU
// 50/50 with tperiodic so eating 140 msec in every 200 msec period can take up to 280 msec(guessing) and therfore its are being
// tnoise prio < 10 makes even worse

// Hack in tperiodic: switch off led 13 by PORTB = .... so we can observe reoccurence of clip




void loop() {}


extern "C" {
	
	void  k_sem_clip(unsigned char nr, int nrClip)
	{
		PORTB = 0x20; //led13
		/* mega PORTB=0x80;  // bit 7 */
	}
}
//220216
#include <krnl.h>

#define STK 110
struct k_t *p1, *p2, *sem1, *sem2;

char st1[STK], st2[STK];

void tperiodic()
{
  // simple periodic sampler/controller/...
  k_set_sem_timer(sem1, 100);

  while (1) {
    k_wait(sem1, 0);
    k_eat_msec(70);  // we eat 70% of cpu time
  }
}


void tnoise()
{
  while (1) {
    k_eat_msec(  random(30, 200) ); // we eat between 30 and 200 msec of time
    k_sleep(500);          // and sleep for 500 msec
  }
}

int err;

void setup()
{
  for (int i = 8; i < 14; i++)   // using PORTB 0..5: d8..d13
    pinMode(i, OUTPUT);
  Serial.begin(115200);

  k_init(2, 1, 0); // init with space for three tasks

  // priority low number higher priority than higher number
  //Task 1
  p1 = k_crt_task(tperiodic, 10, st1, STK); // t1 as task, priority 10, 100 B stak
  //Task 2
  p2 = k_crt_task(tnoise, 11 , st2, STK); // t1 as task, priority 10, 100 B stak
  
  sem1 = k_crt_sem(0,5);
  
  err = k_start(); // 1 milli sec tick speed
  Serial.print("start error ");
  Serial.print(err);

}

void loop() {}


extern "C" {

  void k_breakout() // called every task shift from dispatcher
  {
    PORTB = (1 << pRun->nr);
    // on MEGA use PORTA
  }
}

// D8  ON dummy eating cpu
// D9  ON Task 1 (the first created
// D10 ON  Task 2
// ...
// only one led ON at a time
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
// 220216
#include <krnl.h>


struct k_t *p1, *p2, *s1, *semmutex;

// shared data
struct shDataTp {
	int v;
	int counter;
};

struct shDataTp  sharedData = {0, 0};

volatile int noDataDelivered = 0;


void saveDataInCritRegion(int v)  // save data from critical region protected by a semaphore
{
	k_wait(semmutex, 0);
	{
		sharedData.v = v;
		sharedData.counter ++;
		
	}
	k_signal(semmutex);
}

int saveDataInCritRegionNoWait(int v)  // picjk up data from critical region protected by a semaphore
{
	int x;
	x = k_wait(semmutex, -1);
	
	if (0 <= x) {  // yes   
		sharedData.v = v;
		sharedData.counter ++;
		k_signal(semmutex);
	}
	
	return x;  // 1: ok we had to wait, 1: ok no wait, -1: timeout, -2: no wait and we wil not wait (k_wait(<sem>,-1))
}



struct shDataTp getDataInCritRegion(void)
{
	struct shDataTp tmp;
	k_wait(semmutex, 0);
	
	k_eat_msec(70);  // to block region some time so teh other cant get it at once
	sharedData.counter--;
	tmp = sharedData;
	
	k_signal(semmutex);
	return tmp;
}

// the sampler task


void t1()
{
	int v;
	
	k_set_sem_timer(s1, 100);
	
	while (1) {
		
		k_wait(s1, 0); //wait until  a kick comes
		
		v = analogRead(A0);  // just to get some noise
		if (0 > saveDataInCritRegionNoWait(v)) {  // or saveDataInCritRegion(v);  if you can afford to wait - may become late
			DI();
			noDataDelivered++;
			EI();
		}
	}
}

void t2()
{
	int ll;
	static struct shDataTp v;
	while (1) {
		
		
		k_sleep(300);
		v = getDataInCritRegion();
		Serial.print(v.v);
		Serial.print(" ");
		Serial.print(v.counter);
		DI();
		ll = noDataDelivered;
		EI();
		Serial.print(" nodata dlv ");
		Serial.println(ll);
	}  // do we keep up in pace ?
}

#define STK 150
char a1[STK], a2[STK];

void setup()
{
	int res;
	Serial.begin(115200);
	while (! Serial) ;
	pinMode(13, OUTPUT);
	
	k_init(2, 2, 0); // init with space for one task
	//         |--- no of mg Queues (0)
	//       |----- no of semaphores (0)
	//     |------- no of tasks (2)
	
	// priority low number higher priority than higher number
	p1 = k_crt_task(t1, 10, a1, STK); // t1 as task, priority 10, 100 B stak
	p2 = k_crt_task(t2, 10, a2, STK); // t1 as task, priority 10, 100 B stak
	
	s1 = k_crt_sem(0, 10);
	
	semmutex = k_crt_sem(1, 10); // must be 1 otherwise no one can come inside
	
	Serial.println("bef start");
	res = k_start(); // 1 milli sec tick speed
	// you will never return from k_start
	Serial.print("ups an error occured: "); Serial.println(res);
	while (1) ;
}

void loop() {}
//220116

#include <krnl.h>

#define STK 150
char a1[STK], a2[STK];

#define TASKPRIO 10


struct k_t * pTask1, *pTask2;

struct k_msg_t *msgQ;

int dataBufForMsgQ[10]; // 10 ints,..

struct k_t *timedSem1, *timedSem2, *mutSem;

volatile int t2Missed = 0, t2Hit = 0;;



// mutSem for protecting Serial print

void task1()
{
	int i = 0;
	char res;
	int lI, lII;
	
	k_set_sem_timer(timedSem1, 80);  // 100 tick aka 100 msec realtime signal from krnl
	
	while (1) {
		
		k_wait(timedSem1, 0); // wait untilkick - realtime
		
		i++;
		res = k_send(msgQ, &i);
		
		k_eat_msec(20);
		
		k_wait(mutSem, 0);
		{
			if (0 <= res) {
				Serial.print("1: did deliver "); Serial.println(i);
			}
			else {
				Serial.print("1: no deliver:>>>>>>>>>>>>>>>>>< "); Serial.println(i);
			}
		}
		k_signal(mutSem);
		k_eat_msec(2);
	}
}

void task2()
{
	char res;
	int ii, lostM;
	k_set_sem_timer(timedSem2, 100);  // 100 tick aka 100 msec realtime signal from krnl
	
	while (1) {
		
		k_wait(timedSem2, 0);
		
		k_wait(mutSem, 0);
		Serial.println("2: bef rcv");
		k_signal(mutSem);
		
		
		res = k_receive(msgQ,  &ii, 0, &lostM);
		
		k_wait(mutSem, 0);
		{
			Serial.print("2: received "); Serial.print( ii);
			Serial.print(" lost: "); Serial.println(lostM);
		}
		k_signal(mutSem);
	}
}

void setup() {
	Serial.begin(115200);
	delay(2000);
	
	Serial.println("just bef init part");
	
	k_init(2, 3, 1); // 2 task, 1 semaphores, 0 messaegQueues */
	
	msgQ = k_crt_send_Q (10, sizeof(int),  dataBufForMsgQ);  // 10 elements of size
	
	pTask1 = k_crt_task(task1, 15, a1, STK);
	pTask2 = k_crt_task(task2, 15 , a2, STK);
	
	timedSem1 = k_crt_sem(0, 1); // 1: start value, 10: max value (clipping)
	timedSem2 = k_crt_sem(0, 1); // 1: start value, 10: max value (clipping)
	
	mutSem = k_crt_sem(1, 1); // 1: start value, 10: max value (clipping)
	
	Serial.println("just bef k_start");
	
	k_start(); /* start krnl timer speed 1 milliseconds*/
	
	Serial.println("If you see this then krnl didnt start :-( ");
}

void loop() {}
#include <krnl.h>

// LED13 on i dummy is runningg

struct k_t *p1, *p2, *p3;

#define STK 110
char st1[STK], st2[STK], st3[STK];

// LED 13 not used from user space bq we use led for indicating dummy i srunning
// when dummy is running it indicates enough cpu power
// if dummy is not running for a longer time you may have problems ....
void t1()
{
  while (1) {

    Serial.print(pRun->nr);
    Serial.println(" I am running");
    k_eat_msec_time(500);  // you are running
    Serial.println("dummy is now running");
    k_sleep(2000);     // you are NOT running so dummmy is running
  }
}

void setup()
{

  Serial.begin(115200);
  pinMode(13, OUTPUT);  // for debug

  k_init(3, 0, 0); // init with space for three tasks

  // priority low number higher priority than higher number
  p1 = k_crt_task(t1, 10, st1, STK); // t1 as task, priority 10, 100 B stak
  p2 = k_crt_task(t1, 11, st2, STK); // t1 as task, priority 10, 100 B stak
//   p3 = k_crt_task(t1, 12, st3, STK); // t1 as task, priority 10, 100 B stak

  k_start(); // 1 milli sec tick speed
}

void loop() {}

/*
  #if defined(__AVR_ATmega168__) || defined(__AVR_ATmega328P__) || defined(__AVR_ATmega328__)
  for (int i = 8; i < 8+6; i++)
    pinMode(i, OUTPUT);  // PORTB

  #elif defined (__AVR_ATmega2560__) || defined (__AVR_ATmega1280__) || defined(__AVR_ATmega2561__)
  for (int i = 22; i < 22+6; i++)
    pinMode(i, OUTPUT);  // PORTA
  #endif

*/
extern "C" {

  void k_breakout() // called every task shift from dispatcher
  {
    //     https://arduino.stackexchange.com/questions/19892/list-of-arduino-board-preprocessor-defines

    // FOR UNO

#if defined (ARDUINO_AVR_UNO)

    if (pRun->nr == 0)
    {
      PORTB = PORTB | B00100000;  // led13 (bit 5) on let the rest be untouched
    }
    else {
      PORTB = PORTB & B11011111;  // led off uno
    }
#endif

#if ( defined (ARDUINO_AVR_MEGA) || defined(ARDUINO_AVR_MEGA2560) )
    if (pRun->nr == 0)
    {
      PORTB = PORTB | B10000000;  // led13 (bit 7) on let the rest be untouched
    }
    else {
      PORTB = PORTB & B01111111;  // led off  mega
    }
#endif

  }
}


/* or just LED13 if dummy ...

  YOu can test for board by #ifdef ARDUINO_AVR_UNO  etc
  see list below

  AVR_ADK
  AVR_BT
  AVR_DUEMILANOVE
  AVR_ESPLORA
  AVR_ETHERNET
  AVR_FIO
  AVR_GEMMA
  AVR_LEONARDO
  AVR_LILYPAD
  AVR_LILYPAD_USB
  AVR_MEGA
  AVR_MEGA2560
  AVR_MICRO
  AVR_MINI
  AVR_NANO
  AVR_NG
  AVR_PRO
  AVR_ROBOT_CONTROL
  AVR_ROBOT_MOTOR
  AVR_UNO
  AVR_YUN

    both: pinMode(13,OUTPUT);
    uno mega

    if (pRun->nr == 0)
    PORTB = PORTB | B00100000;  // led on uno
                  | B10000000;  // mega

    else

    PORTB = PORTB & B11011111;  // led off uno
                  & B01111111;   / mega

    mega

*/

/*

   Suggested PORTS TO USE

  UNO
  pin port
  8   PB0
  9   PB1
  10  PB2
  11  PB3
  12  PB4
  13  PB5  LED13
  PB6 and 7  etc not to be used !


  MEGA
  pin port
  78  PA0
  77  PA1
  76  PA2
  75  PA3
  74  PA4
  73  PA5
  72  PA6
  71  PA7
  13  PB7 LED13

  MICRO
  8  PB4
  9  PB5
  10 PB6
  11 PB7
  13 PC7 LED13

  NANO
  D8  PD0
  D9  PD1
  D2  PD2
  D3  PD3
  D4  PD4
  D5  PD5
  D6  PD6
  D7  PD7
  13  PB5  LED13

  PRO MINI
  2  PD2
  3  PD3
  4  PD4
  5  PD5
  6  PD6
  13 PB5 LED13

 * */
//220216
#include <krnl.h>

#define STK 110
struct k_t *p1, *p2, *p3;
char st1[STK], st2[STK], st3[STK];

void tgeneric()
{
  while (1) {
    k_eat_msec(pRun->nr * 100);  // tasks has 1,2,3 so mul by 100 gives 100msec,...
    k_sleep(1000);
  }
}

void setup()
{

  Serial.begin(115200);

  for (int i = 8; i < 14; i++)   // using PORTB 0..5: d8..d13
    pinMode(i, OUTPUT);
  /* MEGA  PORTA (int i=22; i < 30; i++) pinMode(i,OUTPUT); */ 

  k_init(3, 0, 0); // init with space for three tasks2

  // priority low number higher priority than higher number
  //Task 1
  p1 = k_crt_task(tgeneric, 10, st1, STK); // t1 as task, priority 10, 100 B stak
  //Task 2
  p2 = k_crt_task(tgeneric, 11, st2,  STK); // t1 as task, priority 10, 100 B stak
  //Task 3
  p3 = k_crt_task(tgeneric, 12, st3, STK); // t1 as task, priority 10, 100 B stak

  k_start(); // 1 milli sec tick speed
}

void loop() {}

extern "C" {

  void k_breakout() // called every task shift from dispatcher
  {
    PORTB = (1 << pRun->nr);   // D8: dummy running, D9 first task created running,...
    // on MEGA use PORTA
  }
}

// D8  ON dummy eating cpu
// D9  ON Task 1 (the first created
// D10 ON  Task 2
// ...
// only one led ON at a time
// dont use led 13 for othe stuff bq it s cleared her
// can be fixed - just ask me

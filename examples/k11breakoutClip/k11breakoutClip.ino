
/* (C) 2012,2013,2014,2015                           *
*                                                    *
  Jens Dalsgaard Nielsen <jdn@es.aau.dk>
  http://es.aau.dk/staff/jdn
  Section of Automation & Control
  Aalborg University,
  Denmark
*                                                    *
  "THE BEER-WARE LICENSE" (frit efter PHK)
  <jdn@es.aau.dk> wrote this file. As long as you
  retain this notice you can do whatever you want
  with this stuff. If we meet some day, and you think
  this stuff is worth it ...
   you can buy me a beer in return :-)
  or if you are real happy then ...
  single malt will be well received :-)
*                                                    *
  Use it at your own risk - no warranty
*                                                    *
  tested with duemilanove w/328, uno R3,
  seeduino 1280 and mega2560
*****************************************************/

/*
  Demonstrates krnl usage with semaphore clip detection and scheduler breakout
  3 tasks - priority low,, mediaum and high
  skeleton: cyclic execxution on timed semaphore and fixed conumation of time
  Typical RMA setup

  Semaphores are created with limit -1 which means that a signal will only be accepted if a
  Task is waiting at the semahore (which is indicated by -1)

  Two user supplied break out functions:
  - k_sem_clip - called when semaphore is clippep
  - k_break_out - called by scheduler every task shift
  Dig out 81-3 is initialized an LEDs attached
  LED 8 on . dummy is running
  LED 9 on low prio is running  (got nr 1 bq it is created first)
  LED 10 on med prio running    ( got 2 bq crt as nr 2 ...)
  LED 11 on high prio is running (etc)
  LED 13 on semaphore clip == low prio is lagging behind

  You can play with RMA
*/

#include <krnl.h>

struct k_t * pt1, *pt2, *pt3,
         *sem1, *sem2, *sem3;

#define TICKSPEED 1

#define STK 100
char st1[STK], st2[STK], st3[STK];

volatile unsigned char led13 = 0;

void highprio()
{
  while (1) {
    k_wait(sem1, 0);
    k_eat_time(100);  // lets eat 200 milliseconds
  }
}


void medprio()
{
  while (1) {
    k_wait(sem2, 0);
    k_eat_time(800);  // lets eat 5 milliseconds
  }
}

void lowprio()
{
  while (1) {
    k_wait(sem3, 0);
    k_eat_time(1480); // approx max value before lagging behind
    //led13 = 0; // reset
  }
}

void initLED()
{
  int i;
  // attach 6 LEDS to pin 8..13 for light show
  for (i = 8; i <= 13; i++) {  // init PORTB for driving LEDs
    pinMode(i, OUTPUT);
    digitalWrite(i, LOW);
  }
}

void initTasks()
{
  pt3 = k_crt_task(lowprio, 30, st3, STK); //declared first so it get tasknr 1
  pt2 = k_crt_task(medprio, 20, st2, STK); // 2 ...
  pt1 = k_crt_task(highprio, 10, st1, STK); // 3
}

void initSemaphores()
{
  sem3 = k_crt_sem(0, 1); // get nr 3 - ...   used to test if task i ready for next cycle or if its lagging behind
  sem2 = k_crt_sem(0, 0); // get nr 2 - ...  meaning at elast one task has to wait at the semaphore
  sem1 = k_crt_sem(0, 0); // get nr 1 -  0 is highest acceptable value on a semaphore ...

  k_set_sem_timer(sem3, 3000 / TICKSPEED);
  k_set_sem_timer(sem2, 1000 / TICKSPEED );
  k_set_sem_timer(sem1, 500 / TICKSPEED);
}

void setup()
{
  initLED();
  // LED13 is pin5 at PORTB
  k_init(3, 3, 0);   // dummy get tasknr 0

  initTasks();
  initSemaphores();

  k_start(TICKSPEED); // 10 millisecond heartbeat
}

void loop() {}


/************************ DEBUG CALLBACK BREAKOUT PART ****************/
// must be extern C ! its not mandatory to supply with these  functions - only if you need

extern "C" {

  // called when a semphore is clipping - nr is id of semaphore and i os nr of times clip has occured

  void k_sem_clip(unsigned char nr, int i)
  {
    if (nr == 2)
      led13 |= 0x20;
  }

  void k_sem_noclip(unsigned char nr)
  {
    if (nr == 2)
      led13 = 0x00;
  }


  /* void k_send_Q_clip(unsigned char nr, int i) {} */

  void k_breakout() // called every task shift from dispatcher
  {
    unsigned char c;
    PORTB  = (1 << pRun->nr) | led13; // arduino uno !! specific usage of PORTB
  }
  // for a MEGA you have to find another port :-)
  // port K (adc8-15) seems feasible
}


 

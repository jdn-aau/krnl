/* (C) 2012,2013,2014,2015                           *
*                                                    *
* Jens Dalsgaard Nielsen <jdn@es.aau.dk>             *
* http://es.aau.dk/staff/jdn                         *
* Section of Automation & Control                    *
* Aalborg University,                                *
* Denmark                                            *
*                                                    *
* "THE BEER-WARE LICENSE" (frit efter PHK)           *
* <jdn@es.aau.dk> wrote this file. As long as you    *
* retain this notice you can do whatever you want    *
* with this stuff. If we meet some day, and you think*
* this stuff is worth it ...                         *
*  you can buy me a beer in return :-)               *
* or if you are real happy then ...                  *
* single malt will be well received :-)              *
*                                                    *
* Use it at your own risk - no warranty              *
*                                                    *
* tested with duemilanove w/328, uno R3,             *
* seeduino 1280 and mega2560                         *
*****************************************************/

/*
Demonstrates krnl usage with semaphore clip detection and scheduler breakout
*/

#include <krnl.h>

struct k_t * pt1, *pt2, *pt3,
    *sem1, *sem2, *sem3;

#define TICKSPEED 10

#define STK 100
char st1[STK], st2[STK], st3[STK];

volatile unsigned char led13 = 0;

void highprio()
{
  int clip;
  Serial.println("hello");
  k_set_sem_timer(sem1, 200 / TICKSPEED); // 200 millisek
  while (1) {
    k_wait(sem1, 0);
    k_eat_time(50);  // lets eat 200 milliseconds
  }
}


void medprio()
{
  int clip;
  k_set_sem_timer(sem2, 1000 / TICKSPEED); //
  while (1) {
    k_wait(sem2, 0);
    k_eat_time(200);  // lets eat 5 milliseconds
  }
}

void lowprio()
{
  int clip;
  k_set_sem_timer(sem3, 3000 / TICKSPEED); //
  while (1) {
    k_wait(sem3, 0);
    k_eat_time(2500);
    led13 = 0; // reset
  }
}


void setup()
{
  {
    int i;
    for (i = 8; i <= 13; i++) {  // init PORTB for driving LEDs
      pinMode(i, OUTPUT);
      digitalWrite(i, LOW);
    }
  }
  // LED13 is pin5 at PORTB
  k_init(4, 3, 0);   // dummy get tasknr 0

  pt3 = k_crt_task(lowprio, 30, st3, STK); //declared first so it get tasknr 1
  pt2 = k_crt_task(medprio, 20, st2, STK); // 2 ...
  pt1 = k_crt_task(highprio, 10, st1, STK); // 3

  sem1 = k_crt_sem(0, -1); // so 1 is highest value
  sem2 = k_crt_sem(0, -1); // so 1 is highest value
  sem3 = k_crt_sem(0, 0); // so 1 is highest value
  k_start(10); // 10 millisecond heartbeat
}

void loop() {}

// must be extern C ! its not mandatory to supply with these  functions - only if you need

extern "C" {

  // called when a semphore is clipping - nr is id of semaphore and i os nr of times clip has occured
  void k_sem_clip(unsigned char nr, int i)
  {
    led13 = 0x20; // LED13 is bit 5 so sem overflow will in fct below switch on LED13
  }

  /* void k_send_Q_clip(unsigned char nr, int i) {} */

  void k_breakout() // called every task shift from dispatcher
  {
    unsigned char c;
    PORTB  = (1 << pRun->nr) | led13; // arduino uno !! specific usage of PORTB
  }

}





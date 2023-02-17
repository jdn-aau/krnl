// 220216 -
#include <krnl.h>

struct k_t *pTask; // pointer to task descriptor

// stak to be used by task
#define STK 150
unsigned char taskStak[STK];

void t1() {
  int cnt = 0;


  // 1 Hz led blink
// so loop takes 500 msec + code time
  while (1) // your task loop - forever
  {
    digitalWrite(13, ! digitalRead(13));
    Serial.println(cnt++);
    k_sleep(500);
  }
}

void setup() {
  int res;
  Serial.begin(115200);
  while (!Serial);

  Serial.println("k01myfirsttask");

  pinMode(13, OUTPUT); // for debug

  k_init(1, 0, 0); // init with space for one task
  //            ^------- number of message queues
  //         ^--------- number of semaphores
  //      ^------- number of tasks
  // RETURNS 0 is all went ok

  // priority low number higher priority than higher number
  pTask =
      k_crt_task(t1, 10, taskStak, STK); // t1 as task, priority 10, 100 B stak
  //                                ^-- array used for stak
  //                                    ^--- stak size in bytes
  //                      ^---------- task priority 1 is highest
  //                  ^------------- function to be used as body for tas
  // return NULL ptr if something went wrong

  res = k_start(); // 1 milli sec tick
  // you will never return from k_start
  // if you come here is it bq krnl has not started due to an error
  // ret vakue is negative and is a count of initalisation errors like
  // no slot for tasks etc
  // see krnl.c approx line ++1270

  Serial.print("ups an error occured: ");
  Serial.println(res);
  while (1)
    ;
}

void loop() {} // is NEVER used

/*
 * README README
 * In task t1 we are waiting 500 msec in each loop by calling k_eat_msec
 * This is busy waiting so t1 is running on CPU all time
 * The debug k_break function below turn led13 on when dummy is running (dummey
 * has pRun->nr  == 0) But led 13 never light up - meaning 100% usage of cpu :-(
 *
 * Changing from k_eat_msec(500)  to k_sleep(500) in t1 we still are waiting 500
 * msec but now in passive mode So try do remove comment from k_sleep and
 * comment out k_eat_msec Now you will observe that led13 is ON all time. Why?
 * Because printing an integer takes nearly no time compared to sleeping in 500
 * msec It you observe by a logic analyser on pin13 you will see it goes low for
 * a very sjort amount of time and then high for 500 msec. So we dont use the
 * CPU for just waiting -
 *
 * 2)
 * Try to change k_eat_msec and k_sleep to (and no comment in from of no one ot
 * them k_eat_msec(500); k_sleep(2000);
 *
 * Then will observe the led13 is off foo 500 msec (when printing and running
 * k_eat_msec) and led13 is ON for 2000 msec(when we are sleeping in
 * k_sleep(2000)
 *
 * You can see when your are printing becasue the led marked TX is ON when the
 * uno is doing serial printing
 */
 
 /***

extern "C" {

void k_breakout(void) // called every task shift from dispatcher
{

  if (pRun->nr == 0) // 0 is dummy task - the eater of excessive CPU when all
                     // user tasks are idling
  {
    PORTB = PORTB | B00100000; // led13 (bit 5) on let the rest be untouched
  } else {
    PORTB = PORTB & B11011111; // led13 off uno
  }
}
}

// on MEGA led13 is on PORTB bit 7.    so use B10000000 instead of B00100000 and
// B011111111 instead of B11011111

*/

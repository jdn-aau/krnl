/* k01
    One task printing own usage of stak
*/

#include <krnl.h>

#define STKSIZE 100

#define TASKPRIO 10

char stak[STKSIZE];
struct k_t * pStak;

void task()
{
char flipflop=0;
  while (1) {
    k_eat_time(10);  // consume 10 millisec of CPU time
    k_sleep(300); // sleep 100 ticks - replacement for delay bq k_seelp releases CPU
  if (flipflop) {
   flipflop = 0;
   digitalWrite(13,HIGH);
  }
  else {
      flipflop = 1;
      digitalWrite(13,LOW);
  }
}
}

void setup() {

  Serial.begin(9600); // if you dont use serial port then avoid initializing bq it takse memory

  pinMode(13,OUTPUT);

  k_init(1, 0, 0); // 1 task, 0 semaphores, 0 messaegQueues */

  pStak = k_crt_task(task, TASKPRIO, stak, STKSIZE);

  k_start(1); /* start krnl timer speed 1 milliseconds*/

  Serial.println("If you see this then krnl didnt start :-( ");
}

void loop() {}

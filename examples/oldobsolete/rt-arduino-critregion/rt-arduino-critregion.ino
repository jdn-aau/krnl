/* critical region - by semaphores - aka mutex
    JDN
 *  */

#include <krnl.h>

#define STK 150

#define TASKPRIO 10


struct k_t * pTask1, *pTask2;

struct k_t *mutSem;

struct k_t *timedSem1, *timedSem2;

volatile int t2Missed = 0, t2Hit = 0;;

void task1()
{
  int lI, lII;
  k_set_sem_timer(timedSem1, 100);  // 100 tick aka 100 msec realtime signal from krnl

  while (1) {
    k_wait(timedSem1, 0); // wait until kick
    DI();  // get it atomic !
    lI = t2Missed;
    lII = t2Hit;
    EI();

    k_wait(mutSem, 0); // wait until access

    k_eat_ticks(50);  // simulate processing
    Serial.print("1:yes "); Serial.print(" hit: ");
    Serial.print(lII);
    Serial.print(" nohit: "); Serial.println(lI);
    k_signal(mutSem);
    k_eat_ticks(2);
  }
}


void task2()
{
  k_set_sem_timer(timedSem2, 10);  // 100 tick aka 100 msec realtime signal from krnl

  while (1) {
    k_wait(timedSem2, 0) ; // wait until kick
    if (0 <= k_wait(mutSem, 10) ) { // wait until access or 10  ticks timeout return 1: ok - no wait 0: ok wait, -1: timeout

      k_eat_ticks(5);
      Serial.println("2:yes");
      t2Hit++;
      k_signal(mutSem);

    }
    else
    {
      DI();  // update atomic
      t2Missed++;
      EI();
    }
    k_eat_ticks(2);
  }
}


void setup() {
  // for debugging - only on uno eq- If Mega etc use PORTA(which is pn 22,23,...)
  // see http://kom.aau.dk/~jdn/edu/doc/arduino/mega.png

  Serial.begin(115200);
  delay(2000);

  Serial.println("just bef init part");

  k_init(2, 3, 0); // 2 task, 1 semaphores, 0 messaegQueues */

  pTask1 = k_crt_task(task1, 15,  STK);
  pTask2 = k_crt_task(task2, 15 ,  STK);

  timedSem1 = k_crt_sem(0, 1); // 1: start value, 10: max value (clipping)
  timedSem2 = k_crt_sem(0, 1); // 1: start value, 10: max value (clipping)
  mutSem = k_crt_sem(1, 1); // 1: start value, 10: max value (clipping)

  Serial.println("just bef k_start");

  k_start(1); /* start krnl timer speed 1 milliseconds*/

  Serial.println("If you see this then krnl didnt start :-( ");
}

void loop() {}

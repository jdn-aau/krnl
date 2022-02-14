/* critical region - by semaphores - aka mutex
    JDN
 *  */

#include <krnl.h>

#define STK 150

#define TASKPRIO 10


struct k_t * pTask1, *pTask2;

struct k_msg_t *msgQ;

int dataBufForMsgQ[10]; // 10 ints,..

struct k_t *timedSem1, *timedSem2, *mutSem;

volatile int t2Missed = 0, t2Hit = 0;;

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

    k_eat_ticks(20);

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
    k_eat_ticks(2);
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
    {
      Serial.println("2: bef rcv");
    }

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

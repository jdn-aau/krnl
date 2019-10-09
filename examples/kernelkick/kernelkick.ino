#include <krnl.h>

// a simple KeRNeL arduino program
// one task waits on sync on a semaphore
// another task do periodic  loop by a semaphore and timer facilility
// and sync the first task


struct k_t *syncSem, *loopSem, *ptask1, *ptask2;

char stak1[100], stak2[100];

void task1Body(void)
{
  while (1) {
    k_wait(syncSem,0);
    Serial.println("kick");
  }
}

void task2Body(void) {
  k_set_sem_timer(loopSem,20);
  while (1) {
    
    k_wait(loopSem,0);
    k_signal(syncSem);
  }
}

void setup()
{
  Serial.begin(9600);
  k_init(5,5,5);
  loopSem = k_crt_sem(0,10); // you should chk if return value = NULL == error
  syncSem = k_crt_sem(0,10);
  ptask1 = k_crt_task(task1Body,10,stak1,100);
  ptask2 = k_crt_task(task2Body,29,stak2,100); // 20 is lower prority than 10
  Serial.println("just bef start");
  k_start(10); // 10 msec tick
  Serial.println("error - you should not com here");
}
void loop()
{
 // you will never come here 
}

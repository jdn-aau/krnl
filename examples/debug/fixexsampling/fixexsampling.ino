
#include <krnl.h>
struct k_t *pSem, *pTask;

#define STK 100
char stak[STK];

void doSamplingAlgorithmsAndControl() {
  static boolean light = false;
  k_eat_time(5); // to simulate code running

  if (light) { // debug :-) blinking
    light = false;
    digitalWrite(13, HIGH);

  }
  else {
    light = true;
    digitalWrite(13, LOW);
  }
}

void task()
{

  k_set_sem_timer(pSem, 1000); // let OS send a signal to semaphore every 1000th kernel timer tick

  while (1) {
    k_wait(pSem, 0);
    doSamplingAlgorithmsAndControl();
  }
}

void setup()
{
  pinMode(13, OUTPUT); // for debugging only
  k_init(1, 1, 0); // 1 task, 1 semaphore, 0 message boxes
  pSem = k_crt_sem(0, 10); // 0 start value missing max value for clipping
  pTask = k_crt_task(task, 10, stak, STK);
  k_start(1);
}

void loop() {}


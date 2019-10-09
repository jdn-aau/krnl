#include <krnl.h>

struct k_t *sem, *mutexSem, *pT1, *pT2;

#define STK 100

char stakH[STK], stakL[STK];


// HOW TO DEBUG ??? :-)
struct dataTp {
  int i, j, k;
};

struct dataTp data;


void initData() // must be called bef krnl is started
{
  data.i = data.j = data.k = 33;
}

void lowTask()
{
  int a, b, c;
  while (1) {

    k_wait(mutexSem, 0);   // wait forever if needed
    // >>>>>>> enter critical region
    data.i++ ; data.j--; data.k; data.k = data.i + data.j;
    // <<<<<<< leaving critical region
    k_signal(mutexSem);

    k_sleep(100);
  }
}

void controlTask()
{
  int mya, myb, myc;

  k_set_sem_timer(sem, 10); // let krnl send a signal to semaphore every 10th kernel timer tick

  while (1) {
    k_wait(sem, 0); // wait forever if ... for sampling

    if (0 <=   k_wait(mutexSem, -1) ) // = so no wait  - might be dangerous bq you might miss region 
    {
      // >>>>>>> enter critical region
      mya = data.i; myb = data.j; myc = data.k;
      // <<<<<<< leaving critical region
      k_signal(mutexSem);
    }


    // do control stuff here with new data :-)
    k_eat_time(5); // fake for doSamplingAlgorithmsAndControl();
  }
}
void setup()
{
  initData();

  k_init(2, 2, 0); // 2 tasks 2 semaphores
  sem = k_crt_sem(0, 10);
  mutexSem = k_crt_sem(1, 10);

  pT1 = k_crt_task(controlTask, 10, stakH, STK);
  pT2 = k_crt_task(lowTask, 20, stakL, STK);

  k_start(1);

}

void loop() {}



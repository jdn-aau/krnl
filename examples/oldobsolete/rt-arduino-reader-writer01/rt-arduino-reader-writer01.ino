#include <krnl.h>

//Jens s2021

// freertos considerations

struct rdWrLockTp {
  k_t *rdSem, *wrSem;
  int rdCount;
};


struct rdWrLockTp rdWrLock1;

volatile int rdInsideFortest=0;

// RD WR LOCK BY SEMAPHORE

int rdWrLockInit(struct rdWrLockTp *lock)
{
  lock->rdSem = k_crt_sem(1, 10);
  lock->wrSem = k_crt_sem(1, 10);
  lock->rdCount = 0;
  return 0; // ok - no real chk
}

void rdEnter(struct rdWrLockTp *lock)
{
  k_wait(lock->rdSem, 0);
  lock->rdCount++;
  if (lock->rdCount == 1) {
    k_wait(lock->wrSem, 0); // wait until writer is finished
  }
  rdInsideFortest++;
  k_signal(lock->rdSem);
}

void wrEnter(struct rdWrLockTp *lock)
{
  k_wait(lock->rdSem, 0);
  k_signal(lock->rdSem);
  k_wait(lock->wrSem, 0);
}

void wrLeave(struct rdWrLockTp *lock)
{
  k_wait(lock->rdSem, 0);
  k_signal(lock->rdSem);
  k_signal(lock->wrSem);
}

void rdLeave(struct rdWrLockTp *lock)
{
  k_wait(lock->rdSem, 0);
  lock->rdCount--;
  rdInsideFortest--;
  if (lock->rdCount == 0) {
    k_signal(lock->wrSem); // wait until writer is finished
  }
  k_signal(lock->rdSem);
}


void wr(void)
{
  int i = 0;
  while (1) {
    k_eat_ticks( random(100, 1000));
    wrEnter(&rdWrLock1);
    Serial.println("wr in");
    k_eat_ticks( random(100, 2500));
    Serial.println("wr leaving");
    // just chekking
    if (0 < rdInsideFortest)
      Serial.println("problemmmmmss");
    wrLeave(&rdWrLock1);

  }
}

volatile int rdNr = 0;
void rd(void)
{
  int n;
  DI();
  n = rdNr;
  rdNr++;
  EI();
  while (1) {
    k_eat_ticks( random(100, 1000));
    rdEnter(&rdWrLock1);
    Serial.print("rd>> "); Serial.println(n);
    k_eat_ticks( random(100, 500));
    Serial.print("rd<<"); Serial.println(n);
    rdLeave(&rdWrLock1);

  }
}
void setup()
{
  Serial.begin(115200);  // for output from task 1
  delay(3000);
  k_init(4, 2, 0);
  k_crt_task(wr, 11, 200);
  k_crt_task(rd, 11, 200);
  k_crt_task(rd, 11, 200);
  k_crt_task(rd, 11, 200);
  rdWrLockInit(&rdWrLock1);
  k_start(1); // start kernel with tick speed 1 milli seconds
  Serial.println("if coming hre then init went wrong");
}

void loop() {
  /* loop will never be called */
}

#include <krnl.h>

struct k_t * pT1, *pT2;

struct k_t *sem1, *sem2;

#define STAKSIZE 100
char stak1[STAKSIZE];
char stak2[STAKSIZE];

struct shft {
  unsigned long t;
  unsigned char nrr;
};

struct shft logg[200];
volatile int wI = 0;


void t1()
{
  k_set_sem_timer(sem1, 100);
  while (1) {
    k_wait(sem1, 0); // wait until signal
    k_eat_time(40); /// eat time acc to code time
    if (200 <= wI) {
      for (int i = 0; i < 200; i++) {
        Serial.print(logg[i].t); Serial.print(" ");
        Serial.println(logg[i].nrr);
      }

      for (int i = 0; i < 200; i++) {
        Serial.print(logg[i].t); Serial.print(" ");
        Serial.println(logg[i].nrr);
      }


      while (1);
    }
  }
}

void t2()

{
  k_set_sem_timer(sem2, 160);
  while (1) {
    k_wait(sem2, 0); // wait until signal
    k_eat_time(40);
  }
}



void setup() {
  pinMode(8, OUTPUT); // high when t1 runs
  Serial.begin(9600);
  k_init(2, 2, 0); // 3 task 3 semafore, 0 message queues

  pT1 = k_crt_task(t1, 50, stak1, STAKSIZE); // periode time as prio
  pT2 = k_crt_task(t2, 80, stak2, STAKSIZE); // ...

  sem1 = k_crt_sem(0, 5);
  sem2 = k_crt_sem(0, 5);

  k_start(1);
}

void loop() {}

extern "C" {

  // called when a semphore is clipping - nr is id of semaphore and i os nr of times clip has occured


  /* void k_send_Q_clip(unsigned char nr, int i) {} */
  extern unsigned long k_millis_counter;
  void k_breakout() // called every task shift from dispatcher
  {
    static unsigned char prevNr = 255;
    if (prevNr != pRun->nr) { // new boy on CPU ?
      if (wI < 200) {
        prevNr = pRun->nr;
        logg[wI].nrr = pRun->nr;
        logg[wI].t = k_millis_counter;


        wI++;
      }
      //register new boy on CPU
    }
  }
}





//220216
#include <krnl.h>

#define STK 110
struct k_t *p1, *p2, *sem1, *sem2;

char st1[STK], st2[STK];

void tperiodic()
{
  // simple periodic sampler/controller/...
  k_set_sem_timer(sem1, 100);

  while (1) {
    k_wait(sem1, 0);
    k_eat_msec(70);  // we eat 70% of cpu time
  }
}


void tnoise()
{
  while (1) {
    k_eat_msec(  random(30, 200) ); // we eat between 30 and 200 msec of time
    k_sleep(500);          // and sleep for 500 msec
  }
}

int err;

void setup()
{
  for (int i = 8; i < 14; i++)   // using PORTB 0..5: d8..d13
    pinMode(i, OUTPUT);
  Serial.begin(115200);

  k_init(2, 1, 0); // init with space for three tasks

  // priority low number higher priority than higher number
  //Task 1
  p1 = k_crt_task(tperiodic, 10, st1, STK); // t1 as task, priority 10, 100 B stak
  //Task 2
  p2 = k_crt_task(tnoise, 11 , st2, STK); // t1 as task, priority 10, 100 B stak
  
  sem1 = k_crt_sem(0,5);
  
  err = k_start(); // 1 milli sec tick speed
  Serial.print("start error ");
  Serial.print(err);

}

void loop() {}


extern "C" {

  void k_breakout() // called every task shift from dispatcher
  {
    PORTB = (1 << pRun->nr);
    // on MEGA use PORTA
  }
}

// D8  ON dummy eating cpu
// D9  ON Task 1 (the first created
// D10 ON  Task 2
// ...
// only one led ON at a time

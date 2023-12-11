#include <krnl.h>
// one task loops and blink
// k_wait ensures proper timing

struct k_t *p1, *p2, *s1;

void t1()
{
  k_set_sem_timer(s1, 100); // let krnl signal sem s1 every 100 tick == 100 msec
  
  while (1) {
    k_wait(s1, 0); //wait until  a kick comes   parm 0 means waith until signal - so no timeout
    digitalWrite(13, ! digitalRead(13));
  }
}


void t2()
{
  int d[40];
  int h,l,sp, tsp;
  int bot;
  unsigned int tickk;
  while (1) {
    Serial.print(tickk);
    Serial.print(" ");
    tickk++;
    d[37] = tickk;
    h = SPH;
    l = SPL;
    sp = h <<8 | l;
    bot = pRun->cnt1;
    Serial.print(d[37]); Serial.print(" ");
    Serial.print(sp); Serial.print(" ");
    Serial.println(bot);
    k_sleep(100)
;  }
}

#define STK 100
char stk1[STK], stk2[STK];

void setup()
{
  int res;
  Serial.begin(115200);
  while (! Serial) ;
  pinMode(13, OUTPUT);

  k_init(2, 1, 0); // init with space for one task
  //         |--- no of mg Queues (0)
  //       |----- no of semaphores (0)
  //     |------- no of tasks (2)

  // priority low number higher priority than higher number
  p1 = k_crt_task(t1, 10, stk1,STK); // t1 as task, priority 10, 100 B stak
  p2 = k_crt_task(t2, 11, stk2,STK); // t2 as task, priority 11 == lower than t1, 100 B stak
  
  s1 = k_crt_sem(0, 10); // crt sem

  res = k_start(); // 1 milli sec tick speed
  // you will never return from k_start
  Serial.print("ups an error occured: "); Serial.println(res);
  while (1) ;
}

void loop() {}

#include <krnl.h>
// one task loops and blink
// k_sleep is used for delay - and ensure no busy waiting
// if delay(...) is used then you use cpu time 

struct k_t *p;
char stak[100];

void t1()
{
  while (1) {

    k_sleep(500);
    digitalWrite(13,HIGH);

    k_sleep(500);
    digitalWrite(13,LOW);

  }
}

void setup()
{
  pinMode(13,OUTPUT);
  k_init(1,0,0);  // init with space for one task

  // priority low number higher priority than higher number
  p = k_crt_task(t1,10,stak,100); // t1 as task, priority 10, 100 B stak

  k_start(1); // 1 milli sec tick speed
}

void loop() {
}


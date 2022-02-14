#include <krnl.h>
// one task loops and blink
// k_sleep is used for delay - and ensure no busy waiting
// if delay(...) is used then you use cpu time 

struct k_t *p;

void t1()
{
  while (1) {

    k_sleep(500);  // 500 * 1 msec
    digitalWrite(13,HIGH);

    k_sleep(100);
    digitalWrite(13,LOW);

  }
}

void emergencyBlink()
{
   while (1) {
      digitalWrite(3,HIGH);
      delay(100);
      digitalWrite(3,LOW);
      delay(100);
   }
}

void setup()
{
  int res;
  pinMode(13,OUTPUT);
  k_init(1,0,0);  // init with space for one task, 0 semaphores, 0 mailboxes

  p = k_crt_task(t1,10,100); // t1 as task, priority 10, 100 Bytes stak

  res= k_start(); // 1 milli sec tick speed


  // you will never return from k_start if init part did succedd 
  // bad stuff can be happended and then you will come here
  emergencyBlink();
 
  while (1) ;
}

void loop() {
}


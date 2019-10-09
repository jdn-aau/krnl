#include <krnl.h>

/* Simple program for testing performance of SNOT
* low prio task signals a high prio task waiting by semaphore
* At every 100 loop a dig port is toggled so you can use a scope
* for seeing performance
* 100 loops == 200 task shift by signal/wait on semaphore and
* takes 4.4 msec on Arduino Uno.
* So one full taskshift by semaphore takes 
*/
struct k_t *pt1,*sem1;
char s1[200];
struct k_t *pt2;
char s2[200];
 
void t1(void)
{
   while (1) {
     k_signal(sem1);
   }
}

void t2(void)
{
  char i=0;
  char togle=0;
  while (1) {
    k_wait(sem1,0); 
    i++;
    if (100 < i)
    {
      i=0;
      if (togle) {
        PORTD = 0xff;
        togle = 0;
      }
      else {
        PORTD= 0x00;
        togle = 1;
      }
    }
  }
}

void setup()
{
  int i;
  Serial.begin(9600);
  k_init(3,3,3);
  for (i=0; i < 8; i++)
   pinMode(i,OUTPUT); // so we can use fast PORTD output
  pt1=k_crt_task(t1,11,s1,200); 
  pt2=k_crt_task(t2,10,s2,200); 
  sem1 = k_crt_sem( 0,100);
  k_start(10);
}

void loop()
{
}



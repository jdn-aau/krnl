#include <krnl.h>

struct k_t *pTaskInfo;

char stak[100];

void code(void)
{
  int r;

  while (1) {
    digitalWrite(13,HIGH);
    k_sleep(300); 
    digitalWrite(13,LOW);
    k_sleep(300); 
  }
}

void setup()
{
  Serial.begin(9600);
  pinMode(13,OUTPUT);

  k_init(1,0,0);

  pTaskInfo=k_crt_task(code,11,stak,100); 

  k_start(1); // krnl runs with 1 msec heartbeat
  /* NOTE: legal time values:
     1-10 : 1-10 milliseconds
     19,20,30,40...10000: 10,20... milliseconds
     if you try negative or 11,54 etc it will not start
  */
}

void loop()
{ // never used
}





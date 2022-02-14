#include <snot2.h>

// mutual exclusion by semaphore :-)
// two tasks want to acces serial line
// they have to reserce the serial port by aquiring a semaphore and release afterwards
// 
 
struct k_t *t1, *t2, *mutexSem;

char stak1[100], stak2[100]; 

void t1Code() {
  while (1) {
    k_wait(mutexSem,0); // lets get the serial port - we will wait forever until granted access
    Serial.println("nr 1 :-)");
    Serial.println("nr 1. :-)");
    Serial.println("nr 1.. :-)");
    Serial.println("nr 1... :-)");
    k_signal(mutexSem);  // release port again
  }
}

void t2Code() {
  while (1) {
    k_wait(mutexSem,0);
    // delay(1000); // w´busy waiting a second - not the way to wait
    Serial.println("nr 2 :-)");
    Serial.println("nr 2. :-)");
    Serial.println("nr 2.. :-)");
    Serial.println("nr 2... :-)");
    k_signal(mutexSem);
  }
}

void setup()
{
  k_init(5,5,5);
  Serial.begin(9600);
  mutexSem = k_crt_sem(1,10);
  t1 = k_crt_task(t1Code,10,stak1,100);
  t2 = k_crt_task(t2Code,10,stak2,100);
  k_start(10);
}
void loop()
{
}


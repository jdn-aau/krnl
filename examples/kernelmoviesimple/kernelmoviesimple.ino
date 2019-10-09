#include <krnl.h>


// an simple example with KeRNeL
// http://www.control.aau.dk/~jdn/edu/doc/kernels/arduino
// Just one task looping and waiting on a Dikstra semaphore
// It timeout and to serial printing
// br Jens

struct k_t *sem, *task;

void taskBody()
{
  while (1) {
    k_wait(sem,50); // wait up to 50 ticks and timeout
    Serial.println("tik ");
  }
}

char stak[100];
void setup()
{
  Serial.begin(9600);
  k_init(5,5,5);
  task = k_crt_task(taskBody,10,stak,100);
  sem = k_crt_sem(0,10);
  Serial.println("just bef starting");
  k_start(10); // start KeRNeL with tickspeed 10 msec
  Serial.println("you should no come here - ");
}

void loop()
{
  // dead function - will never be called
}


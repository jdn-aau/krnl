
#include <krnl.h>
 //Jens s2021
// https://en.wikipedia.org/wiki/Readers%E2%80%93writers_problem


struct k_rwlock_t lock1; 

 
void wr(void)
{
  int i = 0;
  unsigned long ii;
  i = ki_millis();
  while (1) {
    k_delay_task_periodic(&ii, 100);//sleep( random(100, 1000)); // NB k_sleep and not k_eat_ticks bq then you will never release CPU
    k_rwWrEnter(&lock1,0);
    Serial.println("wr in");
    k_eat_ticks( random(100, 2500));
      Serial.println("wr leaving");
    k_rwWrLeave(&lock1);
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
    k_sleep( random(100, 1000));  // NB k_sleep and not k_eat_ticks bq then you will never release CPU
    k_rwRdEnter(&lock1,0);
    Serial.print("rd>> "); Serial.println(n);
    k_eat_ticks( random(100, 500));
    Serial.print("rd<<"); Serial.println(n);
    k_rwRdLeave(&lock1);
  }
}
void setup()
{
  Serial.begin(115200);  // for output from task 1
  delay(2000);
  Serial.print("RW START");
  k_init(4, 3, 0);
  k_crt_task(wr, 12, 200);
  k_crt_task(rd, 11, 200);
  k_crt_task(rd, 11, 200);
  k_crt_task(rd, 11, 200);
  k_rwInit(&lock1);
  k_start(1); // start kernel with tick speed 1 milli seconds
  Serial.println("if coming hre then init went wrong");
}

void loop() {
  /* loop will never be called */
}

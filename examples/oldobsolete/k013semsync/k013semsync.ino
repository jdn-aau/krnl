#include <krnl.h>
// one task loops and blink
// k_wait ensures proper timing

struct k_t *p1,*s1;

void t1()
{  
  k_set_sem_timer(s1,100);
  while (1) {  
    k_wait(s1,0); //wait until  a kick comes
    digitalWrite(13,! digitalRead(13));
  }  
}

 
void setup()
{
  int res;
  Serial.begin(9600);
  while (! Serial) ;
  pinMode(13,OUTPUT);
  
  k_init(1,1,0);  // init with space for one task
  //         |--- no of mg Queues (0)
  //       |----- no of semaphores (0)  
  //     |------- no of tasks (2)

  // priority low number higher priority than higher number
  p1 = k_crt_task(t1,10,100); // t1 as task, priority 10, 100 B stak
  s1 = k_crt_sem(0,10);
  res= k_start(1); // 1 milli sec tick speed
  // you will never return from k_start
  Serial.print("ups an error occured: "); Serial.println(res);
  while (1) ;
}

void loop() {}

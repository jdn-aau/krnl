#include <krnl.h>
// one task loops and blink
// one task loops and print

struct k_t *p1, *p2;

void t1()
{
  while (1) {  
    k_sleep(500);
    digitalWrite(13,HIGH);

    k_sleep(100);
    digitalWrite(13,LOW);
  } 
}

void t2()
{
int i = 0;
  while (1) {  
    k_sleep(300);
    Serial.println(i++);
  } 
}


void setup()
{
  int res;
  Serial.begin(9600);
  while (! Serial) ;
  pinMode(13,OUTPUT);
  
  k_init(2,0,0);  // init with space for one task
  //         |--- no of mg Queues (0)
  //       |----- no of semaphores (0)  
  //     |------- no of tasks (2)

  // priority low number higher priority than higher number
  p1 = k_crt_task(t1,10,100); // t1 as task, priority 10, 100 B stak
  p2 = k_crt_task(t2,10,100); // t2 as task, priority 10, 100 B stak
  res= k_start(1); // 1 milli sec tick speed
  // you will never return from k_start
  Serial.print("ups an error occured: "); Serial.println(res);
  while (1) ;
}

void loop() {}

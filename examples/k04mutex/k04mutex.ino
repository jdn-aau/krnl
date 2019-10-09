#include <krnl.h>

// A small krnl program with two independent tasks
// They share data which is guarded by a semaphore implemented mutex 
// Tasks has same priority in this examples
// you can have more mutex regions by using more sempahores :-)

struct k_t *pt1, // pointer to hold reference
*pt2,          // to taskdescriptor for t1 and t2  
*mutexSem; 

char s1[200]; // stak for task t1
char s2[200]; // stak for task t2

void toggleLED13()
{
  static char flag=0;
  if (flag) {
    flag = 0;
    digitalWrite(13,HIGH);
  }
  else {
    flag = 1;
    digitalWrite(13,LOW);
  }
} 
}

void setup()
{
  Serial.begin(9600);  // for output from task 1
  pinMode(13,OUTPUT);  // for blink on LED from task 2

  // init krnl so you can create 2 tasks, 1 semaphore and no message queues
  k_init(2,1,0); 

  // two task are created
  //                |------------ function used for body code for task
  //                |  |--------- priority (lower number= higher prio
  //                |  | |------- array used for stak for task 
  //                |  | |   |--- staksize for array s1
  pt1=k_crt_task(t1,11,s1,200);  // 
  pt2=k_crt_task(t2,11,s2,200);  //  

  //                    |--- startvalue is 1 bq it is for mutex usage
  //                       |-- max value before maxing out
  mutexSem = k_crt_sem(1,10);

  k_start(1); // start kernel with tick speed 10 milli seconds
}

void loop(){ /* loop will never be called */
}






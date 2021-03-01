#include <krnl.h>

// A small krnl program with two independent tasks
// They run at same priority so krnl will do timeslicing between them
// Watch LED and Serial TX 

struct k_t *pt1, // pointer to hold reference
  *pt2;          // to taskdescriptor for t1 and t2  
 
char s1[200]; // stak for task t1
char s2[200]; // stak for task t2
 
void t1(void)
{
  // a task must have an endless loop
  // if you end and leave the task function - a crash will occur!!
  // so this loop is the code body for task 1
  while (1) {  
    Serial.println("Hello World");
    k_sleep(1000); // let task sleep for 1000 kernel ticks
  }               // lenght of ticks in millisec is specified in
}                 // k_start call called from setup

void t2(void)
{
  // and task body for task 2
  // runs independent of task t1
   while (1) {
     digitalWrite(13,HIGH);  // led 13 ON
     k_sleep(500);           // sleep 500 ticks
     digitalWrite(13,LOW);   // led 13 OFF
     k_sleep(500);           // and sleep
   }
}

void setup()
{
  Serial.begin(9600);  // for output from task 1
  pinMode(13,OUTPUT);  // for blink on LED from task 2

  // init krnl so you can create 2 tasks, 0 semaphores and no message queues
  k_init(2,0,0); 

// two task are created
//               |------------ function used for body code for task
//               |  |--------- priority (lower number= higher prio
//               |  | |------- array used for stak for task 
//               |  | |   |--- staksize for array s1

  pt1=k_crt_task(t1,11,s1,200); 
  pt2=k_crt_task(t2,11,s2,200);
  
  
  // NB-1 remember an Arduino has only 2-8 kByte RAM
  // NB-2 remember that stak is used in function calls for
  //  - return address
  //  - registers stakked
  //  - local variabels in a function
  //  So having 200 Bytes of stak excludes a local variable like ...
  //    int arr[400];  
  // krnl call k_unused_stak returns size of unused stak
  // Both task has same priority so krnl will shift between the
  // tasks every 10 milli second (speed set in k_start)

  k_start(1); // start kernel with tick speed 10 milli seconds
}

void loop(){ /* loop will never be called */ }




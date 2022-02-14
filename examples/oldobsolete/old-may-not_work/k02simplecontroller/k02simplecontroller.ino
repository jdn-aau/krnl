#include <krnl.h>

// A small krnl program with two independent tasks
// High priority task (t1) acts like a realtime controller with
// fixed samplings frequency.
// A low priority task(t2) do non real time actions (just some printing)

struct k_t *pt1, // pointer to hold reference
*pt2,          // to taskdescriptor for t1 and t2  
*samplingsSem;

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

void t1(void)
{

  k_set_sem_timer(samplingsSem,100); // krnl will signal sem every 10 tick

  while (1) {  
    // wait until next kick from timer
    // In this way we will run realtime with a fixed frequencey
    // but beware of tasks with highe priority bq they can spoil your
    // realtime bq you will be ready but not running until they leave the
    // active queue
    
    k_wait(samplingsSem,0); 
    toggleLED13(); // chk frequency with an oscilloscope 

    // AD(); 
    // pid();
    // DA();
    // hope you get the plot :-)
    
  }         
}             

void t2(void)
{
  // and task body for task 2
  // runs independent of task t1
  while (1) {
    Serial.println("tik"); // just do something non realtime :-)
    k_sleep(1000);
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
  pt1=k_crt_task(t1,10,s1,200);  // highest prio
  pt2=k_crt_task(t2,11,s2,200);  // lowest - 10 < 11 as well as 5 < 100 :-)

  //                         |--- startvalue (can be 0,1,2, not negative 
  //                          |-- max value before maxing out
  samplingsSem = k_crt_sem(0,10);

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

void loop(){ /* loop will never be called */
}





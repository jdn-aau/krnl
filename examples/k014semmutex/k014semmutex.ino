#include <krnl.h>
// one task loops and blink
// k_wait ensures proper timing

struct k_t *p1, *p2,*s1, *semmutex;

// shared data
struct shDataTp{
int v;
int counter;
};

struct shDataTp  sharedData={0,0};

void saveDataInCritRegion(int v)
{
  k_wait(semmutex,0);
  sharedData.v = v;
  sharedData.counter ++;
  k_signal(semmutex);
}

struct shDataTp getDataInCritRegion(void)
{
struct shDataTp tmp;
  k_wait(semmutex,0);
  sharedData.counter--;
  tmp = sharedData;
  k_signal(semmutex);
  return tmp;
}

// the sampler task
void t1()
{  
int v;
  k_set_sem_timer(s1,100);
  while (1) {
   
    k_wait(s1,0); //wait until  a kick comes

    v = analogRead(A0);
    saveDataInCritRegion(v);
 }  
}

void t2()
{  
 static struct shDataTp v;
  while (1) { 
     
    k_sleep(345);
    v = getDataInCritRegion();
    Serial.print(v.v);
    Serial.print(" ");
    Serial.println(v.counter);
  }  // do we keep up in pace ?  
}

 
void setup()
{
  int res;
  Serial.begin(9600);
  while (! Serial) ;
  pinMode(13,OUTPUT);
  
  k_init(2,2,0);  // init with space for one task
  //         |--- no of mg Queues (0)
  //       |----- no of semaphores (0)  
  //     |------- no of tasks (2)

  // priority low number higher priority than higher number
  p1 = k_crt_task(t1,10,100); // t1 as task, priority 10, 100 B stak
  p2 = k_crt_task(t2,10,100); // t1 as task, priority 10, 100 B stak

  s1 = k_crt_sem(0,10);
  semmutex = k_crt_sem(1,10); // must be 1 otherwise no one can come inside
  Serial.println("bef start");
  res= k_start(1); // 1 milli sec tick speed
  // you will never return from k_start
  Serial.print("ups an error occured: "); Serial.println(res);
  while (1) ;
}

void loop() {}

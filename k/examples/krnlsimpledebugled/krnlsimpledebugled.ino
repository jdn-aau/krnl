//220216
#include <krnl.h>

/*
 * LED on dig pin 8,9,10
 * D8  dummy running
 * D9 first task created running
 * D10 second ...
 * 
 */
#define STK 110
struct k_t *p1, *p2, *p3;
char st1[STK], st2[STK], st3[STK];

void tgeneric()
{
  while (1) {
    k_eat_msec(pRun->nr * 100);  // tasks has 1,2,3 so mul by 100 gives 100msec,...
    k_sleep(1000);
  }
}

void setup()
{

  Serial.begin(115200);

  for (int i = 8; i < 14; i++)   // using PORTB 0..5: d8..d13
    pinMode(i, OUTPUT);
  /* MEGA  PORTA (int i=22; i < 30; i++) pinMode(i,OUTPUT); */ 

  k_init(3, 0, 0); // init with space for three tasks2

  // priority low number higher priority than higher number
  //Task 1
  p1 = k_crt_task(tgeneric, 10, st1, STK); // t1 as task, priority 10, 100 B stak
  //Task 2
  p2 = k_crt_task(tgeneric, 11, st2,  STK); // t1 as task, priority 10, 100 B stak
  //Task 3
  p3 = k_crt_task(tgeneric, 12, st3, STK); // t1 as task, priority 10, 100 B stak

  k_start(); // 1 milli sec tick speed
}

void loop() {}

extern "C" {

  void k_breakout() // called every task shift from dispatcher
  {
    PORTB = (1 << pRun->nr);   // D8: dummy running, D9 first task created running,...
    // on MEGA use PORTA
  }
}

// D8  ON dummy eating cpu
// D9  ON Task 1 (the first created
// D10 ON  Task 2
// ...
// only one led ON at a time
// dont use led 13 for othe stuff bq it s cleared her
// can be fixed - just ask me

#include <krnl.h>
 
// LED 13 is on when dummy is running - otherwise no light
// can be used as skeletonfor extended debug LEDing

struct k_t *p;
char stak[100];

// LED 13 not used from user space bq we use led for indicating dummy i srunning
// when dummy is running it indicates enough cpu power
// if dummy is not running for a longer time you may have problems ....
void t1()
{
  while (1) {

    Serial.println("I am running");
    k_eat_msec_time(1000);  // you are running
    Serial.println("dummy is now running");
    k_sleep(1000);     // you are NOT running so dummmy is running
  }
}

void setup()
{

  Serial.begin(9600);
  pinMode(13, OUTPUT);
  k_init(1, 0, 0); // init with space for one task

  // priority low number higher priority than higher number
  p = k_crt_task(t1, 10, stak, 100); // t1 as task, priority 10, 100 B stak

  k_start(1); // 1 milli sec tick speed
}

void loop() {
}



extern "C" {


  void k_breakout() // called every task shift from dispatcher
  {
    static char c = 0;

    if (pRun->nr == 0) {
      //PORTB =  PORTB | B00010000; // LED 12 on if dmy is running
      PORTB =  PORTB |   B00100000; // LED 13 .. 
    }
    else {
      //PORTB =  PORTB &  B11101111;
       PORTB =  PORTB &   B11011111;     // led 13 ...
      }
  }
}


/*

   Suggested PORTS TO USE
  uno
  pin port
  8   PB0
  9   PB1
  10  PB2
  11  PB3
  12  PB4
  13  PB5  (led)
  PB6,7 etc not to be used !


  MEGA
  pin port
  78  PA0
  77  PA1
  76  PA2
  75  PA3
  74  PA4
  73  PA5
  72  PA6
  71  PA7

  MICRO
  8  PB4
  9  PB5
  10 PB6
  11 PB7

  NANO
  D8  PD0
  D9  PD1
  D2  PD2
  D3  PD3
  D4  PD4
  D5  PD5
  D6  PD6
  D7  PD7

  PRO MINI
  2 PD2
  3 PD3
  4 PD4
  5 PD5
  6 PD6

 * */

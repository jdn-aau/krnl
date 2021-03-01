#include <krnl.h>
// one task loops and blink
// k_sleep is used for delay - and ensure no busy waiting
// if delay(...) is used then you use cpu time

struct k_t *p;
char stak[100];
volatile int cntt1 = 0, cntt2 = 0;
void t1()
{
  while (1) {


    digitalWrite(13, HIGH);
    //delay(1000);
    k_eat_msec_time(1000);
    digitalWrite(13, LOW);
    Serial.print(cntt1); Serial.print("  "); Serial.println(cntt2);
    k_sleep(1000);

  }
}

void setup()
{
  Serial.begin(9600);
  pinMode(12, OUTPUT); // krnl breakout
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

    if (pRun->nr == 0) {
      PORTB |=  0x10;
      cntt1++;

    }
    else {
      cntt2++;
      PORTB &=  0x20;
    }

    // for a MEGA you have to find another port :-)
    // port K (adc8-15) seems feasible
  }
}

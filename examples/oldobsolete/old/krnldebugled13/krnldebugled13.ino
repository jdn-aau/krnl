#include <krnl.h>

// LED13 on i dummy is runningg

struct k_t *p1, *p2, *p3;
char st1[100], st2[100], st3[100];

// LED 13 not used from user space bq we use led for indicating dummy i srunning
// when dummy is running it indicates enough cpu power
// if dummy is not running for a longer time you may have problems ....
void t1()
{
  while (1) {

    Serial.print(pRun->nr);
    Serial.println(" I am running");
    k_eat_msec_time(500);  // you are running
    Serial.println("dummy is now running");
    k_sleep(2000);     // you are NOT running so dummmy is running
  }
}

void setup()
{

  Serial.begin(9600);
  pinMode(13, OUTPUT);  // for debug

  k_init(3, 0, 0); // init with space for three tasks

  // priority low number higher priority than higher number
  p1 = k_crt_task(t1, 10, st1, 100); // t1 as task, priority 10, 100 B stak
  p2 = k_crt_task(t1, 11, st2, 100); // t1 as task, priority 10, 100 B stak
  p3 = k_crt_task(t1, 12, st3, 100); // t1 as task, priority 10, 100 B stak

  k_start(1); // 1 milli sec tick speed
}

void loop() {}

/*
  #if defined(__AVR_ATmega168__) || defined(__AVR_ATmega328P__) || defined(__AVR_ATmega328__)
  for (int i = 8; i < 8+6; i++)
    pinMode(i, OUTPUT);  // PORTB

  #elif defined (__AVR_ATmega2560__) || defined (__AVR_ATmega1280__) || defined(__AVR_ATmega2561__)
  for (int i = 22; i < 22+6; i++)
    pinMode(i, OUTPUT);  // PORTA
  #endif

*/
extern "C" {

  void k_breakout() // called every task shift from dispatcher
  {
    //     https://arduino.stackexchange.com/questions/19892/list-of-arduino-board-preprocessor-defines

    // FOR UNO

#if defined (ARDUINO_AVR_UNO)

    if (pRun->nr == 0)
    {
      PORTB = PORTB | B00100000;  // led13 (bit 5) on let the rest be untouched
    }
    else {
      PORTB = PORTB & B11011111;  // led off uno
    }
#endif

#if ( defined (ARDUINO_AVR_MEGA) || defined(ARDUINO_AVR_MEGA2560) )
    if (pRun->nr == 0)
    {
      PORTB = PORTB | B10000000;  // led13 (bit 7) on let the rest be untouched
    }
    else {
      PORTB = PORTB & B01111111;  // led off  mega
    }
#endif

  }
}


/* or just LED13 if dummy ...

  YOu can test for board by #ifdef ARDUINO_AVR_UNO  etc
  see list below

  AVR_ADK
  AVR_BT
  AVR_DUEMILANOVE
  AVR_ESPLORA
  AVR_ETHERNET
  AVR_FIO
  AVR_GEMMA
  AVR_LEONARDO
  AVR_LILYPAD
  AVR_LILYPAD_USB
  AVR_MEGA
  AVR_MEGA2560
  AVR_MICRO
  AVR_MINI
  AVR_NANO
  AVR_NG
  AVR_PRO
  AVR_ROBOT_CONTROL
  AVR_ROBOT_MOTOR
  AVR_UNO
  AVR_YUN

    both: pinMode(13,OUTPUT);
    uno mega

    if (pRun->nr == 0)
    PORTB = PORTB | B00100000;  // led on uno
                  | B10000000;  // mega

    else

    PORTB = PORTB & B11011111;  // led off uno
                  & B01111111;   / mega

    mega

*/

/*

   Suggested PORTS TO USE

  UNO
  pin port
  8   PB0
  9   PB1
  10  PB2
  11  PB3
  12  PB4
  13  PB5  LED13
  PB6 and 7  etc not to be used !


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
  13  PB7 LED13

  MICRO
  8  PB4
  9  PB5
  10 PB6
  11 PB7
  13 PC7 LED13

  NANO
  D8  PD0
  D9  PD1
  D2  PD2
  D3  PD3
  D4  PD4
  D5  PD5
  D6  PD6
  D7  PD7
  13  PB5  LED13

  PRO MINI
  2  PD2
  3  PD3
  4  PD4
  5  PD5
  6  PD6
  13 PB5 LED13

 * */

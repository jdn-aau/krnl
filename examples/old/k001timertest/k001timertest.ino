/*****************************************************
*  TEST TEST TEST
* krnl.c  part of kernel KRNL               *
 * this version adapted for Arduino                   *
*                                                    *
* (C) 2012,2013,2014                                 *
*                                                    *
* Jens Dalsgaard Nielsen <jdn@es.aau.dk>             *
* http://es.aau.dk/staff/jdn                         *
* Section of Automation & Control                    *
* Aalborg University,                                *
* Denmark                                            *
*                                                    *
* "THE BEER-WARE LICENSE" (frit efter PHK)           *
* <jdn@es.aau.dk> wrote this file. As long as you    *
* retain this notice you can do whatever you want    *
* with this stuff. If we meet some day, and you think*
* this stuff is worth it ...                         *
*  you can buy me a beer in return :-)               *
* or if you are real happy then ...                  *
* single malt will be well received :-)              *
*                                                    *
* Use it at your own risk - no warranty              *
*                                                    *
* tested with duemilanove w/328, uno R3,             *
* seeduino 1280 and mega2560                         *
*****************************************************/


/*
First 50 mea without krnl  0.6% overhead
Next  59 mea witrh krnl    4.1% overhead
only one task created. more task and sem will increase overhead
because more elm will be checked in timer isr

Please note it is quite stable

/Jens  april 2015


1000 builtin/k-eat: 1005 / 1006
1000 builtin/k-eat: 1007 / 1006
1000 builtin/k-eat: 1006 / 1007
1000 builtin/k-eat: 1006 / 1005
1000 builtin/k-eat: 1006 / 1007
1000 builtin/k-eat: 1007 / 1006
1000 builtin/k-eat: 1005 / 1006
1000 builtin/k-eat: 1007 / 1006
1000 builtin/k-eat: 1006 / 1007
1000 builtin/k-eat: 1006 / 1006
1000 builtin/k-eat: 1007 / 1006
1000 builtin/k-eat: 1007 / 1007
1000 builtin/k-eat: 1006 / 1005
1000 builtin/k-eat: 1006 / 1007
1000 builtin/k-eat: 1007 / 1006
1000 builtin/k-eat: 1005 / 1006
1000 builtin/k-eat: 1007 / 1005
1000 builtin/k-eat: 1006 / 1007
1000 builtin/k-eat: 1006 / 1005
1000 builtin/k-eat: 1006 / 1006
1000 builtin/k-eat: 1007 / 1007
1000 builtin/k-eat: 1005 / 1005
1000 builtin/k-eat: 1007 / 1006
1000 builtin/k-eat: 1007 / 1006
1000 builtin/k-eat: 1005 / 1006
1000 builtin/k-eat: 1007 / 1005
1000 builtin/k-eat: 1006 / 1007
1000 builtin/k-eat: 1006 / 1005
1000 builtin/k-eat: 1006 / 1006
1000 builtin/k-eat: 1007 / 1006
1000 builtin/k-eat: 1005 / 1006
1000 builtin/k-eat: 1007 / 1006
1000 builtin/k-eat: 1006 / 1007
1000 builtin/k-eat: 1006 / 1007
1000 builtin/k-eat: 1007 / 1005
1000 builtin/k-eat: 1006 / 1007
1000 builtin/k-eat: 1006 / 1006
1000 builtin/k-eat: 1005 / 1006
1000 builtin/k-eat: 1007 / 1006
1000 builtin/k-eat: 1006 / 1007
1000 builtin/k-eat: 1006 / 1005
1000 builtin/k-eat: 1006 / 1007
1000 builtin/k-eat: 1007 / 1007
1000 builtin/k-eat: 1005 / 1006
1000 builtin/k-eat: 1007 / 1007
1000 builtin/k-eat: 1007 / 1006
1000 builtin/k-eat: 1005 / 1006
1000 builtin/k-eat: 1007 / 1006
1000 builtin/k-eat: 1006 / 1007
1000 builtin/k-eat: 1006 / 1005
<<
1000 builtin/k-eat: 1042 / 1042
1000 builtin/k-eat: 1041 / 1042
1000 builtin/k-eat: 1042 / 1041
1000 builtin/k-eat: 1041 / 1041
1000 builtin/k-eat: 1041 / 1042
1000 builtin/k-eat: 1041 / 1040
1000 builtin/k-eat: 1041 / 1040
1000 builtin/k-eat: 1040 / 1040
1000 builtin/k-eat: 1040 / 1041
1000 builtin/k-eat: 1042 / 1041
1000 builtin/k-eat: 1042 / 1041
1000 builtin/k-eat: 1041 / 1042
1000 builtin/k-eat: 1041 / 1042
1000 builtin/k-eat: 1042 / 1041
1000 builtin/k-eat: 1042 / 1041
1000 builtin/k-eat: 1041 / 1042
1000 builtin/k-eat: 1041 / 1042
1000 builtin/k-eat: 1042 / 1041
1000 builtin/k-eat: 1041 / 1040
1000 builtin/k-eat: 1040 / 1041
1000 builtin/k-eat: 1040 / 1041
1000 builtin/k-eat: 1041 / 1040
1000 builtin/k-eat: 1041 / 1041
1000 builtin/k-eat: 1041 / 1042
1000 builtin/k-eat: 1042 / 1041
1000 builtin/k-eat: 1042 / 1041
1000 builtin/k-eat: 1041 / 1041
1000 builtin/k-eat: 1041 / 1042
1000 builtin/k-eat: 1042 / 1041
1000 builtin/k-eat: 1042 / 1041
1000 builtin/k-eat: 1041 / 1042
1000 builtin/k-eat: 1041 / 1041
1000 builtin/k-eat: 1042 / 1040
1000 builtin/k-eat: 1040 / 1040
1000 builtin/k-eat: 1040 / 1041
1000 builtin/k-eat: 1040 / 1041
1000 builtin/k-eat: 1042 / 1041
1000 builtin/k-eat: 1041 / 1041
1000 builtin/k-eat: 1041 / 1042
1000 builtin/k-eat: 1042 / 1041
1000 builtin/k-eat: 1042 / 1041
1000 builtin/k-eat: 1041 / 1041
1000 builtin/k-eat: 1041 / 1042
1000 builtin/k-eat: 1042 / 1041
1000 builtin/k-eat: 1042 / 1041
1000 builtin/k-eat: 1041 / 1041
1000 builtin/k-eat: 1040 / 1041
1000 builtin/k-eat: 1041 / 1040
1000 builtin/k-eat: 1040 / 1040
1000 builtin/k-eat: 1041 / 1042


*/

#include <i2a.h>

#include <krnl.h>

char a[10];

void test (int msec)
{
  unsigned long t1, t2;
  i2a(a, msec);

  Serial.print(a);
    Serial.print(" builtin/k-eat: ");
  t1 = millis();
  while (msec > 10) {
    delayMicroseconds(10000);
    msec -= 10;
  }
  delayMicroseconds(msec * 1000);
  t2 = millis();
  t2 -= t1;
  ul2a(a, t2);
  Serial.print(a);

}

void ktest(int msec)
{
  unsigned long t1, t2;

  Serial.print(" / ");
  t1 = millis();
  k_eat_time(msec);
  t2 = millis();
  t2 -= t1;
  ul2a(a, t2);
  Serial.println(a);
  k_eat_time(100); // wait på rs232
}

char s1[100];
void t1()
{
  int i = 50;
  while (i--) {
    test(1000);
    ktest(1000);
  }
  while (1);
}


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  for (int i = 0; i < 50; i++) {
    test(1000);
    ktest(1000);
  }
  Serial.println("<<");
  k_init(1, 0, 0);
  k_crt_task(t1, 10, s1, 100);
  k_start(1);
}

void loop() {}  // put your main code here, to run repeatedly:



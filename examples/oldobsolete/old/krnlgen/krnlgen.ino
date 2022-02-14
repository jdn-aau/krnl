#include <krnl.h>

struct k_t *t1, *t2, *s1, *s2;

struct k_msg_t *mq1, *mq2;

#define STK 150

char stk1[STK], stk2[STK];

void tcode1()
{
  while (1) {

  }
}

void tcode2()
{
  while (1) {

  }
}


void setup() {
  Serial.begin(9600);

  k_init(2, 2, 2);

  t1 = k_crt_task(tcode1, 10, stk1, STK);
  t2 = k_crt_task(tcode2, 10, stk2, STK);
  s1 = k_crt_sem(1, 10);
  s2 = k_crt_sem(1, 10);

  k_start(1);
  Serial.println("shouldnt come her - KRNL did not start");
}

void loop() {

}

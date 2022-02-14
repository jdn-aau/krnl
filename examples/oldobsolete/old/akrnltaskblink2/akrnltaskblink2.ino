#include <krnl.h>

struct k_t * pt1, *pt2, *pt3,
    *sem1, *sem2, *sem3;

#define TICKSPEED 10

#define STK 100
char st1[STK], st2[STK], st3[STK];

volatile int cnt = 0;

void highprio()
{
  int clip;
  Serial.println("hello");
  k_set_sem_timer(sem1, 200 / TICKSPEED); //
  while (1) {
    k_wait_lost(sem1, 0, &clip);
    if (clip != 0) {
      //    digitalWrite(13, HIGH); // led on bq we are behind
    }
    k_eat_time(50);  // lets eat 200 milliseconds
  }
}


void medprio()
{
  int clip;
  k_set_sem_timer(sem2, 1000 / TICKSPEED); //
  while (1) {
    k_wait_lost(sem2, 0, &clip);
    if (clip != 0) {
      //      digitalWrite(13, HIGH); // led on bq we are behind
    }
    k_eat_time(200);  // lets eat 5 milliseconds
  }
}

void lowprio()
{
  int clip;
  k_set_sem_timer(sem3, 3000 / TICKSPEED); //
  while (1) {

    k_wait_lost(sem3, 0, &clip);
    if (clip != 0) {
      //      digitalWrite(13, HIGH); // led on bq we are behind
    }
    k_eat_time(100);
  }

}


void setup()
{
  {
    int i;
    for (i = 8; i <= 13; i++)
    {
      pinMode(i, OUTPUT);
      digitalWrite(i, LOW);
    }
  }
  pinMode(13, OUTPUT); //LED
  digitalWrite(13, LOW);
  k_init(4, 3, 0);   // dummy get tasknr 0
  Serial.begin(9600);
  pt3 = k_crt_task(lowprio, 30, st3, STK); //get tasknr 1
  pt2 = k_crt_task(medprio, 20, st2, STK); // 2 ...
  pt1 = k_crt_task(highprio, 10, st1, STK); // 3

  sem1 = k_crt_sem(0, 2); // so 1 is highest value
  sem2 = k_crt_sem(0, 2); // so 1 is highest value
  sem3 = k_crt_sem(0, 2); // so 1 is highest value
  Serial.println("stnart");
  k_start(10); // 10 millisecond heartbeat
  Serial.print("oev");
}

extern "C" {


  void k_sem_clip(unsigned char nr, int i)
  {
    cnt++;
  }

  void k_send_Q_clip(unsigned char nr, int i)
  {
    cnt++;
  }
  void k_breakout() // called every task shift
  {
    unsigned char c;
    PORTB  = 1 << pRun->nr; // arduino uno !!
  }

}



void loop() {}

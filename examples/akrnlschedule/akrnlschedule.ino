#include <krnl.h>

struct k_t * pt1, *pt2, *pt3, *sem1, *sem2, *sem3;


#define TICKSPEED 10

#define STK 100
char st1[STK], st2[STK], st3[STK];

volatile int cnt=0;

void highprio()
{
  int clip;
  int loop = 0;
  int eat = 50;
  Serial.println("hello"); 
  k_set_sem_timer(sem1, 100/TICKSPEED); //
   while (1) {
     Serial.println(cnt);
     k_wait_lost(sem1, 0, &clip);
    if (clip != 0) {
  //    digitalWrite(13, HIGH); // led on bq we are behind
    }
    loop++;
    if (30 < loop) {
      loop = 0;
      eat+=3;
    }
    k_eat_time(eat);  // lets eat 5 milliseconds
  }
}



void medprio()
{
  int clip;
  k_set_sem_timer(sem2, 200/TICKSPEED); //
  while (1) {
    k_wait_lost(sem2, 0, &clip);
    if (clip != 0) {
//      digitalWrite(13, HIGH); // led on bq we are behind
    }
    k_eat_time(50);  // lets eat 5 milliseconds
  }
}

void lowprio()
{
  int clip;
  k_set_sem_timer(sem3, 500/TICKSPEED); //
  while (1) {

    k_wait_lost(sem3, 0, &clip);
    if (clip != 0) {
      digitalWrite(13, HIGH); // led on bq we are behind
    }
    k_eat_time(300);   
  }

}


void setup()
{
  pinMode(13, OUTPUT); //LED
  digitalWrite(13,LOW);
  k_init(4, 3, 0);
  Serial.begin(9600);
  pt1 = k_crt_task(highprio, 10, st1, STK);
  pt2 = k_crt_task(medprio, 20, st2, STK);
  pt3 = k_crt_task(lowprio, 30, st3, STK);

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
  //Serial.print(i);
}

void k_send_Q_clip(unsigned char nr, int i)
{
  cnt++;
}
} // extern "C"

void loop() {}

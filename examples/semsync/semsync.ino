#include <krnl.h>

struct k_t *pt1,*sem1;
char s1[200];
struct k_t *pt2;
char s2[200];

void doBlink(void) {
static char flag = 0;
  flag = !flag;
  digitalWrite(13,flag);
}

void t1(void)
{
  int i=0;
  char r;
  while (1) {
    doBlink(); 
    k_wait(sem1,0);
    i++;
    Serial.println("w"); 
  }
}

void t2(void)
{
  while (1) {
    k_sleep(30);
    k_signal(sem1);
  }
}

void setup()
{
  Serial.begin(9600);
  k_init(3,3,3);
  pinMode(13,OUTPUT);
  pt1=k_crt_task(t1,11,s1,200); 
  pt2=k_crt_task(t2,11,s2,200); 
  sem1 = k_crt_sem(1,100);
  k_start(10);
}

void loop()
{
}



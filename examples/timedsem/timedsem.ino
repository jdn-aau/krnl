#include <krnl.h>
// eller krnl.h 

struct k_t *pt1,*sem1,*sem2;
char s1[100];


void doBlink(void) {
  static char flag = 0;

  flag = ! flag;
  digitalWrite(13,flag);
}

void t1(void)
{
  int r;

  while (1) {
    // no signal timeout after 300 tick
    // k_wait returns -1 to indicate timeout
    r = k_wait(sem1,300); 
    doBlink();
  }
}

struct k_t *pt2;
char s2[100];
void t2(void)
{
  int i=0,j;
  char r;
  k_set_sem_timer(sem2,100);
  while (1) {
    Serial.println("TAK"); 
    k_wait(sem2,0);
    k_signal(sem1);
  }
}

void setup()
{
  Serial.begin(9600);
  pinMode(13,OUTPUT);
  k_init(4,4,4);
  pt1=k_crt_task(t1,11,s1,100); 
  pt2=k_crt_task(t2,11,s2,100);
  sem1 = k_crt_sem(1,100);
  sem2 = k_crt_sem(1,100);
  k_start(10);
}

void loop()
{
}




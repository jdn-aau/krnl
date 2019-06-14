#include <krnl.h>
 
struct k_t * p_t1;

#define STK_SIZE 200

char s1[STK_SIZE]; // stak for t1 ... and t2
 
volatile int icnt=0;

void doBlink(void) {
  static char flag = 0;

  flag = ! flag;
  digitalWrite(13,flag);
}


void t1()
{
  while (1) {
    doBlink();
    k_sleep(100);
  }
}
 
void setup() {

  Serial.begin(9600);
  pinMode(13,OUTPUT);

  k_init(1,0,0); // from now you can crt task,sem etc
 
  p_t1 = k_crt_task(t1,10,s1,200);
  Serial.println("just bef");
  k_start(10); // now we are runnning   with timer 10 msev
  Serial.println("oev");
}

void loop(void) {}  // just for compilation - will never be called

/* QED :-) */





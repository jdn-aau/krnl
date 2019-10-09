#include <krnl.h>

// External triggered ISR
// An Interrupt Service Routine is attached to pin 2
// So when pin2 is drived to ground (by a wire) an interrupt is generated.
// The ISR increment a counter and send it to a message Q
// naming ki_send .... "i" indicates it can be used in an ISR and demand interrupt to be disabled prio to call
// and that no task shift takes place in the call
// demonstrates ISR with  message Q and preemption(task shift) in the ISR
// NB Take a look on the ISR. For 1280 and 2560 it is INT4 but for 168,328,.. it's INTO
// It is taken care by a compile flag
// (c) beerware license JDN 2013
// AS USUAL Serial.print is not krnl safe !!! cant break your program

struct k_t * p_t1,*p_t2,*sem1;

#define STK_SIZE 400

char s1[STK_SIZE]; // stak for t1 ... and t2
char s2[STK_SIZE]; // stak for t1 ... and t2

volatile int icnt=0;

void doBlink(void) {
  static char flag = 0;
  flag = ! flag;
  digitalWrite(13,flag);
}


void t2()
{
  while (1) {
    // doBlink();
   // k_sleep(100);
  }
}

void t1(void) {

  while (1) {
  int i;

    k_wait(sem1,0); // wait forever
    doBlink();
  }

}

void xxx()
{

  // no local vars ?!?  ! I think
  PUSHREGS();
  if (!k_running)
    goto exitt ;

  icnt++;   
  ki_signal(sem1); 
  K_CHG_STAK();
exitt:

  POPREGS();
  RETI();
}

void initISR()
{
  
  attachInterrupt(0,xxx,FALLING);
  digitalWrite(2,HIGH);
 }

void setup() {

  Serial.begin(9600);
  pinMode(13,OUTPUT);

  k_init(2,1,0); // from now you can crt task,sem etc

  sem1 = k_crt_sem(0,10); // 

  p_t1 = k_crt_task(t1, 10, s1, STK_SIZE);
  p_t2 = k_crt_task(t2, 10, s2, STK_SIZE);

  initISR();

  Serial.println("just bef");
  k_start(10); // now we are runnning   with timer 10 msev
  //Serial.println("oev");

  // main will not come back and will sleep rest of life
}

void loop(void) {
  // just for compilation - will never be called
}

/* QED :-) */





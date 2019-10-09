
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

// It handles glotch (prel) with a requirement of at lest 10 msec between to interrupts)
// (c) beerware license JDN 2013

struct k_t * tSm1, *tSm2; 
struct k_t * p_t1, *p_t2; 

struct k_msg_t *pMsg,*pMsg2;
char mar[10*2];
char mar2[10*2];

#define STK_SIZE 200

char s1[STK_SIZE]; // stak for t1 ... and t2
char s2[STK_SIZE];

volatile int icnt=0;

volatile unsigned long nowISR=0,lastISR=0;


void doBlink(void) {
  static char flag = 0;
  flag = !flag;
	digitalWrite(13,flag);
}

void t1(void) {
  int i;
  while (1) {
    delay(100);
    if (0 <= k_receive(pMsg2,&i,-1,NULL) ) {
      doBlink();
    }
  }
}

void t2(void) {
  int i;
  i = 0;
  while (1) {
    if (0 == k_receive(pMsg,&i,0,NULL) ) {
      Serial.println(i);
      k_send(pMsg2,&i);
    }
    else 
      Serial.println("-1");
  }
}

#if defined (__AVR_ATmega2560__) || defined (__AVR_ATmega1280__)
ISR(INT4_vect,ISR_NAKED) {
#else
ISR(INT0_vect, ISR_NAKED) {
#endif
  // no local vars ?!?  ! I think
  PUSHREGS();
  if (!k_running)
    goto exitt ;
    
    nowISR = millis();
  if (10 < (nowISR - lastISR)) {
    lastISR= nowISR;
    icnt++;   
    ki_send(pMsg,(void *)&icnt);

    K_CHG_STAK();
  }
exitt:

  POPREGS();
  RETI();
}

void installISR2()
{
  DI();
  pinMode(2,INPUT);  // set som input
  digitalWrite(2,HIGH);  // enable pullup resistor

#if defined (__AVR_ATmega2560__) || defined (__AVR_ATmega1280__)
  // 1280/2560 mega pin2 intr:4, pin5 intr:5
  EIMSK |= (1 << INT4);  // enable external intr
  EICRB |= (1 << ISC41); // trigger INT4 on falling edge
#else
  EIMSK |= (1 << INT0);  // enable external intr
  EICRA |= (1 << ISC01); // trigger INT0 on falling edge
#endif

  EI();
}

void setup() {

  Serial.begin(9600);

  k_init(2,5,2); // from now you can crt task,sem etc

  tSm1 = k_crt_sem(0,10); // 
  tSm2 = k_crt_sem(0,10); // 
  p_t1 = k_crt_task(t1, 10, s1, STK_SIZE);
  p_t2 = k_crt_task(t2, 9, s2, STK_SIZE);

  pMsg = k_crt_send_Q(10,2,mar); 
  pMsg2 = k_crt_send_Q(10,2,mar2); 
  pinMode(13,OUTPUT);
  Serial.print("start ");
  Serial.println(KRNL_VRS);
  delay(2000);

  installISR2();
  Serial.println("bef gogo");

  k_start(10); // now we are runnning   with timer 10 msev
  Serial.println("shit - should not come here");

  // main will not come back and will sleep rest of life
}

void loop(void) {/* just for compilation - will never be called*/
}

/* QED :-) */




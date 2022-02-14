 
#include <krnlt.h>

// External triggered ISR
// An Interrupt Service Routine is attached to pin 2
// So when pin2 is drived to ground (by a wire) an interrupt is generated.
// The ISR increment a counter and send it to a message Q
// naming ki_send .... "i" indicates it can be used in an ISR and demand interrupt to be disabled prio to call
// and that no task shift takes place in the call
// demonstrates ISR with  message Q and preemption(task shift) in the ISR
// NB Take a look on the ISR. For 1280 and 2560 it is INT4 but for 168,328,.. it's INTO
// It is taken care by a compile flag
// (c) JDN

struct k_t * tSm1, *tSm2; 
struct k_t * p_t1, *p_t2; 

struct k_msg_t *pMsg;
char mar[10*2];

#define STK_SIZE 200

char s1[STK_SIZE]; // stak for t1 ... and t2
char s2[STK_SIZE];
 
volatile int icnt=0;


void t1(void) {
 
  while (1) {
     k_signal(tSm2);
     delay(1500);
 }
}

void t2(void) {
  int i;
  i = 0;
  pinMode(13,OUTPUT);
  k_set_sem_timer(tSm2,50);
   while (1) {
      if (0 == k_wait(tSm2,20))
      {
        Serial.print("pong ");
      }
      else {
        Serial.print("no pong ");
      }
      if (0 == k_receive(pMsg,&i,-1) )
         Serial.println(i);
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
   goto exitt;
   
    
   icnt++;   
  ki_send(pMsg,(void *)&icnt);
  K_CHG_STAK();
   exitt:

   POPREGS();
   RETI();
 }
 
void setup() {

  Serial.begin(9600);
  
  k_init(5,5,5); // from now you can crt task,sem etc
  
  tSm1 = k_crt_sem(0,10); // 
  tSm2 = k_crt_sem(0,10); // 
   
  p_t1 = k_crt_task(t1, 10, s1, STK_SIZE);
  p_t2 = k_crt_task(t2, 9, s2, STK_SIZE);
 
  // If you want to test bef start
  // Serial.print("\nfree ram: ");
  //  ii = freeRam();
  //  Serial.println(ii);
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


  pMsg = k_crt_send_Q(10,2,mar); 
 Serial.println("start");
 delay(2000);
  k_start(10); // now we are runnning   with timer 10 msev
 Serial.println("oev");

  // main will not come back and will sleep rest of life
}

void loop(void) {
  // just for compilation - will never be called
}

/* QED :-) */



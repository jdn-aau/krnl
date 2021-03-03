
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

struct k_t * p_t1, *p_t2;
struct k_msg_t   *pMsg2;
 
char mar2[10 * 2];  // 10 ints each 2 bytes   could instead write 10 *sizeof(int) 

#define STK_SIZE 200

char s1[STK_SIZE]; // stak for t1 ... and t2
char s2[STK_SIZE];

volatile int icnt = 0;


void doBlink(void) {
  static char flag = 0;
  flag != flag;
  digitalWrite(13, flag);
}

void t1(void) {
  int i;
  while (1) {
    delay(100);
    if (0 <= k_receive(pMsg2, &i, -1, NULL) ) {
      doBlink();
    }
  }
}

void t2(void) {
  int i;
  i = 0;
  while (1) {
    k_sleep(20); // just ZZZZZZZZZZZZZZZZ
    k_send(pMsg2, &i); //just send 0,1,2,3,4....
    i++; 
  }
}
 
void setup() {

  Serial.begin(9600);

  k_init(2, 0, 1); // from now you can crt task,sem etc

   p_t1 = k_crt_task(t1, 10, s1, STK_SIZE);
  p_t2 = k_crt_task(t2, 9, s2, STK_SIZE);

 
  pMsg2 = k_crt_send_Q(10, 2, mar2);
  pinMode(13, OUTPUT);
  Serial.print("start ");
  Serial.println(KRNL_VRS);
  delay(2000);

  Serial.println("bef gogo");

  k_start(10); // now we are runnning   with timer 10 msev
  Serial.println("shit - should not come here");

  // main will not come back and will sleep rest of life
}

void loop(void) {/* just for compilation - will never be called*/
}

/* QED :-) */




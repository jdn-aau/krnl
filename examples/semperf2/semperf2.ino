#include <krnl2.h>

// Performance test
// task shift fore every single signal and wait
// task1 (t1) toggles pin 13 for every 100 loop
// one loop have two task shift
// So a full square wave will have 400 task shift !
// Measured on an oscilloscope this takes 9.60 msec
// -> a semaphore operation with a task shift takes ...
// 9.60msec/400 = 24 usec !


struct k_t * tSm1;
struct k_t * p_t1, *p_t2;

#define STK_SIZE 90

char s1[STK_SIZE]; // stak for t1 ... and t2
char s2[STK_SIZE];

void t1(void) {
  int toggle=0;
  int c=0,c1=0;

  pinMode(13,OUTPUT); 

  while (1) {
    c++;
    if (c == 1000) {
      c = 0;
      c1++; 
      if (c1 == 100) {
        c1 = 0;
        digitalWrite(13, toggle == 0 ? HIGH : LOW);  
        toggle = ~toggle;
      }
    }
    k_wait(tSm1,0);
  }
}

void t2(void) {
  while (1) {
    k_signal(tSm1);
  }
}



// speed program

// led 13 is toggled every second loop of task t1
// one loop equals 2 task shift so half a period equals 2 shift
// TEST
// LED toggles approx every 4 msec which equals 200 task shift
// -> one task shift takes approx 20 usec

void setup() {
  int ii;

  Serial.begin(9600);

  k_init(5,5,5); // from now you can crt task,sem etc

  tSm1 = k_crt_sem(0,10); // 100 msec

  p_t1 = k_crt_task(t1, 9, s1, STK_SIZE);
  p_t2 = k_crt_task(t2, 10, s2, STK_SIZE);


  // If you want to test bef start
  // Serial.print("\nfree ram: ");
  //  ii = freeRam();
  //  Serial.println(ii);
  Serial.println("hh");
  k_start(1); // now we are runnning   with timer
  // main will not come back and will sleep rest of life
}

void loop(void) {
  // no one will come here
}

/* QED :-) */


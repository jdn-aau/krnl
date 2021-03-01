/* critical region - by semaphores - aka mutex
    JDN
 *  */

#include <krnl.h>

#define STK 150

#define TASKPRIO 10


struct k_t * pTask1;

 
struct k_t *timedSem1; 
 
void task1()
{
    k_set_sem_timer(timedSem1, 100);  // 100 tick aka 100 msec realtime signal from krnl

  while (1) {
    k_wait(timedSem1, 0); // wait until kick
  
    k_eat_ticks(50);  // simulate realtime code
    Serial.println(k_millis());  // NB k_millis does NOT insert leap msec as std millis

   }
}

 
void setup() {
 
  Serial.begin(115200);
  delay(2000);

  Serial.println("just bef init part");

  k_init(1,1, 0); // 2 task, 1 semaphores, 0 messaegQueues */

  pTask1 = k_crt_task(task1, 15,  STK);
 
  timedSem1 = k_crt_sem(0, 1); // 1: start value, 10: max value (clipping)
  
  Serial.println("just bef k_start");

  k_start(1); /* start krnl timer speed 1 milliseconds*/

  Serial.println("If you see this then krnl didnt start :-( ");
}

void loop() {}


/*
 * from arduino runtimelibrary - insertino of leap due to not exact 1msec
 *   timer0_millis += MILLIS_INC;
  timer0_fractt += FRACT_INC;
  if (timer0_fractt >= FRACT_MAX) {
    timer0_fractt -= FRACT_MAX;
    timer0_millis += 1;
  }
  timer0_overflow_count++;
  

#define MICROSECONDS_PER_TIMER0_OVERFLOW (1024) //clockCyclesToMicroseconds(64 * 256))

// the whole number of milliseconds per timer0 overflow
#define MILLIS_INC (MICROSECONDS_PER_TIMER0_OVERFLOW / 1000)

// the fractional number of milliseconds per timer0 overflow. we shift right
// by three to fit these numbers into a byte. (for the clock speeds we care
// about - 8 and 16 MHz - this doesn't lose precision.)
#define FRACT_INC ((MICROSECONDS_PER_TIMER0_OVERFLOW % 1000) >> 3)
#define FRACT_MAX (1000 >> 3)
  
 */

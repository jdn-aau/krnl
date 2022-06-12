#include <krnl.h>
#include <ktmrcfg.h>

#include <avr/interrupt.h>

#ifdef WDT_TIMER
#include <avr/wdt.h>
#endif



//---HW timer IRS--timer section------------------------

/*
 *   The KRNL Timer is driven by timer
 * 
 *   Install the Interrupt Service Routine (ISR) for Timer2 overflow.
 *   This is normally done by writing the address of the ISR in the
 *   interrupt vector table but conveniently done by using ISR()
 * 
 *   Timer2 reload value, globally available
 */
/***********************

   NB NB ABOUT WRAP AROUND

   Krnl maintain a milisecond timer (k_millis_counter)
   It s 32 bit unsigned long so it wraps around after 49.7 days.
   As all timing internal in krnl is relative (from now) then
   wrap around will have no influence on krnl !!!fk_eat

   NB NB ABOUT TIMERS PORTS ETC

   You can configure krnl to use timer 1,2,3,4,5

   If you want to use timer 0 then you need to

   - set KRNLTMR = 0 just below in USER CONFIGURATION PART
   - mangle with Arduino library code in ...
      hardware/arduino/avr/cores/arduino/wiring.c
      rename  ISR ... with function head:
  #if defined(__AVR_ATtiny24__) || defined(__AVR_ATtiny44__) || defined(__AVR_ATtiny84__)
        ISR(TIM0_OVF_vect)
  #else
        ISR(TIMER0_OVF_vect)
  #endif
     to something else : mayby void kurt()

   This is because krnl install an ISR on timer0 and there cant be two
   Krnl to maintain the millis counters so everything with millis, micros etc is as usual.


   Buth normally you cant use timer 0 bq it is used for millis and preallocated.

   See below

   When using a timer you must be aware of that it will prohibit you from things like
   - tone (pwm sound) uses timer2

   ... from http://blog.oscarliang.net/arduino-timer-and-interrupt-tutorial/

   Timer0:
   - 8bit timer.
   - In the Arduino world Timer0 is been used for the timer functions, like delay(), millis() and micros().
   -  If you change Timer0 registers, this may influence the Arduino timer function.
   - So you should know what you are doing.

   - 16bit timer.
   - In the Arduino world the Servo library uses Timer1 on Arduino Uno (Timer5 on Arduino Mega).

   Timer2:
   - 8bit timer like Timer0.
   - In the Arduino world the tone() function uses Timer2.

   Timer3 16 bits
   - 1280/1284P and 2560 only

   Timer4, Timer5 16 bits
   - 1280 and 2560 only

   On Uno
   - Pins 5 and 6: controlled by timer0
   - Pins 9 and 10: controlled by timer1
   - Pins 11 and 3: controlled by timer2

   On the Arduino Mega we have 6 timers and 15 PWM outputs:

   TODO pinout below need checkup

   - Pins 4 and 13: controlled by timer0
   - Pins 11 and 12: controlled by timer1
   - Pins 9 and10: controlled by timer2
   - Pin 2, 3 and 5: controlled by timer 3
   - Pin 6, 7 and 8: controlled by timer 4
   - Pin 46, 45 and 44:: controlled by timer 5

   ... from http://arduino-info.wikispaces.com/Timers-Arduino

   - Servo Library uses Timer1.
   -- You can’t use PWM on Pin 9, 10 when you use the Servo Library on an Arduino.
   -- For Arduino Mega it is a bit more difficult. The timer needed depends on the number of servos.
   -- Each timer can handle 12 servos.
   -- For the first 12 servos timer 5 will be used (losing PWM on Pin 44,45,46).
   -- For 24 Servos timer 5 and 1 will be used (losing PWM on Pin 11,12,44,45,46)..
   -- For 36 servos timer 5, 1 and 3 will be used (losing PWM on Pin 2,3,5,11,12,44,45,46)..
   -- For 48 servos all 16bit timers 5,1,3 and 4 will be used (losing all PWM pins).

   - Pin 11 has shared functionality PWM and MOSI.
   -- MOSI is needed for the SPI interface, You can’t use PWM on Pin 11 and the SPI interface at the same time on Arduino.
   -- On the Arduino Mega the SPI pins are on different pins.

   - tone() function uses at least timer2.
   -- You can’t use PWM on Pin 3,11 when you use the tone() function an Arduino and Pin 9,10 on Arduino Mega.

   SO BEWARE !!!


   PERFORMANCE

   std internal speed 1 kHz

   Rudimentary prog with one task the timer ISR takes about 21 usec
   Uno, leonardo and mega measures the same

   period:
   timer0 1024 usec for 1 msec ...
   timer1,2,3... 1007 usec for 1 msec


   >>>>>>>>>>>>

   MODIFY OF/hardware/arduino/avr/cores/arduino/wiring.c

   I have added the MOD part (MOD: modification)

   So if you go for timer0
   1)remove the // in front of #define MOD
   2) change here in krnl.h KRNLTMR to 0 for your architecture

   ONLY SUPPORT FOR AVR's (uno,leonardo,nano,mega,...)



   //#define MOD

  #if defined (MOD)

   void justFakeFctHead()
  #else
  #if defined(__AVR_ATtiny24__) || defined(__AVR_ATtiny44__) || defined(__AVR_ATtiny84__)
   ISR(TIM0_OVF_vect)
   769769876987 lalalalalalal
  #else
   ISR(TIMER0_OVF_vect)
  #endif

  #endif
   >>>>>>>>>>>>

***********************************************************/


extern struct k_t *task_pool,          // array of descriptors for tasks
*sem_pool,                      // .. for semaphores
AQ,                            // Q head for active Q
*pmain_el,                      // procesdecriptor for main
*pAQ,                           // head of activeQ (AQ)
// *pDmy,       // ref to dummy task
*pRun,                          // who is running ?
*pSleepSem;                     // one semaphor for all to sleep at
 

struct k_t *pE;

int tmr_indx;                   // for travelling Qs in tmr isr


/* from wiring.c
 *  // the prescaler is set so that timer0 ticks every 64 clock cycles, and the
 *  // the overflow handler is called every 256 ticks.
 *  #define MICROSECONDS_PER_TIMER0_OVERFLOW (clockCyclesToMicroseconds(64 * 256))
 * 
 *  // the whole number of milliseconds per timer0 overflow
 *  #define MILLIS_INC (MICROSECONDS_PER_TIMER0_OVERFLOW / 1000)
 * 
 *  // the fractional number of milliseconds per timer0 overflow. we shift right
 *  // by three to fit these numbers into a byte. (for the clock speeds we care
 *  // about - 8 and 16 MHz - this doesn't lose precision.)
 *  #define FRACT_INC ((MICROSECONDS_PER_TIMER0_OVERFLOW % 1000) >> 3)
 *  #define FRACT_MAX (1000 >> 3) which is
 * 
 */

ISR(KRNLTMRVECTOR, ISR_NAKED)   // naked so we have to supply with prolog and epilog (push pop stack of regs)
{
	PUSHREGS();                 // no local vars ! I think
	TCNTx = tcntValue;          // Reload the timer
	#ifdef WDT_TIMER
	if (k_wdt_enabled)
		wdt_reset();
	#endif
	
	if (!k_running) {
		goto exitt;
	}
	
	k_millis_counter += k_tick_size;    // my own millis counter
	
	// the following may look crazy: to go through all semaphores and tasks
	// but you may have 3-4 tasks and 3-6 semaphores in your code
	// so - seems to be efficient :-)
	// so - it's a good idea not to init krnl with more items
	// (tasks/Sem/msg descriptors than needed)
	
	
	pE = sem_pool;              // Semaphore timer - check timers on semaphores
	
	for (tmr_indx = 0; tmr_indx < nr_sem; tmr_indx++) {
		if (0 < pE->cnt2)       // timer on semaphore ?
		{
			pE->cnt2--;         // yep  decrement it
			if (pE->cnt2 <= 0)  // timeout  ?
			{
				pE->cnt2 = pE->cnt3;    // preset again - if cnt3 == 0 and >= 0 the rep timer
				ki_signal(pE);  //issue a signal to the semaphore
			}
		}
		pE++;
	}
	
	pE = task_pool;             // Chk timers on tasks - they may be one shoot waiting
	
	for (tmr_indx = 0; tmr_indx < nr_task; tmr_indx++) {
		if (0 < pE->cnt2)       // timer active on task ?
		{
			pE->cnt2--;         // yep so let us do one down count
			if (pE->cnt2 <= 0)  // timeout ? ( == 0 )
			{
				((struct k_t *) (pE->cnt3))->cnt1++;    // leaving sem so adjust semcount on sem
				prio_enQ(pAQ, deQ(pE)); // and rip task of semQ and insert in activeQ
				pE->cnt2 = -1;  // indicate timeout in this semQ for the task that is restartet
			}
		}
		pE++;
	}
	
	prio_enQ(pAQ, deQ(pRun));   // round robbin
	
	K_CHG_STAK();
	
	exitt:
	
	POPREGS();
	RETI();
}

/**HERE**/

//----------------------------------------------------------------------------
// inspired from ...
// http://arduinomega.blogspot.dk/2011/05/timer2-and-overflow-interrupt-lets-get.html
// Inspiration from  http://popdevelop.com/2010/04/mastering-timer-interrupts-on-the-arduino/
// Inspiration from "Multitasking on an AVR" by Richard Barry, March 2004
// and  http://www.control.aau.dk/~jdn/kernels/krnl/
//----------------------------------------------------------------------------
// avrfreaks.net
// and my old kernel from last century
// and a lot other stuff
// basic concept from my own very old kernels dated back bef millenium

unsigned long ki_millis(void)
{
	unsigned long l;
	DI();                       // just to be sure
	l = k_millis_counter;
	return l;
}

unsigned long k_millis(void)
{
	unsigned long l;
	
	l = ki_millis(); 
	EI();
	return l;
}

int k_tmrInfo(void)
{
	return (KRNLTMR);
}





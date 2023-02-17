/*******************************************************
 *                                                     *
 *                                                     *
 *             | |/ /___|  _ \| \ | | ___| |           *
 *             | ' // _ \ |_) |  \| |/ _ \ |           *
 *             | . \  __/  _ <| |\  |  __/ |___        *
 *             |_|\_\___|_| \_\_| \_|\___|_____|       *
 *                                                     *
 *                                                     *
 * you are watching krnl.c                             *
 *                                                     *
 *       March 2015,2016,..,2018                       *
 *       Author: jdn                                   *
 *      final sep 2022                                 *
 *.                                                    *
 *******************************************************

  you are watching krnl.c
 
        March 2015,2016,..,2018
        Author: jdn
        final sep 2022

        2022 ..
   this version adapted for Arduino

   (C) 2012,2013,2014
      2017,2018,2019,2021,2022


  IF YOU ARE LUCKY LOOK HERE

  https://github.com/jdn-aau/krnl



   Jens Dalsgaard Nielsen <jdn@es.aau.dk>
   http://es.aau.dk/staff/jdn
   Section of Automation & Control
   Aalborg University,
   Denmark

   "THE BEER-WARE LICENSE" (frit efter PHK)
   <jdn@es.aau.dk> wrote this file. As long as you
   retain this notice you can do whatever you want
   with this stuff. If we meet some day, and you think
   this stuff is worth it ...
   you can buy me a beer in return :-)
   or if you are real happy then ...
   single malt will be well received :-)

   Use it at your own risk - no warranty

   nice info...
   http://www.nongnu.org/avr-libc/user-manual/FAQ.html#faq_cplusplus
   at 20090611
   - k_eat_time now eats time in quants of krnl tick speed but not one quant
boundary
   - added k_malloc and k_free
      k_free dont free memory bq we do not want fragmentation
      so DONT USE k_free
   - k_malloc and k_free are weak functions so you can just add your own
versions
   - watchdog is enabled (2sec and reset in krnl timer) in weak function
k_enable_wdt

************************************************************************/

#include "krnl.h"


#ifdef WDT_TIMER 
#include <avr/wdt.h>

#endif

#include <avr/interrupt.h>

#include <util/delay.h>

#include <stdlib.h>

// CPU frequency - for adjusting delays
#if (F_CPU == 8000000)
#pragma message("krnl detected 8 MHz")
#endif

#if (KRNL_VRS != 20231234)
#error "KRNL VERSION NOT UPDATED in krnl.c "
#endif

/*
    #ifdef __cplusplus
    extern "C" {
    #endif
*/

/* which timer to use for krnl heartbeat
    timer 0 ( 8 bit) is normally used by millis - avoid !
        or modify TIMER0 in wiring.c
    timer 1 (16 bit)  
    timer 2 ( 8 bit)  DEFAULT
    timer 3 (16 bit) 1280/1284P/2560 only (MEGA)
    timer 4 (16 bit) 1280/2560 only (MEGA)
    timer 5 (16 bit) 1280/2560 only (MEGA)
*/

#if (KRNLTMR == 0)
JDN SIGER NOGO IN SIMPLIFY VRS
#pragme err "tbf 0 "

// normally not goood bq of arduino sys timer is on timer 0so you wil get a
// compile error 8 bit timer !!!
//#define KRNLTMRVECTOR TIMER0_OVF_vect
/* we use setting from original timer0
    #define TCNTx TCNT0
    #define TCCRxA TCCR0A
    #define TCCRxB TCCR0B
    #define TCNTx TCNT0
    #define OCRxA OCR0A
    #define TIMSKx TIMSK0
    #define TOIEx TOIE0
    #define PRESCALE 0x07
    #define COUNTMAX 0xff
    #define DIVV 15.625
    #define DIVV8 7.812
*/
#elif (KRNLTMR == 1)

empty

#elif (KRNLTMR == 2)

// 8 bit timer !!! STANDARD
// standard for krnl CHECK which pwm, tone etc is on this timer

#pragma warn "krnl timer on timer 2 - check PWM "
#define KRNLTMRVECTOR TIMER2_OVF_vect
#define TCNTx TCNT2
#define TCCRxA TCCR2A
#define TCCRxB TCCR2B
#define TCNTx TCNT2
#define OCRxA OCR2A
#define TIMSKx TIMSK2
#define TOIEx TOIE2
#define CNT_1MSEC 240
#define CNT_10MSEC 99 

#define PRESCALE  ( ( 1<< CS22) | (1 << CS21) | ( 1<< CS20))

#elif (KRNLTMR == 3)

// 32u4
#define KRNLTMRVECTOR TIMER3_OVF_vect
#define TCNTx TCNT3
#define TCCRxA TCCR3A
#define TCCRxB TCCR3B
#define TCNTx TCNT3
#define OCRxA OCR3A
#define TIMSKx TIMSK3
#define TOIEx TOIE3
#define PRESCALE  ( ( 1<< CS02) | (1 << CS01) | ( 1<< CS00))
#define COUNTMAX 0xffff
#define CNT_1MSEC 65520
#define CNT_10MSEC 65381
  
#elif (KRNLTMR == 4)

#pragme err "tbf 4"
 
#elif (KRNLTMR == 5)


#pragme err "tbf 5"
 
#else

#pragma err "KRNL: no valid tmr selected"

#endif

// ---------- KRNL VARS ----------------
// ---------- KRNL VARS ----------------
// ---------- KRNL VARS ----------------
// ---------- KRNL VARS ----------------

 
    struct k_t *task_pool, // array of descriptors for tasks
    *sem_pool,             // .. for semaphores
    AQ,                    // Q head for active Q
    *pmain_el,             // procesdecriptor for main eq dummy
    *pAQ,                  // head of activeQ (AQ)
    *pRun,                 // who is running ?
    *pSleepSem;            // one semaphor for all to sleep at

struct k_msg_t *send_pool; // ptr to array for msg sem pool

int k_task, k_sem, k_msg; // how many did you request in k_init of descriptors ?
char nr_task = 0, nr_sem = 0, nr_send = 0; // counters for created KeRNeL items

volatile char k_running = 0, k_err_cnt = 0;

#ifdef  WDT_TIMER

volatile char k_wdt_enabled = 1;

#endif
volatile unsigned char tcntValue; // counters for timer system
unsigned long k_millis_counter = 0;
unsigned int k_tick_size;

unsigned char k_coopFlag=0;

int tmr_indx; // for travelling Qs in tmr isr


// --------- ENDE KRNL VARS --------------
// --------- ENDE KRNL VARS --------------
// --------- ENDE KRNL VARS --------------

//---QOPS--- double chained lists with qhead as a element
/*      -<------<-------<--------<---------<------------<-
*       |                                                |
*       \/                                               |
*     QHEAD (next)-->first-elm (next)-->next-elm(next) --|
*        ^                   ^
*        ------------(pred)  -----------(pred)  char
* 
*/

void enQ(struct k_t *Q, struct k_t *el) {
  el->next = Q;
  el->pred = Q->pred;
  Q->pred->next = el;
  Q->pred = el;
}

struct k_t *deQ(struct k_t *el) {
  el->pred->next = el->next;
  el->next->pred = el->pred;

  return (el);
}

void prio_enQ(struct k_t *Q, struct k_t *el) {
  char prio = el->prio;

  Q = Q->next; // bq first elm is Q head itself

  while (Q->prio <= prio) // find place before next with lower prio
  {
    Q = Q->next;
  }

  el->next = Q; // insert before element referred by Q
  el->pred = Q->pred;
  Q->pred->next = el;
  Q->pred = el;
}

/***** eat time ***/
/* real eat time so if you are blocked by higher priority task
the blockug time do not count
Can be use for simulate CPU usage
*/
void k_eat_msec(unsigned int eatTime) {
  while (10 < eatTime) {
    delayMicroseconds(10000);
    eatTime -= 10;
  }
  delayMicroseconds(eatTime * 1000);
}

//---HW timer IRS--timer section------------------------

/*
    The KRNL Timer is driven by timer

    Install the Interrupt Service Routine (ISR) for Timer2 overflow.
    This is normally done by writing the address of the ISR in the
    interrupt vector table but conveniently done by using ISR()

    Timer2 reload value, globally available
*/


/* from wiring.c
   // the prescaler is set so that timer0 ticks every 64 clock cycles, and the
   // the overflow handler is called every 256 ticks.
   #define MICROSECONDS_PER_TIMER0_OVERFLOW (clockCyclesToMicroseconds(64 *
   256))

   // the whole number of milliseconds per timer0 overflow
   #define MILLIS_INC (MICROSECONDS_PER_TIMER0_OVERFLOW / 1000)

   // the fractional number of milliseconds per timer0 overflow. we shift right
   // by three to fit these numbers into a byte. (for the clock speeds we care
   // about - 8 and 16 MHz - this doesn't lose precision.)
   #define FRACT_INC ((MICROSECONDS_PER_TIMER0_OVERFLOW % 1000) >> 3)
   #define FRACT_MAX (1000 >> 3) which is

*/
struct k_t *pE;  // used only in ISR as "temporary var"

ISR(KRNLTMRVECTOR, ISR_NAKED) // naked so we have to supply with prolog and
                              // epilog (push pop stack of regs)
{
  PUSHREGS();        // no local vars ! I think
  
  if (!k_running) {
    goto exitt;
  }
  #if (K_TICK == 1)
  TCNT2 = CNT_1MSEC; // Reload the timer  1 msec
  #elif (K_TICK == 10)
    TCNT2 = CNT_10MSEC; // Reload the timer  10 msec
  #else
  #pragma err "bad K_TICK - you can only select 1 or 10
  #endif
 
#ifdef WDT_TIMER
  if (k_wdt_enabled)
    wdt_reset();
#endif


  k_millis_counter += k_tick_size; // my own millis counter

  // the following may look crazy: to go throuh all semaphores and tasks
  // but you may have 3-4 tasks and 3-6 semaphores in your code
  // so - seems to be efficient :-)
  // so - it's a good idea not to init krnl with more items
  // (tasks/Sem/msg descriptors than needed)

  pE = sem_pool; // Semaphore timer - check timers on semaphores

  for (tmr_indx = 0; tmr_indx < nr_sem; tmr_indx++) {
    if (0 < pE->cnt2) // timer on semaphore ?
    {
      pE->cnt2--;        // yep  decrement it
      if (pE->cnt2 <= 0) // timeout  ?
      {
        pE->cnt2 =
            pE->cnt3;  // preset again - if cnt3 == 0 and >= 0 the rep timer
        ki_signal(pE); // issue a signal to the semaphore
      }
    }
    pE++;
  }

  pE = task_pool; // Chk timers on tasks - they may be one shoot waiting

  for (tmr_indx = 0; tmr_indx < nr_task; tmr_indx++) {
    if (0 < pE->cnt2) // timer active on task ?
    {
      pE->cnt2--;        // yep so let us do one down count
      if (pE->cnt2 <= 0) // timeout ? ( == 0 )
      {
        ((struct k_t *)(pE->cnt3))
            ->cnt1++;           // leaving sem so adjust semcount on sem
        prio_enQ(pAQ, deQ(pE)); // and rip task of semQ and insert in activeQ
        pE->cnt2 =
            -1; // indicate timeout in this semQ for the task that is restartet
      }
    }
    pE++;
  }

  if (!k_coopFlag) {
    prio_enQ(pAQ, deQ(pRun)); // round robbin
    K_CHG_STAK();             // let first in AQ run
  }

exitt:

  POPREGS();
  RETI();
}

int k_ticksize(void)
{
   return k_tick_size;
}

/**HERE**/

//----------------------------------------------------------------------------
// inspired from ...
// http://arduinomega.blogspot.dk/2011/05/timer2-and-overflow-interrupt-lets-get.html
// Inspiration from
// http://popdevelop.com/2010/04/mastering-timer-interrupts-on-the-arduino/
// Inspiration from "Multitasking on an AVR" by Richard Barry, March 2004
// and  http://www.control.aau.dk/~jdn/kernels/krnl/
//----------------------------------------------------------------------------
// avrfreaks.net
// and my old kernel from last century
// and a lot other stuff
// basic concept from my own very old kernels dated back bef millenium

void __attribute__((naked, noinline)) ki_task_shift(void) {
  PUSHREGS();    // push task regs on stak so we are rdy to task shift
  K_CHG_STAK();  // find taskstak for task in front of activeQ
  POPREGS();     // restore regs
  RETI();        // and do a reti NB this also enables interrupt !!!
}

#ifdef BACKSTOPPER
void jumper() {

  while (1) {
    (*(pRun->pt))(); // call task
    // you can just jump back to task, but your local vars in 
    // the task body will be wiped out
    // or
  // just hanging here

   #ifdef STOP_IN_BACKSTOPPER
   k_set_prio(ZOMBI_PRIO); // priority lower than dummy so you just stops
   while (1) ; // just in case
   #endif
  }
}
#endif

struct k_t *k_crt_task(void (*pTask)(void), char prio, char *pStk,
                       int stkSize) {
  struct k_t *pT;

  int i;
  char *s;

  // sanity chek
  if ((k_running) || ((prio <= 0) || (DMY_PRIO <= prio)) ||
      (k_task <= nr_task)) {
    goto badexit;
  }

  if (pStk == NULL) {  // you didnt give me a stack
    goto badexit;
  }
  
  pT = task_pool + nr_task; // lets take a task descriptor
  pT->nr = nr_task;
  nr_task++;

  pT->cnt2 = 0; // no time out running on you for the time being
  pT->cnt3 = 0; // no time out semaphore for you

  pT->cnt1 = (int)(pStk); // ref to my stack


// ----- HW DEPENDENT START --------------
// ----- HW DEPENDENT START --------------

// paint stack with hash code to be used by k_unused_stak()
  for (i = 0; i < stkSize; i++) {
    pStk[i] = STAK_HASH;
  }

  s = pStk + stkSize - 1; // now we point on top of stak
  *(s--) = 0x00;          // 1 byte safety distance :-)

  // an interrupt do only push PC on stack by HW - can be 2 or 3 bytes
  // depending of 368/.../1280/2560
#ifdef BACKSTOPPER
  pT->pt = pTask;
  *(s--) = lo8(jumper); //  so top now holds address of function
  *(s--) = hi8(jumper); // which is code body for task
#else
  *(s--) = lo8(pTask); //  so top now holds address of function
  *(s--) = hi8(pTask); // which is code body for task
#endif
  // NB  NB 2560 use 3 byte for call/ret addresses the rest only 2
#if defined(__AVR_ATmega2560__) || defined(__AVR_ATmega2561__) 
  (s--) = EIND; // best guess : 3 byte addresses !!! or just 0
#endif

  // r1 is the socalled zero value register
  // see https://gcc.gnu.org/wiki/avr-gcc
  // can tmp be non zero (multiplication etc)
  *(s--) = 0x00; // r1
  *(s--) = 0x00; // r0
  *(s--) = 0x00; // sreg

  // 1280 and 2560 need to save rampz reg just in case
#if defined(__AVR_ATmega2560__) || defined(__AVR_ATmega1280__) ||              \
    defined(__AVR_ATmega1284P__) || defined(__AVR_ATmega2561__) 
  (s--) = RAMPZ; // best guess  0x3b
#endif

#if defined(__AVR_ATmega2560__) || defined(__AVR_ATmega1280__) ||              \
    defined(__AVR_ATmega2561__) 
  (s--) = EIND; // best guess 0x3c
#endif

  // rest of the resg on the stack
  for (i = 0; i < 30; i++) // r2-r31 = 30 regs
  {
    *(s--) = 0x00;
  }

  pT->sp_lo = lo8(s); // now we just need to save stakptr
  pT->sp_hi = hi8(s); // in thread descriptor

// ----- HW DEPENDENT ENDE --------------
// ----- HW DEPENDENT ENDE --------------
// ----- HW DEPENDENT ENDE --------------
 
  pT->prio = prio; 
  pT->maxv = (int)prio; // maxv for holding org prio for inheritance
  prio_enQ(pAQ, pT); // and put task in active Q

  return (pT);

badexit:
  k_err_cnt++;
  return (NULL);
}

int freeRam(void) {
  extern int __heap_start, *__brkval;
  int x, v;
  // NASTY
  x = ((int)&v - (__brkval == 0 ? (int)&__heap_start
                                : (int)__brkval)); // to remove warning
  return x;
  // hw specific :-/
  // return ((int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval));

  /* from
     http://www.noah.org/wiki/Arduino_notes#debug_memory_problems_.28out_of_RAM.29
     int freeRam () {
     // __brkval is the address of the top of the heap if memory has been
     allocated.
     // If __brkval is zero then it means malloc has not used any memory yet, so
     // we look at the address of __heap_start.
     extern int __heap_start
     extern int *__brkval; // address of the top of heap
     int stack_top;
     return (int)&stack_top - ((int)__brkval == 0 ? (int)&__heap_start :
     (int)__brkval);
     }
   */
}

int k_sleep(int time) {
  int r;
  if (time <= 0)
    return -1;
  r = k_wait(pSleepSem, time);
  if (r == -1) // timeout ? yes :-)
    return 0;
}

int k_unused_stak(struct k_t *t) {
  int i;
  char *pstk;

  if (t) // another task or yourself - NO CHK of validity !!!!!
  {
    pstk = (char *)(t->cnt1);
  } else {
    pstk = (char *)(pRun->cnt1);
  }

  DI();
  // look for stack paint
  i = 0;
  while (*pstk == STAK_HASH) {
    pstk++;
    i++;
  }
  EI();

  return (i);
}

int k_set_prio(char prio) {
  int i;

  if (!k_running) {
    return (-2);
  }

  DI();

  if ((prio <= 0) || (DMY_PRIO <= prio)) // not legal value my friend
  {
    EI();
    return (-1);
  }
  i = pRun->prio;

  pRun->prio = prio;

  prio_enQ(pAQ, deQ(pRun));
  ki_task_shift();

  EI();

  return (i);
}

int k_mut_ceil_set(struct k_t *sem, char prio) {
  // NB NB assume semaphore is created prior to this call
  // NO CHECK - no mercy !!!
  // work only BEFORE k_start
  if (k_running) {
    return (-2); // bad bad
  }

  if ((prio <= 0) || (DMY_PRIO <= prio)) {
    return (-1); // bad bad
  }
  sem->ceiling_prio = prio;
  return 0; // OK
}

struct k_t *k_crt_sem(int init_val, int maxvalue) {
  struct k_t *sem;

  if (k_running) {
    return (NULL);
  }

  if ((maxvalue < init_val) 
  	|| (MAX_SEM_VAL < maxvalue)
  	|| (init_val < 0) 
  	|| (maxvalue < 0) ) {
    goto badexit;
  }

  if (k_sem <= nr_sem) // no vacant in buf
  {
    goto badexit;
  }

  sem = sem_pool + nr_sem; // allocate it
  sem->nr = nr_sem;
  nr_sem++;

  sem->cnt2 = 0;               // no timer running
  sem->next = sem->pred = sem; // point at myself == no one in Q
  sem->prio = QHD_PRIO;
  sem->cnt1 = init_val;

  sem->maxv = maxvalue;
  sem->clip = 0;

  sem->ceiling_prio = -1; // to indicate it is not active
  sem->saved_prio = -1;

  return (sem);

badexit:
  k_err_cnt++; // one more error so krnl will not start

  return (NULL);
}

int k_set_sem_timer(struct k_t *sem, int val) {
  // there is no k_stop_sem_timer fct just call with val== 0 for stopping timer
  // fct

  if (val <= 0) {
    return -1;
  }

  DI();
  if (0 < sem->cnt1) {
    sem->cnt1 = 0; // reset
  }

  sem->cnt2 = sem->cnt3 = val; // if 0 then timer is not running -
  EI();

  return (0);
}

int ki_signal(struct k_t *sem) {
  DI(); // just in case
  if (sem->cnt1 < sem->maxv) {

    sem->cnt1++; // Salute to Dijkstra

#ifdef KRNLBUG
    k_sem_signal(sem->nr, sem->cnt1); // call to breakout functino
#endif

    if (sem->cnt1 <= 0) {
      sem->next->cnt2 = 0; // return code == ok in waiting tasks pocket(cnt2)
      prio_enQ(pAQ, deQ(sem->next));
      return (0); // a task was waiting and is now in AQ
    } else {
      return (1); // just delivered a signal - no task was waiting
    }
  } // CLIP :-(
  else {
    if (sem->clip < MAX_SEM_VAL + 1) {
      sem->clip++;
    }
    // here we are on bad clip failure no signal takes place
    // signal is lost !!!
#ifdef KRNLBUG
    k_sem_clip(sem->nr, sem->clip);
#endif
    return (-1);
  }
}

int k_signal(struct k_t *sem) {
  int res;

  DI();

  res = ki_signal(sem); // 1: ok no task to AQ, 0: ok task to AQ

  if (res == 0) {
    ki_task_shift(); // bq maybe started task has higher prio than me
  }

  EI();

  return (res);
}

/* normally ki_wait should not be used by user */
int ki_wait(struct k_t *sem, int timeout) {
  DI();

  if (0 < sem->cnt1) {
    sem->cnt1--; // Salute to Dijkstra
    return (1);  // ok: 1 bq we are not suspended
  }

  if (timeout < 0) // no luck, dont want to wait so bye bye
  {
    return (-1); // will not wait so bad luck
  }
  // from here we want to wait
  pRun->cnt2 = timeout; //  0 == wait forever

  if (timeout) {           //  so we can be removed if timeout occurs
    pRun->cnt3 = (int)sem; // nasty keep ref to semaphore in task stomach
  }

  sem->cnt1--; // Salute to Diocjkstra

  enQ(sem, deQ(pRun));
  ki_task_shift();

  // back again - have semaphore received signal or timeout ?
  pRun->cnt3 = 0; // reset ref to timer semaphore

  return ((char)(pRun->cnt2)); // 0: ok , -1: timeout
}

int k_wait(struct k_t *sem, int timeout) {
  int retval;
  DI();
  retval = ki_wait(sem, timeout);
  EI();
  return retval; // 0: ok, -1: timeout
}

int k_clear_sem(struct k_t *sem) {
  int retval;
  DI();
  retval = sem->cnt1;
  if (0 < retval) {
    sem->clip += retval;
    sem->cnt1 = 0;
  }
  EI();
  return retval; // 0: ok, -1: timeout
}

int k_wait2(struct k_t *sem, int timeout, int *nrClip) {
  int retval;
  DI();
  retval = ki_wait(sem, timeout);
  if (nrClip) {
    *nrClip = sem->clip;
    sem->clip = 0;
  }
  EI();
  return retval; // 0: ok, -1: timeout
}

struct k_t *k_crt_mut(int ceiling_prio, int init_val, int maxvalue) {
  struct k_t *mut;

  if (k_running) {
    return (NULL);
  }

  if (ceiling_prio < 0) {
    k_err_cnt++;
    return NULL;
  }

  mut = k_crt_sem(init_val, maxvalue);

  if (mut == NULL) {
    k_err_cnt++;
    return NULL;
  }
  mut->ceiling_prio = ceiling_prio;

  return mut;
}

int k_mut_ceil(struct k_t *sem, int timeout, void (*fct)(void)) {
  int r;
  r = k_mut_ceil_enter(sem, timeout);
  if (r < 0) {
    return r; // bad bad
  }

  (*fct)(); // call mutex function

  k_mut_ceil_leave(sem);
  return r;
}

int k_mut_ceil_enter(struct k_t *sem, int timeout) {
  int retval;
  DI();

  if (sem->ceiling_prio < 0) {
    EI();
    return CEILINGFAILNOTCEIL;
  }

  if (pRun->prio <
      sem->ceiling_prio) { // I have higher priority than ceiling :-(
    EI();
    return CEILINGFAILPRIO;
  }
  // now we play imm ceiling protocol
  sem->saved_prio = pRun->prio; // do im ceiling
  pRun->prio =
      sem->ceiling_prio; // dont need to reinsert in AQ bq ceil prio is higher
                         // or equal to mine and Im already in front of AQ
  prio_enQ(pAQ, deQ(pRun)); // resinsert me in AQ acc to nwe(old) priority
  retval = ki_wait(sem, timeout);
  // coming back interrupt is still disabled !

  // chk if we did get semaphore
  if (retval < 0) {               // NOPE we did not
    pRun->prio = sem->saved_prio; // reset to my old priority
    prio_enQ(pAQ, deQ(pRun));     // reinsert me in AQ acc to nwe(old) priority
    ki_task_shift(); // bq maybe started task has higher prio than me
  }
  EI();
  return retval; // 0(has waited),1(straight through) : ok, -1: timeout
}

int k_mut_ceil_leave(struct k_t *sem) {
  int res;

  DI();
  res = ki_signal(sem); // 1: ok no task to AQ, 0: ok task to AQ

  // coming back interrupt is still disabled !
  pRun->prio = sem->saved_prio; // reset to my old priority

  prio_enQ(pAQ, deQ(pRun)); // resinsert me in AQ acc to nwe(old) priority

  ki_task_shift(); // bq maybe started task has higher prio than me

  EI();

  return (res);
}

int k_sem_signals_lost(struct k_t *sem) {
  int x;

  DI();

  x = sem->clip;
  sem->clip = 0;

  EI();
  return x;
}

int ki_semval(struct k_t *sem) {
  DI(); // dont remove this - bq k_semval depends on it

  return (sem->cnt1);
}

int k_semval(struct k_t *sem) {
  int v;
  v = ki_semval(sem);
  EI();
  return v;
}

int ki_msg_count(struct k_msg_t *m) {
  DI(); // dont remove this - bq k_semval depends on it
  return m->cnt;
}

int k_msg_count(struct k_msg_t *m) {
  int v;
  // not needed to DI - its in ki_msg_count ... DI ();
  v = ki_msg_count(m);
  EI();
  return v;
}

struct k_msg_t *k_crt_send_Q(int nr_el, int el_size, void *pBuf) {
  struct k_msg_t *pMsg;

  if (k_running) {
    return (NULL);
  }

  if (k_msg <= nr_send) {
    goto errexit;
  }

  if (k_sem <= nr_sem) {
    goto errexit;
  }

  pMsg = send_pool + nr_send;
  pMsg->nr = nr_send; // I am element nr nr_send in msgQ pool
  nr_send++;

  pMsg->sem =
      k_crt_sem(0, nr_el); // we are using a sem for sync part snd <-> rcv

  if (pMsg->sem == NULL) {
    goto errexit;
  }

  pMsg->pBuf = (char *)pBuf;
  pMsg->r = pMsg->w = -1;
  pMsg->el_size = el_size;
  pMsg->nr_el = nr_el;
  pMsg->lost_msg = 0;
  pMsg->cnt = 0; // count nr elm in Q

  return (pMsg);

errexit:
  k_err_cnt++;
  return (NULL);
}

int ki_clear_msg_Q(struct k_msg_t *pB) {
  int ret;
  if (k_running) {
    return -2;
  }

  ret = pB->cnt;
  if (0 < ret) { // messages pending s0
    pB->lost_msg = 0;
    pB->cnt = 0; // reset
    pB->r = pB->w = -1;
    // clear sem - can do it bq no one is waiting bq 0 < ret  == pending
    // messages
    pB->sem->cnt1 = 0; // Serious NASTY
  }
  return ret;
}

int k_clear_msg_Q(struct k_msg_t *pB) {
  int r;
  DI();
  r = ki_clear_msg_Q(pB);
  EI();
  return r;
}

char ki_send(struct k_msg_t *pB, void *el) {

  int i;
  char *pSrc, *pDst;

  if (pB->nr_el <= pB->cnt) {
    // nope - no room for a putting new msg in Q ?
    if (pB->lost_msg < MAX_SEM_VAL) {
      pB->lost_msg++;
    }
#ifdef KRNLBUG
    k_send_Q_clip(pB->nr, pB->lost_msg);
#endif
    return (-1); // nope
  } else {

    pB->cnt++;

    pSrc = (char *)el;

    pB->w++;
    if (pB->nr_el <= pB->w) // simple wrap around
    {
      pB->w = 0;
    }

    pDst = pB->pBuf +
           (pB->w * pB->el_size); // calculate where we shall put msg in ringbuf

    for (i = 0; i < pB->el_size; i++) {
      // copy to Q
      *(pDst++) = *(pSrc++);
    }
    return (ki_signal(pB->sem)); // indicate a new msg is in Q
  }
}

char k_send(struct k_msg_t *pB, void *el) {
  char res;

  DI();

  res = ki_send(pB, el);
  if (res == 0) // if new task in AQ == someone was waiting for msg
  {             // if 1 then nobody was waiting so no neeed for task shift
    ki_task_shift();
  }

  EI();
  return (res);
}

char ki_receive(struct k_msg_t *pB, void *el, int *lost_msg) {
  int i;
  char r, *pSrc, *pDst;

  // can be called from ISR bq no blocking
  DI(); // just to be sure

  if (0 <= (r = ki_wait(pB->sem, -1))) {

    pDst = (char *)el;
    pB->r++;
    pB->cnt--; // got one

    if (pB->nr_el <= pB->r) {
      pB->r = 0;
    }

    pSrc = pB->pBuf + pB->r * pB->el_size;

    for (i = 0; i < pB->el_size; i++) {
      *(pDst++) = *(pSrc++);
    }
    if (lost_msg) {
      *lost_msg = pB->lost_msg;
      pB->lost_msg = 0;
    }
    return (r); // yes
  }

  return (-1); // nothing for you my friend
}

char k_receive(struct k_msg_t *pB, void *el, int timeout, int *lost_msg) {

  int i;
  char r, *pSrc, *pDst;

  DI();

  if (0 <= (r = ki_wait(pB->sem, timeout))) {
    // yes we did get a msg :-)
    // ki_wait bq then intr is not enabled when coming back

    pDst = (char *)el;
    pB->r++;
    pB->cnt--; // got one

    if (pB->nr_el <= pB->r) {
      pB->r = 0;
    }

    pSrc = pB->pBuf + pB->r * pB->el_size;

    for (i = 0; i < pB->el_size; i++) {
      *(pDst++) = *(pSrc++);
    }

    if (lost_msg) {
      *lost_msg = pB->lost_msg;
      pB->lost_msg = 0;
    }

    EI();
    return (r); // 1 if no suspension bq msg was already present, 0: ok  if you
                // have waited on msg
  }

  EI();
  return (-1); // nothing for you my friend
}

#ifdef READERWRITER
// https://en.wikipedia.org/wiki/Readers%E2%80%93writers_problem

void k_rwInit(struct k_rwlock_t *lock) {
  lock->nrReaders = 0;
  lock->rdwrSem = k_crt_sem(1, 2);
  lock->rdSem = k_crt_sem(1, 2);
  lock->fifoSem = k_crt_sem(1, 2);
}

int k_rwRdEnter(struct k_rwlock_t *lock, int timeout) {
  // timeout tbi later - if...
  k_wait(lock->fifoSem, 0);
  k_wait(lock->rdSem, 0);

  lock->nrReaders++;
  if (lock->nrReaders == 1)
    k_wait(lock->rdwrSem, 0);
  k_signal(lock->fifoSem);
  k_signal(lock->rdSem);
}

int k_rwWrEnter(struct k_rwlock_t *lock, int timeout) {
  k_wait(lock->fifoSem, 0);
  k_wait(lock->rdwrSem, 0);
  k_signal(lock->fifoSem);
}

int k_rwRdLeave(struct k_rwlock_t *lock) {
  k_wait(lock->rdSem, 0);
  lock->nrReaders--;
  if (lock->nrReaders == 0) {
    k_signal(lock->rdwrSem);
  }
  k_signal(lock->rdSem);
}

int k_rwWrLeave(struct k_rwlock_t *lock) { k_signal(lock->rdwrSem); }

#endif

void k_round_robbin(void) {

  // reinsert running task in activeQ if round robbin is selected
  DI();

  prio_enQ(pAQ, deQ(pRun));
  ki_task_shift();

  EI();
}

void k_release(void) {

  // let next run
  DI();
  ki_task_shift();
  EI();
}

/* NASTY void from vrs 2001 it is main itself can be changed back
 */

/*
   void
   dummy_task (void)
   {
   while (1) {
    k_round_robbin ();
   }
   }
*/

// char dmy_stk[DMY_STK_SZ]; // dmy duty is nwo maintained by org main

int k_init(int nrTask, int nrSem, int nrMsg) {
  if (k_running) {
    return (-666);
  }

  k_task = nrTask + 1;       // +1 due to dummy
  k_sem = nrSem + nrMsg + 1; // due to that every msgQ has a builtin semaphore
  k_msg = nrMsg + 1;         // to align so first user msgQ has index 1
  nr_send++; // to align so we waste one but ... better equal access
  task_pool = (struct k_t *)malloc(k_task * sizeof(struct k_t));
  sem_pool = (struct k_t *)malloc(k_sem * sizeof(struct k_t));
  send_pool = (struct k_msg_t *)malloc(k_msg * sizeof(struct k_msg_t));

  // we dont accept any errors
  if ((task_pool == NULL) || (sem_pool == NULL) || (send_pool == NULL)) {
    k_err_cnt++;
    goto leave;
  }
  // init AQ as empty double chained list
  pAQ = &AQ;
  pAQ->next = pAQ->pred = pAQ;
  pAQ->prio = QHD_PRIO;

  // crt dummy
  // pDmy = k_crt_task (dummy_task, DMY_PRIO, DMY_STK_SZ);

  pmain_el = task_pool;
  pmain_el->nr = 0;
  pmain_el->cnt2 = pmain_el->cnt3 = 0;
  nr_task++;
  pmain_el->prio = DMY_PRIO; // main is dummy
  prio_enQ(pAQ, pmain_el);

  pSleepSem = k_crt_sem(0, 10);

leave:
  return k_err_cnt;
}


int k_start() {
  /*
  https://microcontrollerslab.com/arduino-timer-interrupts-tutorial/
  https://wolles-elektronikkiste.de/en/timer-and-pwm-part-1-8-bit-timer0-2
  
     FOR TIMER 2 !!!  not TIMER0(8 bit) AND NOT TIMER 1(16bit) !!
     TCCRxB
     48,88,168,328, 1280,2560
     timer 0 and 2 has same prescaler config:
     8 bit:
     0 0 0 No clock source (Timer/Counter stopped).
     0 0 1 clk T2S /(No prescaling)
     0 1 0 clk T2S /8 (From prescaler)      16M/2 = 2000000 intr/sec at 1 downcount
     0 1 1 clk T2S /32 (From prescaler)      500000 intr/sec ...
     1 0 0 clk T2S /64 (From prescaler)      250000
     1 0 1 clk T2S /128 (From prescaler)     125000
     1 1 0 clk T 2 S /256 (From prescaler)    62500
     1 1 1 clk T 2 S /1024 (From prescaler)   16M/1024=15625  eq 15.625 count down for 1
     millisec so 255 counts ~= 80.32 milli sec timer

     timer 1(328+megas), 3,4,5(megas only)
     1280, 2560,2561 has same prescaler config :
     FOR 16 bits !
     prescaler in cs2 cs1 cs0
     0   0   0   none
     0   0   1   /1 == none
     0   1   0   /8     2000000 intr/sec
     0   1   1   /64     250000 intr/sec
     1   0   0   /256     62500 intr/sec
     1   0   1   /1024    15625 intr/sec
     16MHz Arduino -> 16000000/1024 =  15625 intr/second at one count
     16MHz Arduino -> 16000000/256  =  62500 ticks/second
     -------------------------/64   = 250000 ticks/second !

     NB 16 bit counter so values >= 65535 is not working
     *************************************************************************************
   */

     
  // will not start if errors during initialization
  if (k_err_cnt) {
    return -k_err_cnt;
  }
   
  k_tick_size = K_TICK;

  DI(); // silencio

  //  outdated ? JDN NASTY
#if defined(__AVR_ATmega32U4__)
  // 32u4 have no intern/extern clock source register
#else
  // should be default ASSR &= ~(1 << AS2);   // Select clock source: internal
  // I/O clock 32u4 does not have this facility
#endif


// TIMER2 I ASSUME !
 
  //set PRESCALE bits cs22 cs21 cs20  to 0x05 fo clock div by 1024
  //16MHz/1024 = 15625 kHz interrupt (or 
  // its equal to 0.064 msec pr count
  // for 1 msec tick we shall count to 15(0.96msec) or 16(1.024msec) 
  // we prefer same freq all time instead of a leap second strategy
  
   
  
   

//https://wolles-elektronikkiste.de/en/timer-and-pwm-part-1-8-bit-timer0-2
 // (1/16000000)*1024*16 = 1,024msec
 //  (1/16000000)*1024*157 = 10,048 msec
 
  DI();
  TCCRxA = 0x00; // Wave Form Generation Mode 0: Normal Mode; OC2A disconnected
  TCCRxB = PRESCALE; //(1<< CS22) | (1 << CS21)| (1 << CS20);//0x05; //  CS2 CS1 CS0 set so  prescaler = 1024
  TIMSKx = (1<<TOIEx); // interrupt when TCNT2 is overflowed
  
  TCNTx = CNT_1MSEC;
   
  
  //  let us start the show
  TIMSKx |= (1 << TOIEx); // enable interrupt
 
 
  pRun = pmain_el; // just for ki_task_shift

  k_running = 1;

  ki_task_shift(); // bye bye from here
 
  EI();

  // this while loop bq main are dummy
  while (1) {
  }

  return 0;
}

int k_stop() {

  /*
     main is dummy task so it gives no meaning to stop krnl this way
     The best and dirty thing is
     her DI();  while (1);
   */

  // DANGEROUS - handle with care - no isr timer control etc etc
  // I WILL NEVER USE IT
  DI(); // silencio
  if (!k_running) {
    EI();
    return -1;
  }
  while (1)
    ; // we stuck here with intr disabled !!!
}

void k_set_coop_multitask(unsigned char onn)
{
  k_coopFlag = onn;
}

unsigned long ki_millis(void) {
  unsigned long l;
  DI(); // just to be sure
  l = k_millis_counter;
  return l;
}

unsigned long k_millis(void) {
  unsigned long l;

  DI();
  l = k_millis_counter;
  EI();
  return l;
}

int k_tmrInfo(void) { return (KRNLTMR); }

#ifdef KRNLBUG

// defined as weak so compiler will take yours instead of mine
void __attribute__((weak)) k_breakout(void) {}

void __attribute__((weak)) k_sem_clip(unsigned char nr, int nrClip) {}

void __attribute__((weak)) k_sem_signal(unsigned char nr, int semVal) {}

void __attribute__((weak)) k_sem_wait(unsigned char nr, int semVal) {}

void __attribute__((weak)) k_send_Q_clip(unsigned char nr, int nrClip) {}

#endif

#ifdef DYNMEMORY
void *__attribute__((weak)) k_malloc(int k) {
  void *m;
  DI();
  m = malloc(k);
  EI();
  return m;
}

void __attribute__((weak)) k_free(void *m) {
  // we dont free memory
}
#endif

 

void __attribute__((weak)) k_wdt_enable(int i) {
  DI();
  wdt_enable(i);
  k_wdt_enabled = 1;
  EI();
}

void __attribute__((weak)) k_wdt_disable(void) {
  DI();
  k_wdt_enabled = 0;
  wdt_disable();
  EI();
}

/* EOF - JDN */

/*
    #ifdef __cplusplus
    }
    #endif
*/

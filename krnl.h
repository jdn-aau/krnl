
#define KRNL_VRS 20250606
/*******************************************************
 *****                                                 *
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
 *                                                     *
 *  this version adapted for Arduino
 *
 *  (C) 2012,2013,2014
 *     2017,2018,2019,2021,2022,2023
 *
 *
 * IF YOU ARE LUCKY LOOK HERE
 *
 * https://github.com/jdn-aau/krnl
 *
 *
 *
 *  Jens Dalsgaard Nielsen <jdn@es.aau.dk>
 *  http://es.aau.dk/staff/jdn
 *  Section of Automation & Control
 *  Aalborg University,
 *  Denmark
 *
 *  "THE BEER-WARE LICENSE" (frit efter PHK)
 *  <jdn@es.aau.dk> wrote this file. As long as you
 *  retain this notice you can do whatever you want
 *  with this stuff. If we meet some day, and you think
 *  this stuff is worth it ...
 *  you can buy me a beer in return :-)
 *  or if you are real happy then ...
 *  single malt will be well received :-)
 *
 *  Use it at your own risk - no warranty
 *
 *  nice info...
 *  http://www.nongnu.org/avr-libc/user-manual/FAQ.html#faq_cplusplus
 *  at 20090611
 *  - k_eat_time now eats time in quants of krnl tick speed but not one quant
 *boundary
 *  - added k_malloc and k_free
 *     k_free dont free memory bq we do not want fragmentation
 *     so DONT USE k_free. I do NOT free memory
 *  - k_malloc and k_free are weak functions so you can just add your own
 *versions
 *  - watchdog is enabled (2sec and reset in krnl timer) in weak function
 *k_enable_wdt
 *
 * remember to update in krnl.c !!!
 *****************************************************/

#ifndef KRNL
#define KRNL

// IMPORTANT DEFINES !!!!
// IMPORTANT DEFINES !!!!
// IMPORTANT DEFINES !!!!
// IMPORTANT DEFINES !!!!
// IMPORTANT DEFINES !!!!
  
#define KRNLBUG // KRNLBUG ACTIVATE BREAK OUT FUNCTIONS

// BACKSTOPPER wraps a looping fct around your task so it will just restart
// like loop function
// BEWARE bq local variables in the task body just evaporate - as always
#define BACKSTOPPER

// if you want to use k_malloc
// NB k_free wont release mem due to possible fragmentation
// SO DONT USE k_free its just a fake
// NEVER !!! free men in a rt system...
// and please do all malloc before starting running

#define DYNMEMORY

// DON NOT CHANGE K_TICK - must be 1 millisec
#define K_TICK 1

// if defined you will stop in backstopper, else you will jump back to task s
// see comments about local vars above
//#define STOP_IN_BACKSTOPPER

//#define WDT_TIMER    // uncomment to activate
#define WDT_PERIOD WDTO_1S


// IF YOU WANT READER WRITER LOCK THEN DEFINE
// experimental - no guarantee
//https://www.geeksforgeeks.org/readers-writers-problem-writers-preference-solution/?ref=rp
//https://www.geeksforgeeks.org/readers-writers-problem-set-1-introduction-and-readers-preference-solution/
// https://www.tutorialspoint.com/readers-writers-problem

//#define READERWRITER

// Define watchdog timer and period
// if you enable it it will be running at once 

// END IMPORTANT DEFINES
// END IMPORTANT DEFINES
// END IMPORTANT DEFINES
// END IMPORTANT DEFINES
// END IMPORTANT DEFINES
// END IMPORTANT DEFINES



// SELECTION OF HW TIMER TO BE USED FOR krnl tick
// SELECTION OF HW TIMER TO BE USED FOR krnl tick
// SELECTION OF HW TIMER TO BE USED FOR krnl tick

// Reverted to use only timer 2 
//HAS IMPACT ON PWM !!!! SEE BELOW !!!
/*
#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)  \
    || defined(__AVR_ATmega2561__) || defined(__AVR_ATmega1284P__)\
    || defined(__AVR_ATmega328P__) || defined(__AVR_ATmega328PB__)\
    || defined(__AVR_ATmega328__) 
#define KRNLTMR 2
 
#elif defined(__AVR_ATmega32U4__)
//#define KRNLTMR 3
#error "32u4 hmm  AVR cpu type - bad place to come"
#else
#error "unknown AVR cpu type - bad place to come"
#endif
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
  #if defined(__AVR_ATtiny24__) || defined(__AVR_ATtiny44__) ||
defined(__AVR_ATtiny84__) ISR(TIM0_OVF_vect) #else ISR(TIMER0_OVF_vect) #endif
     to something else : mayby void kurt()

   This is because krnl install an ISR on timer0 and there cant be two
   Krnl to maintain the millis counters so everything with millis, micros etc is
as usual.

   Buth normally you cant use timer 0 bq it is used for millis and preallocated.

   See below

   When using a timer you must be aware of that it will prohibit you from things
like
   - tone (pwm sound) uses timer2

   ... from http://blog.oscarliang.net/arduino-timer-and-interrupt-tutorial/

   Timer0:
   - 8bit timer.
   - In the Arduino world Timer0 is been used for the timer functions, like
delay(), millis() and micros().
   -  If you change Timer0 registers, this may influence the Arduino timer
function.
   - So you should know what you are doing.

   - 16bit timer.
   - In the Arduino world the Servo library uses Timer1 on Arduino Uno (Timer5
on Arduino Mega).

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

   ... from http://arduino-info.wikispGitte Merete
Holm-Pedersenaces.com/Timers-Arduino

   - Servo Library uses Timer1.
   -- You can’t use PWM on Pin 9, 10 when you use the Servo Library on an
Arduino.
   -- For Arduino Mega it is a bit more difficult. The timer needed depends on
the number of servos.
   -- Each timer can handle 12 servos.
   -- For the first 12 servos timer 5 will be used (losing PWM on Pin 44,45,46).
   -- For 24 Servos timer 5 and 1 will be used (losing PWM on Pin
11,12,44,45,46)..
   -- For 36 servos timer 5, 1 and 3 will be used (losing PWM on Pin
2,3,5,11,12,44,45,46)..
   -- For 48 servos all 16bit timers 5,1,3 and 4 will be used (losing all PWM
pins).

   - Pin 11 has shared functionality PWM and MOSI.
   -- MOSI is needed for the SPI interface, You can’t use PWM on Pin 11 and the
SPI interface at the same time on Arduino.
   -- On the Arduino Mega the SPI pins are on different pins.

   - tone() function uses at least timer2.
   -- You can’t use PWM on Pin 3,11 when you use the tone() function an Arduino
and Pin 9,10 on Arduino Mega.

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
   
   MAY WORK OR NOT !!!! BEWARE  /Jens

  #if defined (MOD)

   void justFakeFctHead()
  #else
  #if defined(__AVR_ATtiny24__) || defined(__AVR_ATtiny44__) ||
defined(__AVR_ATtiny84__) ISR(TIM0_OVF_vect) 769769876987 lalalalalalal #else
   ISR(TIMER0_OVF_vect)
  #endif

  #endif
   >>>>>>>>>>>>

***********************************************************/

 
// >>>>>>>>>>>>>>>>> USER CONFIGURATION PART <<<<<<<<<<<<<<<<<<

 

// We do use a char forholding prio so ax max interval is 0 -> 127
// as Qheads has lowest prio we here have the limit 0-99 for tasks
// see below
#define QHD_PRIO 102 // Queue head prio - for sentinel use
#define ZOMBI_PRIO (QHD_PRIO - 1)
#define DMY_PRIO (QHD_PRIO - 2) // dummy task prio (0 == highest prio)

// no real dummytask - we do use ya old mmin loop "task""
#define DMY_STK_SZ 90           // staksize for dummy

#define STAK_HASH 0x5c          // just a hashcode
#define MAX_SEM_VAL 30000        // NB is also max for nr elem in msgQ !
#define MAX_INT 0x7FFF 
 
#define CEILINGFAILNOTCEIL -3
#define CEILINGFAILPRIO -4

/* which timer to use for krnl heartbeat
   timer 0 ( 8 bit) is normally used by millis - avoid !
   timer 1 (16 bit)
   timer 2 ( 8 bit) not impl on 32u4
   timer 3 (16 bit) 32u4/1280/1284p/2560 only
   timer 4 (16 bit) 1280/2560 only (MEGA)
   timer 5 (16 bit) 1280/2560 only (MEGA)
 */

// END USER CONFIGURATION

//----------------------------------------------------------

#ifdef __cplusplus
extern "C" {
#endif


/***** KeRNeL data types *****/
struct k_t {
#ifdef BACKSTOPPER
  void (*pt)(void);
#endif
  unsigned char nr;
  struct k_t *next,    // task,sem: double chain lists ptr
      *pred;           // task,sem: double chain lists ptr
  volatile char sp_lo, // sem:vacant    | task: low 8 byte of stak adr
      sp_hi;           // sem: vacant   |task: high 8 byte of stak adr
  char prio,           // task & sem:  priority
      ceiling_prio,    // sem
      saved_prio;      // semaohore
  volatile int cnt1,   // sem: sem counter | task: ptr to stak
      cnt2,            // asem: dyn part of time counter | task: timeout
      cnt3, // sem: preset timer value |  task: ptr to Q we are hanging in
      maxv, // sem: max value |         task: org priority
      clip; // sem: counter for lost signals | task: vacant
};

extern unsigned char k_coopFlag;

struct k_msg_t {
  // msg type
  unsigned char nr;
  struct k_t *sem;
  char *pBuf; // ptr to user supplied ringbuffer
  volatile int nr_el, el_size, lost_msg;
  volatile int r, w, cnt;
};

#ifdef READERWRITER
struct k_rwlock_t {
  int nrReaders;
  struct k_t *rdwrSem, *rdSem, *fifoSem;
};
#endif
/***** KeRNeL variables *****/

// keep track of k alloc
extern int k_task, k_sem, k_msg;
extern volatile char krnl_preempt_flag;
extern char dmy_stk[DMY_STK_SZ];

extern volatile char k_wdt_enabled;

// QUEUES
extern struct k_t *task_pool, *sem_pool, AQ, // activeQ
    *pmain_el, *pAQ, *pDmy,                  // ptr to dummy task descriptor
    *pRun,                                   // ptr to running task
    *pSleepSem;

extern struct k_msg_t *send_pool;

extern char nr_task, nr_sem, nr_send;

extern volatile char k_running; // no running

extern volatile char k_err_cnt; // every time an error occurs cnt is incr by one
extern unsigned long k_millis_counter;
extern char k_preempt_flag;

/******************************************************
   MACROS MACROS

   PUSHREGS AND POPREGS
   is actual staklayout plus task address at top
   A push/pop takes 2 cycles
   a call takes 3 cycles
   ret/iret 3-4 cycles
   So a PUSHREGS is 33 instructions(@ 2 cycles) = 66  cycles ~= 66 cycles /"16
MHz" ~= 4.1 usec So an empty iSR which preserves all registers takes 2*4.1usec +
8-10 cycles (intr + iret) ~= 9 usec So max isr speed with all regs saved is
around 100 kHz but then with no code inside ISR !

   WARNING
   The 2560 series has 3 bytes PC the rest only 2 bytes PC !!! (PC. program
counter) and no tall has rampz and eind register

   REGISTER NAMING AND INTERNAL ADRESSING

https://en.wikipedia.org/wiki/Atmel_AVR_instruction_set
http://www.rjhcoding.com/avr-asm-io.php
Register	I/O address	Data address
SREG	0x3F	        0x5F
SP	0x3E:0x3D	0x5E:0x5D
EIND	0x3C	        0x5C
RAMPZ	0x3B	        0x5B
RAMPY	0x3A	        0x5A
RAMPX	0x39	        0x59
RAMPD	0x38	        0x58
A typical ATmega memory map may look like:

Data address	I/O address	Contents
0x0000 – 0x001F		Registers R0 – R31

general purpose regs
r0    mem addr
...
0 0   0x0000
1 1   0x0001
2 2
3 3
4 4
5 5
6 6
7 7
8 8
9 9
a 10
b 11
c 12
d 13
e 14
f 15
10 16
11 17
12 18
13 19
14 20
15 21
16 22
17 23
18 24
19 25
1a 26  Xref L
1b 27  Xreg H
1c 28  Yreg L
1d 29  Yreg H
1e 30  Zreg L
1f 31  Zreg H  0x001F

0x0020 – 0x003F	0x00 – 0x1F	I/O registers (bit-addressable)
0x0040 – 0x005F	0x20 – 0x3F	I/O registers (not bit-addressable)
0x0060 – 0x00FF		Extended I/O registers (memory-mapped I/O only)
0x0100 – RAMEND		Internal SRAM

   Register addresses
   IO adr      data addr
   0x3f SREG    0x5f
   0x3e SPH
   0x3d SPL
   0x3c EIND    0x5c 1280/2560 only
   0x3b RAMPZ   0x5b 1280/2560/1284p only
   ...
   0x1f R31
   etc
   0x01 R1
   0x00 R0

   PC is NOT available
 */

#define lo8(X) ((unsigned char)((unsigned int)(X)))
#define hi8(X) ((unsigned char)((unsigned int)(X) >> 8))
 
#define DI() __asm__ volatile("cli")
#define EI() __asm__ volatile("sei")
#define RETI() __asm__ volatile("reti")

extern volatile char k_bug_on;

// IF KRLBUG IS DEFINED THEN breakout is called for every time a taskshift appears
// IF KRLBUG IS DEFINED THEN breakout is called for every time a taskshift appears

#ifdef KRNLBUG   
#define K_CHG_STAK()                                                           \
                                                                               \
  if (pRun != AQ.next) {                                                       \
    pRun->sp_lo = SPL;                                                         \
    pRun->sp_hi = SPH;                                                         \
    pRun = AQ.next;                                                            \
    k_breakout();                                                              \
    SPL = pRun->sp_lo;                                                         \
    SPH = pRun->sp_hi;                                                         \
  }                                                                            

#else  
#define K_CHG_STAK()                                                           \
  if (pRun != AQ.next) {                                                       \
    pRun->sp_lo = SPL;                                                         \
    pRun->sp_hi = SPH;                                                         \
    pRun = AQ.next;                                                            \
    SPL = pRun->sp_lo;                                                         \
    SPH = pRun->sp_hi;                                                         \
  }                                                                            

#endif                                                                     
 
 

/* below: r1 must/shall always assumed to be zero in c code (gcc issue I think)
*/

// AVR / ARDUINO PART PUSH POP
// 0x3b RAMPZ extended z-pointer register
// 0x3c EIND extended indirect register


/* below: r1 must/shall always assumed to be zero in c code (gcc issue I think)
 */

#if defined(__AVR_ATmega2560__) || defined(__AVR_ATmega1280__) ||              \
    defined(__AVR_ATmega2561__)

// 0x3b RAMPZ extended z-pointer register
// 0x3c EIND extended indirect register

#define PUSHREGS()                                                             \
  __asm__ volatile("push r1  \n\t"                                             \
                   "push r0  \n\t"                                             \
                   "in r0, __SREG__ \n\t"                                      \
                   "cli \n\t"                                                  \
                   "push r0  \n\t"                                             \
                   "in r0 , 0x3b \n\t"                                         \
                   "push r0 \n\t"                                              \
                   "in r0 , 0x3c \n\t"                                         \
                   "push r0 \n\t"                                              \
                   "clr r1 \n\t"                                               \
                   "push r2  \n\t"                                             \
                   "push r3  \n\t"                                             \
                   "push r4  \n\t"                                             \
                   "push r5  \n\t"                                             \
                   "push r6  \n\t"                                             \
                   "push r7  \n\t"                                             \
                   "push r8  \n\t"                                             \
                   "push r9  \n\t"                                             \
                   "push r10 \n\t"                                             \
                   "push r11 \n\t"                                             \
                   "push r12 \n\t"                                             \
                   "push r13 \n\t"                                             \
                   "push r14 \n\t"                                             \
                   "push r15 \n\t"                                             \
                   "push r16 \n\t"                                             \
                   "push r17 \n\t"                                             \
                   "push r18 \n\t"                                             \
                   "push r19 \n\t"                                             \
                   "push r20 \n\t"                                             \
                   "push r21 \n\t"                                             \
                   "push r22 \n\t"                                             \
                   "push r23 \n\t"                                             \
                   "push r24 \n\t"                                             \
                   "push r25 \n\t"                                             \
                   "push r26 \n\t"                                             \
                   "push r27 \n\t"                                             \
                   "push r28 \n\t"                                             \
                   "push r29 \n\t"                                             \
                   "push r30 \n\t"                                             \
                   "push r31 \n\t")

#define POPREGS()                                                              \
  __asm__ volatile("pop r31 \n\t"                                              \
                   "pop r30 \n\t"                                              \
                   "pop r29 \n\t"                                              \
                   "pop r28 \n\t"                                              \
                   "pop r27 \n\t"                                              \
                   "pop r26 \n\t"                                              \
                   "pop r25 \n\t"                                              \
                   "pop r24 \n\t"                                              \
                   "pop r23 \n\t"                                              \
                   "pop r22 \n\t"                                              \
                   "pop r21 \n\t"                                              \
                   "pop r20 \n\t"                                              \
                   "pop r19 \n\t"                                              \
                   "pop r18 \n\t"                                              \
                   "pop r17 \n\t"                                              \
                   "pop r16 \n\t"                                              \
                   "pop r15 \n\t"                                              \
                   "pop r14 \n\t"                                              \
                   "pop r13 \n\t"                                              \
                   "pop r12 \n\t"                                              \
                   "pop r11 \n\t"                                              \
                   "pop r10 \n\t"                                              \
                   "pop r9  \n\t"                                              \
                   "pop r8  \n\t"                                              \
                   "pop r7  \n\t"                                              \
                   "pop r6  \n\t"                                              \
                   "pop r5  \n\t"                                              \
                   "pop r4  \n\t"                                              \
                   "pop r3  \n\t"                                              \
                   "pop r2  \n\t"                                              \
                   "pop r0  \n\t"                                              \
                   "out 0x3c , r0 \n\t "                                       \
                   "pop r0  \n\t"                                              \
                   "out 0x3b , r0 \n\t "                                       \
                   "pop r0  \n\t"                                              \
                   "out __SREG__ , r0 \n\t "                                   \
                   "pop r0  \n\t"                                              \
                   "pop r1  \n\t")

#elif defined(__AVR_ATmega1284P__)

// 0x3b RAMPZ extended z-pointer register
// 0x3c EIND extended indirect register

#define PUSHREGS()                                                             \
  __asm__ volatile("push r1  \n\t"                                             \
                   "push r0  \n\t"                                             \
                   "in r0, __SREG__ \n\t"                                      \
                   "cli \n\t"                                                  \
                   "push r0  \n\t"                                             \
                   "in r0 , 0x3b \n\t"                                         \
                   "push r0 \n\t"                                              \
                   "clr r1 \n\t"                                               \
                   "push r2  \n\t"                                             \
                   "push r3  \n\t"                                             \
                   "push r4  \n\t"                                             \
                   "push r5  \n\t"                                             \
                   "push r6  \n\t"                                             \
                   "push r7  \n\t"                                             \
                   "push r8  \n\t"                                             \
                   "push r9  \n\t"                                             \
                   "push r10 \n\t"                                             \
                   "push r11 \n\t"                                             \
                   "push r12 \n\t"                                             \
                   "push r13 \n\t"                                             \
                   "push r14 \n\t"                                             \
                   "push r15 \n\t"                                             \
                   "push r16 \n\t"                                             \
                   "push r17 \n\t"                                             \
                   "push r18 \n\t"                                             \
                   "push r19 \n\t"                                             \
                   "push r20 \n\t"                                             \
                   "push r21 \n\t"                                             \
                   "push r22 \n\t"                                             \
                   "push r23 \n\t"                                             \
                   "push r24 \n\t"                                             \
                   "push r25 \n\t"                                             \
                   "push r26 \n\t"                                             \
                   "push r27 \n\t"                                             \
                   "push r28 \n\t"                                             \
                   "push r29 \n\t"                                             \
                   "push r30 \n\t"                                             \
                   "push r31 \n\t")

#define POPREGS()                                                              \
  __asm__ volatile("pop r31 \n\t"                                              \
                   "pop r30 \n\t"                                              \
                   "pop r29 \n\t"                                              \
                   "pop r28 \n\t"                                              \
                   "pop r27 \n\t"                                              \
                   "pop r26 \n\t"                                              \
                   "pop r25 \n\t"                                              \
                   "pop r24 \n\t"                                              \
                   "pop r23 \n\t"                                              \
                   "pop r22 \n\t"                                              \
                   "pop r21 \n\t"                                              \
                   "pop r20 \n\t"                                              \
                   "pop r19 \n\t"                                              \
                   "pop r18 \n\t"                                              \
                   "pop r17 \n\t"                                              \
                   "pop r16 \n\t"                                              \
                   "pop r15 \n\t"                                              \
                   "pop r14 \n\t"                                              \
                   "pop r13 \n\t"                                              \
                   "pop r12 \n\t"                                              \
                   "pop r11 \n\t"                                              \
                   "pop r10 \n\t"                                              \
                   "pop r9  \n\t"                                              \
                   "pop r8  \n\t"                                              \
                   "pop r7  \n\t"                                              \
                   "pop r6  \n\t"                                              \
                   "pop r5  \n\t"                                              \
                   "pop r4  \n\t"                                              \
                   "pop r3  \n\t"                                              \
                   "pop r2  \n\t"                                              \
                   "pop r0  \n\t"                                              \
                   "out 0x3b , r0 \n\t "                                       \
                   "pop r0  \n\t"                                              \
                   "out __SREG__ , r0 \n\t "                                   \
                   "pop r0  \n\t"                                              \
                   "pop r1  \n\t")

#elif defined(__AVR_ATmega168__) || defined(__AVR_ATmega328P__) ||  defined(__AVR_ATmega328PB__)  \
                   ||  defined(__AVR_ATmega328__) || defined(__AVR_ATmega32U4__)

#define PUSHREGS()                                                             \
  __asm__ volatile("push r1 \n\t"                                              \
                   "push r0 \n\t"                                              \
                   "in r0, __SREG__ \n\t"                                      \
                   "cli \n\t"                                                  \
                   "push r0  \n\t"                                             \
                   "clr r1 \n\t"                                               \
                   "push r2  \n\t"                                             \
                   "push r3  \n\t"                                             \
                   "push r4  \n\t"                                             \
                   "push r5  \n\t"                                             \
                   "push r6  \n\t"                                             \
                   "push r7  \n\t"                                             \
                   "push r8  \n\t"                                             \
                   "push r9  \n\t"                                             \
                   "push r10 \n\t"                                             \
                   "push r11 \n\t"                                             \
                   "push r12 \n\t"                                             \
                   "push r13 \n\t"                                             \
                   "push r14 \n\t"                                             \
                   "push r15 \n\t"                                             \
                   "push r16 \n\t"                                             \
                   "push r17 \n\t"                                             \
                   "push r18 \n\t"                                             \
                   "push r19 \n\t"                                             \
                   "push r20 \n\t"                                             \
                   "push r21 \n\t"                                             \
                   "push r22 \n\t"                                             \
                   "push r23 \n\t"                                             \
                   "push r24 \n\t"                                             \
                   "push r25 \n\t"                                             \
                   "push r26 \n\t"                                             \
                   "push r27 \n\t"                                             \
                   "push r28 \n\t"                                             \
                   "push r29 \n\t"                                             \
                   "push r30 \n\t"                                             \
                   "push r31 \n\t")

#define POPREGS()                                                              \
  __asm__ volatile("pop r31 \n\t"                                              \
                   "pop r30 \n\t"                                              \
                   "pop r29 \n\t"                                              \
                   "pop r28 \n\t"                                              \
                   "pop r27 \n\t"                                              \
                   "pop r26 \n\t"                                              \
                   "pop r25 \n\t"                                              \
                   "pop r24 \n\t"                                              \
                   "pop r23 \n\t"                                              \
                   "pop r22 \n\t"                                              \
                   "pop r21 \n\t"                                              \
                   "pop r20 \n\t"                                              \
                   "pop r19 \n\t"                                              \
                   "pop r18 \n\t"                                              \
                   "pop r17 \n\t"                                              \
                   "pop r16 \n\t"                                              \
                   "pop r15 \n\t"                                              \
                   "pop r14 \n\t"                                              \
                   "pop r13 \n\t"                                              \
                   "pop r12 \n\t"                                              \
                   "pop r11 \n\t"                                              \
                   "pop r10 \n\t"                                              \
                   "pop r9  \n\t"                                              \
                   "pop r8  \n\t"                                              \
                   "pop r7  \n\t"                                              \
                   "pop r6  \n\t"                                              \
                   "pop r5  \n\t"                                              \
                   "pop r4  \n\t"                                              \
                   "pop r3  \n\t"                                              \
                   "pop r2  \n\t"                                              \
                   "pop r0  \n\t"                                              \
                   "out __SREG__ , r0 \n\t "                                   \
                   "pop r0  \n\t"                                              \
                   "pop r1  \n\t")

#else
#error "unknown arch"
#endif


// function prototypes
// naming convention
// k_... function do a DI/EI and can impose task shift
// ki_... expects interrupt to be disablet and do no task shift
// rest is internal functions

#ifdef DYNMEMORY

/**
  call malloc protected by DI and EI
  if dft as weak so you can supply with your own
*/
void *k_malloc(int k);

/**
  call free
  empty - no function
  if dft as weak so you can supply with your own
  @remark do no free mem bq we do not want fragmented memory
*/
void k_free(void *m);
#endif

/**
 * Call wdt_enable (AVR lib)
 *  DI();
 *  wdt_enable(WDT_PERIOD);
 *  EI();
 * WDT_PERIOD is in KRNL i sec
 * @param[t] watchdog timout WDT_PERIOD is 1 sec
 * See avrlib docu for more info
 * The watchdog is reset in krnls timer ISR when active
 */
void __attribute__((weak)) k_wdt_enable(int t);

/**
 *   disable wdt
 *   Disable the watchdog timer
 */
void __attribute__((weak)) k_wdt_disable();

/**
   Returns nr of milliseconds since krnl was started by k_start
   @return: unsigned long is returned
   @remark only to be called after start of KRNL and only when interrupt is
   disabled
   @remark To be called within an ISR so interrupts is no disabled
*/
unsigned long ki_millis(void);

/**
   Returns nr of milliseconds since krnl was started by k_start
   @return: unsigned long is returned
   @remark only to be called after start of KRNL
   @remark Interrupt is disabled when reading krnl time and is enabled
   afterwards
*/
unsigned long k_millis(void);

/**
 * Eat time in quant of krnl ticks
 * starting from now but not from next tick
 *   Eats CPU time in k_tick quants
   @param[in] tyick  number of ticks to eat
 */
void k_eat_msec(unsigned int eatTime);

/**
   issues a task shift - handle with care
   Not to be used by normal user
 */
void ki_task_shift(void) __attribute__((naked));

/**
   Set task asleep for a number of ticks.
   @param[in] time nr of ticks to sleep < 0
   @return: 0 is ok(has been suspended), 1(is  not suspended), -1 is wait time
   is less than 0
   @remark only to be called after start of KRNL
 */
int k_sleep(int time);

/**
   creates a task and put it in the active Q
   @param[in] pTask pointer to function for code ( void task(void) ...)
   @param[in] stkSize size of data area(in bytes) to be used for stak
   @param[in] prio - task prio max: 1 lowest < DUMMY_PRIO
   @return: pointer to task handle or NULL if no success
   @remark only to be called before start of KRNL but after k_init
 */
struct k_t *k_crt_task(void (*pTask)(void), char prio, char *pS, int stkSize);

/**
   change priority of calling task)
   @param[in] prio new prio, Priority 1: highest (QHEAD_PRIO-1): lowest
   @return: 0: ok, -1: KRNL not running, -2: illegal value
   @remark only to be called after start of KRNL
 */

int k_set_prio(char prio);

/**
   creates a standard Dijkstra semaphore. It can be initialized to values in
   range [0..maxvalue]
   @param[in] init_val startvalue of semaphore 0,1,2,... maxvalue
   @param[in] maxvalue which is maxvalue semaphore can reach
   @return handle to semaphore or NULL pointer
   @remark only to be called before start of KRNL
 */
struct k_t *k_crt_sem(int init_val, int maxvalue);

/**
   adds ceiling priority to semaphore
   use k_mut_ceil_enter k_mut_ceil_leave instead of wait and signal
   Can only be called before k_start
   @param[in] sem reference to sem used for mutex - no check
   @param[in] prio Ceiling pirority
   @return 0: ok , otherwise bad bad
   @remark only to be called before start of KRNL
 */

/**
   creates a standard mutex. It can be initialized to values in range
   [0..maxvalue]
   @param[in] init_val startvalue of semaphore 0,1,2,... maxvalue
   @param[in] maxvalue which is maxvalue semaphore can reach
   @return handle to semaphore or NULL pointer
   @remark only to be called before start of KRNL
 */
struct k_t *k_crt_mut(char ceiling_prio, int init_val, int maxvalue);

int k_mut_ceil_set(struct k_t *sem, char prio);

/**
   Enter mutex (eq to k_wait...)
   use k_mut_enter / k_mut_leave instead of wait and signal
   @param[in] sem aka mutex
   @param[in] timeout  timeout value
   @return 0: if you have been waiting, 1 if you just went through, -1 for
   timeout, -3 if your priority was higher than ceiling priority
 */
int k_mut_ceil_enter(struct k_t *sem, int timeout);

/**
   Leave mutex (eq to k_signal...)
   use k_mut_enter/ k_mut_leave instead of wait and signal
   @param[in] sem used as mutex
   @return 0: ok otherwise bad bad
 */
int k_mut_ceil_leave(struct k_t *sem);

/**
   Mutex enter and leave w imm priority ceiling in one operation

   @param sm mutex with ceiling
   @param timeout 0 == forever
   @fct  void fct(void) the function w critical code

   @return 0: ok ,waited, 1 straight through; -1 timeout, -3 CEILINGFAILNOTCEIL
   not a semaphore w ceiling, -4 CEILINGFAIL my priority is above ceil max prio
*/
int k_mut_ceil(struct k_t *sem, int timeout, void (*fct)(void));

/**
   attach a timer to the semaphore so KRNL will signal the semaphore with
   regular intervals. Can be used for cyclic real time run of a task.
   @param[in] sem semaphore handle
   @param[in] val interval in quant of KRNL ticks (0: disable cyclic timer,
   1,2,3... cyclic quant)
   @return -1: negative val, 0. ok
   @remark only to be called after start of KRNL
 */
int k_set_sem_timer(struct k_t *sem, int val);

/**
   Signal a semaphore. Can be called from an ISR when interrupt is disabled. No
   task shift will occur - only queue manipulation.
   @param[in] sem semaphore handle
   @return 0: ok , -1: max value of semaphore reached
   @remark only to be called after start of KRNL
 */
int ki_signal(struct k_t *sem);

/**
   Signal a semaphore. Task shift will task place if a task is started by the
   signal and has higher priority than you.
   @param[in] sem semaphore handle
   @return 0: ok , -1: max value of semaphore reached
   @remark The ki_ indicates that interrups is NOT enabled when leaving
   ki_signal
   @remark only to be called after start of KRNL
 */
int k_signal(struct k_t *sem);

/**
   Wait on a semaphore. Task shift will task place if you are blocked.
   @param[in] sem semaphore handle
   @param[in] timeout "<0" you will be started after timeout ticks, "=0" wait
   forever "-1" you will not wait
   @return 1: ok there was a signal hanging - so no suspension
   @return 0: ok- you have been suspended
   @return -1: timeout has occured, -2 no wait bq timeout was -1 and semaphore
   was negative
   @remark only to be called after start of KRNL
 */
int k_wait(struct k_t *sem, int timeout);

/**
   Wait on a semaphore. Task shift will task place if you are blocked.
   @param[in] sem semaphore handle
   @param[in] timeout "<0" you will be started after timeout ticks, "=0" wait
   forever "-1" you will not wait
   @param[out] nrClip If not NULL you will get in return nr of clip on signals
   on sem and clip counter will be set to 0
   @return 1: ok there was a signal hanging - so no suspension
   @return 0: ok- you have been suspended
   @return -1: timeout has occured, -2 no wait bq timeout was -1 and semaphore
   was negative
   @remark only to be called after start of KRNL
 */
int k_wait2(struct k_t *sem, int timeout, int *nrClip);

/**
   Returns how many signals has been lost on semaphore due to saturation
   @param[in] sem semaphore handle
   @return nr of signals lost
   @remark only to be called after start of KRNL
 */
int k_sem_signals_lost(struct k_t *sem);

/**
   Like k_wait with the exception interrupt is NOT enabled when leaving
   @param[in] sem semaphore handle
   @param[in] timeout "<0" you will be started after timeout ticks, "=0" wait
   forever "-1" you will not wait
   @return 0: ok , -1: could do wait bw blocking would have taken place
   @remark The ki_ indicates that interrups is NOT enabled when leaving ki_wait
   @remark only to be called after start of KRNL
   @remark normally not to be used by users !!! BEWARE
 */
int ki_wait(struct k_t *sem, int timeout);

/**
   returns value of semaphore
   @param[in] sem semaphore handle
   @return 1: ok not suspended, 0: ok you have been suspended
   @return -1 no wait maybe bq no timeout was allowed
   @remark only to be called after start of KRNL
   @remark do not enable interrupt upon leaving
 */
int ki_semval(struct k_t *sem);

/**
   returns value of semaphore
   @param[in] sem semaphore handle
   @return 1: ok not suspended, 0: ok you have been suspended
   @return -1 no wait maybe bq no timeout was allowed
   @remark only to be called after start of KRNL
 */
int k_semval(struct k_t *sem);

/**
 *  Clear semaphore if possible.
 *  Will set semaphore vallue 0 if possible (if no tasks is waiting)
 * @param[in] sem pointer ref to semaphore
 * @return semValue - if negative it indicates task are waiting and no clear
 *
 */
int k_clear_sem(struct k_t *sem);

/**
   returns nr of pending messages
   @param[in] msgbuffer handle
   xxx
 */
int ki_msg_count(struct k_msg_t *msgbuffer);

/**
   returns nr of pending messages
   @param[in] msgbuffer handle
   @return 1: ok not suspended, 0: ok you have been suspended
   @return -1 no wait maybe bq no timeout was allowed
   @remark only to be called after start of KRNL
 */
int k_msg_count(struct k_msg_t *m);

/*********************************************************************
************************* DEBUG BREAKOUT FUNCTIONS *******************
**********************************************************************
*
* Breakout functions can be removed by commenting out define of
* KRNLBUG in krnl.h  (located around line 216)

* All semaphore calls k_sem_clip, k_sem_signal, k_Sem_wait are all called
* AFTER decrement/incr of semaphore has taken place BUT before eventually
* taskshift.
*
* For message buffers/semaphores an internal semaphore is used for
synchronization
* so referring to the msgQ is by
* struct k_msg_t *msgSem; msgSem->sem->nr
*
*/

/**
   a function for overloading on usersite which is called when a semaphore is
   overflooding no  occur - it's only reading out semaphore idendity Signal
   operations has not taken place ! 1: means first semahore allocated by user, 2
   the next etc Interrupt is disabled when called and must not be enabled
   during.. so no print etc
   @param[in] nr  id of semaphore 1,2,3,...
   @param[in] nrClip number of times clip has occured (may be reset by call
   k_wait_lost)
 */
#ifdef KRNLBUG
void __attribute__((weak)) k_sem_clip(unsigned char nr, int nrClip);
#endif

/**
 * called in a signal call val is sem val AFTER signal has taken place
 * @param [out] nr number of semaphore. Semaphores are allocated by user buth
 * also in message queues. So it may be diff to find the semaphore...
 *
 */
#ifdef KRNLBUG
void __attribute__((weak)) k_sem_signal(unsigned char nr, int val);
#endif

/**
 * called when wait called. val is sem value after countdown has taken place
 * NB is the wait call willl not wait and there is no token at the semaphore
 *  val will be -1111
 */
#ifdef KRNLBUG
void __attribute__((weak)) k_sem_wait(unsigned char nr, int val);
#endif

/**
   a function for overloading on usersite which is called when a msgQ is
   overflooding no reset occur - it's only readind out smsgQ idendity 1: means
   first msgQ allocated by user etc Interrupt is disabled when called and must
   not be enabled during.. so no print etc
   @param [out] nr : id of send Q 0,1,2,...
   @param [out] nrClip: number of times clip has occured (may be reset by call
   k_receive and lost parm not eq NULL)
 */
#ifdef KRNLBUG
void __attribute__((weak)) k_send_Q_clip(unsigned char nr, int nrClip);
#endif
 
struct k_msg_t *k_crt_send_Q(int nr_el, int el_size, void *pBuf);

/**
   Put data (one element of el_size)in the ringbuffer if there are room for it.
   Intended for ISR use
   DONE BY COPY !
   @param[in] pB Ref to message buffer
   @param[in] el Reference to data to be put in buffer. Size if already given in
   k_crt_send
   @return 0: operation did succed, -1: no room in ringbuffer
   @remark Interrupt will not enabled upon leaving, so ki_send is intended to be
   used from an ISR
   @remark only to be called before start of KRNL
 */
char ki_send(struct k_msg_t *pB, void *el);

/**
   Put data (one element of el_size)in the ringbuffer if there are room for it.
   DONE BY COPY !
   @param[in] pB Ref to message buffer
   @param [in] el Reference to data to be put in buffer. Size if already given
   in k_crt_send
   @return 0: operation did succed, -1: no room in ringbuffer
   @remark only to be called after start of KRNL
   @remark k_send does not block if no space in buffer. Instead -1 is returned
 */
char k_send(struct k_msg_t *pB, void *el);

/**
   Receive data (one element of el_size)in the ringbuffer if there are data
   DONE BY COPY !
   @param[in] pB Ref to message buffer
   @param [out] el Reference to where data shall be copied to at your site
   @param[in] timeout Max time you want to wait on data, -1: never, 0: forever,
   positive: nr of KRNL timer quants
   @param[out] lost_msg nr of lost messages since last receive. will clip at
   10000. If lost_msg ptr is NULL then overrun counter is not reset to 0.
   @return 1: ok no suspension, 0: operation did succed, -1: no data in
   ringbuffer
   @remark only to be called after start of KRNL
 */
char k_receive(struct k_msg_t *pB, void *el, int timeout, int *lost_msg);

/**
   Receive data (one element of el_size)in the ringbuffer if there are data
   DONE BY COPY !
   No blocking if no data
   Interrupt will not be enabled after ki_receive and intr must be blocked prior
   to call
   @param[in] pB Ref to message buffer
   @param[out] el Reference to where data shall be copied to at your site
   @param[out] lost_msg nr of lost messages since last receive. will clip at
   10000.  If lost_msg ptr is NULL then overrun counter is not reset to 0

     @return 1: data was rdy no suspension, 0: ok you have been suspended , -1:
   no data in ringbuffer
   @remark can be used from ISR
   @remark only to be called after start of KRNL
 */
char ki_receive(struct k_msg_t *pB, void *el, int *lost_msg);

/**
 * Empty sendQ
 * @return  given no of deleted messages
 * @return -1 if its no a msg q
 * @return -2 if krnl is not running
 * @return 0 or positive gives no of messages deleted
 */
int ki_clear_msg_Q(struct k_msg_t *pB);

/**
 * Empty sendQ
 * See ki_clear_msg-Q for description
 */
int k_clear_msg_Q(struct k_msg_t *pB);

#ifdef READERWRITER
/**
 initialise a reader-writers comples
*/
void k_rwInit(struct k_rwlock_t *lock);

/**
reader-writer Read enter
*/
int k_rwRdEnter(struct k_rwlock_t *lock, int timeout);

/**
reader-writer Write enter
*/
int k_rwWrEnter(struct k_rwlock_t *lock, int timeout);

/**
  reader-writer Read leave
*/
int k_rwRdLeave(struct k_rwlock_t *lock);

/**
  reader-writer Write leave
*/
int k_rwWrLeave(struct k_rwlock_t *lock);

#endif

/**
   returns which timer is used
   @return 0,1,2,3,4,5 ...
 */
int k_tmrInfo(void); // tm in milliseconds

/**
   Initialise KRNL. First function to be called.
   You have to give max number of tasks, semaphores and message queues you will
   use
   @param[in] nrTask ...
   @param[in] nrSem ...
   @param[in] nrMsg ...
 */
int k_init(int nrTask, int nrSem, int nrMsg);

/**
  Starts krnl is possible
  If there has been ciritcal errors between k_init and k_start (like no more
  task descriptors) k_start will return a negative number indicatin how many
  errors

  If k_start starts you will not return from k_start
  */

int k_start(void); // tm in milliseconds

/**
   stop KRNL
   parm can be 1 or 10 can only run 1 og 10 msec tick speed
   If krnl is not yet running you will return with a -1 as return value
   If krnl is running interrupt will be disabled and k_stop ends in an infinite
   loop
*/
int k_stop(); // tm in milliseconds

/**
   Reset by disable interrupt plus activate watchdog (15 millisseconds) and just
 wait...
 **/
void k_reset();

/**
   Initialise blink on pin 13
   ON when dummy is running
   Nov 2014 - fake do not use it bq it will not work
   for emergency use :-)
 */

/**
  returns nr of unbytes bytes on stak.
  For chekking if stak is too big or to small...
  @param[in] t Reference to task (by task handle) If null then yourself
  @return: nr of unused bytes on stak (int)
  @remark only to be called after start of KRNL
  @remark no chk of if it is a valid task
*/
int k_stk_chk(struct k_t *t);

/**
   Returns amount of unused stak
   @param[in] t  Ptr to taskdescriptor. If NULL it is yourself
   @return: amount of unused stak(in bytes)
   @remark: a watermark philosophy is used
   @remark can be called from ISR . iif userapce you must disable intr
 **/
int ki_unused_stak(struct k_t *t);

/**
   Returns amount of unused stak
   @param[in] t  Ptr to taskdescriptor. If NULL it is yourself
   @return: amount of unused stak(in bytes)
   @remark: a watermark philosophy is used
 **/
int k_unused_stak(struct k_t *t);
 
 /**
   Returns amount of unused stak for running task
   @return: amount of unused stak(in bytes)
   @remark: a watermark philosophy is used
   @remark can be called from ISR . iif userapce you must disable intr
 **/
int ki_my_unused_stak();

/**
 * round robbin
 * reinsert running task in activeQ.
 * so running will be inserted after other tasks with same or higher priority
 **/
void k_round_robbin(void);

/**
 * release
 * switch running to task in front of Active Q
 * calls ki_task_shift (); enveloped by a DI and EI
 * no round robbin
 **/
void k_release(void);

/**
*/
void k_set_coop_multitask(unsigned char onn);

/**
 *   returns amount of free memory in your system
 *   (free size of heap) se docu in krnl.c
 */
int freeRam(void);

#ifdef KRNLBUG

/**
 *  Breakout function called from scheduler
 * You can use to examine who is runnning and who is next
 * NB NB should be fast code and remember interrupt is and must be disables
 *  pRun->nr   Nr of running.
 * dummy has nr == 0
 * then task is numbered 1.. in acc with order of creation of tasks
 * k_breakout is defined as weak function so you can write your own k_breakout
 * The builtin k_breakout has no code and just a placeholder to be subst
 * with your code.
 * setting led8-12 on uno as task number
 *   PORTB  = (1 << pRun->nr) | led13
 **/
void __attribute__((weak)) k_breakout(void);
#endif

#ifdef __cplusplus
}
#endif
#endif // #ifndef KRNL

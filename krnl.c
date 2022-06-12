/********************fwdt-res**********************************
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
 *       13 March 2018                                 *
 *                                                     *
 *******************************************************
 * 2022 ..
 *  this version adapted for Arduino
 * 
 *  (C) 2012,2013,2014
 *     2017,2018,2019,2021
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
 *  - k_eat_msec now eats time in quants of krnl tick speed but not one quant boundary
 *  - added k_malloc and k_free
 *     k_free dont free memory bq we do not want fragmentation
 *     so DONT USE k_free
 *  - k_malloc and k_free are weak functions so you can just add your own versions
 *  - watchdog is enabled (2sec and reset in krnl timer) in weak function k_enable_wdt
 * 
 * 
 * feb 2022  - krnl now only runs at 1 msec tick speed.
 * k_eat_msec eats ... msec
 */


#include "krnl.h"
#include <ktmrcfg.h>

#include <avr/interrupt.h>

#ifdef WDT_TIMER
#include <avr/wdt.h>
#endif

#include <util/delay.h>
#include <stdlib.h>


// CPU frequency - for adjusting delays
#if (F_CPU == 8000000)
#pragma message ("krnl detected 8 MHz")
#endif

#if (KRNL_VRS != 2022-06-01)
#error "KRNL VERSION NOT UPDATED in krnl.c "
#endif

/*
 *   #ifdef __cplusplus
 *   extern "C" {
 *   #endif
 */


//----------------------------------------------------------------------------

struct k_t *task_pool,          // array of descriptors for tasks
*sem_pool,                      // .. for semaphores
AQ,                            // Q head for active Q
*pmain_el,                      // procesdecriptor for main
*pAQ,                           // head of activeQ (AQ)
// *pDmy,       // ref to dummy task
*pRun,                          // who is running ?
*pSleepSem;                     // one semaphor for all to sleep at
 
struct k_msg_t *send_pool;      // ptr to array for msg sem pool

int k_task, k_sem, k_msg;       // how many did you request in k_init of descriptors ?
char nr_task = 0, nr_sem = 0, nr_send = 0;      // counters for created KeRNeL items

volatile char k_running = 0, k_err_cnt = 0;

volatile unsigned int tcntValue;        // counters for timer system
volatile int fakecnt,           // counters for letting timer ISR go multipla faster than krnl timer
fakecnt_preset;                // ...

volatile char k_wdt_enabled = 0;
static volatile char stopp = 0; // main will loop on stop as dummy task

volatile int k_drop_out_cnt = 0;  // nr of task that has falled out of task function (missing while (1) loop)
unsigned long k_millis_counter = 0;
unsigned int k_tick_size;

int tmr_indx;                   // for travelling Qs in tmr isr


/***** eat time ***/

void k_eat_msec(unsigned int eatTime)
{    // TODO need refurbish
	while (10 < eatTime) {
		delayMicroseconds(10000);
		eatTime -= 10;
	}
	delayMicroseconds(eatTime * 1000);
}


/* NASTY void from vrs 2001 it is main itself can be changed back
 */

/*
 *  void
 *  dummy_task (void)
 *  {
 *  while (1) {
 *   k_round_robbin ();
 *  }
 *  }
 */


int k_init(int nrTask, int nrSem, int nrMsg)
{
	if (k_running)              // are you a fool ???
	{
		return (-666);
	}
	
	k_task = nrTask + 1;        // +1 due to dummy
	k_sem = nrSem + nrMsg + 1;  // due to that every msgQ has a builtin semaphore
	k_msg = nrMsg + 1;          // to align so first user msgQ has index 1
	nr_send++;                  // to align so we waste one but ... better equal access
	task_pool = (struct k_t *) malloc(k_task * sizeof(struct k_t));
	sem_pool = (struct k_t *) malloc(k_sem * sizeof(struct k_t));
	send_pool = (struct k_msg_t *) malloc(k_msg * sizeof(struct k_msg_t));
	
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
	//pDmy = k_crt_task (dummy_task, DMY_PRIO, DMY_STK_SZ);
	
	pmain_el = task_pool;
	pmain_el->nr = 0;
	pmain_el->cnt2 = pmain_el->cnt3 = 0;
	nr_task++;
	pmain_el->prio = DMY_PRIO;  // main is dummy
	prio_enQ(pAQ, pmain_el);
	
	pSleepSem = k_crt_sem(0, 10);
	
	leave:
	return k_err_cnt;
}

int k_start()
{
	/*
	 *     TCCRxB
	 *     48,88,168,328, 1280,2560
	 *     timer 0 and 2 has same prescaler config:
	 *     8 bit:
	 *     0 0 0 No clock source (Timer/Counter stopped).
	 *     0 0 1 clk T2S /(No prescaling)
	 *     0 1 0 clk T2S /8 (From prescaler)      2000000 intr/sec at 1 downcount
	 *     0 1 1 clk T2S /32 (From prescaler)      500000 intr/sec ...
	 *     1 0 0 clk T2S /64 (From prescaler)      250000
	 *     1 0 1 clk T2S /128 (From prescaler)     125000
	 *     1 1 0 clk T 2 S /256 (From prescaler)    62500
	 *     1 1 1 clk T 2 S /1024 (From prescaler)   15625  eq 15.625 count down for 1 millisec
	 *     so 255 counts ~= 80.32 milli sec timer
	 * 
	 *     timer 1(328+megas), 3,4,5(megas only)
	 *     1280, 2560,2561 has same prescaler config :
	 *     FOR 16 bits !
	 *     prescaler in cs2 cs1 cs0
	 *     0   0   0   none
	 *     0   0   1   /1 == none
	 *     0   1   0   /8     2000000 intr/sec
	 *     0   1   1   /64     250000 intr/sec
	 *     1   0   0   /256     62500 intr/sec
	 *     1   0   1   /1024    15625 intr/sec
	 *     16MHz Arduino -> 16000000/1024 =  15625 intr/second at one count
	 *     16MHz Arduino -> 16000000/256  =  62500 ticks/second
	 *     -------------------------/64   = 250000 ticks/second !
	 * 
	 *     NB 16 bit counter so values >= 65535 is not working
	 ************************************************************************************* */
	int tm = 1;  // from 2022 always 1 msec

	if (k_err_cnt) {
		return -k_err_cnt;
	}

/*	
	if (tm != 1) {
		return -999;
	}

	// will not start if errors during initialization
	// boundary check
	if (tm <= 0) {
		return -555;
	}
*/	
	k_tick_size = fakecnt = tm;
	fakecnt_preset = 0;         // on duty for every interrupt
	
	DI();                       // silencio
	
	//  outdated ? JDN NASTY
	#if defined(__AVR_ATmega32U4__)
	// 32u4 have no intern/extern clock source register
	#else
	// should be default ASSR &= ~(1 << AS2);   // Select clock source: internal I/O clock 32u4 does not have this facility
	#endif
	
	#if (KRNLTMR !=0)
	TCCRxA = 0;
	TCCRxB = PRESCALE;          // atm328s  2560,...
	
	if (F_CPU == 16000000L) {
		tcntValue = COUNTMAX - DIVV;
	} else {
		tcntValue = COUNTMAX - DIVV8;   // 8 Mhz wwe assume
	}
	
	TCNTx = tcntValue;
	
	//  let us start the show
	TIMSKx |= (1 << TOIEx);     // enable interrupt
	#endif
	
	// if timer 0 we will alwasy run 1 msec !!!! and run org millis om nehalf of original Arduino interrupt
	
	DI();
	pRun = pmain_el;            // just for ki_task_shift
	
	k_running = 1;
	
	ki_task_shift();            // bye bye from here
	EI();
	
	// this while loop bq main are dummy
	while (!stopp) {
	}
	
	return (pmain_el->cnt1);    // haps from pocket from kstop
}

int k_stop(int exitVal)
{
	// DANGEROUS - handle with care - no isr timer control etc etc
	// I WILL NEVER USE IT
	DI();                       // silencio
	if (!k_running) {
		EI();
		return -1;
	}
	
	pmain_el->cnt1 = exitVal;   // transfer in pocket
	//NASTY
	// stop tick timer isr
	#if (KRNLTMR != 0)
	TIMSKx &= ~(1 << TOIEx);
	#endif
	
	stopp = 1;
	// back to main
	AQ.next = pmain_el;         // we will be the next BRUTAL WAY TO DO IT NASTY
	ki_task_shift();
	while (1);                  // you will never come here
}

void(* resetFunc) (void) = 0;  // jump/call adr 0 == reset

void k_reset()
{
  resetFunc();
}

/* EOF - JDN */

/*
 *   #ifdef __cplusplus
 *   }
 *   #endif
 */

#include <krnl.h>

 #include <avr/interrupt.h>
 
 void __attribute__((naked, noinline)) ki_task_shift(void)
{
	PUSHREGS();                 // push task regs on stak so we are rdy to task shift
	K_CHG_STAK();
	POPREGS();                  // restore regs
	RETI();                     // and do a reti NB this also enables interrupt !!!
}


#ifdef BACKSTOPPER
void jumper()
{
	// do not want to call back 
	// (*(pRun->pt)) ();           // call task code
	DI();
	k_drop_out_cnt++;
	k_drop_out(pRun->nr);
	k_set_prio(ZOMBI_PRIO);     // priority lower than dummy so you just stops
	while (1);                  // just in case - will never come here
}
#endif

struct k_t *k_crt_task(void (*pTask)(void), char prio, char *pStk, int stkSize
)
{
	struct k_t *pT;
	
	
	int i;
	char *s;
	
	if ((k_running) || ((prio <= 0) || (DMY_PRIO < prio))
		|| (k_task <= nr_task)) {
		goto badexit;
		}
		
		if (pStk == NULL) {
			goto badexit;
		}
		pT = task_pool + nr_task;   // lets take a task descriptor
		pT->nr = nr_task;
	nr_task++;
	
	pT->cnt2 = 0;               // no time out running on you for the time being
	pT->cnt3 = 0;               // no time out semaphore
	
	pT->cnt1 = (int) (pStk);    // ref to my stack
	
	// stack paint :-)
	for (i = 0; i < stkSize; i++)       // put hash code on stak to be used by k_unused_stak()
	{
		pStk[i] = STAK_HASH;
	}
	
	s = pStk + stkSize - 1;     // now we point on top of stak
	*(s--) = 0x00;              // 1 byte safety distance :-)
	
	// an interrupt do only push PC on stack by HW - can be 2 or 3 bytes
	// depending of 368/.../1280/2560
	#ifdef BACKSTOPPER
	pT->pt = pTask;
	*(s--) = lo8(jumper);       //  so top now holds address of function
	*(s--) = hi8(jumper);       // which is code body for task
	#else
	*(s--) = lo8(pTask);        //  so top now holds address of function
	*(s--) = hi8(pTask);        // which is code body for task
	#endif
	// NB  NB 2560 use 3 byte for call/ret addresses the rest only 2
	#if defined (__AVR_ATmega2560__) || defined(__AVR_ATmega2561__)
	*(s--) = EIND;              // best guess : 3 byte addresses !!! or just 0
	#endif
	
	// r1 is the socalled zero value register
	// see https://gcc.gnu.org/wiki/avr-gcc
	// can tmp be non zero (multiplication etc)
	*(s--) = 0x00;              // r1
	*(s--) = 0x00;              // r0
	*(s--) = 0x00;              // sreg
	
	//1280 and 2560 need to save rampz reg just in case
	#if defined (__AVR_ATmega2560__) || defined (__AVR_ATmega1280__) || defined (__AVR_ATmega1284P__) || defined(__AVR_ATmega2561__)
	*(s--) = RAMPZ;             // best guess  0x3b
	// obsolete JDN    *(s--) = EIND;             // best guess
	#endif
	
	#if defined (__AVR_ATmega2560__) || defined (__AVR_ATmega1280__) || defined(__AVR_ATmega2561__)
	*(s--) = EIND;              // best guess 0x3c
	#endif
	
	for (i = 0; i < 30; i++)    //r2-r31 = 30 regs
	{
		*(s--) = 0x00;
	}
	
	pT->sp_lo = lo8(s);         // now we just need to save stakptr
	pT->sp_hi = hi8(s);         // in thread descriptor
	
	// HW DEPENDENT PART - ENDE
	
	pT->prio = prio;            // maxv for holding org prio for inheritance
	pT->maxv = (int) prio;
	prio_enQ(pAQ, pT);          // and put task in active Q
	
	return (pT);
	
	badexit:
	k_err_cnt++;
	return (NULL);
}

int freeRam(void)
{
	extern int __heap_start, *__brkval;
	int x, v;
	// NASTY
	x = ((int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval));    // to remove warning
	return x;
	// hw specific :-/
	//return ((int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval));
	
	/* from http://www.noah.org/wiki/Arduino_notes#debug_memory_problems_.28out_of_RAM.29
	 *     int freeRam () {
	 *     // __brkval is the address of the top of the heap if memory has been allocated.
	 *     // If __brkval is zero then it means malloc has not used any memory yet, so
	 *     // we look at the address of __heap_start.
	 *     extern int __heap_start
	 *     extern int *__brkval; // address of the top of heap
	 *     int stack_top;
	 *     return (int)&stack_top - ((int)__brkval == 0 ? (int)&__heap_start : (int)__brkval);
}
*/
}

int k_sleep(int time)
{
	int r;
	if (time <= 0)
		return -1;
	r = k_wait(pSleepSem, time);
	if (r == -1)                // timeout ? yes :-)
		return 0;
}


int k_unused_stak(struct k_t *t)
{
	int i;
	char *pstk;
	
	if (t)                      // another task or yourself - NO CHK of validity !!!!!
	{
		pstk = (char *) (t->cnt1);
	} else {
		pstk = (char *) (pRun->cnt1);
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

int k_set_prio(char prio)
{
	int i;
	
	if (!k_running) {
		return (-2);
	}
	
	DI();
	
	if ((prio <= 0) || (DMY_PRIO <= prio))      // not legal value my friend
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



void k_round_robbin(void)
{
	
	// reinsert running task in activeQ if round robbin is selected
	DI();
	
	prio_enQ(pAQ, deQ(pRun));
	ki_task_shift();
	
	EI();
}

void k_release(void)
{
	
	// let next run
	DI();
	ki_task_shift();
	EI();
}


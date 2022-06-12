#include <krnl.h>

/*
* no chk - so if you use if wrong then you may have a problem
  sem = k_crt_sem(1,1);
  
  ...

  if ( 0 <= k_mut_ceil_enter(sem,"timeout") {
     you got it 
     ...
     k_mut_ceil_leave(sem);
  }
  else {
     // did not get it :-(
  }

*/


int k_mut_ceil_set(struct k_t *sem, char prio)
{
	// NB NB assume semaphore is created prior to this call
	// NO CHECK - no mercy !!!
	// work only BEFORE k_start
	// a task will fail if it has higher priority than the ceil on the ceil_mutex...
	if (k_running) {
		return (-2);            // bad bad
	}
	
	if ((prio <= 0) || (DMY_PRIO <= prio)) {
		return (-1);            // bad bad
	}
	sem->ceiling_prio = prio;
	return 0;                   // OK
}

int k_mut_ceil_enter(struct k_t *sem, int timeout)
{
	int retval;
	DI();
	// if ceiling_prio < 0 then its a normal wait call
	if (sem->ceiling_prio < 0) {
		retval = ki_wait(sem, timeout); // could call k_wait but just extra fct call
		EI();
		return retval;
	}
	
	if (pRun->prio < sem->ceiling_prio) {       // I have higher priority than ceiling :-(
		EI();
		return CEILINGFAIL;
	}
	
	retval = ki_wait(sem, timeout);
	// coming back interrupt is still disabled !
	
	
	// chk if we did get semaphore
	if (0 <= retval ) {           // YES did not
		// now we play imm ceiling protocol
		sem->saved_prio = pRun->prio;       // do im ceiling
		pRun->prio = sem->ceiling_prio;     // dont need to reinsert in AQ bq ceil prio is higher or equal to mine and Im already in front of AQ
		prio_enQ(pAQ, deQ(pRun));   // resinsert me in AQ acc to nwe(old) priority	
		// no need for task shift bq we do have highest prio before ceiling
	}
	EI();
	return retval;              // 0(has waited),1(straight through) : ok, -1: timeout== no ceiling
}

int k_mut_ceil_leave(struct k_t *sem)
{
	int res;
	
	DI();
	if (sem->ceiling_prio < 0) {        // just std signal
		return k_signal(sem);
	}
	
	res = ki_signal(sem);       // 1: ok no task to AQ, 0: ok task to AQ
	
	// coming back interrupt is still disabled !
	pRun->prio = sem->saved_prio;       // reset to my old priority	
	prio_enQ(pAQ, deQ(pRun));   // resinsert me in AQ acc to nwe(old) priority	
	ki_task_shift();             // bq maybe started task has higher prio than me
	
	EI();
	
	return (res);
}


#include <krnl.h>


struct k_t *k_crt_sem(char init_val, int maxvalue)
{
	struct k_t *sem;
	
	if (k_running) {
		return (NULL);  // Only before start of krnl
	}
	
	if ((init_val < 0)  || (maxvalue < 0)
		|| (SEM_MAX_VALUE < init_val)
		|| (SEM_MAX_VALUE < maxvalue)
		|| (maxvalue < init_val)) {
		goto badexit;
	}

	if (k_sem <= nr_sem)        // no vacant in buf
	{
		goto badexit;
	}
				
	sem = sem_pool + nr_sem;    // allocate it
	sem->nr = nr_sem;
	nr_sem++;
	
	sem->cnt2 = 0;              // no timer running
	sem->next = sem->pred = sem;        // point at myself == no one in Q
	sem->prio = QHD_PRIO;
	sem->cnt1 = init_val;
		
	sem->maxv = maxvalue;
	sem->clip = 0;
		
	sem->ceiling_prio = -1;     // to indicate ceiling protocol it is not active
	sem->saved_prio = -1;
		
	return (sem);
	
	badexit:
	k_err_cnt++;                // one more error so krnl will not start
		
	return (NULL);
}


int k_set_sem_timer(struct k_t *sem, int val)
{
	// there is no k_stop_sem_timer fct just call with val== 0 for stopping timer fct
	
	if (MAX_SEM_TIMER < val) {
	  return -1 ; // BAD BAD
	}
	
	if (val < 0) {
		val = 0;                // stop stop stop :-)
	}
	
	DI();
	
	if (0 < sem->cnt1) {
		sem->cnt1 = 0; 
	}
	
	sem->cnt2 = sem->cnt3 = val;        // if val is 0 then timer is not running -
	EI();
	
	return (0);
}

int ki_signal(struct k_t *sem)
{
	DI();                       // just in case
	if (sem->cnt1 < sem->maxv) {
		
		sem->cnt1++;            // Salute to Dijkstra
		
		#ifdef KRNLBUG
		k_sem_signal(sem->nr, sem->cnt1);
		#endif
		
		if (sem->cnt1 <= 0) {
			sem->next->cnt2 = 0;        // return code == ok
			prio_enQ(pAQ, deQ(sem->next));
			return (0);         // task was waiting
		} else {
			return (1);         // just delivered a signal - no task was waiting
		}
	}                           
	else {  // CLIP :-(
		if (sem->clip <= MAX_SEM_VAL) {
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

int k_signal(struct k_t *sem)
{
	int res;
	
	DI();
	res = ki_signal(sem);       // 1: ok no task to AQ, 0: ok task to AQ
	
	if (res == 0) {
		ki_task_shift();        // bq maybe started task has higher prio than me
	}
	EI();
	
	return (res);
}

/* normally ki_wait should not be used by user */
int ki_wait(struct k_t *sem, int timeout)
{
	DI();
	
	if (0 < sem->cnt1) {        // is there a signal waiting on us ?
		sem->cnt1--;            // Salute to Dijkstra
		return (1);             // ok: 1 bq we are not suspended
	}
	
	if (timeout < 0)            // no luck, dont want to wait so bye bye
	{
		return (-1); 	        // will not wait so bad luck
	}
	// from here we want to wait
	pRun->cnt2 = timeout;       //  0 == wait forever
	
	if (timeout) {              //  so we can be removed if timeout occurs
		pRun->cnt3 = (int) sem; // nasty keep ref to semaphore in task stomach
	}
	
	sem->cnt1--;                // Salute to Diocjkstra
	
	
	enQ(sem, deQ(pRun));
	ki_task_shift();
	
	// back again - have semaphore received signal or timeout ?
	pRun->cnt3 = 0;             // reset ref to timer semaphore
	
	return ((char) (pRun->cnt2));       // 0: ok , -1: timeout
}

int k_wait(struct k_t *sem, int timeout)
{
	int retval;
	DI();
	retval = ki_wait(sem, timeout);
	EI();
	return retval;              // 0: ok, -1: timeout
}

int k_wait2(struct k_t *sem, int timeout, int *nrClip)
{
	int retval;
	DI();
	retval = ki_wait(sem, timeout);
	if (nrClip) {
		*nrClip = sem->clip;
		sem->clip = 0;
	}
	EI();
	return retval;              // 0: ok, -1: timeout
}


// XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXx

int k_sem_signals_lost(struct k_t *sem)
{
	int x;
	
	DI();
	
	x = sem->clip;
	sem->clip = 0;
	
	EI();
	return x;
}

int ki_semval(struct k_t *sem)
{
	DI();                       // dont remove this - bq k_semval depends on it
	
	return (sem->cnt1);
}

int k_semval(struct k_t *sem)
{
	int v;
	v = ki_semval(sem);
	EI();
	return v;
}


 

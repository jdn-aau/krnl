#include <stdio.h>

#ifdef READERWRITER
//https://en.wikipedia.org/wiki/Readers%E2%80%93writers_problem

void k_rwInit(struct k_rwlock_t *lock)
{
	lock->nrReaders = 0;
	lock->rdwrSem = k_crt_sem(1, 2);
	lock->rdSem = k_crt_sem(1, 2);
	lock->fifoSem = k_crt_sem(1, 2);
}



int k_rwRdEnter(struct k_rwlock_t *lock, int timeout)
{
	// timeout tbi later - if...
	k_wait(lock->fifoSem, 0);
	k_wait(lock->rdSem, 0);
	
	lock->nrReaders++;
	if (lock->nrReaders == 1)
		k_wait(lock->rdwrSem, 0);
	k_signal(lock->fifoSem);
	k_signal(lock->rdSem);
	
}

int k_rwWrEnter(struct k_rwlock_t *lock, int timeout)
{
	k_wait(lock->fifoSem, 0);
	k_wait(lock->rdwrSem, 0);
	k_signal(lock->fifoSem);
	
}

int k_rwRdLeave(struct k_rwlock_t *lock)
{
	k_wait(lock->rdSem, 0);
	lock->nrReaders--;
	if (lock->nrReaders == 0) {
		k_signal(lock->rdwrSem);
	}
	k_signal(lock->rdSem);
}

int k_rwWrLeave(struct k_rwlock_t *lock)
{
	k_signal(lock->rdwrSem);
}
#endif

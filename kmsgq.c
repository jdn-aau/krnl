#include <krnl.h>

int k_msg_count(struct k_msg_t *m)
{
	int v;
	// not needed to DI - its in ki_msg_count ... DI ();
	v = ki_msg_count(m);
	EI();
	return v;
}

struct k_msg_t *k_crt_send_Q(int nr_el, int el_size, void *pBuf)
{
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
	pMsg->nr = nr_send;         // I am element nr nr_send in msgQ pool
	nr_send++;
	
	pMsg->sem = k_crt_sem(0, nr_el);    // we are using a sem for sync part snd <-> rcv
	
	if (pMsg->sem == NULL) {
		goto errexit;
	}
	
	pMsg->pBuf = (char *) pBuf;
	pMsg->r = pMsg->w = -1;
	pMsg->el_size = el_size;
	pMsg->nr_el = nr_el;
	pMsg->lost_msg = 0;
	pMsg->cnt = 0;              // count nr elm in Q
	
	return (pMsg);
	
	errexit:
	k_err_cnt++;
	return (NULL);
}

int ki_clear_msg_Q (struct k_msg_t *pB)
{
	int ret;	
	if (k_running) {
		return -2;
	}
	
	ret = pB->cnt;
	if ( 0 < ret) { // messages pending s0
		pB->lost_msg  = 0;
		pB->cnt = 0; // reset
		pB->r = pB->w = -1;
		// clear sem - can do it bq no one is waiting bq 0 < ret  == pending messages
		pB->sem->cnt1 = 0;  // Serious NASTY
	}
	return ret;
}

int k_clear_msg_Q (struct k_msg_t *pB)
{
	int r;
	DI();
	r = ki_clear_msg_Q(pB);
	EI();
	return r;
}

char ki_send(struct k_msg_t *pB, void *el)
{
	
	int i;
	char *pSrc, *pDst;
	
	if (pB->nr_el <= pB->cnt) {
		// nope - no room for a putting new msg in Q ?
		if (pB->lost_msg < SEM_MAX_VALUE) {
			pB->lost_msg++;
		}
		#ifdef KRNLBUG
		k_send_Q_clip(pB->nr, pB->lost_msg);
		#endif
		return (-1);            // nope
	} else {
		
		pB->cnt++;
		
		pSrc = (char *) el;
		
		pB->w++;
		if (pB->nr_el <= pB->w) // simple wrap around
		{
			pB->w = 0;
		}
		
		pDst = pB->pBuf + (pB->w * pB->el_size);        // calculate where we shall put msg in ringbuf
		
		for (i = 0; i < pB->el_size; i++) {
			// copy to Q
			*(pDst++) = *(pSrc++);
		}
		return (ki_signal(pB->sem));    // indicate a new msg is in Q
	}
}

char k_send(struct k_msg_t *pB, void *el)
{
	char res;
	
	DI();
	
	res = ki_send(pB, el);
	if (res == 0)               // if new task in AQ == someone was waiting for msg
	{                           // if 1 then nobody was waiting so no neeed for task shift
		ki_task_shift();
	}
	
	EI();
	return (res);
}




char ki_receive(struct k_msg_t *pB, void *el, int *lost_msg)
{
	int i;
	char r, *pSrc, *pDst;
	
	// can be called from ISR bq no blocking
	DI();                       // just to be sure
	
	if (0 <= (r = ki_wait(pB->sem, -1))) {
		
		pDst = (char *) el;
		pB->r++;
		pB->cnt--;              // got one
		
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
		return (r);             // yes
	}
	
	return (-1);                // nothing for you my friend
}

char k_receive(struct k_msg_t *pB, void *el, int timeout, int *lost_msg)
{
	
	int i;
	char r, *pSrc, *pDst;
	
	DI();
	
	if (0 <= (r = ki_wait(pB->sem, timeout))) {
		// yes we did get a msg :-)
		// ki_wait bq then intr is not enabled when coming back
		
		pDst = (char *) el;
		pB->r++;
		pB->cnt--;              // got one
		
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
		return (r);             // 1 if no suspension bq msg was already present, 0: ok  if you have waited on msg
	}
	
	EI();
	return (-1);                // nothing for you my friend
}

int ki_msg_count(struct k_msg_t *m)
{
	DI();                       // dont remove this - bq k_semval depends on it
	return m->cnt;
}



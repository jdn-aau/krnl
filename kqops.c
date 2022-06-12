#include <krnl.h>


//---QOPS--- double chained lists with qhead as a element

/*        ------------------------------------------------
 *          |                                              |
 *       \/                                              |
 *    QHEAD (next)-->first-elm (next)-->next-elm(next) --|
 *       ^                   ^
 *       ------------(pred)  -----------(pred)  char s1[150],s2[150],s3[150],s4[150];
 * 
 */


void enQ(struct k_t *Q, struct k_t *el)
{
	el->next = Q;
	el->pred = Q->pred;
	Q->pred->next = el;
	Q->pred = el;
}

struct k_t *deQ(struct k_t *el)
{
	el->pred->next = el->next;
	el->next->pred = el->pred;
	
	return (el);
}

void prio_enQ(struct k_t *Q, struct k_t *el)
{
	char prio = el->prio;
	
	Q = Q->next;                // bq first elm is Q head itself
	
	while (Q->prio <= prio)     // find place before next with lower prio
	{
		Q = Q->next;
	}
	
	el->next = Q;               // insert before element referred by Q
	el->pred = Q->pred;
	Q->pred->next = el;
	Q->pred = el;
}


#include <krnl.h>


#ifdef DYNMEMORY
void * __attribute__((weak)) k_malloc(int k)
{
	void *m;
	DI();
	m = malloc(k);
	EI();
	return m;
}

/* never relase mem again - bq over time oyo may have fragmented memory */

void __attribute__((weak)) k_free(void *m)
{
}
#endif


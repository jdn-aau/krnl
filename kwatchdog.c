#include <krnl.h>

#ifdef WDT_TIMER
#include <avr/wdt.h>
#endif

#include <avr/interrupt.h>


// -----------------------------


void __attribute__((weak)) k_wdt_enable(int i)
{
	DI();
	wdt_enable(i);
	k_wdt_enabled = 1;
	EI();
}


void __attribute__((weak)) k_wdt_disable(void)
{
	DI();
	k_wdt_enabled = 0;
	wdt_disable();
	EI();
}


//220216

/* critical region - by semaphores - aka mutex
 *    Signal from ISR
 * 
 *    Work on uno and mega :-)
 *    JDN
 *  */

#include <krnl.h>

#define STK 150

#define TASKPRIO 10


struct k_t * pTask1;

struct k_t *mutSem, *syncSem;

struct k_t *timedSem1, *timedSem2;

volatile int t2Missed = 0, t2Hit = 0;


volatile int ISRoverflow = 0;


#define debounceTime 200
unsigned long lastISR = 0;

#if defined (__AVR_ATmega2560__) || defined (__AVR_ATmega1280__) || defined(__AVR_ATmega2561__)
ISR(INT4_vect, ISR_NAKED) // digital pin 2 onmega
#endif

//UNO  set interrupt on pin 2
#if defined(__AVR_ATmega168__) || defined(__AVR_ATmega328P__) || defined(__AVR_ATmega328__) || defined(__AVR_ATmega32U4__)
ISR(INT0_vect, ISR_NAKED) // digital pin 2 on uno
#endif
{
	PUSHREGS();
	
	if (!k_running)
		goto exitt ;
	
	// comment out down to endofdebounce if you do not like Sw debounce
	if (debounceTime > (k_millis_counter - lastISR)) {
		goto exitt;
	}
	
	lastISR += debounceTime;
	
	// endofdebounce
	
	if (-1 == ki_signal(syncSem))
		ISRoverflow++;
	
	K_CHG_STAK();
	
	exitt:
	POPREGS();
	RETI();
}


/*
 *   sw debounce
 *   #define debounceTime 200
 *   unsigned long lastISR=0;
 * 
 *   ...
 *   PUSHREGS();
 *   if (debounceTime < (k_millis_counter - lastISR)) {
 *     lastISR+=debounceTime;
 *     ISR code
 *   }
 *   POPREGS();
 *   RETI();
 */

void task1()
{
	int i = 0;
	int miss = 0;
	int clp, sumclp = 0;
	while (1) {
		
		if (0 > k_wait2(syncSem, 1000, &clp)) {  // or k_wait(syncSem,1000)  with no clip info
			
			//if (-1 == k_wait(syncSem, 1000)) {
			miss++;
			Serial.print("Timeout "); Serial.print(miss); Serial.print(" "); Serial.println(ISRoverflow);
		}
		else {
			i++;
			miss = 0;
			Serial.print("get kick no "); Serial.print(i); Serial.print(" "); Serial.println(ISRoverflow);
		}
		sumclp += clp;
		Serial.println("lost msg "); Serial.println(sumclp);
	}
}


char a[STK];


void setup() {
	// for debugging - only on uno eq- If Mega etc use PORTA(which is pn 22,23,...)
	// see http://kom.aau.dk/~jdn/edu/doc/arduino/mega.png
	
	Serial.begin(115200);
	delay(500);
	
	Serial.println("\n\njust bef init part");
	
	
	k_init(1, 1, 0); // 2 task, 1 semaphores, 0 messaegQueues */

pTask1 = k_crt_task(task1, 15, a,  STK);

syncSem = k_crt_sem(0, 10); // 1: start value, 10: max value (clipping)


pinMode(2, INPUT_PULLUP);  // on uno - - a button to ground (or wire) every time it goes low vupti an interrupt

// ABOUT ISR
// UNO/LENARDO pp 54   https://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-7810-Automotive-Microcontrollers-ATmega328P_Datasheet.pdf  p 54
//MEGA 2560 pp 110 https://ww1.microchip.com/downloads/en/devicedoc/atmel-2549-8-bit-avr-microcontroller-atmega640-1280-1281-2560-2561_datasheet.pdf

__asm__ volatile ("cli");  // disable interrupt

#if defined (__AVR_ATmega2560__) || defined (__AVR_ATmega1280__) || defined(__AVR_ATmega2561__)
EICRB |= (1 << ISC41); //  B00000010; // for mega on pin 2
EIMSK |= (1 << INT4); //B00010000; // enable INT 4  for mega

Serial.println(EICRB, HEX);
Serial.println(EIMSK, HEX);
#endif

//UNO  set interrupt on pin 2
#if defined(__AVR_ATmega168__) || defined(__AVR_ATmega328P__) || defined(__AVR_ATmega328__) || defined(__AVR_ATmega32U4__)
EICRA |= (1 << ISC01); // UNON UNO .,. trigger INT0 on falling edge
EIMSK |= (1 << INT0);  // enable external intr
#endif

__asm__ volatile ("sei");  // enable interrupt

k_start(); /* start krnl timer speed 1 milliseconds*/

Serial.println("If you see this then krnl didnt start :-( ");
}

void loop() {}

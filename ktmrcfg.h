#include <krnl.h>

/* which timer to use for krnl heartbeat
 *   timer 0 ( 8 bit) is normally used by millis - avoid !
 *   timer 1 (16 bit)  DEFAULT
 *   timer 2 ( 8 bit)
 *   timer 3 (16 bit) 1280/1284P/2560 only (MEGA)
 *   timer 4 (16 bit) 1280/2560 only (MEGA)
 *   timer 5 (16 bit) 1280/2560 only (MEGA)
 */

#if (KRNLTMR == 0)
JDN SIGER NOGO IN SIMPLIFY VRS
// normally not goood bq of arduino sys timer is on timer 0so you wil get a compile error
// 8 bit timer !!!
#define KRNLTMRVECTOR TIMER0_OVF_vect
/* we use setting from original timer0
 *   #define TCNTx TCNT0
 *   #define TCCRxA TCCR0A
 *   #define TCCRxB TCCR0B
 *   #define TCNTx TCNT0
 *   #define OCRxA OCR0A
 *   #define TIMSKx TIMSK0
 *   #define TOIEx TOIE0
 *   #define PRESCALE 0x07
 *   #define COUNTMAX 255
 *   #define DIVV 15.625
 *   #define DIVV8 7.812
 */
#elif (KRNLTMR == 1)
#define KRNLTMRVECTOR TIMER1_OVF_vect
#define TCNTx TCNT1
#define TCCRxA TCCR1A
#define TCCRxB TCCR1B
#define TCNTx TCNT1
#define OCRxA OCR1A
#define TIMSKx TIMSK1
#define TOIEx TOIE1
#define PRESCALE 0x03
#define COUNTMAX 0xffff
#define DIVV 250
#define DIVV8 (DIVV/2)

#elif (KRNLTMR == 2)

// 8 bit timer !!!
// standard for krnl CHECK which pwm, tone etc is on this timer
#define KRNLTMRVECTOR TIMER2_OVF_vect
#define TCNTx TCNT2
#define TCCRxA TCCR2A
#define TCCRxB TCCR2B
#define TCNTx TCNT2
#define OCRxA OCR2A
#define TIMSKx TIMSK2
#define TOIEx TOIE2
#define PRESCALE 0x05
#define COUNTMAX 255
#define DIVV 125
#define DIVV8 64

#elif (KRNLTMR == 3)

#define KRNLTMRVECTOR TIMER3_OVF_vect
#define TCNTx TCNT3
#define TCCRxA TCCR3A
#define TCCRxB TCCR3B
#define TCNTx TCNT3
#define OCRxA OCR3A
#define TIMSKx TIMSK3
#define TOIEx TOIE3
#define PRESCALE 0x03
#define COUNTMAX 0xffff
#define DIVV 250
#define DIVV8 (DIVV/2)

#elif (KRNLTMR == 4)

#define KRNLTMRVECTOR TIMER4_OVF_vect
#define TCNTx TCNT4
#define TCCRxA TCCR4A
#define TCCRxB TCCR4B
#define TCNTx TCNT4
#define OCRxA OCR4A
#define TIMSKx TIMSK4
#define TOIEx TOIE4
#define PRESCALE 0x03
#define COUNTMAX 0xffff
#define DIVV 250
#define DIVV8 (DIVV/2)

#elif (KRNLTMR == 5)

#define KRNLTMRVECTOR TIMER5_OVF_vect
#define TCNTx TCNT5
#define TCCRxA TCCR5A
#define TCCRxB TCCR5B
#define TCNTx TCNT5
#define OCRxA OCR5A
#define TIMSKx TIMSK5
#define TOIEx TOIE5
#define PRESCALE 0x03
#define COUNTMAX 0xffff
#define DIVV 250
#define DIVV8 (DIVV/2)

#else

#pragma err "KRNL: no valid tmr selected"

#endif


/*
 *         >>>> krnl.h <<<<
 * my own small KeRNeL adapted for Arduino
 *
 * this version adapted for Arduino
 *
 * (C) 2012
 * 
 * Jens Dalsgaard Nielsen <jdn@es.aau.dk>
 * http://www.control.aau.dk/~jdn
 * Section of Automation & Control
 * AAU SATLAB
 * Aalborg University,
 * Denmark
 *
 * "THE BEER-WARE LICENSE"
 * <jdn@es.aau.dk> wrote this file. As long as you retain this notice and follow GPL v2 you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return :-) or if you are real happy then ...
 * single malt will be well received :-)
 *
 

****
 *
 * Background for KeRNeL
 * - educational purpose and need for an easy open well function preemptive kernel.
 *   - straight easy to read and understand code
 *   - ... which may have a slight impact on performance
 *   - no install circus - just open a sketch 
 * - Minimal impact on std Arduino SW
 *   - "steal" timer2 and therefor pwm 3 and 11
 *   - rest is leaved untouched
 * - Simple approach: you cant create task, semaphores and msgQ after k_start
 *
 * WARNING WARNING WARNING
 * - WARNING It seems that Serial.print has problems when printing numbers
 *  int i;
 *  Serial.print(i)  may fail !!!!
 * The eror has been reproduced on avr32 + freertos (JAL) - maybe a C++ lib problem ?
 *
 * - SUGGESTION
 * int i;
 * char s[1];
 *  itoa(i,s,10);
 * Serial.print(s)  seems to work ...
 *
 * FOR float USE
 * http://arduino.cc/forum/index.php/topic,44262.0.html 
 *   
 char *ftoa(char *a, double f, int precision)
  {
  long p[] = {0,10,100,1000,10000,100000,1000000,10000000,100000000};
  
  char *ret = a;
  long heiltal = (long)f;
  itoa(heiltal, a, 10);
  while (*a != '\0') a++;
  *a++ = '.';
  long desimal = abs((long)((f - heiltal) * p[precision]));
  itoa(desimal, a, 10);
  return ret;
  }
  
 * /Jens
 *
 *****************************************************************************************
 *
 ***** KeRNeL calls KeRNeL calls KeRNeL calls KeRNeL calls KeRNeL calls KeRNeL calls  ****

NB NB 
1. k_init();
  - init kernel but do not start !
2. k_start(msec)    starts KeRNeL. Main program will NOT return from k_start
  - msec(int) timertick in msec.From 1 to more than 32000 msec
  - To reduce timer2 isr load select timertick as high as possible
   
CHECK. 
  - Only check carried  out is that the create calls (task, semaphores and mesQs) will not work after k_start.
  - No control on KeRNeL elements so take care. I you do a k_signal to a thread descriptor then it's your problem

MEMORY USAGE
  - Please adjust KeRNeL data structs size by setting defines (mentioned below) so you dont have no more KeRNeL elements than needed.  
   - #define K_USR_TASK 5
   - #define K_USR_SEM 4
   - #define K_USR_SENDQ 2

TIMER2 / tone library (/pwm out ch 3,11
  - KeRNeLs heartbeat uses Timer2. 
  - So you cant use PWM on pin 3 or 11 and the same goes for sound (tone)
  - You can ofcourse use another timer or external interrupt. 
 
COMMENTS IN GENERAL
  - It seems that millis etc works okay
  - Take care when using non re-entrant code like Serial.print - do a mutex with a semaphore
   - Can be a good idea to inspec k_err_cnt after create of tasks etc just to see if there
    were any errors.
 
******

Some add tech info

HOW TO GET INFO about your progtram on a linux box
jdn@jdn-lat ~ $ cd /tmp
jdn@jdn-lat /tmp $ find . -name krnl*elf -print
./build6916815778938108877.tmp/krnl01.cpp.elf
find: `./pulse-PKdhtXMmr18n': Permission denied
jdn@jdn-lat /tmp $ cd build6916815778938108877.tmp/
jdn@jdn-lat /tmp/build6916815778938108877.tmp $ avr-size -C krnl01.cpp.elf 
AVR Memory Usage
----------------
Device: Unknown

Program:    4048 bytes
(.text + .data + .bootloader)

Data:        437 bytes
(.data + .bss + .noinit)

TO see the generated codde avr-objdump -S xxx.cpp.elf  (xxx your file as above) 


*********** 




int freeRam(void)

------------------ 

YOUR OWN ISRs
See timer interrupt and the NAKED approach. Its important
so you keep same stak layout so you can call do your own task shift

It is not promoted as a good idea to do cascade interrupts, meaning dont enable interrupt
inside an ISR routine


*****/


/**** http://arduino.cc/forum/index.php?topic=92664.0

// FOR INSPIRATION
#include <avr/interrupt.h>

{ 
    pinMode(2, INPUT); 
    digitalWrite(2, HIGH);    // Enable pullup resistor 
    sei();                    // Enable global interrupts 
    EIMSK |= (1 << INT0);     // Enable external interrupt INT0 
    EICRA |= (1 << ISC01);    // Trigger INT0 on falling edge 
} 
                              // 
void loop(void) 
{ 
                              // 
} 
                              // 

**** To integrate with KeRNeL

// Interrupt Service Routine attached to INT0 vector 
ISR(EXT_INT0_vect,ISR_NAKED) 
{ 
  PUSHREGS();
    digitalWrite(13, !digitalRead(13));    // Toggle LED on pin 13 
    you can do a ki_signal
    
    POPREGS();
    reti();
}

*/


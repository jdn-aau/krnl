      >>>  KRNL - a small preemptive kernel for small systems <<<
       
I have found it interesting to develop an open source realtime kernel 

for the Arduino platform - but is also portable to other platforms


2021 March
Added multiple reader/writers. in version 2021-03-01 or newer.

// https://en.wikipedia.org/wiki/Readers-writers_problem

2021 January:
Krnl used timer0 uns a 1 msec kernel tick even if you give another value to k_start.

It can be changed in the c source awhere i

- SEE SOME NOTES BELOW ABOUT TIMERS AND PINS 
- Now doxygen docu at html directory :-)
- See krnl.h for further comments
- - timers
- - 8/16 MHz setting
- - etc

June 2016 - added watch dog timer - see below

Some highlights
---------------

last news
Jan 2021 - std timer0 for all AVRs CPU and hearbeat is 1msec
           millis is still working - but dont  use it :-)
- open source (beer license)
- well suited for teaching
 - easy to read and understand source
 - priority to straight code instead insane optimization(which will make it nearly unreadable)

- well suited for serious duties - but with no warranty what so ever - only use it at own risk !!!

- easy to use
 - just import library krnl and you are ready

- automatic recognition of Arduino architeture
 - supports all atmega variants I have had available (168,328,1280,2560 - uno, duemillanove, mega 1280 and 2560)
Some characteristics:

- preemptive scheduling 
 - Basic heart beat at 1 kHz. KRNL can have heeartbeat in quants of milli seconds
 - static priority scheme
-  support task, semaphores, message queues
 - All elements shall be allocated prior to start of KRNL
- support user ISRs and external interrupts

- supervision og KeRNeL calls
- all suspending calls (like k_wait, k_receive) returns 
-- 1 if there was a signal/message waiting for you have not been on hold
-- 0 is you have been suspended but later receive signal/message
-- -1 : timeout
- all signalling calls (like k_signal, k_send) returns
-- 1 if signal/message has been delivered but no receiver present at primitive
-- 0 if signal/message has been delivered and receiever was present and was moved to activeQ
-- -1 if max limit of semaphore/msg Q has been exceeded 

- timers
 - krnl can be configures to use tmr 1,2 and for mega also 3,4,5 for running krnl tick
 - For timer 0 you should take care of millis and it will require some modifications in arduino lib
 - see krnl.h for implications (like 

- Accuracy
 - 8 bit timers (0,2) 1 millisecond is 15.625 countdown on timer
   - example 10 msec 156 instead of 156.25 so an error of 0.25/156.25 ~= 0.2%
 - 16 bit timers count down is 1 millisecond for 62.5 count
 - - example 10 msec ~ 625 countdown == precise :-)

- Watchdog timer
From vrs 2016056 the timer interrupt do issue a wdt_reset() for every timer interrupt.
Krnl can run sys tick at 1-10,20,30,40,... msec. 
If tick speed slower than 10 msec is selected krnl runs a 10 msec tick speed and drive the krnl code in fraction hereof.
Which means ... that the wdt is reset at least every 10 millisecond.

To enable do:

1. in top of your sketch : #include \<avr/wdt.\h>
2. first line in setup:  wdt_disable() 
3. last line before k_start:  wdt_enable(WDTO_30MS)
    or whatever interval you find appropriate.

Beware of not seeting it to small so you Arduino wont be able to start before wdt timout
Wdt setup will also be active after wdt reset og button reset. 
the only way to get rid of it is by powr off system. And if your sw enable it again then ...

So wdt time to 60/120 msec AND follow the procedure above might be the what to do it.

So think a litle about wdt "timing"q

In avr/wdt.h following is defined:

- WDTO_15MS   0
- WDTO_30MS   1
- WDTO_60MS   2
- WDTO_120MS   3
- WDTO_250MS   4
- WDTO_500MS   5
- WDTO_1S   6
- WDTO_2S   7
- WDTO_4S   8
- WDTO_8S   9




See in krnl.h for information like ...

... from http://blog.oscarliang.net/arduino-timer-and-interrupt-tutorial/
Timer0:
- Timer0 and 2  is a 8bit timer.
- In the Arduino world Timer0 is been used for the timer functions, like delay(), millis() and micros().
-  If you change Timer0 registers, this may influence the Arduino timer function.
- So you should know what you are doing.

Timer1:
- Timer1 is a 16bit timer.
- In the Arduino world the Servo library uses Timer1 on Arduino Uno (Timer5 on Arduino Mega).

Timer2:
- Timer2 is a 8bit timer like Timer0.
- In the Arduino work the tone() function uses Timer2.

Timer3, Timer4, Timer5: Timer 3,4,5 are only available on Arduino Mega boards.
- These timers are all 16bit timers.


Install from github:

1) cd whatever/sketchbook/libraries   - see Preferences for path to sketchbook
2) git clone https://github.com/jdn-aau/krnl.git

NB NB NB - TIMER HEARTBEAT
 From vrs 1236 you can change which timer to use in krnl.c Just look in top of file for KRNLTMR
 - tested with uno and mega 256

In krnl.c you can configure KRNL to use timer (0),1,2,3,4 or 5. (3,4,5 only for 1280/2560 mega variants)

Timer 1,2,3,.. can be selected by a define in top of krnl.h  (KRNLTMR)
See krnl.h

Timer0 is a litle tricky
1) Set KRNLTMR to 0
AND ...
2) Comment out timer0 ISR in Arduino <path to your install>/hardware/arduino/avr/cores/arduino/wiring.c (ISR(TIMER0\_OVF\_vect)) By setting KRNLTMR to 0 KeRNeL timer replace Arduinos timer at timer0 and maintain millis and micros counters so no animal will be harmed


You can select heartbeat between 1 and 32767 milliseconds in 1 msec steps.

- Timer0 - An 8 bit timer used by Arduino functions delay(), millis() and micros(). BEWARE
- Timer1 - A 16 bit timer used by the Servo() library
- Timer2 - An 8 bit timer used by the Tone() library
- Timer3,4,5 16 bits
    
    
... from http://arduino-info.wikispaces.com/Timers-Arduino

- Servo Library uses Timer1. 
- -  You can’t use PWM on Pin 9, 10 when you use the Servo Library on an Arduino. 
- -  For Arduino Mega it is a bit more difficult. The timer needed depends on the number of servos. 
- -  Each timer can handle 12 servos. 
- -  For the first 12 servos timer 5 will be used (losing PWM on Pin 44,45,46). 
- -  For 24 Servos timer 5 and 1 will be used (losing PWM on Pin 11,12,44,45,46).. 
- -  For 36 servos timer 5, 1 and 3 will be used (losing PWM on Pin 2,3,5,11,12,44,45,46).. 
- -  For 48 servos all 16bit timers 5,1,3 and 4 will be used (losing all PWM pins).

- Pin 11 has shared functionality PWM and MOSI. 
- -  MOSI is needed for the SPI interface, You can’t use PWM on Pin 11 and the SPI interface at the same time on Arduino. 
- -  On the Arduino Mega the SPI pins are on different pins.

- tone() function uses at least timer2. 
- -  You can’t use PWM on Pin 3,11 when you use the tone() function an Arduino and Pin 9,10 on Arduino Mega.

(c)
* "THE BEER-WARE LICENSE" (frit efter PHK)           *
 * <jdn@es.aau.dk> wrote this file. As long as you    *
 * retain this notice you can do whatever you want    *
 * with this stuff. If we meet some day, and you think*
 * this stuff is worth it ...                         *
 *  you can buy me a beer in return :-)               *
 * or if you are real happy then ...                  *
 * single malt will be well received :-)              *
 *                                                    *
 * Use it at your own risk - no warranty       

Happy hacking

See also http://kom.aau.dk/~jdn/edu/doc/arduino/krnl

/Jens

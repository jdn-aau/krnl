The examples for krnl er enumerated k01, k02 etc
The idea is there is a progressing in the demoprograms

See github.com/jdn-aau or
es.aau.dk/staff/jdn/edu/doc/arduino/krnl

Happy krnl hacking :-)


... this file need a cleanup ... /Jens ...

/***********************

NB NB ABOUT WRAP AROUND AND KRNL TIMERS

Krnl maintain a milisecond timer (k_millis_counter)
It s 32 bit unsigned long so it wraps around after 49.7 days.
As all timing internal in krnl is relative (from now) then
wrap around will have no influence on krnl !!!

NB NB ABOUT TIMERS PORTS ETC

You can configure krnl to use timer 0,1,2,3,4,5

Default uno,mega timer 2
leonardo (32u4 cpu) timer 3

Normally you cant use timer 0 bq it is used for millis and preallocated.

YOU CAN configure KeRNeL for using timer 0.
It requires modification in ..../wiring.c where you shall disable the original INT0 timer isr in wiring.c
By setting KRNLTMR to 0 you will enable a KeRNel builtin replacement for timer0 so everything 
including millis and micros will work as usual.


See below

When using a timer you must be aware of that it will prohibit you from things like
- tone (pwm sound) uses timer2

... from http://blog.oscarliang.net/arduino-timer-and-interrupt-tutorial/

Timer0:
- Timer0 is a 8bit timer.
- In the Arduino world Timer0 is been used for the timer functions, like delay(), millis() and micros().
-  If you change Timer0 registers, this may influence the Arduino timer function.
- So you should know what you are doing.

- Timer1 is a 16bit timer.
- In the Arduino world the Servo library uses Timer1 on Arduino Uno (Timer5 on Arduino Mega).

Timer2:
 - Timer2 is a 8bit timer like Timer0.
 -In the Arduino work the tone() function uses Timer2.

Timer3 16 bits
- 1280/1284P and 2560 only

Timer4, Timer5 16 bits
- 1280 and 2560 only

On Uno
- Pins 5 and 6: controlled by timer0
- Pins 9 and 10: controlled by timer1
- Pins 11 and 3: controlled by timer2

On the Arduino Mega we have 6 timers and 15 PWM outputs:

TODO pinout below need checkup

- Pins 4 and 13: controlled by timer0
- Pins 11 and 12: controlled by timer1
- Pins 9 and10: controlled by timer2
- Pin 2, 3 and 5: controlled by timer 3
- Pin 6, 7 and 8: controlled by timer 4
- Pin 46, 45 and 44:: controlled by timer 5

... from http://arduino-info.wikispaces.com/Timers-Arduino

- Servo Library uses Timer1.
-- You can’t use PWM on Pin 9, 10 when you use the Servo Library on an Arduino.
-- For Arduino Mega it is a bit more difficult. The timer needed depends on the number of servos.
-- Each timer can handle 12 servos.
-- For the first 12 servos timer 5 will be used (losing PWM on Pin 44,45,46).
-- For 24 Servos timer 5 and 1 will be used (losing PWM on Pin 11,12,44,45,46)..
-- For 36 servos timer 5, 1 and 3 will be used (losing PWM on Pin 2,3,5,11,12,44,45,46)..
-- For 48 servos all 16bit timers 5,1,3 and 4 will be used (losing all PWM pins).

- Pin 11 has shared functionality PWM and MOSI.
-- MOSI is needed for the SPI interface, You can’t use PWM on Pin 11 and the SPI interface at the same time on Arduino.
-- On the Arduino Mega the SPI pins are on different pins.

- tone() function uses at least timer2.
-- You can’t use PWM on Pin 3,11 when you use the tone() function an Arduino and Pin 9,10 on Arduino Mega.

SO BEWARE !!!

***********************/


Jens, AAU

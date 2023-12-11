/*! \mainpage KRNL
 *
 * my own small KeRNeL adapted for Arduino
 *
 * (C) 2012,2013,2014,...,2022
 *
 * Jens Dalsgaard Nielsen <jdn@es.aau.dk>
 * http://www.control.aau.dk/~jdn
 * Section of Automation & Control
 * Aalborg University,
 * Denmark
 *
 * "THE BEER-WARE LICENSE" (frit efter PHK)
 * <jdn@es.aau.dk> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return :-) or if you are real happy then ...
 * single malt will be well received :-)
 *
 * (C) Jens Dalsgaard Nielsen
 *
 * \section Introduction
 * KRNL is a preemptive realtime kernel here ported for Arduino
 *
 * See krnl.h for documentation or follow the links below
*
* \tableofcontents

 One note before starting:

\section NOTE
In standard version you can only select 1 millisecond in k_start. Otherwise it will not start
\section  a1 Before you start

All initialization must be carried out after k_init and BEFORE k_start

So no creation of semaphores, tasks, etc when the system is running.

Why ? because we are dealing with embedded systems and once your system is configured it shall just run.

I do also really advise not to use dynamic memory after k_start: no malloc and free when your system is running.

If ... you need to do so please protect memory by a mutex (semaphore + k_wait and k_signal) or just encapsule malloc in DI() and EI().
And never never use and rely on free. You may end up with fragmented memory which on the long run is of no use.

So the advise is to

- create all tasks, semaphores etc
- allocate what is needed of memory, resources etc
- and then k_start

\section a112 Memory usage

Use freeRam for testing (part of krnl)

- Task descriptor 17B
- Semaphore descriptor 17B
- Message Queue descriptor 33B (17B for semaphore + 16B)


On the 1280/2560 a few more bytes is used due to extended program counter register

KRNL itself uses approx 190B before allocating task/sem/msgQ descriptors

\section a3 Initialization
If any call fails (like no more RAM or bad parameters) KRNL will not start but will return an error code in k_start
- int k_init(int nrTask, int nrSem, int nrMsg);
- int k_start(); // heartbeat is 1 msec  

\section a4 Creation calls - before k_start
\subsection a41 Semaphore
- struct k_t * k_crt_sem(char init_val, int maxvalue);
\subsection a42 Task
- struct k_t * k_crt_task(void (*pTask)(void), char prio, char *pStk, int stkSize);
\subsection a43 Message Queue
- struct k_msg_t * k_crt_send_Q(int nr_el, int el_size, void *pBuf);


\section a55 Semaphore runtime calls
\subsection a5 User space
- char k_set_sem_timer(struct k_t * sem, int val);
- char k_signal(struct k_t * sem);
- char k_wait(struct k_t * sem, int timeout);
- char k_wait2(struct k_t * sem, int timeout, int * clip);


\subsection a51 ISR space
_i indicates that no lock/unlock(disable/enable) is carried out

- char ki_signal(struct k_t * sem);
- char ki_nowait(struct k_t * sem);
- char ki_wait(struct k_t * sem, int timeout);
- int ki_semval(struct k_t * sem);

\section a61 Message Queue calls
\subsection a611 User space
- char k_send(struct k_msg_t *pB, void *el);
- char k_receive(struct k_msg_t *pB, void *el, int timeout, int *lost_msg);

\subsection a612 ISR space
- char ki_send(struct k_msg_t *pB, void *el);
- char ki_receive(struct k_msg_t *pB, void *el, int * lost_msg);

\section a8 Task calls
- void ki_task_shift(void) __attribute__ ((naked));
- char k_sleep(int time);
- char k_set_prio(char prio);
- int k_stk_chk(struct k_t *t);

\section a9 Div calls
- int k_unused_stak(struct k_t *t);
- int freeRam(void);
- void k_eat_msec(unsigned int time);

\section timers
https://ceezblog.info/2018/07/10/arduino-timer-pwm-cheat-sheet/

UNO and mega uses timer2 for krnl ticks so avoid using libraries that are using these timers

So look below and observe that UNO timer2 has normally PWM support based on timer2 on pin 3 and 11

On UNO tone do also use timer2 !! so dont use tone hen running timer (or find library newTone)

Arduino UNO (ATmega328p) has three 8bit timers

- Timer0 - used for millis() micros() delay()… and is on pin 5, 6
- Timer1 - 16bit timer is on pin 9, 10
- Timer2 - 8bit timer is on pin 3, 11

Arduino Mega has six 8bit timers

- Timer0 - millis, micros… and is on pin 4, 13
- Timer1 - is on pin 11, 12
- Timer2 - is on pin 9, 10
- Timer3 - is on pin 2, 3, 5
- Timer4 - is on pin 6, 7, 8
- Timer5 - is on pin 46, 45, 44

 Don’t mess with Timer0 since we need to use millis() or micros() for measuring time
 
\section Pitfalls Pitfalls when using timers
Welcome to the real world :-)
 
There exist some pitfalls you may encounter, when programming your Arduino and making use of functions or libraries that uses timers.

- Servo Library uses Timer1. 
	- You can’t use PWM on Pin 9, 10 when you use the Servo Library on an Arduino. 	
	- For Arduino Mega it is a bit more difficult. The timer needed depends on the number of servos. 
	- Each timer can handle 12 servos. For the first 12 servos timer 5 will be used (losing PWM on Pin 44,45,46). 
	- For 24 Servos timer 5 and 1 will be used (losing PWM on Pin 11,12,44,45,46).. 
	- For 36 servos timer 5, 1 and 3 will be used (losing PWM on Pin 2,3,5,11,12,44,45,46).. 
	- For 48 servos all 16bit timers 5,1,3 and 4 will be used (losing all PWM pins).
   
- Pin 11 has shared functionality PWM and MOSI. MOSI is needed for the SPI interface, 
	- You can’t use PWM on Pin 11 and the SPI intefk_rface at the same time on Arduino. 
	- On the Arduino Mega the SPI pins are on different pins.
    
- tone() function uses at least timer2. 
	- You can’t use PWM on Pin 3,11 when you use the tone() function an Arduino and Pin 9,10 on Arduino Mega.



*/

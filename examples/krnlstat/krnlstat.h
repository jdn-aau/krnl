/**
 *         >>>> krnl.h<<<<
 *
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
 * <jdn@es.aau.dk> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return :-) or if you are real happy then ...
 * single malt will be well received :-)
 *
 * See readme file for more info
 *
 * tested with duemilanove w/328, mega2560, uno R3, seeduino 1280,
 */

#ifndef KRNL
#define KRNL

// remember to update in krnl.c !!!
#define KERNEL_VRS 108

// for guessing on architecture ...

#if defined(__AVR_ATmega168__) 
#define ARCH_SLCT 1
#endif

#if defined(__AVR_ATmega328P__) 
#define ARCH_SLCT 2
#endif

#if defined(__AVR_ATmega32U4__)
#define ARCH_SLCT 3
#endif

#if defined (__AVR_ATmega1280__) 
#define ARCH_SLCT 4
#endif

#if  defined(__AVR_ATmega2560__)
#define ARCH_SLCT 6
#endif

#ifndef ARCH_SLCT
#error Failing unknown architecture - JDN
#endif

// some compile time defines

//#define DMYBLINK     // ifdef then led (pin13) will light when dummy is running
// USE k_unused_stak instead and do printing by yourself
#define MSGQ           // ifdef then msg q's are created
// ---------------------------

#define K_USR_TASK 5
#define K_USR_SEM 10
#define K_USR_SENDQ 5

#define K_TASK  (K_USR_TASK+1)    			
#define K_SEM   (K_USR_SEM+K_USR_SENDQ)
#define K_MSG K_USR_SENDQ

#define QHD_PRIO 100			// Queue head prio - for sentinel use
#define DMY_PRIO (QHD_PRIO-2)	        // dummy task prio (0== highest prio)
#define DMY_STK_SZ	90		  	// staksize for dummy
#define MAIN_PRIO 	50  		// main task prio
#define STAK_HASH 	0x5c		// just a hashcode
#define MAX_SEM_VAL 50                  // NB is also max for nr elem in msgQ !
/***** KeRNeL data types *****/
// NBNB without volatile you may see problems ...
struct k_t {
	struct k_t *next,   // double chain lists ptr
			*pred;
	volatile
	char sp_lo, sp_hi,  // HW: for saving task stak stk ptr
			prio;             // priority
	volatile int cnt1,           // counters for timers etc
			cnt2, cnt3, maxv;
};

struct k_msg_t {
	struct k_t *sem;  //
	char *pBuf;    // ptr to user supplied ringbuffer
	volatile int nr_el, el_size;
	volatile int r, w, cnt;
};

/***** KeRNeL variables *****/

extern struct k_t task_pool[K_TASK];
extern struct k_t sem_pool[K_SEM];

#ifdef MSGQ
extern struct k_msg_t send_pool[K_MSG];
#endif

extern struct k_t AQ,			// activeQ
		main_el, *pAQ, *pDmy, 			        // ptr to dummy task descriptor
		*pRun; 				// ptr to running task

/* counters for created KeRNeL items */
extern char nr_task, nr_sem, nr_send;

extern volatile char k_running;  // no running

// counters for allocated tasks/semaphores/timer semaphores

extern volatile char k_err_cnt;	// every time an error occurs cnt is incr by one

/***** HW DEPENDENT PART FOR TASKS *****/
#define K_CHG_STAK()  \
if (pRun != AQ.next) {  \
    pRun->sp_lo = SPL;  \
    pRun->sp_hi = SPH;  \
    pRun = AQ.next;     \
    SPL = pRun->sp_lo;  \
    SPH = pRun->sp_hi;  \
  }

#define DI() asm volatile ("cli")
#define EI() asm volatile ("sei")
#define RETI() asm volatile ("reti")
 
#define lo8(X) ((unsigned char)((unsigned int)(X)))
#define hi8(X) ((unsigned char)((unsigned int)(X) >> 8))
//#define hi8(X) ((uint8_t)((uint16_t)(X) >> 8))

void ki_task_shift(void) __attribute__ ((naked));
void ki_task_shift(void);
struct k_t * k_crt_task(void (*pTask)(void), char prio, char *pStk, int stkSize);
char k_set_prio(char prio);

struct k_t * k_crt_sem(char init_val, int maxvalue);
char k_set_sem_timer(k_t * sem, int val);
char ki_signal(struct k_t * sem);
char k_signal(struct k_t * sem);
char k_wait(struct k_t * sem, int timeout);
int ki_semval(struct k_t * sem);

struct k_msg_t * k_crt_send_Q(int nr_el, int el_size, void *pBuf);
char ki_send(struct k_msg_t *pB, void *el);
char k_send(struct k_msg_t *pB, void *el);
char k_receive(struct k_msg_t *pB, void *el, int timeout);

int k_start(int tm); // tm in milliseconds
int k_init(int dmy1, int dmy2, int dmy3);
// for chekking stack etc - REMEMBER to set STAK_USAGE in this file before compiling
int k_unused_stak(struct k_t *t);
int freeRam(void);

/** PUSHREGS AND POPREGS
 * is actual staklayout plus task address at top
 * A push/pop takes 2 cycles
 * a call takes 3 cycles
 * ret/iret 3-4 cycles
 * So a PUSHREGS is 33 instructions(@ 2 cycles) = 66  cycles ~= 66 cycles /"16 MHz" ~= 4.1 usec
 * So an empty iSR which preserves all registers takes 2*4.1usec + 8-10 cycles (intr + iret) ~= 9 usec
 * So max isr speed with all regs saved is around 100 kHz but then with no code inside ISR !
 *
 * rel / register on stak
 *
 * SEE BELOW
 *
 * Register addresses
 * 0x3f SREG
 * 0x3e SPH
 * 0x3d SPL
 * 0x3c EIND
 * 0x3b RAMPZ
 * ...
 * 0x1f R31
 * etc 
 * 0x01 R1
 * 0x00 R0
 *
 */

#if defined (__AVR_ATmega2560__) || defined (__AVR_ATmega1280__)

#define PUSHREGS() asm volatile ( \
"push r1  \n\t" \
"push r0  \n\t" \
"in r0, __SREG__ \n\t" \
"cli \n\t" \
"push r0  \n\t" \
"in r0 , 0x3b \n\t" \
"push r0 \n\t" \
"in r0 , 0x3c \n\t" \
"push r0 \n\t" \
"push r2  \n\t" \
"push r3  \n\t" \
"push r4  \n\t" \
"push r5  \n\t" \
"push r6  \n\t" \
"push r7  \n\t" \
"push r8  \n\t" \
"push r9  \n\t" \
"push r10 \n\t" \
"push r11 \n\t" \
"push r12 \n\t" \
"push r13 \n\t" \
"push r14 \n\t" \
"push r15 \n\t" \
"push r16 \n\t" \
"push r17 \n\t" \
"push r18 \n\t" \
"push r19 \n\t" \
"push r20 \n\t" \
"push r21 \n\t" \
"push r22 \n\t" \
"push r23 \n\t" \
"push r24 \n\t" \
"push r25 \n\t" \
"push r26 \n\t" \
"push r27 \n\t" \
"push r28 \n\t" \
"push r29 \n\t" \
"push r30 \n\t" \
"push r31 \n\t" \
)

#define POPREGS() asm volatile ( \
"pop r31 \n\t" \
"pop r30 \n\t" \
"pop r29 \n\t" \
"pop r28 \n\t" \
"pop r27 \n\t" \
"pop r26 \n\t" \
"pop r25 \n\t" \
"pop r24 \n\t" \
"pop r23 \n\t" \
"pop r22 \n\t" \
"pop r21 \n\t" \
"pop r20 \n\t" \
"pop r19 \n\t" \
"pop r18 \n\t" \
"pop r17 \n\t" \
"pop r16 \n\t" \
"pop r15 \n\t" \
"pop r14 \n\t" \
"pop r13 \n\t" \
"pop r12 \n\t" \
"pop r11 \n\t" \
"pop r10 \n\t" \
"pop r9  \n\t" \
"pop r8  \n\t" \
"pop r7  \n\t" \
"pop r6  \n\t" \
"pop r5  \n\t" \
"pop r4  \n\t" \
"pop r3  \n\t" \
"pop r2  \n\t" \
"pop r0  \n\t" \
"out 0x3c , r0 \n\t " \
"pop r0  \n\t" \
"out 0x3b , r0 \n\t " \
"pop r0  \n\t" \
"out __SREG__ , r0 \n\t " \
"pop r0  \n\t" \
"pop r1  \n\t" \
)

#else

#define PUSHREGS() asm volatile ( \
"push r1 \n\t" \
"push r0 \n\t" \
"in r0, __SREG__ \n\t" \
"cli \n\t" \
"push r0  \n\t" \
"push r2  \n\t" \
"push r3  \n\t" \
"push r4  \n\t" \
"push r5  \n\t" \
"push r6  \n\t" \
"push r7  \n\t" \
"push r8  \n\t" \
"push r9  \n\t" \
"push r10 \n\t" \
"push r11 \n\t" \
"push r12 \n\t" \
"push r13 \n\t" \
"push r14 \n\t" \
"push r15 \n\t" \
"push r16 \n\t" \
"push r17 \n\t" \
"push r18 \n\t" \
"push r19 \n\t" \
"push r20 \n\t" \
"push r21 \n\t" \
"push r22 \n\t" \
"push r23 \n\t" \
"push r24 \n\t" \
"push r25 \n\t" \
"push r26 \n\t" \
"push r27 \n\t" \
"push r28 \n\t" \
"push r29 \n\t" \
"push r30 \n\t" \
"push r31 \n\t" \
)

#define POPREGS() asm volatile ( \
"pop r31 \n\t" \
"pop r30 \n\t" \
"pop r29 \n\t" \
"pop r28 \n\t" \
"pop r27 \n\t" \
"pop r26 \n\t" \
"pop r25 \n\t" \
"pop r24 \n\t" \
"pop r23 \n\t" \
"pop r22 \n\t" \
"pop r21 \n\t" \
"pop r20 \n\t" \
"pop r19 \n\t" \
"pop r18 \n\t" \
"pop r17 \n\t" \
"pop r16 \n\t" \
"pop r15 \n\t" \
"pop r14 \n\t" \
"pop r13 \n\t" \
"pop r12 \n\t" \
"pop r11 \n\t" \
"pop r10 \n\t" \
"pop r9  \n\t" \
"pop r8  \n\t" \
"pop r7  \n\t" \
"pop r6  \n\t" \
"pop r5  \n\t" \
"pop r4  \n\t" \
"pop r3  \n\t" \
"pop r2  \n\t" \
"pop r0  \n\t" \
"out __SREG__ , r0 \n\t " \
"pop r0  \n\t" \
"pop r1  \n\t" \
)

#endif  // 1280/2560
#endif   // #ifndef KRNL

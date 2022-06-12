//220216
#include <krnl.h>

#define STK 110
struct k_t *p1, *p2, *sem1, *sem2;

char st1[STK], st2[STK];

// HERE WE WILL LIGNT ON LED IF OVERFLOW ON sem1 == tperiodic is behind

int x = 0;
int clipp;
void tperiodic()
{
	// simple periodic sampler/controller/...
	k_set_sem_timer(sem1, 200);
	
	while (1) {
		k_wait(sem1, 0);
		
		k_eat_msec(140);  // we eat cpu time
		
		Serial.print(x++);
		Serial.print(" nr of clip on sem1 ");
		
		DI();  // silencio disable interrupt
		clipp = sem1->clip;
		EI();
		
		Serial.println(clipp);
		
		// reset bit 5 LED13 if there has been an overflow on the semaphore
		DI();
		PORTB &= B11011111;  // reset bit 5 == led13 on uno (on mega its bit7) 0x80 or B10000000
		EI();
	}
}


// sem->clip

void tnoise()
{
	while (1) {
		k_eat_msec( 600 ); // we eat between 30 and 200 msec of time
		k_sleep(1000);          // and sleep for 500 msec
	}
}

int err;

void setup()
{
	
	for (int i = 8; i < 14; i++) {
		pinMode(i, OUTPUT);
		digitalWrite(i, LOW);
	}
	
	Serial.begin(115200);
	
	k_init(2, 1, 0); // init with space for three tasks
	
	// priority low number higher priority than higher number
	//Task 1
	p1 = k_crt_task(tperiodic, 10, st1, STK);
	
	//Task 2
	p2 = k_crt_task(tnoise, 11 , st2, STK);
	
	sem1 = k_crt_sem(0, 1);
	
	err = k_start(); // 1 milli sec tick speed
	
	Serial.print("start error ");
	Serial.print(err); // if error is in -1 to -20 its because you need to adjust in k_init
	
}


// LED13 will go ON if overflow på semaphore sem1
// YOU can see nr of clip/saturatino situation in the printotu (terminal)

// FOr tnoise:
// prio = 11  no overflow bq tperodic has highest prio
// prio =10 overflow willl occur bq tnoise is eating 600 msec and therefore share CPU
// 50/50 with tperiodic so eating 140 msec in every 200 msec period can take up to 280 msec(guessing) and therfore its are being
// tnoise prio < 10 makes even worse

// Hack in tperiodic: switch off led 13 by PORTB = .... so we can observe reoccurence of clip




void loop() {}


extern "C" {
	
	void  k_sem_clip(unsigned char nr, int nrClip)
	{
		PORTB = 0x20; //led13
		/* mega PORTB=0x80;  // bit 7 */
	}
}

// 220216
#include <krnl.h>


struct k_t *p1, *p2, *s1, *semmutex;

// shared data
struct shDataTp {
	int v;
	int counter;
};

struct shDataTp  sharedData = {0, 0};

volatile int noDataDelivered = 0;


void saveDataInCritRegion(int v)  // save data from critical region protected by a semaphore
{
	k_wait(semmutex, 0);
	{
		sharedData.v = v;
		sharedData.counter ++;
		
	}
	k_signal(semmutex);
}

int saveDataInCritRegionNoWait(int v)  // picjk up data from critical region protected by a semaphore
{
	int x;
	x = k_wait(semmutex, -1);
	
	if (0 <= x) {  // yes   
		sharedData.v = v;
		sharedData.counter ++;
		k_signal(semmutex);
	}
	
	return x;  // 1: ok we had to wait, 1: ok no wait, -1: timeout, -2: no wait and we wil not wait (k_wait(<sem>,-1))
}



struct shDataTp getDataInCritRegion(void)
{
	struct shDataTp tmp;
	k_wait(semmutex, 0);
	
	k_eat_msec(70);  // to block region some time so teh other cant get it at once
	sharedData.counter--;
	tmp = sharedData;
	
	k_signal(semmutex);
	return tmp;
}

// the sampler task


void t1()
{
	int v;
	
	k_set_sem_timer(s1, 100);
	
	while (1) {
		
		k_wait(s1, 0); //wait until  a kick comes
		
		v = analogRead(A0);  // just to get some noise
		if (0 > saveDataInCritRegionNoWait(v)) {  // or saveDataInCritRegion(v);  if you can afford to wait - may become late
			DI();
			noDataDelivered++;
			EI();
		}
	}
}

void t2()
{
	int ll;
	static struct shDataTp v;
	while (1) {
		
		
		k_sleep(300);
		v = getDataInCritRegion();
		Serial.print(v.v);
		Serial.print(" ");
		Serial.print(v.counter);
		DI();
		ll = noDataDelivered;
		EI();
		Serial.print(" nodata dlv ");
		Serial.println(ll);
	}  // do we keep up in pace ?
}

#define STK 150
char a1[STK], a2[STK];

void setup()
{
	int res;
	Serial.begin(115200);
	while (! Serial) ;
	pinMode(13, OUTPUT);
	
	k_init(2, 2, 0); // init with space for one task
	//         |--- no of mg Queues (0)
	//       |----- no of semaphores (0)
	//     |------- no of tasks (2)
	
	// priority low number higher priority than higher number
	p1 = k_crt_task(t1, 10, a1, STK); // t1 as task, priority 10, 100 B stak
	p2 = k_crt_task(t2, 10, a2, STK); // t1 as task, priority 10, 100 B stak
	
	s1 = k_crt_sem(0, 10);
	
	semmutex = k_crt_sem(1, 10); // must be 1 otherwise no one can come inside
	
	Serial.println("bef start");
	res = k_start(); // 1 milli sec tick speed
	// you will never return from k_start
	Serial.print("ups an error occured: "); Serial.println(res);
	while (1) ;
}

void loop() {}

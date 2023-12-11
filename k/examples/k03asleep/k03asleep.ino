 //220216
 #include <krnl.h>
 // one task loops and blink
 
 
 // hint using k_sleep instead og k_eat_ticks
 // k_eat_ticks eat your cpu time 
 
 // k_eat_ticks is busy waiting meaning it is using cpu time
 // See in krnl.c approx line 597
 // In contradition k_sleep deactivate the calling task for the time given as parameter
 // and is reactivated (reinserted in the activeQ) after timeout
 
 // ?: how much cputime does t2 used in one loop
 // ?: how much time does t1 use in one loop
 // ?: what will happen if you increase t1 priority to 9 ?
 // ?: why is t2 running in bursts
 // ?: Can you predict the minimum time it takes to k_eat_ticks(2000) if you do not know
 //    what other tasks are doing  (a:no)
 // ?: Can you predict minimu time it takes to k_sleep(2000);   (a: yes)
 
 struct k_t *p1, *p2;
 #define STK 110
 
 unsigned char s1[STK], s2[STK];
 
 void t1()
 {
	 int v;
	 
	 while (1) {
		 k_eat_msec(2000);   // burn of cpu time for 2000 ticks
		 k_sleep(2000);      // sleep - no cpu eat for 2000 ticks
	 }
 }
 
 void t2()
 {
	 int i = 0;
	 while (1) {
		 digitalWrite(13, HIGH);
		 k_sleep(50);
		 digitalWrite(13, LOW);
		 //k_sleep(50);
		 //or
		 k_eat_msec(50);
	 }
	 // do we keep up in pace ?
 }
 
 
 void setup()
 {
	 int res;
	 Serial.begin(115200);
	 while (! Serial) ;
	 pinMode(13, OUTPUT);
	 
	 k_init(2, 0, 0); // init with space for one task
	 //         |--- no of mg Queues (0)
	 //       |----- no of semaphores (0)
	 //     |------- no of tasks (2)
	 
	 // priority low number higher priority than higher number
	 p1 = k_crt_task(t1, 10, s1,STK); // t1 as task, priority 9, 100 B stak
	 p2 = k_crt_task(t2, 10, s2,STK); // t2 as task, priority 10, 100 B stak
	 
	 Serial.println("bef start");
	 res = k_start(); // 1 milli sec tick speed
	 // you will never return from k_start
	 Serial.print("ups an error occured: "); Serial.println(res);
	 while (1) ;
 }
 
 void loop() {}
 

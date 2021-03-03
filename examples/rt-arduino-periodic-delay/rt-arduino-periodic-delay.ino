
#include <krnl.h>
//Jens 2021



// k_task_periodic_delay  - for sampling etc

// return 0: ok and have been waiting - was arriving in good time
// 1: ok just on time - no wait
// 2: behind - no wait
// -1 Time parameter above 60000


void wr(void)
{
  int i = 0,ee;
  unsigned long ii, x;

  // similar o freertos' vTaskdDelayUntil
  i = k_millis(); // get curren tick from kernel  as starting point
  
  while (1) {
    ee = k_task_periodic_delay(&ii, 100);//sleep( random(100, 1000)); // NB k_sleep and not k_eat_ticks bq then you will never release CPU
    DI();
    x = k_millis_counter;
    EI();
    Serial.print(ee); Serial.print(" ");
    Serial.println(x);
    k_sleep( random(90, 105));
  }
}

void setup()
{
  Serial.begin(115200);  // for output from task 1
  delay(2000);
  Serial.print("RW START");
  k_init(1, 0, 0);
  k_crt_task(wr, 12, 200);

  k_start(1); // start kernel with tick speed 1 milli seconds
  Serial.println("if coming hre then init went wrong");
}

void loop() {
  /* loop will never be called */
}

#include <krnl.h>


char s[150];
struct k_t * e;
void t1()
{
  int i=0,j;
  while (1) {
    i++;
    Serial.print(i);
    j = k_unused_stak(e);
    Serial.print(" ");
    Serial.println(j);
    //delay(100);
  }
}

void setup()
{
  Serial.begin(9600);
    int i=0;
   k_init(5,5,5);  
  e = k_crt_task(t1,10,s,150);
  k_start(10);
}
  void loop()
  {
  }


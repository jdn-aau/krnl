#include <krnl.h>


void setup()
{
  Serial.begin(9600);
  k_init(0,0,0);
  Serial.println(freeRam());
}

void loop() {}



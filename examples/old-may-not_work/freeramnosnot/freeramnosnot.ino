#include <freeram.h>

void setup()
{
  Serial.begin(9600);
  Serial.println(freeram());
}

void loop() {}



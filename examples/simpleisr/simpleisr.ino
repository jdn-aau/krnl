// Definition of interrupt names
#include <avr/io.h>
// ISR interrupt service routine
#include <avr/interrupt.h>

volatile int i=0;
volatile int j,k,l,m;


#if defined (__AVR_ATmega2560__) || defined (__AVR_ATmega1280__)
 ISR(INT4_vect,ISR_NAKED) {
#else
 ISR(INT0_vect, ISR_NAKED) {
#endif

   i++;
 }

void setup()
{
  Serial.begin(9600);
   pinMode(2,INPUT);  // set som input
   cli(); // disable interrupt
  digitalWrite(2,HIGH);  // enable pullup resistor
  
#if defined (__AVR_ATmega2560__) || defined (__AVR_ATmega1280__)
  // 1280/2560 mega pin2 intr:4, pin5 intr:5
  EIMSK |= (1 << INT4);  // enable external intr
  EICRB |= (1 << ISC41); // trigger INT4 on falling edge
#else
  EIMSK |= (1 << INT0);  // enable external intr
  EICRA |= (1 << ISC01); // trigger INT0 on falling edge
#endif
   sei(); // enable interrupt
}

void loop()
{
  Serial.print(i); 
  delay(1000);
}

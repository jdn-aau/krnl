

#define sbi(r,b)  r |= _BV(b)
#define cbi(r,b) r &= ~_BV(b)
#define cbi(r,b) r &= ~_BV(b)


void setup() {


  sbi(DDRB, 5);
  cbi(DDRB, 0);

  sbi(PORTB, 0);

  sbi(PORTB, 5);
  delay(1000);
  cbi(PORTB, 5);
  delay(1000);
  sbi(PORTB, 5);
  delay(1000);
  cbi(PORTB, 5);
  delay(1000);

  Serial.begin(9600);
}
int i = 0;
void loop() {
   
  // put your main code here, to run repeatedly:
  if (! rbi(PINB, 0) )
    Serial.println(i++);
}


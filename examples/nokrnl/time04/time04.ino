const unsigned long sampleInterval1 = 10, sampleInterval2 = 7;
unsigned long  t, tNow1, tNow2;

void setup() {
  pinMode(13, OUTPUT);
  tNow1 = sampleInterval1;
  tNow2 = sampleInterval2;
}

void runMyCode1() {
  delay(3);
}
void runMyCode2() {
  delay(4);
}

extern unsigned long timer0_overflow_count;

void loop() {

  cli();  // disable interrupt
  t = timer0_overflow_count;
  sei();  // enable ...

  if (tNow1 <= t ) {
    runMyCode1();
    tNow1 += sampleInterval1;
  }

  cli();  // disable interrupt
  t = timer0_overflow_count;
  sei();  // enable ...

  if (tNow2 <= t ) {
    runMyCode1();
    tNow2 += sampleInterval2;
  }
}
}

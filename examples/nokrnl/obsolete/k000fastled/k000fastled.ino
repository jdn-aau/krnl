void setup() {
  pinMode(13, OUTPUT);
  Serial.begin(115200);
  delay(1000);
  Serial.println("starting");
}
void xx()
{
  digitalWrite(13, HIGH);
  digitalWrite(13, LOW);
  digitalWrite(13, HIGH);
  digitalWrite(13, LOW);
  digitalWrite(13, HIGH);
  digitalWrite(13, LOW);
  digitalWrite(13, HIGH);
  digitalWrite(13, LOW);
  digitalWrite(13, HIGH);
  digitalWrite(13, LOW);
  digitalWrite(13, HIGH);
  digitalWrite(13, LOW);

}
void loop() {
  Serial.println(millis());
  for (int i = 0; i < 30000; i ++ ) {
    xx();
  }
  Serial.println(millis());




  Serial.println("port approach");
  Serial.println(millis());
  for (int i = 0; i < 30000; i ++ ) {
    asm ("push r24\n"
         "ldi    r24, 0x20 \n"
         "out 0x05, r24\n"
         "ldi    r24, 0x00 \n"
         "out 0x05, r24\n"
         "ldi    r24, 0x20 \n"
         "out 0x05, r24\n"
         "ldi    r24, 0x00 \n"
         "out 0x05, r24\n"
         "ldi    r24, 0x20 \n"
         "out 0x05, r24\n"
         "ldi    r24, 0x00 \n"
         "out 0x05, r24\n"
         "ldi    r24, 0x20 \n"
         "out 0x05, r24\n"
         "ldi    r24, 0x00 \n"
         "out 0x05, r24\n"
         "ldi    r24, 0x20 \n"
         "out 0x05, r24\n"
         "ldi    r24, 0x00 \n"
         "out 0x05, r24\n"
         "ldi    r24, 0x20 \n"
         "out 0x05, r24\n"
         "ldi    r24, 0x00 \n"
         "out 0x05, r24\n"
         "pop r24"
        );
  }
  Serial.println(millis());
  Serial.println("ende");
  while (1);
}

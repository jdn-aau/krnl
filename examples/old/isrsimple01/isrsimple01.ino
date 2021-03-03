


void instructions()
{
  Serial.println(F("Interrupt test program by JDN"));
  Serial.println(F("-----------------------------"));
  Serial.println(F("Program will put a square wave on pin 10 with a frequency of 500 Hz"));
  Serial.println(F("You shall connect pin 10 to pin 2"));
  Serial.println(F("And an interrupt service routine(ISR) will count nr of interrupts on pin 2"));
  Serial.println(F("it will count falling levels"));
  Serial.println(F("DONT use delay  inside your ISR ! "));
  Serial.println(F("Count should go up approx 500 each second"));
  Serial.println(F("---"));
  Serial.println(F("An experiment"));
  Serial.println(F("Try to pull up wire from pin 10(500 Hz output)"));
  Serial.println(F("now - try to touch metal shield at usb plug (which is ground)"));
  Serial.println(F("every time you touch the shield (GROUND) you will see a count"));
  Serial.println(F("and !!! observe you will get many counts even if you touch very rapid"));
  Serial.println(F("it's called contact bounce  "));
  Serial.println(F("---"));
  Serial.println(F("try to keep it up in free air "));
  Serial.println(F("you should observe some irregular increase in count - weird")); 
  Serial.println(F("it's because you have an open input and the wire acts like an antenna"));  
  Serial.println(F("in pinMode line change INPUT to INPUT_PULLUP and recompile"));
  Serial.println(F("now you will (hopfully) see no increase in count when wire is not connected to pin 10"));
  }

volatile int cnt = 0;

void myISR()
{
  cnt++;
}

void setup() {
  Serial.begin(9600);
  delay(1000);
  instructions();
  pinMode(2, INPUT_PULLUP); // ISR pin
  attachInterrupt(digitalPinToInterrupt(2), myISR, FALLING);
  // Mode can be FALLING, RISING, CHANGE, LOW(when low), HIGH(high)

  delay(1000);

  analogWrite(10, 127); // just start PWM on pin 10

}

void loop() {
  // put your main code here, to run repeatedly:

  delay(1000);
  Serial.println(cnt);
}

/* critical region - by semaphores - aka mutex
    Signal from ISR

    Work on uno and mega :-)
    JDN
 *  */

#include <krnl.h>

#define STK 150

#define TASKPRIO 10


struct k_t * pTask1;

struct k_t *mutSem, *syncSem;

struct k_t *timedSem1, *timedSem2;

volatile int t2Missed = 0, t2Hit = 0;


volatile int ISRoverflow = 0;

#if defined (__AVR_ATmega2560__) || defined (__AVR_ATmega1280__) || defined(__AVR_ATmega2561__)
ISR(INT4_vect, ISR_NAKED) { // digital pin 2 onmega
#endif

  //UNO  set interrupt on pin 2
#if defined(__AVR_ATmega168__) || defined(__AVR_ATmega328P__) || defined(__AVR_ATmega328__) || defined(__AVR_ATmega32U4__)
  ISR(INT0_vect, ISR_NAKED) { // digital pin 2 on uno
#endif
    static volatile unsigned int lastISR = 0;
    PUSHREGS();
    if (!k_running)
      goto exitt ;

    // debounce in SW

    if (ki_millis() - lastISR > 1000) {  // deounce time 1000 ticks == 1000 msec
      lastISR = ki_millis();

      if (-1 == ki_signal(syncSem))
        ISRoverflow++;
      else
        K_CHG_STAK();
    }

exitt:
    POPREGS();
    RETI();
  }


  void task1()
  {
    int i = 0;
    int miss = 0;
    while (1) {

      if (-1 == k_wait(syncSem, 1000)) {
        miss++;
        Serial.print("timeout "); Serial.print(miss); Serial.print(" "); Serial.println(ISRoverflow);
      }
      else {
        i++;
        Serial.print("get kick no "); Serial.print(i); Serial.print(" "); Serial.println(ISRoverflow);
      }
    }
  }

 




  void setup() {
    // for debugging - only on uno eq- If Mega etc use PORTA(which is pn 22,23,...)
    // see http://kom.aau.dk/~jdn/edu/doc/arduino/mega.png

    Serial.begin(115200);
    delay(2000);

    Serial.println("just bef init part");

    k_init(1, 1, 0); // 2 task, 1 semaphores, 0 messaegQueues */

    pTask1 = k_crt_task(task1, 15,  STK);
    

    syncSem = k_crt_sem(0, 10); // 1: start value, 10: max value (clipping)

    Serial.println("just bef k_start");


    pinMode(2, INPUT_PULLUP);

    // ABOUT ISR
    // UNO/LENARDO pp 54   https://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-7810-Automotive-Microcontrollers-ATmega328P_Datasheet.pdf  p 54
    //MEGA 2560 pp 110 https://ww1.microchip.com/downloads/en/devicedoc/atmel-2549-8-bit-avr-microcontroller-atmega640-1280-1281-2560-2561_datasheet.pdf

    __asm__ volatile ("cli");  // disable interrupt

#if defined (__AVR_ATmega2560__) || defined (__AVR_ATmega1280__) || defined(__AVR_ATmega2561__)
    EICRB |= (1 << ISC41); //  B00000010; // for mega on pin 2
    EIMSK |= (1 << INT4); //B00010000; // enable INT 4  for mega

    Serial.println(EICRB, HEX);
    Serial.println(EIMSK, HEX);
#endif

    //UNO  set interrupt on pin 2
#if defined(__AVR_ATmega168__) || defined(__AVR_ATmega328P__) || defined(__AVR_ATmega328__) || defined(__AVR_ATmega32U4__)
    EICRA |= (1 << ISC01); // UNON UNO .,. trigger INT0 on falling edge
    EIMSK |= (1 << INT0);  // enable external intr
#endif

    __asm__ volatile ("sei");  // enable interrupt

    k_start(1); /* start krnl timer speed 1 milliseconds*/

    Serial.println("If you see this then krnl didnt start :-( ");
  }

  void loop() {}

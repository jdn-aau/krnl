const byte ledPin = 13;
const byte interruptPin = 2;

volatile byte state = LOW; // to avoid register optimization

void setup() {
  pinMode(ledPin, OUTPUT);
  pinMode(2, INPUT_PULLUP);

  EICRA = 0x02;  // set falling edge
  EIMSK |= 0x01; // enable ISR 0
  // FOR ARDUINO MEGA YOU CAN USE INT0 .. INT5
  // INT6 and INT7 is not connected to the pins and is only om the cpu
  // EIMSK so INT7 is bit 7 in EIMSK etc

  // intr falling,rising etc,
  // EICRA is or INT0 -> INT3
  // EICRB is for INT4-INT7 
  // https://ww1.microchip.com/downloads/en/devicedoc/atmel-2549-8-bit-avr-microcontroller-atmega640-1280-1281-2560-2561_datasheet.pdf
  // EICRA, EICRB on p 110,111
  // EIMSK on p 111
}

ISR(INT0_vect)  
{
  state = !state;
}


// ISR(INT1_vect) etc
  
void loop() {
  digitalWrite(ledPin, state);
}

/*
  ISR(INT0_vect)
  {
  174: 1f 92           push    r1
  176:   0f 92           push    r0
  178:   0f b6           in  r0, 0x3f    ; 63
  17a:   0f 92           push    r0
  17c:   11 24           eor r1, r1
  17e:   8f 93           push    r24
  180:   9f 93           push    r25

  state = !state;
  182:   90 91 00 01     lds r25, 0x0100 ; 0x800100 <_edata>
  186:   81 e0           ldi r24, 0x01   ; 1
  188:   91 11           cpse    r25, r1
  18a:   80 e0           ldi r24, 0x00   ; 0
  18c:   80 93 00 01     sts 0x0100, r24 ; 0x800100 <_edata>

  190:   9f 91           pop r25
  192:   8f 91           pop r24
  194:   0f 90           pop r0
  196:   0f be           out 0x3f, r0    ; 63
  198:   0f 90           pop r0
  19a:   1f 90           pop r1
  19c:   18 95           reti
  }


  //https://www.arxterra.com/11-atmega328p-external-interrupts/

  // http://www.gammon.com.au/interrupts

  /*1  Reset
  2  External Interrupt Request 0  (pin D2)          (INT0_vect)
  3  External Interrupt Request 1  (pin D3)          (INT1_vect)
  4  Pin Change Interrupt Request 0 (pins D8 to D13) (PCINT0_vect)
  5  Pin Change Interrupt Request 1 (pins A0 to A5)  (PCINT1_vect)
  6  Pin Change Interrupt Request 2 (pins D0 to D7)  (PCINT2_vect)
  7  Watchdog Time-out Interrupt                     (WDT_vect)
  8  Timer/Counter2 Compare Match A                  (TIMER2_COMPA_vect)
  9  Timer/Counter2 Compare Match B                  (TIMER2_COMPB_vect)
  10  Timer/Counter2 Overflow                         (TIMER2_OVF_vect)
  11  Timer/Counter1 Capture Event                    (TIMER1_CAPT_vect)
  12  Timer/Counter1 Compare Match A                  (TIMER1_COMPA_vect)
  13  Timer/Counter1 Compare Match B                  (TIMER1_COMPB_vect)
  14  Timer/Counter1 Overflow                         (TIMER1_OVF_vect)
  15  Timer/Counter0 Compare Match A                  (TIMER0_COMPA_vect)
  16  Timer/Counter0 Compare Match B                  (TIMER0_COMPB_vect)
  17  Timer/Counter0 Overflow                         (TIMER0_OVF_vect)
  18  SPI Serial Transfer Complete                    (SPI_STC_vect)
  19  USART Rx Complete                               (USART_RX_vect)
  20  USART, Data Register Empty                      (USART_UDRE_vect)
  21  USART, Tx Complete                              (USART_TX_vect)
  22  ADC Conversion Complete                         (ADC_vect)
  23  EEPROM Ready                                    (EE_READY_vect)
  24  Analog Comparator                               (ANALOG_COMP_vect)
  25  2-wire Serial Interface  (I2C)                  (TWI_vect)
  26  Store Program Memory Ready                      (SPM_READY_vect)
*/

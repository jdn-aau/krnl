// time08/JDN

// no check - no mercy

void
setup() {
    Serial.begin(115200); // so high to speed up transmission
}

const unsigned long tPer1 = 300,
    tPer2 = 500; // sampling period
volatile unsigned long nextTimeToRun1 = 0, nextTimeToRun2 = 0;

void
controlCode1() {
    Serial.print(millis());
    Serial.println(" 1"); // debug
    // your code instead of delay which simulate code execution
    delay(100);
}

// we do split our code up int 5 states
// so actual Td = 5* Td-1-state = 5*tPer2;

void
controlCode2() {
    static char mystate = 0;

    Serial.print(millis());
    Serial.print(" 2 - state "); // debug
    Serial.println((int) mystate);
    // your code instead of delay which simulate code execution
    switch (mystate) {
    case 0:
        delay(90);
        mystate++;
        break;
    case 1:
        delay(90);
        mystate++;
        break;
    case 2:
        delay(90);
        mystate++;
        break;
    case 3:
        delay(90);
        mystate++;
        break;
    case 4...7: // 4 states
        mystate++;
        break;
    case 8: // plus 1  in all 4+4+1= 9 states
        mystate = 0;
        break;
    default:
        mystate = 0;
    }
}

unsigned long tt;

int
kicker(void( * f)(), unsigned long * t, unsigned long per) {
    // is it time to run ?
    if ( * t <= tt) {
        ( * f)();
        * t += per; // update next time to run
        return 1; // did run code
    }
    return 0; //did not run code - not time for it
}

void
loop() {
    tt = millis(); // or timer0....
    // in ascending order regarding period
    kicker(controlCode1, & nextTimeToRun1, tPer1);
    kicker(controlCode2, & nextTimeToRun2, tPer2);
}

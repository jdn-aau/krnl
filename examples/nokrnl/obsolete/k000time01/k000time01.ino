// k000time01 /JDN

void setup() {
  Serial.begin(115200); // so high to speed up transmission
}

const unsigned long Tper = 100;  // sampling period
int tMin = 10, tMax = 110;

unsigned long nextTimeToRun = 0, timeToDelay = 0;

void realTimeCode()
{
  Serial.println(millis());

  // to mimic code takes time
  // NB : some times codes takes longer time than period
  // just to show

  delay( random (tMin, tMax ) ) ;
}


unsigned long tt;
void loop() {

  realTimeCode();

  nextTimeToRun += Tper;  // calc next time to run

  // new version 210203 14:22
  // we will only wait if we are ahead of time
  // otherwise just pass wait part if we are behind ...
  //we just read millis once to avoid it will advance during this critical code
  tt = millis();
  if (tt < nextTimeToRun) {
    timeToDelay = nextTimeToRun - tt;   // wait until ...

    // just for testning - print is not the rigt way to do it
    if (timeToDelay < 15)
      Serial.println(" approaching limit");

    delay (timeToDelay);
  }
  else {
    Serial.println(" no wait: behind");  // you should not print here - just for showing stuff
  }
}

/*
  8800 85
  8900 79
  8999 31
  9100 62
  9200 87
  9299 24
  9400 87
  9500 58
  9601 44
  9700 13 approaching limit
  9799 72
  9901 53
  10000 65
  10099 28
  10201 73
  10300 58
  10400 22
  10500 72
  10600 12 approaching limit
  10699 32
  10801 13 approaching limit
  10900 35
  10999 25
  11100 81
  11199 18
  11299 25
  11401 75
  11500 54
  11599 31
  11700 28
  11800 11 approaching limit
  11900 42
  12001 77
  12101 70
  12200 52
  12301 16
  12400 81
  12499 14 approaching limit
  12600 41
*/

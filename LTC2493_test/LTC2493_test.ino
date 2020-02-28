#include <Metro.h>
#include <Streaming.h>
#include "LTC2499.h" // https://github.com/IowaScaledEngineering/ard-ltc2499

Ltc2499 ltc2493; 

#define _ <<" "<<

void setup() 
{ Serial.begin(115200);
  Wire.begin();
  byte status = ltc2493.begin(ADDR_0ZZ); 
  if(status)
  { Serial <<"\nError LTC2493" _ status;
    while(1);
  }

  Serial << "Setup\n"; 
  ltc2493.changeConfiguration(CONFIG2_60_50HZ_REJ);  
  ltc2493.changeConfiguration(CONFIG2_SPEED_2X); // 75ms
  ltc2493.changeChannel(CHAN_SINGLE_0P); // ADC starts converting now .............
  //ltc2493.changeChannel(CHAN_DIFF_0P_1N);
  
  delay(80); // run tasks here during the 75ms ADC conversion time
  
  unsigned long t1 = millis();
  long adc = ltc2493.read(); // read takes no time since the result is available already, a new conversion starts now .................
  Serial << adc _ millis()-t1 _ "ms" _ endl; 
  
  t1 = millis();
  adc = ltc2493.read(); // read takes 75ms, a new conversion starts now .................
  Serial << adc _ millis()-t1 _ "ms" _ endl; 
  Serial << "Loop\n"; 
}

void loop() 
{ delay(1000); // run tasks here during the 75ms ADC conversion time
  unsigned long t1 = millis();
  long adc = ltc2493.read(); // a new conversion starts now .................
  Serial << adc _ millis()-t1 _ "ms" _ endl; 
}

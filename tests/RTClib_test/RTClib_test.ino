/* 
Sketch to test the RTClib library in comparison with AqRTC
Last test on Uno R3 with NO RTC installed:
Sketch size:  7242 Byte
Free memory:  1502 Byte
Loop ticks: 33000
Last test on Uno R3 WITH RTC installed:
Sketch size:  7242 Byte
Free memory:  1502 Byte
Loop ticks: 9300
*/

#include <Wire.h>
#include <RTClib.h>

// used to track loop speed
unsigned long nextOutput;
unsigned long loopTicks;

// no RTC predeclared in library
RTC_DS1307 RTC;

// helper to calculate free memory
extern int __bss_end;
extern void *__brkval;

int memoryFree()
{
  int freeValue;  
  if((int)__brkval == 0)
     freeValue = ((int)&freeValue) - ((int)&__bss_end);
  else
    freeValue = ((int)&freeValue) - ((int)__brkval);

  return freeValue;
}

// Setup, runs once
void setup(){
  Serial.begin(38400);  // Set up serial communication
  Wire.begin();         // Set up i2c communication (needed for by the RTC)
  RTC.begin();          // Set up RTC, will NOT automatically adjust the RTC to compile time if it's not running
  if(!RTC.isrunning()){
    Serial.println("RTC is not running!");  // but we will fake time based on compile time
    RTC.adjust(DateTime(__DATE__, __TIME__));
  }
  
  // prepare for loop
  nextOutput = millis() + 10000;
  loopTicks = 0;
}

// Helper to print clock digits
void serialPrintDigits(byte digits){
  if(digits < 10)
    Serial.print(F("0"));
  Serial.print(digits, DEC);
}

// Loop, runs continuously
void loop(){
  RTC.now(); // This is NOT needed in a real-world project, it's used to test the effect of i2c caching
  if(millis() > nextOutput){
    // Output date and time
    Serial.print(RTC.now().year());
    Serial.print(".");
    Serial.print(RTC.now().month());
    Serial.print(".");
    Serial.print(RTC.now().day());
    Serial.print(" ");
    serialPrintDigits(RTC.now().hour());
    Serial.print(":");
    serialPrintDigits(RTC.now().minute());
    Serial.print(":");
    serialPrintDigits(RTC.now().second());
    Serial.println();
  
    // Output free memory
    Serial.print("Free memory: ");
    Serial.println(memoryFree());
  
    // Output loop speed
    Serial.print("Loop ticks: ");
    Serial.println(loopTicks);
    
    loopTicks = 0;
    // meet you again in 10 seconds
    nextOutput += 10000;
  }
  loopTicks++;
}

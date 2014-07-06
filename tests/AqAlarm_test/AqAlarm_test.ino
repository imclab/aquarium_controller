// Last Test: 8804Byte Sketch size, 1438RAM

#include <Wire.h>         //needed for AqRTC
#include <AqRTC.h>        //needed for AqAlarms
#include <AqAlarms.h>

Timer onceTimer;
Timer repeatTimer;
Timer repeatTimer2;
Alarm repeatAlarm;

void SerialPrintDigits(byte digits){
  if(digits < 10)
    Serial.print('0');
  Serial.print(digits, DEC);
}

void onceTimerAction(){
  SerialPrintDigits(RTC.now().hour());
  Serial.print(":");
  SerialPrintDigits(RTC.now().minute());
  Serial.println(" onceTimerAction called!");
}

void repeatTimerAction(){
  SerialPrintDigits(RTC.now().hour());
  Serial.print(":");
  SerialPrintDigits(RTC.now().minute());
  Serial.println(" repeatTimerAction called!");
}

void repeatTimerAction2(){
  SerialPrintDigits(RTC.now().hour());
  Serial.print(":");
  SerialPrintDigits(RTC.now().minute());
  Serial.println(" repeatTimerAction2 called!");
}

void repeatAlarmAction(){
  SerialPrintDigits(RTC.now().hour());
  Serial.print(":");
  SerialPrintDigits(RTC.now().minute());
  Serial.println(" repeatAlarmAction called!");
}

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

void setup(){  
  Wire.begin();
  Serial.begin(38400);
  RTC.begin(DateTime(__DATE__, __TIME__));
  onceTimer.set(30000, onceTimerAction, false);
  repeatTimer.set(10000, repeatTimerAction, true);
  repeatTimer2.set(11000, repeatTimerAction2, true);
  repeatAlarm.set(15, 31, 0, repeatAlarmAction, true);
  Serial.println(memoryFree());
}

void loop(){ 
  delay(1);
  onceTimer.check();
  repeatTimer.check();
  repeatTimer2.check();
  repeatAlarm.check();
}

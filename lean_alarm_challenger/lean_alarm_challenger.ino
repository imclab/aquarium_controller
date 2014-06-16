#include <Wire.h>
#include <RTClib.h>
#include <Time.h>
#include <TimeAlarms.h>

RTC_DS1307 RTC;

void SerialPrintDigits(byte digits){
  if(digits < 10)
    Serial.print('0');
  Serial.print(digits, DEC);
}

time_t syncProvider(){
  return RTC.now().unixtime();
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
  setSyncProvider(syncProvider);
  Alarm.timerOnce(30, onceTimerAction); 
  Alarm.timerRepeat(10, repeatTimerAction); 
  Alarm.timerRepeat(11, repeatTimerAction2); 
  Alarm.alarmRepeat(15, 29, 00, repeatAlarmAction);
  Serial.println(memoryFree());
}

void loop(){
  Alarm.delay(1);
}



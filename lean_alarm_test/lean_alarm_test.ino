#include <Wire.h>         //needed for RTClib
#include <RTClib.h>       //needed for LeanAlarms
#include <LeanAlarms.h>

TimerOnce onceTimer;
TimerRepeat repeatTimer;
AlarmRepeat repeatAlarm;

void onceTimerAction(){
  Serial.println("onceTimerAction called!");
}

void repeatTimerAction(){
  Serial.println("repeatTimerAction called!");
}

void repeatAlarmAction(){
  Serial.println("repeatAlarmAction called!");
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
  onceTimer.set(30000, onceTimerAction);
  repeatTimer.set(10000, repeatTimerAction);
  repeatAlarm.set(21, 15, 00, repeatAlarmAction);
  Serial.println(memoryFree());
}

void loop(){ 
  delay(1);
  onceTimer.check();
  repeatTimer.check();
  repeatAlarm.check();
}

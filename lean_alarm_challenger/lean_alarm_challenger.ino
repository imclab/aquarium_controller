#include <Wire.h>
#include <RTClib.h>
#include <Time.h>
#include <TimeAlarms.h>

RTC_DS1307 RTC;

time_t syncProvider(){
  return RTC.now().unixtime();
}

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
  setSyncProvider(syncProvider);
  Alarm.timerOnce(30, onceTimerAction); 
  Alarm.timerRepeat(10, repeatTimerAction); 
  Alarm.alarmRepeat(21, 15, 00, repeatAlarmAction);
  Serial.println(memoryFree());
}

void loop(){
  Alarm.delay(1);
}



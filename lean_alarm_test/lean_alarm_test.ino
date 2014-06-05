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

void setup(){  
  Serial.begin(38400);
  onceTimer.set(30000, onceTimerAction);
  repeatTimer.set(10000, repeatTimerAction);
  repeatAlarm.set(21, 15, 00, repeatAlarmAction);
}

void loop(){ 
  delay(1);
  onceTimer.check();
  repeatTimer.check();
  repeatAlarm.check();
}

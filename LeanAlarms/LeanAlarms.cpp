/*
  LeanAlarms.h - A memory-efficient, millisecond-precise library for timed events on the Arduino.
  Created by Chris Lüscher, 2014.
  Released under GPL V2.
*/

#include "Arduino.h"
#include "LeanAlarms.h"
#include "RTClib.h"

RTC_DS1307 myRTC;

TimerOnce::TimerOnce(){
	_active = false;
}

void TimerOnce::set(unsigned long duration, AlarmCallback_t callback){
	_start = millis();
	_duration = duration;
	_callback = callback;
	_active = true;
}

void TimerOnce::check(){
    if(_active && (millis() >= (_start + _duration))){
      _active = false;
      (*_callback)();
    }
}


TimerRepeat::TimerRepeat(){
	_active = false;
}

void TimerRepeat::set(unsigned long duration, AlarmCallback_t callback){
	_start = millis();
	_duration = duration;
	_callback = callback;
	_active = true;
}

void TimerRepeat::check(){
    if(_active && (millis() >= (_start + _duration))){
      _start = millis();
      (*_callback)();
    }
}


AlarmRepeat::AlarmRepeat(){
	_next = 0;
}

void AlarmRepeat::set(byte hour, byte minute, byte second, AlarmCallback_t callback){
	_hour = hour;
	_minute = minute;
	_second = second;
	_callback = callback;
    uint32_t time = myRTC.now().unixtime();
	// set the event for today
	_next = previousMidnight(time) + _hour * SECS_PER_HOUR + _minute * SECS_PER_MIN + _second;
	// did the event already pass for today? if so, set it for tomorrow
	if(time >= _next){
		_next = nextMidnight(time) + _hour * SECS_PER_HOUR + _minute * SECS_PER_MIN + _second;
	}
}

void AlarmRepeat::check(){
    if(_next > 0){
	  uint32_t time = myRTC.now().unixtime();
	  if((time >= _next)){
    	_next = nextMidnight(time) + _hour * SECS_PER_HOUR + _minute * SECS_PER_MIN + _second;
        (*_callback)();
      }
    }
}
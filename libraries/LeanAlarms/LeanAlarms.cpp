/*
  LeanAlarms.h - A memory-efficient, millisecond-precise library for timed events on the Arduino.
  Created by Chris Lüscher, 2014.
  Released under GPL V2.
*/

#include "Arduino.h"
#include <Wire.h>
#include <RTClib.h>
#include "LeanAlarms.h"

RTC_DS1307 leanRTC;
const unsigned long syncInterval = 30000; //interval to sync with RTC in milliseconds, here: 30s
unsigned long lastSync;
unsigned long lastTime;

Timer::Timer(){
	_active = false;
}

void Timer::set(unsigned long duration, AlarmCallback_t callback, boolean repeat){
	_start = millis();
	_duration = duration;
	_callback = callback;
	_active = true;
	_repeat = repeat;
}

void Timer::check(){
    if(_active && (millis() >= (_start + _duration))){
	  if(_repeat){
		_start = millis();	
	  }else{
        _active = false;
  	  }
      (*_callback)();
    }
}

Alarm::Alarm(){
	_next = 0;
}

void Alarm::set(byte hour, byte minute, byte second, AlarmCallback_t callback, boolean repeat){
	_hour = hour;
	_minute = minute;
	_second = second;
	_callback = callback;
	lastTime = leanRTC.now().unixtime();
	lastSync = millis();
	// set the event for today
	_next = previousMidnight(lastTime) + _hour * SECS_PER_HOUR + _minute * SECS_PER_MIN + _second;
	// did the event already pass for today? if so, set it for tomorrow
	if(lastTime >= _next){
		_next = nextMidnight(lastTime) + _hour * SECS_PER_HOUR + _minute * SECS_PER_MIN + _second;
	}
	_repeat = repeat;
}

void Alarm::check(){
    if(_next > 0){
	  if(millis() >= (lastSync + syncInterval)){
	  	lastTime = leanRTC.now().unixtime();
	  	lastSync = millis();
	  }
	  unsigned long time = lastTime + ((millis() - lastSync) / 1000); 
	  if((time >= _next)){
		if(_repeat){
    	  _next = nextMidnight(time) + _hour * SECS_PER_HOUR + _minute * SECS_PER_MIN + _second;
	    }else{
		  _next = 0;
	    }
        (*_callback)();
	  }
   }
}
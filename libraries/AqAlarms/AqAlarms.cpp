/*
  LeanAlarms.h - A memory-efficient, millisecond-precise library for timed events on the Arduino.
  Created by Chris Lüscher, 2014.
  Released under GPL V2.
*/

#include "Arduino.h"
#include <Wire.h>
#include <AqRTC.h>
#include "AqAlarms.h"

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
	// set the event for today
	_next = previousMidnight(RTC.now().unixtime()) + _hour * SECS_PER_HOUR + _minute * SECS_PER_MIN + _second;
	// did the event already pass for today? if so, set it for tomorrow
	if(RTC.now().unixtime() >= _next){
		_next = nextMidnight(RTC.now().unixtime()) + _hour * SECS_PER_HOUR + _minute * SECS_PER_MIN + _second;
	}
	_repeat = repeat;
}

void Alarm::check(){
    if(_next > 0){
	  if((RTC.now().unixtime() >= _next)){
		if(_repeat){
    	  _next = nextMidnight(RTC.now().unixtime()) + _hour * SECS_PER_HOUR + _minute * SECS_PER_MIN + _second;
	    }else{
		  _next = 0;
	    }
        (*_callback)();
	  }
   }
}
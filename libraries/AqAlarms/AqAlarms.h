/*
  AqAlarms.h - A memory-efficient, millisecond-precise library for timed events on the Arduino.
  Created by Chris Lüscher, 2014.
  Released under GPL V2.
*/

#ifndef AqAlarms_h
#define AqAlarms_h

#include "Arduino.h"

typedef void (*AlarmCallback_t)();

#define SECS_PER_MIN  (60UL)
#define SECS_PER_HOUR (3600UL)
#define SECS_PER_DAY  (SECS_PER_HOUR * 24UL)

#define previousMidnight(_time_) (( _time_ / SECS_PER_DAY) * SECS_PER_DAY)  // time at the start of the given day
#define nextMidnight(_time_) ( previousMidnight(_time_)  + SECS_PER_DAY )   // time at the end of the given day 

// A timer that fires after a certain duration given in milliseconds(!)
class Timer
{
  public:
    Timer();
    void set(unsigned long duration, AlarmCallback_t callback, boolean repeat);
    void check();
  private:
	AlarmCallback_t _callback;
	unsigned long _start;
	unsigned long _duration;
	boolean _active;
	boolean _repeat;
};

// An alarm that fires at a certain time
class Alarm
{
  public:
    Alarm();
    void set(byte hour, byte minute, byte second, AlarmCallback_t callback, boolean repeat);
    void check();
  private:
	AlarmCallback_t _callback;
	byte _hour;
	byte _minute;
	byte _second;
	uint32_t _next;
	boolean _repeat;
};

#endif
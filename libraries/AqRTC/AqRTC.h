/*
AqRTC: Arduino RTC library with safe i2c communication, plausibility checking and caching
Rationale: I've seen all kind of strange things happen on my I2C bus. I was able to resolve
most of this, but who guarantees that all my I2C based RTC readings will always be 100% correct?
Regular RTC libraries just accept blindly whatever they read from the RTC. This library does some
advanced checking:
- did the read succeed at all? (yes, this is not checked in the libraries I saw)
- is the time I read within +/-5 seconds of what the Arduino would expect based on millis()?
- did I read two correct times in succession?
If any of these tests fail, this RTC will fall back to guessing the time based on millis(). 
If there are no time reads at all, it will fall back to guessing based on compile time.
If there's an RTC, but it's not running, it will be set to compile time. Compile time
has to be passed to the constructor since libraries are not recompiled on a regular basis.
To improve speed, this library only reads time every 30s.
Currently supports: DS1307, DS3231
Chris Lüscher, July 2014
*/

#ifndef _AQRTC_H_
#define _AQRTC_H_

// Simple general-purpose date/time class (no TZ / DST / leap second handling!)
// Code based on work by JeeLabs http://news.jeelabs.org/code/
class DateTime {
public:
    DateTime (uint32_t t =0);
    DateTime (uint16_t year, uint8_t month, uint8_t day,
                uint8_t hour =0, uint8_t min =0, uint8_t sec =0);
    DateTime (const char* date, const char* time);
    uint16_t year() const       { return 2000 + yOff; }
    uint8_t month() const       { return m; }
    uint8_t day() const         { return d; }
    uint8_t hour() const        { return hh; }
    uint8_t minute() const      { return mm; }
    uint8_t second() const      { return ss; }
    uint8_t dayOfWeek() const;

    // 32-bit times as seconds since 1/1/2000
    long secondstime() const;   
    // 32-bit times as seconds since 1/1/1970
    uint32_t unixtime(void) const;

protected:
    uint8_t yOff, m, d, hh, mm, ss;
};

class AqRTC {
public:
    void begin(const DateTime& compileTime);
    void adjust(const DateTime& dt);
    bool isRunning();
    DateTime now();
protected:
    bool readTime();
    unsigned long lastSync;
    uint32_t lastTime;
    bool proven; 
};

extern AqRTC RTC;

#endif // _AqRTC_H_

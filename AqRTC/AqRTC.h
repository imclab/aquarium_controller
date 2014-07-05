/*
AqRTC: Arduino RTC library with safe i2c communication, plausibility checking and caching
Will try to fake real time based on compile date/time if no RTC is found
Currently supports: DS1307
Chris Lüscher, July 2014
*/

#ifndef _AQRTC_H_
#define _AQRTC_H_

// Simple general-purpose date/time class (no TZ / DST / leap second handling!)
// Code by JeeLabs http://news.jeelabs.org/code/
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
    uint8_t begin(void);
    void adjust(const DateTime& dt);
    uint8_t isRunning(void);
    DateTime now();
protected:
	bool readTime();
	unsigned long lastSync;
	uint32_t lastTime;
};

extern AqRTC RTC;

#endif // _AqRTC_H_

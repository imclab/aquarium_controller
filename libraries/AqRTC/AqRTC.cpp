/*
AqRTC: Arduino RTC library with safe i2c communication, plausibility checking and caching
Chris Lüscher, July 2014
*/

#include <Wire.h>
#include "AqRTC.h"
#include <avr/pgmspace.h>
#include <Arduino.h>

#define RTC_ADDRESS 0x68
#define SECONDS_FROM_1970_TO_2000 946684800
#define SYNC_INTERVAL 30000

////////////////////////////////////////////////////////////////////////////////
// utility code, some of this could be exposed in the DateTime API if needed
// Code based on work by JeeLabs http://news.jeelabs.org/code/

const uint8_t daysInMonth [] PROGMEM = { 31,28,31,30,31,30,31,31,30,31,30,31 };

// number of days since 2000/01/01, valid for 2001..2099
static uint16_t date2days(uint16_t y, uint8_t m, uint8_t d) {
    if (y >= 2000)
        y -= 2000;
    uint16_t days = d;
    for (uint8_t i = 1; i < m; ++i)
        days += pgm_read_byte(daysInMonth + i - 1);
    if (m > 2 && y % 4 == 0)
        ++days;
    return days + 365 * y + (y + 3) / 4 - 1;
}

uint32_t time2uint32(uint16_t days, uint8_t h, uint8_t m, uint8_t s) {
    return ((days * 24L + h) * 60 + m) * 60 + s;
}

uint32_t toUnixtime(uint16_t y, uint8_t m, uint8_t d, uint8_t hh, uint8_t mm, uint8_t ss){
    return time2uint32(date2days(y, m, d), hh, mm, ss) + SECONDS_FROM_1970_TO_2000;
}

////////////////////////////////////////////////////////////////////////////////
// DateTime implementation - ignores time zones and DST changes
// NOTE: also ignores leap seconds, see http://en.wikipedia.org/wiki/Leap_second
// Code based on work by JeeLabs http://news.jeelabs.org/code/

DateTime::DateTime (uint32_t t) {
  t -= SECONDS_FROM_1970_TO_2000;    // bring to 2000 timestamp from 1970

    ss = t % 60;
    t /= 60;
    mm = t % 60;
    t /= 60;
    hh = t % 24;
    uint16_t days = t / 24;
    uint8_t leap;
    for (yOff = 0; ; ++yOff) {
        leap = yOff % 4 == 0;
        if (days < 365 + leap)
            break;
        days -= 365 + leap;
    }
    for (m = 1; ; ++m) {
        uint8_t daysPerMonth = pgm_read_byte(daysInMonth + m - 1);
        if (leap && m == 2)
            ++daysPerMonth;
        if (days < daysPerMonth)
            break;
        days -= daysPerMonth;
    }
    d = days + 1;
}

DateTime::DateTime (uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t min, uint8_t sec) {
    if (year >= 2000)
        year -= 2000;
    yOff = year;
    m = month;
    d = day;
    hh = hour;
    mm = min;
    ss = sec;
}

static uint8_t conv2d(const char* p) {
    uint8_t v = 0;
    if ('0' <= *p && *p <= '9')
        v = *p - '0';
    return 10 * v + *++p - '0';
}

// A convenient constructor for using "the compiler's time":
// DateTime now (__DATE__, __TIME__);
DateTime::DateTime (const char* date, const char* time) {
    // sample input: date = "Dec 26 2009", time = "12:34:56"
    yOff = conv2d(date + 9);
    // Jan Feb Mar Apr May Jun Jul Aug Sep Oct Nov Dec 
    switch (date[0]) {
        case 'J': m = date[1] == 'a' ? 1 : m = date[2] == 'n' ? 6 : 7; break;
        case 'F': m = 2; break;
        case 'A': m = date[2] == 'r' ? 4 : 8; break;
        case 'M': m = date[2] == 'r' ? 3 : 5; break;
        case 'S': m = 9; break;
        case 'O': m = 10; break;
        case 'N': m = 11; break;
        case 'D': m = 12; break;
    }
    d = conv2d(date + 4);
    hh = conv2d(time);
    mm = conv2d(time + 3);
    ss = conv2d(time + 6);
}

uint8_t DateTime::dayOfWeek() const {    
    uint16_t day = date2days(yOff, m, d);
    return (day + 6) % 7; // Jan 1, 2000 is a Saturday, i.e. returns 6
}

uint32_t DateTime::unixtime(void) const {
  return toUnixtime(yOff, m, d, hh, mm, ss);
}

////////////////////////////////////////////////////////////////////////////////
// RTC implementation

static uint8_t bcd2bin (uint8_t val) { return val - 6 * (val >> 4); }
static uint8_t bin2bcd (uint8_t val) { return val + 6 * (val / 10); }

void AqRTC::begin(const DateTime& compileTime){ 
    lastTime = 0;
    proven = false;    
    if(!isRunning())
        adjust(compileTime);
    else
        if(!readTime())
            lastTime = compileTime.unixtime();
    lastSync = millis();
}

bool AqRTC::readTime(){
    uint32_t newTime;
    uint32_t guessTime;

    Wire.beginTransmission(RTC_ADDRESS);
    Wire.write(0);  
    if(Wire.endTransmission() == 0){ // no error encountered
        if(Wire.requestFrom(RTC_ADDRESS, 7) == 7){ // RTC sent back the required number of bytes
            uint8_t ss = bcd2bin(Wire.read() & 0x7F);
            uint8_t mm = bcd2bin(Wire.read());
            uint8_t hh = bcd2bin(Wire.read());
            Wire.read();
            uint8_t d = bcd2bin(Wire.read());
            uint8_t m = bcd2bin(Wire.read());
            uint16_t y = bcd2bin(Wire.read());

            newTime = toUnixtime(y, m, d, hh, mm, ss);
            // never read a time before, so let's live with what we have for now
            if(!lastTime){
                lastTime = newTime;
                return true;
            }
            guessTime = lastTime + ((millis() - lastSync)/1000);
            // this read is within reasonable limits compared to the Arduino's estimate
            if((guessTime + 5 > newTime) && (guessTime - 5 < newTime)){
                proven = true;
                lastTime = newTime; 
                return true;
            } else {
                // if the last read was unproven, we rather belive the new one
                // ((if it was proven, we implicitly believe the old one, but set it to be unproven))
                if(!proven){
                    lastTime = newTime; 
                    return true;                    
                }
            }
        }
    }  
    proven = false;
    return false;
}

bool AqRTC::isRunning() {
    Wire.beginTransmission(RTC_ADDRESS);
    Wire.write(0);
    if(Wire.endTransmission() == 0){
        if(Wire.requestFrom(RTC_ADDRESS, 1) == 1){
            uint8_t ss = Wire.read();
            return !(ss>>7);
        }
    }
    return false;
}

void AqRTC::adjust(const DateTime& dt) {
    Wire.beginTransmission(RTC_ADDRESS);
    Wire.write(0);
    Wire.write(bin2bcd(dt.second()));
    Wire.write(bin2bcd(dt.minute()));
    Wire.write(bin2bcd(dt.hour()));
    Wire.write(bin2bcd(0));
    Wire.write(bin2bcd(dt.day()));
    Wire.write(bin2bcd(dt.month()));
    Wire.write(bin2bcd(dt.year() - 2000));
    Wire.write(0);
    Wire.endTransmission();

    lastTime = dt.unixtime();
}

DateTime AqRTC::now() {
    if((millis() - lastSync) >= SYNC_INTERVAL){
        if(!readTime())
            lastTime += (millis() - lastSync)/1000; //this is not the most precise way to do this, millis might get lost
        lastSync = millis();                          //but heck, we are simulating time based on compile time
        return DateTime(lastTime);
    } else {
        return DateTime(lastTime + ((millis() - lastSync)/1000));
    }
}

AqRTC RTC = AqRTC();
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////              
// Arduino Based Freshwater Aquarium Control, Set Clock Helper
////////////////////////////////////////////////////////////////////////////////              
// Version: Alpha 4
// Author: Chris Lüscher (mail@christophluescher.ch)
//
// Sets the RTC time to the time of your PC, just upload to the Arduino
// And observe the output
//
// Some of this code courtesy of the Adafruit example files
//
// Disclaimer: this is a personal project. Pumping stuff into your aquarium
// is DANGEROUS and so is controlling your aquarium hardware via RC switches.
// Use this project as an inspiration only if you know what you are doing.
// I provide this information 'as is' without warranty of any kind.
////////////////////////////////////////////////////////////////////////////////     

#include <Wire.h>
#include "RTClib.h"
 
RTC_DS1307 RTC;
 
void setup () {
  Serial.begin(38400);
  Wire.begin();
  RTC.begin();

  if (! RTC.isrunning()) {
    Serial.println("RTC is NOT running!");
    // following line sets the RTC to the date & time this sketch was compiled
    RTC.adjust(DateTime(__DATE__, __TIME__));
  } else {
    RTC.adjust(DateTime(__DATE__, __TIME__));
  }
 
}
 
void loop () {
    DateTime now = RTC.now();
 
    Serial.print(now.year(), DEC);
    Serial.print('/');
    Serial.print(now.month(), DEC);
    Serial.print('/');
    Serial.print(now.day(), DEC);
    Serial.print(' ');
    Serial.print(now.hour(), DEC);
    Serial.print(':');
    Serial.print(now.minute(), DEC);
    Serial.print(':');
    Serial.print(now.second(), DEC);
    Serial.println();
 
    Serial.print(" since 1970 = ");
    Serial.print(now.unixtime());
    Serial.print("s = ");
    Serial.print(now.unixtime() / 86400L);
    Serial.println("d");
 
    // calculate a date which is 7 days and 30 seconds into the future
    DateTime future (now.unixtime() + 7 * 86400L + 30);
 
    Serial.print(" now + 7d + 30s: ");
    Serial.print(future.year(), DEC);
    Serial.print('/');
    Serial.print(future.month(), DEC);
    Serial.print('/');
    Serial.print(future.day(), DEC);
    Serial.print(' ');
    Serial.print(future.hour(), DEC);
    Serial.print(':');
    Serial.print(future.minute(), DEC);
    Serial.print(':');
    Serial.print(future.second(), DEC);
    Serial.println();
 
    Serial.println();
    delay(3000);
}

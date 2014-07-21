////////////////////////////////////////////////////////////////////////////////              
// Arduino Based Freshwater Aquarium Control
////////////////////////////////////////////////////////////////////////////////              
// Version: Alpha 4
// Author: Chris Lüscher (mail@christophluescher.ch)
// Current features:
//   - Measure: pH, O2, temperature
//   - Report: pH, O2, temperature, internal time, Arduino RAM usage, fan speed
//   - Fertilize: 3 fertilizer pumps
//   - Control: 5 RC power switches
//   - Cooling: control one set of 12v pc case cooling fans
//   - Display: 16x2 LCD display
// DISCLAIMER: this is a personal project. Pumping stuff into your aquarium
// is DANGEROUS and so is controlling your aquarium hardware via RC switches.
// Use this project as an inspiration only if you know what you are doing.
// I provide this information 'as is' without warranty of any kind.
////////////////////////////////////////////////////////////////////////////////              

#include <SoftwareSerial.h>
#include <Wire.h>
#include <OneWire.h>
#include <Adafruit_MCP23017.h>
#include <Adafruit_RGBLCDShield.h>
#include <AqAlarms.h>
#include <Adafruit_MotorShield.h>
#include <avr/pgmspace.h>
#include <AqRTC.h>
#include "definitions.h"
#include <avr/wdt.h>

////////////////////////////////////////////////////////////////////////////////              
// setup, runs once
////////////////////////////////////////////////////////////////////////////////
void setup(){  
  // tell the watchdog to reboot the Arduino if it doesn't react for 8s
  wdt_enable(WDTO_8S);
  
  // Set up serial communication
  Serial.begin(38400);
  
  // Set up wire library (i2c communication)
  Wire.begin();
  
  // Set up real time clock
  RTC.begin(DateTime(__DATE__, __TIME__));

  // Set up LCD display
  lcd.begin(16, 2);
  
  // Set up switches
  pinMode(RCLpin, OUTPUT);
    
  // Set up fertilization
  AFMS.begin();
  
  // Alarm for daily fertilization
  fertilizeAlarm.set(fertilizeStartHour, fertilizeStartMinute, 0, Fertilize, true);

  // Set up sensors
  pHSerial.begin(38400);
  O2Serial.begin(38400);
  O2Serial.print(F("25,0\r")); 
  O2Serial.print(F("L0\r"));
  pHSerial.print(F("25,0\r"));
  pHSerial.print(F("L0\r"));
  pHSerial.listen();
  
  // Temperature
  tempSensor.target_search(0x28); //DS18B20 Temperature Sensor
  if(!tempSensor.search(tempAddr))
     Serial.println(F("ST No temp sensor found."));
 
  // Timer to update serial, sensor calibration, fan speed, switch states
  updateTimer.set(updateInterval, update, true); 
  
  // set all switches to correct state
  checkSwitches();
  
  // we are ready now
  lcd.setCursor(0, 0);
  lcd.print(F("READY"));
  Serial.println(F("ST Ready"));
}

////////////////////////////////////////////////////////////////////////////////              
// loop, runs continuously 
////////////////////////////////////////////////////////////////////////////////
void loop(){  
  // tell the watchdog that we are still alive
  wdt_reset();
  
  measurepH();
  measureO2();
  measureTemperature();
  
  checkMenu();
  
  pumpReleaseTimer.check();
  updateTimer.check();
  fertilizeAlarm.check();
}


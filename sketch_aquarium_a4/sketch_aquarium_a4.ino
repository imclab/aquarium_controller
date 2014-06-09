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
// DISCLAIMER: this is a personal project. Pumping stuff into your aquarium
// is DANGEROUS and so is controlling your aquarium hardware via RC switches.
// Use this project as an inspiration only if you know what you are doing.
// I provide this information 'as is' without warranty of any kind.
////////////////////////////////////////////////////////////////////////////////              

#include <SoftwareSerial.h>
#include <Wire.h>
#include <LeanAlarms.h>
#include <Adafruit_MotorShield.h>
#include <avr/pgmspace.h>
#include <RTClib.h>
#include "definitions.h"

////////////////////////////////////////////////////////////////////////////////              
// setup, runs once
////////////////////////////////////////////////////////////////////////////////
void setup(){  
  // Serial Communication
  Serial.begin(38400);
  
  // Real Time Clock
  Wire.begin();
  
  //Switches
  pinMode(RCLpin, OUTPUT);
  checkSwitches();
    
  // Fertilization
  AFMS.begin();
  pump1->setSpeed(255);
  pump2->setSpeed(255);
  pump3->setSpeed(255);
  fertilizeAlarm.set(fertilizeStartHour, fertilizeStartMinute, 0, Fertilize, true);

  // Sensors
  pHSerial.begin(38400);
  O2Serial.begin(38400);
  pHSerial.listen();
  
  // Update serial, sensor calibration, fan speed, switch states
  updateTimer.set(updateInterval, Update, true); 
  
  Serial.println(F("ST Controller initialized"));
}

////////////////////////////////////////////////////////////////////////////////              
// loop, runs continuously 
////////////////////////////////////////////////////////////////////////////////
void loop(){ 
  float measured;
  char inchar;
  float temperature;
  
  //get pH readings from sensor
  while (pHSerial.available() && (pHSensorLength < sensorBufferSize)) {                                            
    inchar = (char)pHSerial.read();                               
    if ((inchar == '\r') || ((pHSensorLength + 1) == sensorBufferSize)) {
      pHSensor[pHSensorLength] = '\0';
      measured = atof(pHSensor);
      if((measured > 0.0) && (abs(pH - int(measured * 100.0)) <= 200)){
        pH = pH - ((pH - int(measured * 100.0)) / 10);
      }
      pHSensorLength = 0;      
      O2Serial.listen();
    } else {
      pHSensor[pHSensorLength] = inchar;
      pHSensorLength++;     
    }     
  }

  // measure board voltage, used for reference later
  // float vcc=float(readVcc())/1000.0;
  // measure temperature
  // float temperature = (((analogRead(temperaturePin)/1024.0) * vcc) - .5) * 100.0;  // TMP36
  temperature = (((analogRead(temperaturePin)/1024.0) * 5.0) * 51.2) - 20.5128;  // Atlas Scientific ENV-TMP
  // smoothen the temperature readings
  tempAverage = tempAverage - ((tempAverage - temperature)/100.0);
  
  //get O2 readings from sensor
  while (O2Serial.available() && (O2SensorLength < sensorBufferSize)) { 
    inchar = (char)O2Serial.read();   
    if ((inchar == '\r') || ((O2SensorLength + 1) == sensorBufferSize)) {
      O2Sensor[O2SensorLength] = '\0';
      measured = atof(O2Sensor);
      if((measured > 0.0) && (abs(O2 - int(measured * 100.0))<=1200)){
        O2 = O2 - ((O2 - int(measured * 100.0)) / 10); 
      }
      O2SensorLength = 0;      
      pHSerial.listen();
    } else {
      O2Sensor[O2SensorLength] = inchar;
      O2SensorLength++;       
    }   
  }
   
  pumpReleaseTimer.check();
  updateTimer.check();
  fertilizeAlarm.check();
}


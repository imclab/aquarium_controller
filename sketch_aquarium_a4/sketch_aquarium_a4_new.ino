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

// Switches
const byte RCLpin=7;
// Hour:minute times to switch on, for up to 5 switches
// the time you set for unused switches does not matter
// trying out a new approach now:
// switches are not turned off/on by alarms, rather
// the correct switch seting is checked every 60 seconds
// and sent to every switch
// so if any switch should "forget" its status, it should timely be 
// brought back to "reason" like this
// switches should not be used for time sensitive tasks (e.g. fertilizer pumps)
const byte switchCheckInterval = 60000; // 60 secs
const byte switchOnHours[5]PROGMEM = {13, 13, 13, 10, 10};
const byte switchOnMinutes[5]PROGMEM = {15, 25, 00, 10, 10};
// Hour:minute times to switch off
const byte switchOffHours[5]PROGMEM = {21, 21, 21, 10, 10};
const byte switchOffMinutes[5]PROGMEM = {15, 25, 00, 10, 10};

// Fertilizer Pumps
// Pump for one minute to measure pump speed (see helper script),
// then pre-calculate pumping times in ms, 0 for don't pump
// at last measurement, my pump capacity was 1.127ml/s
// use unsigned long if you want to pump longer than 65535ms 
const unsigned int pump1Time = 10647; //12ml
const unsigned int pump2Time = 6211;  //7ml
const unsigned int pump3Time = 0;     //don't pump
// Hour:minute time to start pumping ever day
const byte fertilizeStartHour = 8;
const byte fertilizeStartMinute = 20;
Adafruit_MotorShield AFMS = Adafruit_MotorShield(); 
Adafruit_DCMotor *pump1 = AFMS.getMotor(1);
Adafruit_DCMotor *pump2 = AFMS.getMotor(2);
Adafruit_DCMotor *pump3 = AFMS.getMotor(3);

// Cooling
// Start cooling at a temperature higher than...
// Stop at a temperature 0.2C lower than...
const float coolingTrigger = 23.7;  
Adafruit_DCMotor *coolingVents = AFMS.getMotor(4);

//Real Time Clock
RTC_DS1307 RTC;

// Output & Update
// for now I pick up this output via a raspberry pi
// and do some reporting there
const unsigned long updateInterval = 10000; // 10 secs

// temperature measurement
const byte temperaturePin = A0;
float tempAverage = 20.0;

// Atlas Scientific Sensors
const byte sensorBufferSize = 15;

// pH measurement
// measurements are stored as int (original number * 100) to save memory
const byte pHRX=2;
const byte pHTX=3;
int pH = 700;
SoftwareSerial pHSerial(pHRX, pHTX);
byte pHSensorLength = 0;
char pHSensor[sensorBufferSize+1];

// O2 measurement
// measurements are stored as int (original number * 100) to save memory
const byte O2RX=4;
const byte O2TX=5;
int O2 = 800;
SoftwareSerial O2Serial(O2RX, O2TX);
byte O2SensorLength = 0;
char O2Sensor[sensorBufferSize+1];

// Alarms/Timers
TimerOnce pumpReleaseTimer;
TimerRepeat updateTimer;
TimerRepeat updateSwitchesTimer;
AlarmRepeat fertilizeAlarm;
  
////////////////////////////////////////////////////////////////////////////////              
// Switches
// Go here to learn more about RC Switches and the Arduino
// https://code.google.com/p/rc-switch/
////////////////////////////////////////////////////////////////////////////////
void RCLswitch(uint16_t code) {
    for (byte nRepeat=0; nRepeat<6; nRepeat++) {
        for (byte i=4; i<16; i++) {
            RCLtransmit(1,3);
            if (((code << (i-4)) & 2048) > 0) {
                RCLtransmit(1,3);
            } else {
                RCLtransmit(3,1);
            }
        }
        RCLtransmit(1,31);    
    }
}

void RCLtransmit(int nHighPulses, int nLowPulses) {
    digitalWrite(RCLpin, HIGH);
    delayMicroseconds( 350 * nHighPulses);
    digitalWrite(RCLpin, LOW);
    delayMicroseconds( 350 * nLowPulses);
}

////////////////////////////////////////////////////////////////////////////////              
// Daily fertilization run
////////////////////////////////////////////////////////////////////////////////
void pump1Timeout(){
  pump1->run(RELEASE);
  if(pump2Time > 0){
     pump2->run(FORWARD);
     pumpReleaseTimer.set(pump2Time, pump2Timeout);
  }else if(pump3Time > 0){
     pump3->run(FORWARD);
     pumpReleaseTimer.set(pump3Time, pump3Timeout);
  }
}

void pump2Timeout(){
  pump2->run(RELEASE);
  if(pump3Time > 0){
     pump3->run(FORWARD);
     pumpReleaseTimer.set(pump3Time, pump3Timeout);
  }
}

void pump3Timeout(){
  pump3->run(RELEASE);
}

void Fertilize(){
  if(pump1Time > 0){
     pump1->run(FORWARD);
     pumpReleaseTimer.set(pump1Time, pump1Timeout);
  }else if(pump2Time > 0){
     pump2->run(FORWARD);
     pumpReleaseTimer.set(pump2Time, pump2Timeout);
  }else if(pump3Time > 0){
     pump3->run(FORWARD);
     pumpReleaseTimer.set(pump3Time, pump3Timeout);
  }
}

////////////////////////////////////////////////////////////////////////////////              
// Regular update: Output to serial, sensor calibration, fan speed
////////////////////////////////////////////////////////////////////////////////
void SerialPrintDigits(byte digits){
  if(digits < 10)
    Serial.print('0');
  Serial.print(digits, DEC);
}

void Update(){  
  Serial.print(F("TE "));
  Serial.println(tempAverage, 1);
  Serial.print(F("PH "));
  Serial.println(float(pH/100.0), 1);
  Serial.print(F("O2 "));
  Serial.println(float(O2/100.0), 2);
  Serial.print(F("TI "));
  SerialPrintDigits(RTC.now().hour());
  Serial.print(F(":"));
  SerialPrintDigits(RTC.now().minute());
  Serial.println();
  Serial.print(F("ME "));
  Serial.println(memoryFree());
  
  char buffer[10];
  dtostrf(tempAverage, -1, 1, buffer);
  buffer[2] = ',';
  buffer[4] = '\r';  
  buffer[5] = '\0';  
  O2Serial.print(buffer); 
  O2Serial.print(F("L1\r"));
  pHSerial.print(buffer);
  pHSerial.print(F("L1\r"));
  
  // cooling
  if(tempAverage >= coolingTrigger){
    float mySpeed = 150 + ((tempAverage - coolingTrigger) *  300);
    if(mySpeed > 255) mySpeed = 255;
    Serial.print(F("FA "));
    Serial.println(int(mySpeed));
    coolingVents->setSpeed(int(mySpeed));
    coolingVents->run(FORWARD);
  }else if(tempAverage <= (coolingTrigger - 0.2)){
    coolingVents->run(RELEASE);
  }
}

////////////////////////////////////////////////////////////////////////////////              
// The difference, in seconds, between the start time and the stop time
// (assuming that the stop time is always later than the start time)
////////////////////////////////////////////////////////////////////////////////
long secDiff(byte startHour, byte startMinute, byte stopHour, byte stopMinute){
  byte hourDiff;
  byte minuteDiff;
  
  if(startHour > stopHour){
    hourDiff = stopHour + 24 - startHour;
  }else{
    hourDiff = stopHour - startHour;
  }
  
  if(startMinute > stopMinute){
    minuteDiff = stopMinute + 60 - startMinute;
    if(hourDiff > 0){
       hourDiff -= 1;
    }else{
       hourDiff = 23;
    }
  }else{
    minuteDiff = stopMinute - startMinute;
  }
  
  return ((long) hourDiff * 60 * 60) + ((long) minuteDiff * 60);
}

////////////////////////////////////////////////////////////////////////////////              
// Are we, now, in the time window between the "on" time and the "off" time?
///////////////////////////////////////////////////////////////////////////////
boolean inTimeWindow(byte onHour, byte onMinute, byte offHour, byte offMinute){
  DateTime now = RTC.now();
  
  DateTime startTime(now.unixtime() - secDiff(onHour, onMinute, now.hour(), now.minute()) - now.second());
  DateTime stopTime(startTime.unixtime() + secDiff(onHour, onMinute, offHour, offMinute));
  
  if((now.unixtime() > startTime.unixtime()) && (now.unixtime() < stopTime.unixtime())){
    return true;
  }else{
    return false;
  }
}

////////////////////////////////////////////////////////////////////////////////              
// Sets the correct state for a switch (e.g. after a reboot)
///////////////////////////////////////////////////////////////////////////////
void setSwitchState(byte nr){  
  if(inTimeWindow(pgm_read_byte(&switchOnHours[nr-1]), pgm_read_byte(&switchOnMinutes[nr-1]), pgm_read_byte(&switchOffHours[nr-1]), pgm_read_byte(&switchOffMinutes[nr-1]))){
    switch (nr) {
    case 1:
      RCLswitch(0b100111000010);
      break;
    case 2:
      RCLswitch(0b100110100010);
      break;
    case 3:
      RCLswitch(0b100110010010);
      break;
    case 4:
      RCLswitch(0b100110001010);
      break;
    case 5:
      RCLswitch(0b100110000110);
      break;
    }
  }else{
    switch (nr) {
    case 1:
      RCLswitch(0b100111000001);
      break;
    case 2:
      RCLswitch(0b100110100001);
      break;
    case 3:
      RCLswitch(0b100110010001);
      break;
    case 4:
      RCLswitch(0b100110001001);
      break;
    case 5:
      RCLswitch(0b100110000101);
      break;
    }
  }
}

void checkSwitches(){
  for (byte i = 1; i < 6; i++){
    setSwitchState(i);
  } 
}

////////////////////////////////////////////////////////////////////////////////              
// setup, runs once
////////////////////////////////////////////////////////////////////////////////
void setup(){  
  // Serial Communication
  Serial.begin(38400);
  
  // Real Time Clock
  Wire.begin();
  RTC.begin();
  
  //Switches
  pinMode(RCLpin, OUTPUT);
    
  checkSwitches();
  
  updateSwitchesTimer.set(switchCheckInterval, checkSwitches); 
  
  // Fertilization
  AFMS.begin();
  pump1->setSpeed(255);
  pump2->setSpeed(255);
  pump3->setSpeed(255);
  fertilizeAlarm.set(fertilizeStartHour, fertilizeStartMinute, 0, Fertilize);

  // Sensors
  pHSerial.begin(38400);
  O2Serial.begin(38400);
  pHSerial.listen();
  
  // Update serial, sensor calibration, fan speed
  updateTimer.set(updateInterval, Update); 
  
  Serial.println(F("ST Controller initialized"));
}

////////////////////////////////////////////////////////////////////////////////              
// returns actual board voltage in millivolts
// used for precise measurement with components that 
// need a reference voltage
////////////////////////////////////////////////////////////////////////////////
long readVcc() {
   long result;
   // Read 1.1V reference against AVcc
   ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
   delay(2); // Wait for Vref to settle
   ADCSRA |= _BV(ADSC); // Convert
   while (bit_is_set(ADCSRA,ADSC));
   result = ADCL;
   result |= ADCH<<8;
   result = 1126400L / result; // Back-calculate AVcc in mV
   return result;
}

////////////////////////////////////////////////////////////////////////////////              
// check free memory
// https://www.inkling.com/read/arduino-cookbook-michael-margolis-2nd/chapter-17/recipe-17-2
////////////////////////////////////////////////////////////////////////////////

extern int __bss_end;
extern void *__brkval;

int memoryFree()
{
  int freeValue;  
  if((int)__brkval == 0)
     freeValue = ((int)&freeValue) - ((int)&__bss_end);
  else
    freeValue = ((int)&freeValue) - ((int)__brkval);

  return freeValue;
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
  tempAverage = tempAverage - ((tempAverage - temperature)/1000.0);
  
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
   
  delay(1);
  pumpReleaseTimer.check();
  updateTimer.check();
  fertilizeAlarm.check();
}


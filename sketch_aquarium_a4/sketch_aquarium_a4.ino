////////////////////////////////////////////////////////////////////////////////              
// Arduino Based Freshwater Aquarium Control
////////////////////////////////////////////////////////////////////////////////              
// Version: Alpha 4
// Author: Chris Lüscher (mail@christophluescher.ch)
// Current features:
//   - Measure: pH, O2, temperature
//   - Report: pH, O2, temperature, internal time
//   - Fertilize: 3 fertilizer pumps
//   - Control: 5 RC power switches
//   - Cooling: control one set of 12v pc case cooling fans
// Disclaimer: this is a personal project. Pumping stuff into your aquarium
// is DANGEROUS and so is controlling your aquarium hardware via RC switches.
// Use this project as an inspiration only if you know what you are doing.
// I provide this information 'as is' without warranty of any kind.
////////////////////////////////////////////////////////////////////////////////              

#include <SoftwareSerial.h>
#include <Wire.h>
#include <RTClib.h>
#include <Time.h>
#include <TimeAlarms.h>
#include <Adafruit_MotorShield.h>

// Switches
#define RCLpin 7
// Hour:minute times to switch on, for up to 5 switches
// the time you set for unused switches does not matter
// trying out a new approach now:
// switches are not turned off/on by alarms, rather
// the correct switch seting is checked every 60 seconds
// and sent to every switch
// so if any switch should "forget" its status, it should timely be 
// brought back to "reason" like this
// switches should not be used for time sensitive tasks (e.g. fertilizer pumps)
const byte switchCheckInterval = 60;
const byte switchOnHours[5] = {13, 13, 13, 10, 10};
const byte switchOnMinutes[5] = {15, 25, 00, 10, 10};
// Hour:minute times to switch off
const byte switchOffHours[5] = {21, 21, 21, 10, 10};
const byte switchOffMinutes[5] = {15, 25, 00, 10, 10};

// Fertilizer Pumps
// Pump capacity in ml/s, for now, we assume that every pump has the same capacity
// Pump for one minute to measure pump speed (see helper script)
const float pumpCapacity = 1.127;
// Volume to pump per day, in ml, 0.0 for don't pump
const float pump1Volume = 12.0;
const float pump2Volume = 12.0;  
const float pump3Volume = 0.0;  
// Hour:minute time to start pumping ever day
const byte fertilizeStartHour = 8;
const byte fertilizeStartMinute = 15;
Adafruit_MotorShield AFMS = Adafruit_MotorShield(); 
Adafruit_DCMotor *pump1 = AFMS.getMotor(1);
Adafruit_DCMotor *pump2 = AFMS.getMotor(2);
Adafruit_DCMotor *pump3 = AFMS.getMotor(3);

// Cooling
// Start cooling at a temperature higher than...
// Stop at a temperature 0.2C lower than...
const float coolingTrigger = 24.5;  
Adafruit_DCMotor *coolingVents = AFMS.getMotor(4);

//Real Time Clock
RTC_DS1307 RTC;

// Output
// for now I pick up this output via a raspberry pi
// and do some reporting there
const byte updateInterval = 10; //10 secs

// temperature measurement
const int temperaturePin = A0;
float tempAverage = 20.0;

// Atlas Scientific Sensors
const byte sensorBufferSize = 15;

// pH measurement
// measurements are stored as int (original number * 100) to save memory
#define pHRX 2
#define pHTX 3
int pH = 700;
SoftwareSerial pHSerial(pHRX, pHTX);
byte pHSensorLength = 0;
char pHSensor[sensorBufferSize+1];

// O2 measurement
// measurements are stored as int (original number * 100) to save memory
#define O2RX 4
#define O2TX 5
int O2 = 800;
SoftwareSerial O2Serial(O2RX, O2TX);
byte O2SensorLength = 0;
char O2Sensor[sensorBufferSize+1];

// Helper stuff stored here to save RAM (?)
char buffer[17];
char tempString[10];
  
////////////////////////////////////////////////////////////////////////////////              
// Used for synchronizing the Time library with the RTC
////////////////////////////////////////////////////////////////////////////////
time_t syncProvider(){
  return RTC.now().unixtime();
}

////////////////////////////////////////////////////////////////////////////////              
// Switches
// Go here to learn more about RC Switches and the Arduino
// https://code.google.com/p/rc-switch/
////////////////////////////////////////////////////////////////////////////////
void RCLswitch(uint16_t code) {
    for (int nRepeat=0; nRepeat<6; nRepeat++) {
        for (int i=4; i<16; i++) {
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
void Fertilize(){
  Serial.println(F("ST FERT START"));
  if(pump1Volume > 0.0){
     pump1->run(FORWARD);
     delay((int) (pump1Volume / pumpCapacity * 1000.0));
     pump1->run(RELEASE);
  }
 if(pump2Volume > 0.0){
     pump2->run(FORWARD);
     delay((int) (pump2Volume / pumpCapacity * 1000.0));
     pump2->run(RELEASE);
  }
  if(pump3Volume > 0.0){
     pump3->run(FORWARD);
     delay((int) (pump3Volume / pumpCapacity * 1000.0));
     pump3->run(RELEASE);
  }
  Serial.println(F("ST FERT DONE"));
}

////////////////////////////////////////////////////////////////////////////////              
// Output to serial
////////////////////////////////////////////////////////////////////////////////
void SerialPrintDigits(byte digits){
  if(digits < 10)
    Serial.print('0');
  Serial.print(digits, DEC);
}

void Output(){  
  strcpy(tempString, dtostrf(tempAverage, -1, 1, buffer));

  Serial.print(F("TE "));
  Serial.println(tempString);
  Serial.print(F("PH "));
  Serial.println(dtostrf(float(pH/100.0), -1, 1, buffer));
  Serial.print(F("O2 "));
  Serial.println(dtostrf(float(O2/100.0), -1, 2, buffer));
  Serial.print(F("TI "));
  SerialPrintDigits(RTC.now().hour());
  Serial.print(F(":"));
  SerialPrintDigits(RTC.now().minute());
  Serial.println();
  
  tempString[2] = ',';
  tempString[4] = '\r';  
  tempString[5] = '\0';  
  O2Serial.print(tempString); 
  O2Serial.print(F("L1\r"));
  pHSerial.print(tempString);
  pHSerial.print(F("L1\r"));
  
  // cooling
  if(tempAverage >= coolingTrigger){
    float mySpeed = 150 + ((tempAverage - coolingTrigger) *  300);
    if(mySpeed > 255) mySpeed = 255;
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
  if(inTimeWindow(switchOnHours[nr-1], switchOnMinutes[nr-1], switchOffHours[nr-1], switchOffMinutes[nr-1])){
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
  for (int i = 1; i < 6; i++){
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
  setSyncProvider(syncProvider);
  
  //Switches
  pinMode(RCLpin, OUTPUT);
    
  checkSwitches();
  
  Alarm.timerRepeat(switchCheckInterval, checkSwitches); 
  
  // Fertilization
  AFMS.begin();
  pump1->setSpeed(255);
  pump1->run(RELEASE); //turn pump off, just to be safe
  pump2->setSpeed(255);
  pump2->run(RELEASE);
  pump3->setSpeed(255);
  pump3->run(RELEASE);
  Alarm.alarmRepeat(fertilizeStartHour, fertilizeStartMinute, 0, Fertilize);

  // Sensors
  pHSerial.begin(38400);
  O2Serial.begin(38400);
  pHSerial.listen();
  
  // Output
  Alarm.timerRepeat(updateInterval, Output); 
}

////////////////////////////////////////////////////////////////////////////////              
// returns actual board voltage in millivolts
// used for precise measurement with components that 
// need a reference voltage
////////////////////////////////////////////////////////////////////////////////
// long readVcc() {
//   long result;
//   // Read 1.1V reference against AVcc
//    ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
//   delay(2); // Wait for Vref to settle
//   ADCSRA |= _BV(ADSC); // Convert
//   while (bit_is_set(ADCSRA,ADSC));
//   result = ADCL;
//   result |= ADCH<<8;
//   result = 1126400L / result; // Back-calculate AVcc in mV
//   return result;
// }

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
   
  Alarm.delay(2);
}


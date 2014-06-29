#ifndef definitions_h
#define definitions_h

// Switches
const byte RCLpin = 7;
// Hour:minute times to switch on, for up to 5 switches
// the time you set for unused switches does not matter
// switches are not turned off/on by alarms, rather the
// correct state is sent to each switch on a regular basis
// switches should not be used for time sensitive tasks (e.g. fertilizer pumps)
const byte switchOnHours[5]PROGMEM = {13, 13, 13, 10, 11};
const byte switchOnMinutes[5]PROGMEM = {15, 25, 00, 10, 10};
// Hour:minute times to switch off
const byte switchOffHours[5]PROGMEM = {23, 23, 23, 10, 23};
const byte switchOffMinutes[5]PROGMEM = {15, 25, 00, 10, 10};
byte updatingNow = 1; //used to loop through all switches

// Fertilizer Pumps
// Pump for one minute to measure pump speed (see helper script),
// then pre-calculate pumping times in ms, 0 for don't pump
// at last measurement, my pump capacity was 1.127ml/s
// use unsigned long if you want to pump longer than 65535ms 
const unsigned int pump1Time = 7098; //8ml
const unsigned int pump2Time = 2661;  //3ml
const unsigned int pump3Time = 0;  //don't pump
// Hour:minute time to start pumping ever day
const byte fertilizeStartHour = 8;
const byte fertilizeStartMinute = 25;
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
Timer pumpReleaseTimer;
Timer updateTimer;
Alarm fertilizeAlarm;

//LCD Output
Adafruit_RGBLCDShield lcd = Adafruit_RGBLCDShield();

//Menu
boolean inMenu = false;
byte menuPosition = 1;
byte menuMaxpos = 2;

//Modes
boolean maintenanceMode = false;

#endif

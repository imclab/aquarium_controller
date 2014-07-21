#ifndef definitions_h
#define definitions_h

// Switches
const byte RCLpin = 7;
// Switch types: 
// T = Timer
// M = Maintenance (Timer, but turned off during maintenance)
// P = pH (Timer, pH value and turned off during maintenance)
// 0 = always off
// 1 = always on
const char switchTypes[]PROGMEM = "TMP0T";
// Hour:minute times to switch on.
// Switches should not be used for highly time sensitive tasks (e.g. fertilizer pumps).
const byte switchOnHours[5]PROGMEM = {13, 13, 13, 10, 11};
const byte switchOnMinutes[5]PROGMEM = {15, 25, 00, 10, 10};
// Hour:minute times to switch off
const byte switchOffHours[5]PROGMEM = {21, 21, 21, 10, 21};
const byte switchOffMinutes[5]PROGMEM = {15, 25, 00, 10, 10};
// pH switches are turned off below this value
const int targetpH = 660;
// stores the switch states for output
char switchMatrix[] = "00000";
byte updatingNow = 1; //used to loop through all switches

// Fertilizer Pumps
// Pump for one minute to measure pump speed (see helper script),
// then pre-calculate pumping times in ms, 0 for don't pump.
// At last measurement, my pump capacity was 1.127ml/s.
// Use unsigned long if you want to pump longer than 65535ms 
const unsigned int pumpTime[]PROGMEM = {6654, 4436, 6654}; //7.5ml 5ml 7.5ml (Basic EI, Mikro Basic, Special N)
// Hour:minute time to start pumping ever day
const byte fertilizeStartHour = 8;
const byte fertilizeStartMinute = 25;
Adafruit_MotorShield AFMS = Adafruit_MotorShield(); 
Adafruit_DCMotor *pump[] = {AFMS.getMotor(1), AFMS.getMotor(2), AFMS.getMotor(3)};
byte pumpingNow = 0; //used to loop through all pumps

// Cooling
// Start cooling at a temperature higher than...
// Stop at a temperature 0.2C lower than...
const float coolingTrigger = 23.7;  
Adafruit_DCMotor *coolingVents = AFMS.getMotor(4);

// Output & Update
// for now I pick up this output via a raspberry pi
// and do some reporting there
const unsigned long updateInterval = 10000; // 10 secs

// temperature measurement, OneWire digital temperature sensor on pin 10
OneWire tempSensor(10);
byte tempAddr[8];
byte tempBuffer[2];
byte tempStep = 0;
unsigned long tempNextStep;
float tempAverage = 20.0;

// Atlas Scientific Sensors
const byte sensorBufferSize = 15;

// pH measurement
// measurements are stored as int (original number * 100) to save memory
const byte pHRX=3;
const byte pHTX=2;
int pH = 700;
SoftwareSerial pHSerial(pHRX, pHTX);
byte pHSensorLength = 0;
char pHSensor[sensorBufferSize+1];

// O2 measurement
// measurements are stored as int (original number * 100) to save memory
const byte O2RX=5;
const byte O2TX=4;
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
byte menuMaxpos = 3;

//Modes
boolean maintenanceMode = false;

#endif

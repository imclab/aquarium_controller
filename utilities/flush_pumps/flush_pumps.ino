////////////////////////////////////////////////////////////////////////////////              
// Arduino Based Freshwater Aquarium Control, Flush Pumps Helper
////////////////////////////////////////////////////////////////////////////////              
// Version: Alpha 4
// Author: Chris Lüscher (mail@christophluescher.ch)
//
// This flushes the 3 peristaltic pumps, e.g. to change
// fertilizers or to empty the pumps/tubes for storage or transport
//
// Disclaimer: this is a personal project. Pumping stuff into your aquarium
// is DANGEROUS and so is controlling your aquarium hardware via RC switches.
// Use this project as an inspiration only if you know what you are doing.
// I provide this information 'as is' without warranty of any kind.
////////////////////////////////////////////////////////////////////////////////    

#include <Wire.h>
#include <Adafruit_MotorShield.h>

Adafruit_MotorShield AFMS = Adafruit_MotorShield(); 

Adafruit_DCMotor *pump1 = AFMS.getMotor(1);
Adafruit_DCMotor *pump2 = AFMS.getMotor(2);
Adafruit_DCMotor *pump3 = AFMS.getMotor(3);

void setup() {
  Serial.begin(38400);
  AFMS.begin();
  delay(10000);
  Serial.println("Flushing Pump 1.");
  pump1->setSpeed(255);
  pump1->run(FORWARD);
  delay(35000);
  pump1->run(RELEASE);
  Serial.println("Flushing Pump 2.");
  pump2->setSpeed(255);
  pump2->run(FORWARD);
  delay(35000);
  pump2->run(RELEASE);
  Serial.println("Flushing Pump 3.");
  pump3->setSpeed(255);
  pump3->run(FORWARD);
  delay(35000);
  pump3->run(RELEASE);
}

void loop() {
}

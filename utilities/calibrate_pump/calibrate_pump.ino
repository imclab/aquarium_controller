////////////////////////////////////////////////////////////////////////////////              
// Arduino Based Freshwater Aquarium Control, Pump Calibration Helper
////////////////////////////////////////////////////////////////////////////////              
// Version: Alpha 4
// Author: Chris Lüscher (mail@christophluescher.ch)
// Disclaimer: this is a personal project. Pumping stuff into your aquarium
// is DANGEROUS and so is controlling your aquarium hardware via RC switches.
// Use this project as an inspiration only if you know what you are doing.
// I provide this information 'as is' without warranty of any kind.
////////////////////////////////////////////////////////////////////////////////              

#include <Wire.h>
#include <Adafruit_MotorShield.h>

// Test Run 1 for my pumps
// 1 Minute
// P1: 69 ml =1.15 ml/s
// P2: 66 ml =1.1 ml/s
// P3: 68 ml =1.13 ml/s
// Avg: 1.127 ml/s
// Spec: 1 ml/s

Adafruit_MotorShield AFMS = Adafruit_MotorShield(); 

Adafruit_DCMotor *myMotor = AFMS.getMotor(3);

void setup() {
  Serial.begin(38400);
  AFMS.begin();
}

void loop() {
  Serial.println("Starting motor calibration in 30 seconds.");
  delay(30000);
  Serial.println("Preparation: pre-run for 1 minute.");
  myMotor->setSpeed(255);
  myMotor->run(FORWARD);
  delay(60000);
  myMotor->run(RELEASE);
  Serial.println("Pre-run completed.");
  Serial.println("GET READY, test run starts in 30 seconds!");
  delay(30000);
  Serial.println("Test run starts now.");
  myMotor->setSpeed(255);
  myMotor->run(FORWARD);
  delay(60000);
  myMotor->run(RELEASE);
  Serial.println("Test run done.");
}

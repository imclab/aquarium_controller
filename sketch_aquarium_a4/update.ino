////////////////////////////////////////////////////////////////////////////////              
// Arduino Based Freshwater Aquarium Control
////////////////////////////////////////////////////////////////////////////////                        
// Update: Output to serial, sensor calibration, fan speed, check switch states
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////              
// Print clock digits to serial
////////////////////////////////////////////////////////////////////////////////
void SerialPrintDigits(byte digits){
  if(digits < 10)
    Serial.print('0');
  Serial.print(digits, DEC);
}

////////////////////////////////////////////////////////////////////////////////              
// Regular update run
////////////////////////////////////////////////////////////////////////////////
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
  
  lcd.setCursor(0, 0);
  lcd.print(tempAverage, 1);
  lcd.print(F("C pH "));
  lcd.print(float(pH/100.0), 1);
  lcd.setCursor(0, 1);
  lcd.print(float(O2/100.0), 2);
  lcd.print(F(" O2"));

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
    lcd.setCursor(13, 0);
    lcd.print(F("FAN"));
    float mySpeed = 150 + ((tempAverage - coolingTrigger) *  300);
    if(mySpeed > 255) mySpeed = 255;
    Serial.print(F("FA "));
    Serial.println(int(mySpeed));
    coolingVents->setSpeed(int(mySpeed));
    coolingVents->run(FORWARD);
  }else if(tempAverage <= (coolingTrigger - 0.2)){
    lcd.setCursor(13, 0);
    lcd.print(F("   "));
    coolingVents->run(RELEASE);
  }
  
  // check switch states every 6 "update" calls (60 seconds)
  updateRuns++;
  if(updateRuns > 5){
    checkSwitches();
    updateRuns = 0;
  }
}


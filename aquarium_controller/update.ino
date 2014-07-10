////////////////////////////////////////////////////////////////////////////////              
// Arduino Based Freshwater Aquarium Control
////////////////////////////////////////////////////////////////////////////////                        
// Tab: Update. Output to serial, sensor calibration, fan speed, check switch states
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////              
// Print clock digits to serial
////////////////////////////////////////////////////////////////////////////////
void serialPrintDigits(byte digits){
  if(digits < 10)
    Serial.print(F("0"));
  Serial.print(digits, DEC);
}

////////////////////////////////////////////////////////////////////////////////              
// Regular update run, should be called every 10 seconds
////////////////////////////////////////////////////////////////////////////////
void update(){ 
  // output to serial
  Serial.print(F("TE "));
  Serial.println(tempAverage, 1);
  Serial.print(F("PH "));
  Serial.println(float(pH/100.0), 1);
  Serial.print(F("O2 "));
  Serial.println(float(O2/100.0), 2);
  Serial.print(F("TI "));
  serialPrintDigits(RTC.now().hour());
  Serial.print(F(":"));
  serialPrintDigits(RTC.now().minute());
  Serial.println();
  Serial.print(F("ME "));
  Serial.println(memoryFree());
  Serial.print(F("SW "));
  Serial.println(switchMatrix);
  
  if(!inMenu){
    // update LCD
    lcd.setCursor(0, 0);
    lcd.print(tempAverage, 1);
    lcd.print(F("C pH "));
    lcd.print(float(pH/100.0), 1);
    lcd.setCursor(0, 1);
    lcd.print(float(O2/100.0), 2);
    if(O2 < 1000)
      lcd.print(F(" "));
    lcd.print(F(" O2 S:"));
    lcd.print(switchMatrix);
  }

  // send calibration data to sensors
  // maybe not necessary? http://www.omega.com/Green/pdf/pHbasics_REF.pdf
  // char buffer[10];
  // dtostrf(tempAverage, -1, 1, buffer);
  // buffer[2] = ',';
  // buffer[4] = '\r';  
  // buffer[5] = '\0';  
  // O2Serial.print(buffer); 
  // O2Serial.print(F("L1\r"));
  // pHSerial.print(buffer);
  // pHSerial.print(F("L1\r"));
  
  // cooling
  if(tempAverage >= coolingTrigger){
    if(!inMenu){
      lcd.setCursor(13, 0);
      lcd.print(F("FAN"));
    }
    float mySpeed = 150 + ((tempAverage - coolingTrigger) *  300);
    if(mySpeed > 255) mySpeed = 255;
    Serial.print(F("FA "));
    Serial.println(int(mySpeed));
    coolingVents->setSpeed(int(mySpeed));
    coolingVents->run(FORWARD);
  }else if(tempAverage <= (coolingTrigger - 0.2)){
    if(!inMenu){
      lcd.setCursor(13, 0);
      lcd.print(F("   "));
    }
    coolingVents->run(RELEASE);
  }
  
  // updating a switch takes some time,
  // so we only check one switch with each update run
  setSwitchState(updatingNow);
  updatingNow++;
  if(updatingNow > 5)
    updatingNow = 1;
}


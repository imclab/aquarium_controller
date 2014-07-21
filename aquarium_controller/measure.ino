////////////////////////////////////////////////////////////////////////////////              
// Arduino Based Freshwater Aquarium Control
////////////////////////////////////////////////////////////////////////////////                        
// Tab: Measure
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////              
// measures pH via Atlas Scientific sensor
// then turns on O2 sensor for O2 measurement
////////////////////////////////////////////////////////////////////////////////
void measurepH(){
  float measured;
  char inchar;
  
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
}

////////////////////////////////////////////////////////////////////////////////              
// measures O2 via Atlas Scientific sensor
// then turns on pH sensor for pH measurement
////////////////////////////////////////////////////////////////////////////////
void measureO2(){
  float measured;
  char inchar;
  
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
}

////////////////////////////////////////////////////////////////////////////////              
// measures water temperature
////////////////////////////////////////////////////////////////////////////////
void measureTemperature(){  
  if(tempStep == 0){ //ask for a conversion first
    tempSensor.reset();
    tempSensor.select(tempAddr);
    tempSensor.write(0x44, 0);
    tempNextStep = millis() + 1000;
    tempStep = 1;
  } else {
    if(millis() >= tempNextStep){
      tempSensor.reset();
      tempSensor.select(tempAddr);
      tempSensor.write(0xBE);
      for (byte i = 0; i < 2; i++)
        tempBuffer[i] = tempSensor.read();  
      tempAverage = (float)((tempBuffer[1] << 8) | tempBuffer[0]) / 16.0;
      tempStep = 0;
    }
  }  
}

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
  // measure board voltage, used for reference later
  float vcc=float(readVcc())/1000.0;
  // measure temperature
  // float temperature = (((analogRead(temperaturePin)/1024.0) * vcc) - .5) * 100.0;  // TMP36
  float temperature = (((analogRead(temperaturePin)/1024.0) * vcc) * 51.2) - 20.5128;  // Atlas Scientific ENV-TMP
  // smoothen the temperature readings
  tempAverage = tempAverage - ((tempAverage - temperature)/1000.0);
}

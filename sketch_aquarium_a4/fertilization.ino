////////////////////////////////////////////////////////////////////////////////              
// Arduino Based Freshwater Aquarium Control
////////////////////////////////////////////////////////////////////////////////                        
// Fertilization
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////              
// Daily fertilization run
////////////////////////////////////////////////////////////////////////////////
void pump1Timeout(){
  pump1->run(RELEASE);
  if(pump2Time > 0){
     pump2->run(FORWARD);
     pumpReleaseTimer.set(pump2Time, pump2Timeout, false);
  }else if(pump3Time > 0){
     pump3->run(FORWARD);
     pumpReleaseTimer.set(pump3Time, pump3Timeout, false);
  }else{
     Serial.println(F("ST Fertilization done"));
  }
}

void pump2Timeout(){
  pump2->run(RELEASE);
  if(pump3Time > 0){
     pump3->run(FORWARD);
     pumpReleaseTimer.set(pump3Time, pump3Timeout, false);
  }else{
     Serial.println(F("ST Fertilization done"));
  }
}

void pump3Timeout(){
  pump3->run(RELEASE);
  Serial.println(F("ST Fertilization done"));
}

void Fertilize(){
  if(pump1Time > 0){
     pump1->run(FORWARD);
     pumpReleaseTimer.set(pump1Time, pump1Timeout, false);
  }else if(pump2Time > 0){
     pump2->run(FORWARD);
     pumpReleaseTimer.set(pump2Time, pump2Timeout, false);
  }else if(pump3Time > 0){
     pump3->run(FORWARD);
     pumpReleaseTimer.set(pump3Time, pump3Timeout, false);
  }
}

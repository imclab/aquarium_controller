////////////////////////////////////////////////////////////////////////////////              
// Arduino Based Freshwater Aquarium Control
////////////////////////////////////////////////////////////////////////////////                        
// Tab: Fertilization
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////              
// Daily fertilization run
////////////////////////////////////////////////////////////////////////////////
void pumpRun(){
  if(pumpingNow > 0)
    pump[pumpingNow - 1]->run(RELEASE);

  for(byte i = pumpingNow + 1; i < 4; i++){
    if(pgm_read_word_near(&pumpTime[i - 1]) > 0){
      pumpingNow = i;
      pump[i - 1]->setSpeed(255);
      pump[i - 1]->run(FORWARD);
      pumpReleaseTimer.set(pgm_read_word_near(&pumpTime[i - 1]), pumpRun, false);
      break;
    }
  }
}

void Fertilize(){
  pumpingNow = 0;
  pumpRun();
  Serial.println(F("ST Fertilization run"));
}

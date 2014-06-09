////////////////////////////////////////////////////////////////////////////////              
// Arduino Based Freshwater Aquarium Control
////////////////////////////////////////////////////////////////////////////////                        
// Switches
// Go here to learn more about RC Switches and the Arduino
// https://code.google.com/p/rc-switch/
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////              
// Transmit switch signal
////////////////////////////////////////////////////////////////////////////////
void RCLswitch(uint16_t code) {
    for (byte nRepeat=0; nRepeat<6; nRepeat++) {
        for (byte i=4; i<16; i++) {
            RCLtransmit(1,3);
            if (((code << (i-4)) & 2048) > 0) {
                RCLtransmit(1,3);
            } else {
                RCLtransmit(3,1);
            }
        }
        RCLtransmit(1,31);    
    }
}

void RCLtransmit(int nHighPulses, int nLowPulses) {
    digitalWrite(RCLpin, HIGH);
    delayMicroseconds( 350 * nHighPulses);
    digitalWrite(RCLpin, LOW);
    delayMicroseconds( 350 * nLowPulses);
}

////////////////////////////////////////////////////////////////////////////////              
// The difference, in seconds, between the start time and the stop time
// (assuming that the stop time is always later than the start time)
////////////////////////////////////////////////////////////////////////////////
long secDiff(byte startHour, byte startMinute, byte stopHour, byte stopMinute){
  byte hourDiff;
  byte minuteDiff;
  
  if(startHour > stopHour){
    hourDiff = stopHour + 24 - startHour;
  }else{
    hourDiff = stopHour - startHour;
  }
  
  if(startMinute > stopMinute){
    minuteDiff = stopMinute + 60 - startMinute;
    if(hourDiff > 0){
       hourDiff -= 1;
    }else{
       hourDiff = 23;
    }
  }else{
    minuteDiff = stopMinute - startMinute;
  }
  
  return ((long) hourDiff * 60 * 60) + ((long) minuteDiff * 60);
}

////////////////////////////////////////////////////////////////////////////////              
// Are we, now, in the time window between the "on" time and the "off" time?
///////////////////////////////////////////////////////////////////////////////
boolean inTimeWindow(byte onHour, byte onMinute, byte offHour, byte offMinute){
  DateTime now = RTC.now();
  
  DateTime startTime(now.unixtime() - secDiff(onHour, onMinute, now.hour(), now.minute()) - now.second());
  DateTime stopTime(startTime.unixtime() + secDiff(onHour, onMinute, offHour, offMinute));
  
  if((now.unixtime() > startTime.unixtime()) && (now.unixtime() < stopTime.unixtime())){
    return true;
  }else{
    return false;
  }
}

////////////////////////////////////////////////////////////////////////////////              
// Sets the correct state for a switch (e.g. after a reboot)
///////////////////////////////////////////////////////////////////////////////
void setSwitchState(byte nr){  
  if(inTimeWindow(pgm_read_byte(&switchOnHours[nr-1]), pgm_read_byte(&switchOnMinutes[nr-1]), pgm_read_byte(&switchOffHours[nr-1]), pgm_read_byte(&switchOffMinutes[nr-1]))){
    switch (nr) {
    case 1:
      RCLswitch(0b100111000010);
      break;
    case 2:
      RCLswitch(0b100110100010);
      break;
    case 3:
      RCLswitch(0b100110010010);
      break;
    case 4:
      RCLswitch(0b100110001010);
      break;
    case 5:
      RCLswitch(0b100110000110);
      break;
    }
  }else{
    switch (nr) {
    case 1:
      RCLswitch(0b100111000001);
      break;
    case 2:
      RCLswitch(0b100110100001);
      break;
    case 3:
      RCLswitch(0b100110010001);
      break;
    case 4:
      RCLswitch(0b100110001001);
      break;
    case 5:
      RCLswitch(0b100110000101);
      break;
    }
  }
}

////////////////////////////////////////////////////////////////////////////////              
// Transmit correct state to all switches
///////////////////////////////////////////////////////////////////////////////
void checkSwitches(){
  for (byte i = 1; i < 6; i++){
    setSwitchState(i);
  } 
}


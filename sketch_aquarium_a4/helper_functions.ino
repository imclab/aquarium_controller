////////////////////////////////////////////////////////////////////////////////              
// Arduino Based Freshwater Aquarium Control
////////////////////////////////////////////////////////////////////////////////                        
// Helper functions
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////              
// returns actual board voltage in millivolts
// used for precise measurement with components that 
// need a reference voltage
////////////////////////////////////////////////////////////////////////////////
long readVcc() {
   long result;
   // Read 1.1V reference against AVcc
   ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
   delay(2); // Wait for Vref to settle
   ADCSRA |= _BV(ADSC); // Convert
   while (bit_is_set(ADCSRA,ADSC));
   result = ADCL;
   result |= ADCH<<8;
   result = 1126400L / result; // Back-calculate AVcc in mV
   return result;
}

////////////////////////////////////////////////////////////////////////////////              
// check free memory
// https://www.inkling.com/read/arduino-cookbook-michael-margolis-2nd/chapter-17/recipe-17-2
////////////////////////////////////////////////////////////////////////////////

extern int __bss_end;
extern void *__brkval;

int memoryFree()
{
  int freeValue;  
  if((int)__brkval == 0)
     freeValue = ((int)&freeValue) - ((int)&__bss_end);
  else
    freeValue = ((int)&freeValue) - ((int)__brkval);

  return freeValue;
}

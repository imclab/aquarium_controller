////////////////////////////////////////////////////////////////////////////////              
// Arduino Based Freshwater Aquarium Control, Probe Calibration Helper
////////////////////////////////////////////////////////////////////////////////              
// Version: Alpha 4
// Author: Chris Lüscher (mail@christophluescher.ch)
//
// Use this to calibrate your Atlas Scientific probes
// You can communicate with the pH/O2 stamp by typing the corresponding
// commands in your Arduino IDE serial monitor, followed by the hash key and enter
// e.g. "L0#" <enter> to send an L0 command to the stamp
//
// Some of this code courtesy of the Atlas Scientific example files
//
// Disclaimer: this is a personal project. Pumping stuff into your aquarium
// is DANGEROUS and so is controlling your aquarium hardware via RC switches.
// Use this project as an inspiration only if you know what you are doing.
// I provide this information 'as is' without warranty of any kind.
////////////////////////////////////////////////////////////////////////////////              


#include <SoftwareSerial.h>                                                    //add the soft serial libray
#define rxpin 2                                                                
#define txpin 3  
#define activatepin 10

SoftwareSerial myserial(rxpin, txpin);                                         //enable the soft serial port

String inputstring = "";                                                       //a string to hold incoming data from the PC
String sensorstring = "";                                                      //a string to hold the data from the Atlas Scientific product
boolean input_stringcomplete = false;                                          //have we received all the data from the PC
boolean sensor_stringcomplete = false;                                         //have we received all the data from the Atlas Scientific product


void setup(){                                                                //set up the hardware
   Serial.begin(38400);                                                      //set baud rate for the hardware serial port to 38400
   myserial.begin(38400);                                                    //set baud rate for software serial port to 38400
   inputstring.reserve(5);                                                   //set aside some bytes for receiving data from the PC
   sensorstring.reserve(30);
   pinMode(activatepin, OUTPUT);
   digitalWrite(activatepin, HIGH);   
   //myserial.print("L0\r");
}
 
void serialEvent() {
  while (Serial.available()) {
    char inChar = (char)Serial.read();
    if (inChar == '#') {
      inputstring += '\r';
      input_stringcomplete = true;
    }else{
      inputstring += inChar;
    }
  }
}

 void loop(){                                                                   
     
  if (input_stringcomplete){                                               
      myserial.print(inputstring);                                             //send that string to the Atlas Scientific product
      inputstring = "";                                                        //clear the string:
      input_stringcomplete = false;                                            //reset the flage used to tell if we have recived a completed string from the PC
      }
 

  while (myserial.available()) {                                               //while a char is holding in the serial buffer
         char inchar = (char)myserial.read();                                  //get the new char
         sensorstring += inchar;                                               //add it to the sensorString
         if (inchar == '\r') {sensor_stringcomplete = true;}                   //if the incoming character is a <CR>, set the flag
         }


   if (sensor_stringcomplete){                                                 //if a string from the Atlas Scientific product has been received in its entirety
       Serial.println(sensorstring);                                           //use the hardware serial port to send that data to the PC
       sensorstring = "";                                                      //clear the string:
       sensor_stringcomplete = false;                                          //reset the flag used to tell if we have received a completed string from the Atlas Scientific product
      }
}




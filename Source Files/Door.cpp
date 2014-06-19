/*
Creator: Gabe Harms
 Date:    07/13/13
 Name:    Door Library
 Description: This class allows for monitoring of a
 door lock which uses a motor to open and close a dead bolt.
 It can automatcially call for opening and closing of the door
 and check itself for any faults
 
 Now for the lcd. This has 6 pins which must be set up using the liquid
 crystal class. The other R/W pin goes to ground and the VO goes to the wiper
 part of the potentiometer while the ends of it are connected to 5V and ground. 
 
 */

#include "Door.h"


 PROGMEM prog_uchar door_open_call[]  = { "Green: called openDoor()      " };
prog_uchar door_close_call[] PROGMEM = { "Green: called closeDoor()      " };
prog_uchar door_opening[] PROGMEM = { "Green: Opening Door      " };
prog_uchar door_open[] PROGMEM = { "Green: Door Open        " };
prog_uchar door_closing[] PROGMEM = { "Green: Closing Door      " };
prog_uchar door_closed[] PROGMEM = { "Green: Door Closed      " };

prog_uchar door_fault_blank[] PROGMEM = { "" };
prog_uchar door_fault_state_conflict[] PROGMEM = { "Red: State Conflict Fault. #" };
prog_uchar door_fault_six_spaces[] PROGMEM = { "      " };
prog_uchar door_fault_opendoor[] PROGMEM = { "Yellow: openDoor() called while doorState is true Fault. #" };
prog_uchar door_fault_closedoor[] PROGMEM = { "Yellow: closeDoor() called while doorState is false Fault. #" };
prog_uchar door_fault_report[] PROGMEM = { "********** Fault Report ************\nFault         Occurances        Time\nState Conflict    \n" };
prog_uchar door_close_fault[] PROGMEM = { "Close Fault       " };
prog_uchar door_open_fault[] PROGMEM = { "Open Fault        " };
prog_uchar door_fault_stars[] PROGMEM = { "************************************" };

prog_uchar time_zero[] PROGMEM = { "0" };
prog_uchar time_colon[] PROGMEM = { ":" };

prog_uchar memory_left[] PROGMEM = { "Memory Left: " };

prog_uint16_t zero = 0;
prog_uint16_t one = 1;
prog_uint16_t three = 3;
prog_uint16_t ten = 10;
prog_uint16_t sixty = 60;

prog_uint16_t delay_time = 300;
prog_uint16_t motor_close_speed_value = 15;
prog_uint16_t motor_open_speed_value = 50;


Door::Door()
{
  //Constructor
  Door(2,11,13,8);
}

Door::Door (int transmitterIn, int motorSpeed, int motorDir, int motorBrake) //initialize door variable
{
  //Contruct)or

  //dynamic initialization
  this->transmitterIn = transmitterIn;
  this->motorSpeed    = motorSpeed;
  this->motorDir      = motorDir;
  this->motorBrake    = motorBrake;
  //static initialization
  this->doorState         = 0;
  this->stateConflict     = 0; 
  this->stateConflictCount= 0;
  this->openFault         = 0;
  this->openFaultCount    = 0;
  this->closeFault        = 0;
  this->closeFaultCount   = 0;
  this->wiggleRoom        = 0;
  this->fault             = 0;
  this->faultReport       = 0;
  
  //set up initial digital pin
  pinMode(transmitterIn, INPUT_PULLUP);
  pinMode(motorSpeed, OUTPUT);
  pinMode(motorDir, OUTPUT);
  pinMode(motorBrake, OUTPUT);

  //set up initial output variables (note speed will wait to be set till open/close door)
  digitalWrite(motorDir,  LOW);
  digitalWrite(motorBrake, LOW);
  

}

void Door::checkCommandStatus()
{   //PRE: transmitterIn, doorState, and both motor variables are initialized
    //POST: if transmitter high, and doorState is closed, then the motor will be turned to open the door
    //      if transmitter low , and doorState is open  , then the motor will be turned to close the door
    //      this function will also increase wiggleRoom everytime the transmitterIn doesn't match the doorState
    //        variable. A fault will be reported if wiggleRoom gets to 3.

  if(digitalRead(transmitterIn) == HIGH && doorState == zero)
  { 
    printProgStr(door_open_call); 
    printTime();
    
    wiggleRoom++;                    //increase wiggleRoom since they don't match
    if (wiggleRoom >= three)
      stateConflict = true;
       
    openDoor();                     //call function to open the door
  }
  
  else if(digitalRead(transmitterIn) == LOW  && doorState == one)
  {
    printProgStr(door_close_call); 
    printTime();
    
    wiggleRoom;                     //increase wiggleRoom since they don't match
    if (wiggleRoom >= three)
      stateConflict = true;
    
    closeDoor();                    //call function to close the door
  }
  
  else 
   {
     wiggleRoom = zero;
   }
   
  faultCheck();

}

const boolean Door::getDoorState()
{//PRE: initialized
 //POST: return the boolean status of the doorState variable
 if (doorState)
   return true;
 else 
   return false;
}

const boolean Door::isThereAFault()
{//PRE: initialized
 //POST: returns true if any faults have ever been reported
 if (fault)
   return true;
 else 
   return false;
}

const int Door::getStateConflictCount()
{//PRE: initialized
 //POST: returns the number of times a stateConflict has been reported
 return this->stateConflictCount;
}

const int Door::getOpenFaultCount()
{//PRE: initialized
 //POST: returns number of times an open fault has been reported
 return this->openFaultCount;
}

const int Door::getCloseFaultCount()
{//PRE: initialized
 //POST: returns number of times a close fault has been reported
 return this->closeFaultCount;
}
 

void Door::openDoor()
{//PRE: this function should not be called if doorState == high
  //POST: the motor will be turned in one direction for 3 secs to open the door

    if (!doorState)                     //extra check to make sure the door isn't open
  {                                   //while its already open
    digitalWrite(motorDir,   HIGH);    //put motor in right direction
    digitalWrite(motorBrake, LOW);     //make sure brake is off
    analogWrite (motorSpeed, motor_open_speed_value);     //turn on motor
    
    printProgStr(door_opening);
   // lcd.print("Opening Door");
    printTime();
    
    delay(delay_time);
    
    analogWrite (motorSpeed, zero);      //stop motor
    digitalWrite(motorBrake, HIGH);
   

    doorState = one;                    //door is now open
    
    printProgStr(door_open); 
    //lcd.print("Door Open");
    printTime();
    
  }
  else{
    openFault = true;                //fault report
  }

  faultCheck();

}

void Door::closeDoor()
{//PRE: the door must be open for this function to be called
  //POST: the door will close

  if(doorState)                            //extra check to make sure closeDoor() isn't
  {                                        //called while door is close
    
    digitalWrite(motorDir, LOW);          //put motor in right direction
    digitalWrite(motorBrake, LOW);         //make sure brake is off
    digitalWrite(motorSpeed, motor_close_speed_value);          //turn motor

    printProgStr(door_closing);    //lcd.print("Closing Door");
    printTime();
    
    delay(delay_time);
    
    
    analogWrite (motorSpeed, zero);            //stop motor
    digitalWrite(motorBrake, HIGH);

    doorState = zero;                        //change the state of the door to close
    
    printProgStr(door_closed); 
    //lcd.print("Door Closed");
    printTime();
  }
  else {
    closeFault = true;
  }

  faultCheck();

}

void Door::faultCheck()
{//PRE:  private variables initialized
  //POST: faults stateConflict, openfault, closefault are checked and reported
  //      it also makes sure the fault is false after reporting it as well as 
  //      counts the amount of each fault

  if (stateConflict)
  {// Reports a state conflict
    fault = one;
    faultReport = one; 
    stateConflictCount++;
    stateConflict = false;
    printProgStr(door_fault_blank); 
    printProgStr(door_fault_state_conflict); 
    Serial.print(stateConflictCount);
    printProgStr(door_fault_six_spaces);
    printTime();
  }

  if (openFault)
  {//reports when the openDoor function has been called while doorState is true
    fault = one;
    faultReport = one;
    openFaultCount++;
    openFault = false;
    printProgStr(door_fault_blank); 
    printProgStr(door_fault_opendoor); 
    Serial.print(openFaultCount);
    printProgStr(door_fault_six_spaces);
    printTime();
  }

  if (closeFault)
  {//reports when the openDoor function has been called while doorState is true
    fault = one;
    faultReport = one;
    closeFaultCount++;
    closeFault = false;
    printProgStr(door_fault_blank);
    printProgStr(door_fault_closedoor);
    Serial.print(closeFaultCount);
    printProgStr(door_fault_six_spaces);
    printTime();
  }

  if (faultReport)
  {//POST: if there are faults led will be turned on
    //digitalWrite(ledPin, HIGH);
    //fault report
    printlnProgStr(door_fault_report);
    Serial.println(stateConflictCount);
    printProgStr(door_close_fault);
    Serial.println(closeFaultCount);
    printProgStr(door_open_fault);
    Serial.println(openFaultCount);
    printlnProgStr(door_fault_stars);

    faultReport = zero;
  }    
}

void Door::printTime()
{
  milliSeconds= millis();

  hours = milliSeconds * 2.778 * .0000001;
  if(hours < ten)
    printProgStr(time_zero);
  Serial.print(hours);
  printProgStr(time_colon);

  minutes = milliSeconds * 1.6667 * .00001;
  if(minutes > sixty)
    minutes = minutes % sixty; 
  if(minutes < ten)
    printProgStr(time_zero); 
  Serial.print(minutes);
  printProgStr(time_colon);

  seconds = milliSeconds * .001;
  if(seconds > sixty) 
    seconds = seconds % sixty;
  if(seconds < ten)
    printProgStr(time_zero);
  Serial.println(seconds);
  
  printProgStr( memory_left );
  Serial.println( freeRam() );
  
}

// given a PROGMEM string, use Serial.print() to send it out
void Door::printProgStr(const prog_uchar str[])
{
  char c;
  if(!str) return;
  while((c = pgm_read_byte(str++)))
    Serial.print(c);
}

void Door::printlnProgStr(const prog_uchar str[])
{
  char c;
  if(!str) return;
  while((c = pgm_read_byte(str++)))
    Serial.print(c);
    
  Serial.println("");
}

int Door::freeRam () 
{
  extern int __heap_start, *__brkval; 
  int v; 
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval); 
}





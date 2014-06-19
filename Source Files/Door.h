#include "Arduino.h"
#include <avr/pgmspace.h>


class Door
{
public:
  Door();
  Door (int transmitterIn, int motorSpeed, int motorDir, int motorBrake);
  void checkCommandStatus();   //checks the status of the transmitter input and changes door state
  void checkDoorStatus();      //checks to make transmitter in equals doorState and sets a fault 
  //accessors
  const boolean getDoorState();      //returns true/false depending on doorState
  const boolean isThereAFault();     //returns true if a fault has been set
  const int  getStateConflictCount();//returns number of state conflicts
  const int  getOpenFaultCount();    //returns number of open faults
  const int  getCloseFaultCount();   //returns number of close faults
private: 
  void openDoor();             //used to turn the motor to open the door, changes doorState to true
  void closeDoor();            //used to turn the motor to close the door, changes doorState to false
  void faultCheck();           //checks to see if any of the three faults were set during opening/closing door
  void printTime();            //prints the time since the arduino started
  // given a PROGMEM string, use Serial.print() to send it out
  void printProgStr(const prog_uchar str[]);
  void printlnProgStr(const prog_uchar str[]);
  int freeRam ();
  //motor state variables
  int transmitterIn;           //input coming from transmitter. decides open/close          
  int motorSpeed;               //one side of motor lead
  int motorDir;                //other side of motor lead
  int motorBrake;              //when high, the motor won't turn
  int doorState;               //keeps track of whether the door is open or closed
  //fault reporting variables
  int stateConflict;           //set true when the doorState doesn't match transmitter in
  int stateConflictCount;      //counts the amount of state conflicts
  int openFault;               //open door function called while doorState is true
  int openFaultCount;          //counts amount of times open door is called while door state is true
  int closeFault;              //closed door function called while doorState is false
  int closeFaultCount;         //counts amount of times close door is called while doorState is false
  int wiggleRoom;              //allows the doorState and transmitter to be different 3 times before reporting a fault
  int fault;                   //if true, main will use an accessor to check, then display it to the lcd
  int faultReport;             //set within faultCheck if there needs to be a fault report, turned off after it is reported in serial
  //time variables
  double milliSeconds;
  int hours;
  int minutes;
  int seconds;
  

};

/*
Fault Look Up table
Name              Conditions for being called       Description
State conflict    doorState != transmitterIn        The door state according to the class
                                                    and computer does not match the 
                                                    the transmitter in which controls if the
                                                    door should be open or closed
Open Fault        openDoor() is called while        The door is trying to be opened while 
                  doorState is true                 it is already open
Close Fault       closeDoor() called while          The door is trying to be closed while 
                  doorState is false                it is already closed
*/       
           
                  




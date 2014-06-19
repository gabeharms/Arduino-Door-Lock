#include <FastIO.h>
#include <I2CIO.h>
#include <LCD.h>
#include <LiquidCrystal.h>
#include <LiquidCrystal_I2C.h>
#include <LiquidCrystal_SR.h>
#include <LiquidCrystal_SR2W.h>
#include <LiquidCrystal_SR3W.h>

#include <avr/pgmspace.h>


const int I2C_ADDR        =  0x3F;  // Define I2C Address where the PCF8574A is
const int BACKLIGHT_PIN   =     3;
const int En_pin          =     2;
const int Rw_pin          =     1;  
const int Rs_pin          =     0;
const int D4_pin          =     4;
const int D5_pin          =     5;
const int D6_pin          =     6;
const int D7_pin          =     7;

LiquidCrystal_I2C	lcd(I2C_ADDR,En_pin,Rw_pin,Rs_pin,D4_pin,D5_pin,D6_pin,D7_pin);

int ledPin                =    13;   //internal LED output
int inputPin              =     2;   //low input here will turn LED on
int motorSpeed            =    11;   //one side of the motor
int motorDir              =    13;   //other side of motor
int motorBrake            =     8;   //brake for motor


//String Declarations
prog_uchar my_name[] PROGMEM = { "By Gabe Harms" };
prog_uchar project_name[] PROGMEM = { "Door Guard 1.1" };
prog_uchar vars_initialized[] PROGMEM = { "Green: Class Variables Initialized      " };
prog_uchar pins_initialized[] PROGMEM = { "Green: Pins intialized      " };

prog_uchar lcd_state[] PROGMEM = { "State:  " };
prog_uchar lcd_four_spaces[] PROGMEM = { "    " };
prog_uchar lcd_five_spaces[] PROGMEM = { "     " };
prog_uchar lcd_open[] PROGMEM = { "Open:   " };
prog_uchar lcd_close[] PROGMEM = { "Close:  " };
prog_uchar lcd_fault_report[] PROGMEM = { "Fault Report  " };

prog_uchar lcd_state_open[] PROGMEM = { "Door is Open   " };
prog_uchar lcd_state_close[] PROGMEM = { "Door is Closed " };
prog_uchar lcd_state_comein[] PROGMEM = { "Come In       " };
prog_uchar lcd_state_stayout[] PROGMEM = { "Stay Out      " };




/*

 Created: 07/10/13
 Author:  Gabe Harms
 Project: Door Lock
 Version: 1.0
 Description: this circuit uses a motor shield and a 16x2 lcd display
 This circuit has a motorspeed pin which you use analogWrite on and choose
 a speed between 0 and 255. The direction and brake pin use digitalWrite. 
 
 It switches the motor on and off using an single input from a wireless transmitter. This input 
 is checked by the Door class. This class turns on and off the motor to switch the 
 door's state.  
*/

/*
Update: Version 1.0.1
Description: 1. Added constant to the member functions that do not alter any variables in the Door class
             2. Removed checkDoorStatus member function from the door class and added its functionality to checkCommandStatus
             3. Removed conditional statement for lcd computing in the loop() function
             4. added the lcdPrintOut() function under the loop() function which now handles all the lcd printing computing and conditional statements
             5. removed all global variables and constants
             6. added a constants.h file which to which all global variable declarations and constants were placed
*/


#include "Door.h"
#include "Arduino.h"
#include <Wire.h>
#include <LCD.h>            //contains all constants and pin declarations and roomDoor initialization


Door roomDoor(inputPin, motorSpeed, motorDir, motorBrake); //initialize door variable


void setup() {

  Serial.begin(9600);
  
  printlnProgStr(vars_initialized);
  printlnProgStr(pins_initialized);
  
  lcd.begin (16,2);
  lcd.setBacklightPin(BACKLIGHT_PIN,POSITIVE);
  lcd.setBacklight(HIGH);        //turn on backlight
  lcdprint(project_name);  
  lcd.setCursor ( 0, 1 );        // go to the 2nd line
  lcdprint(my_name);

}

void loop()
{
  roomDoor.checkCommandStatus();      //checks the transmitter input     
  lcdPrintOut();                      //handles all lcd print computing
}



void lcdPrintOut()
{//POST: Will decide if both printFaultReport and lcdPrintDoorState or
 //      if just lcdPrintDoorState needs to be called
  if (roomDoor.isThereAFault())
  {  
    lcdPrintFaultReport();
    delay(1000);
    lcdPrintDoorState();
  }
  else
    lcdPrintDoorState();
}

void lcdPrintFaultReport()
{//POST: Prints all faults that have been reported on the lcd
  lcd.setCursor(0,0);
  lcdprint(lcd_fault_report);
  
  lcd.setBacklight(LOW);           //flash lcd
  delay(500);
  lcd.setBacklight(HIGH);
  
  lcd.setCursor(0,1);
  lcdprint(lcd_state);
  lcd.print(roomDoor.getStateConflictCount());
  lcdprint(lcd_five_spaces);
  delay(1000);
  lcd.setCursor(0,1);
  lcdprint(lcd_open);
  lcd.print(roomDoor.getOpenFaultCount());
  lcdprint(lcd_four_spaces);
  delay(1000);
  lcd.setCursor(0,1);
  lcdprint(lcd_close);
  lcd.print(roomDoor.getCloseFaultCount());
  lcdprint(lcd_four_spaces);
  delay(1000);
}
  
void lcdPrintDoorState()
{//POST: prints door open/closed on the lcd
  if (roomDoor.getDoorState())
  {
    lcd.setCursor(0,0);
    lcdprint(lcd_state_open);
    lcd.setCursor(0,1);
    lcdprint(lcd_state_comein);
  }
  else
  {
    lcd.setCursor(0,0);
    lcdprint(lcd_state_close);
    lcd.setCursor(0,1);
    lcdprint(lcd_state_stayout);
  }
}

// given a PROGMEM string, use Serial.print() to send it out
void printProgStr(const prog_uchar str[])
{
  char c;
  if(!str) return;
  while((c = pgm_read_byte(str++)))
    Serial.print(c);
}

void printlnProgStr(const prog_uchar str[])
{
  char c;
  if(!str) return;
  while((c = pgm_read_byte(str++)))
    Serial.print(c);
    
  Serial.println("");
}

void lcdprint(const prog_uchar str[])
{
  char c;
  if(!str) return;
  while((c = pgm_read_byte(str++)))
    lcd.print(c);
}




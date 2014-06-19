Arduino_Door_Lock
=================

All files in the Source Files directory, in addition to Door_Lock.ino were written by Gabe Harms.

This program was used in order to lock and unlock my room door wirelessly. A wireless reciever sends commands to
the arduino, which then sends commands to the motor controlling the door lock.

The Door class was written in order to simplify the .ino file. It contains many private and public methods which 
allow the arduino program to simply check for commands from the wireless reciever and control the motor all in one
function call. See the Door Class Methods file for a full description of the class


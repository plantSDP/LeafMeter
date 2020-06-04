/*
  Written in C++ but does not make large use of OOP functionallity. 
  This code is intended to be used with the Arduino IDE in conjunction with the file 'RemoteControllerMain.ino'
  
  
  Written by CO2 leaf meter
*/


#ifndef CLIENT_FSM
#define CLIENT_FSM 

/*******************************************************************************
 * PUBLIC #INCLUDES                                                            *
 ******************************************************************************/


#include "Arduino.h"   //This gives access to a variety of functions and #defines that ease development. (Part of the native arduino toolchain)



/*******************************************************************************
 * PUBLIC FUNCTION PROTOTYPES                                                  *
 ******************************************************************************/

// This function runs the state machine with an INIT_EVENT to initialize the state machine. It should only be called once. When called the state machine is set to Idle
uint8_t Init_ClientFSM(void); 


//This function runs a state machine that operates on events related to the stateful protocol between the field unit and the remote controller. It handles all 
//functions related to this protocol including transmission, retransmission, reception 
Event Run_ClientFSM(Event thisEvent);

#endif /* CLIENT_FSM_H */
#ifndef SUB_HSM_INIT_H
#define SUB_HSM_INIT_H  

/*******************************************************************************
 * PUBLIC #INCLUDES                                                            *
 ******************************************************************************/

#include "Configure.h"   // defines events
#include "EventCheckers.h"


/*******************************************************************************
 * PUBLIC FUNCTION PROTOTYPES                                                  *
 ******************************************************************************/

/*
This function initializes the state machine with an INIT_EVENT. 
In regards to the state machine, it transitions the machine out of the initial pseudostate and performs one-time setup functions

Parameters: uint8_t resetFlag, 
            if true: current state will be reset to the starting pseudostate, 
            if false, current state is unchanged. Used to restart system after numCycles depletes.
Return: TRUE on success, FALSE on failure
*/
uint8_t Init_SubHSM_Init(uint8_t resetFlag);

/* 
Contains a nested switch-case that implements the state machine.

Parameters: Event thisEvent - a struct that contains an EventType and EventParameter, defined in Configure.h
Return: NO_EVENT if thisEvent is handled/consumed, return thisEvent unchanged if thisEvent not consumed 
*/
Event Run_SubHSM_Init(Event thisEvent);

#endif /* SUB_HSM_INIT_H */
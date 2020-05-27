#ifndef SUB_HSM_LEAK_H
#define SUB_HSM_LEAK_H  

/*******************************************************************************
 * PUBLIC #INCLUDES                                                            *
 ******************************************************************************/

#include "Configure.h"   // defines architecture
#include "Arduino.h"


/*******************************************************************************
 * PUBLIC FUNCTION PROTOTYPES                                                  *
 ******************************************************************************/

/*
This function initializes the state machine with an INIT_EVENT. 
In regards to the state machine, it transitions the machine out of the initial pseudostate and performs one-time setup functions

Parameters: none
Return: TRUE on success, FALSE on failure
*/
uint8_t Init_SubHSM_Leak(void);

/*
This function contains the nested switch statement that implements the state machine.
It requires an Event as a parameter.
If the event is consumed/handled in the state machine, then the function returns NO_EVENT
If the event is not consumed/handled, then the event is returned unchanged.
*/
Event Run_SubHSM_Leak(Event thisEvent);

#endif /* SUB_HSM_LEAK_H */
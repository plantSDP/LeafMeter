#ifndef SUB_HSM_ACTIVE_H
#define SUB_HSM_ACTIVE_H  

/*******************************************************************************
 * PUBLIC #INCLUDES                                                            *
 ******************************************************************************/

#include "Configure.h"   // defines architecture
#include "EventCheckers.h"


/*******************************************************************************
 * PUBLIC FUNCTION PROTOTYPES                                                  *
 ******************************************************************************/

// Resets the static CurrentState to InitPState, allowing the sub statemachine to start at the first state instead of a previously saved one 
void Reset_SubHSM_Active(void);

/*
This function initializes the state machine with an INIT_EVENT. 
In regards to the state machine, it transitions the machine out of the initial pseudostate and performs one-time setup functions

Parameters: uint8_t resetFlag, if true: current state will be reset to the starting pseudostate, if false, current state is unchanged. Used to start a new waiting cycle
Return: TRUE on success, FALSE on failure
*/
uint8_t Init_SubHSM_Active(uint8_t resetFlag);

/*
This function contains the nested switch statement that implements the state machine.
It requires an Event as a parameter.
If the event is consumed/handled in the state machine, then the function returns NO_EVENT
If the event is not consumed/handled, then the event is returned unchanged.
*/
Event Run_SubHSM_Active(Event thisEvent);

#endif /* SUB_HSM_ACTIVE_H */

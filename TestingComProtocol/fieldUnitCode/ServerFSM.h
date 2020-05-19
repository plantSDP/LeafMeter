#ifndef SERVER_FSM
#define SERVER_FSM 

/*******************************************************************************
 * PUBLIC #INCLUDES                                                            *
 ******************************************************************************/

#include "ServerConfigure.h"   // defines events
#include "Arduino.h"


/*******************************************************************************
 * PUBLIC FUNCTION PROTOTYPES                                                  *
 ******************************************************************************/

uint8_t Init_ServerFSM(void);

Event Run_ServerFSM(Event thisEvent);

#endif /* SERVER_FSM_H */
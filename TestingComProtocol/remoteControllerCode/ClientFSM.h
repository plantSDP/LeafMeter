#ifndef CLIENT_FSM
#define CLIENT_FSM 

/*******************************************************************************
 * PUBLIC #INCLUDES                                                            *
 ******************************************************************************/

#include "ClientConfigure.h"   // defines events
#include "Arduino.h"


/*******************************************************************************
 * PUBLIC FUNCTION PROTOTYPES                                                  *
 ******************************************************************************/

uint8_t Init_ClientFSM(void);

Event Run_ClientFSM(Event thisEvent);

#endif /* CLIENT_FSM_H */
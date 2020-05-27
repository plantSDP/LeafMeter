#ifndef SUB_HSM_WAIT_H
#define SUB_HSM_WAIT_H  

/*******************************************************************************
 * PUBLIC #INCLUDES                                                            *
 ******************************************************************************/

#include "Configure.h"   // defines events
#include "Arduino.h"


/*******************************************************************************
 * PUBLIC FUNCTION PROTOTYPES                                                  *
 ******************************************************************************/

uint8_t Init_SubHSM_Wait(uint8_t resetFlag);

Event Run_SubHSM_Wait(Event thisEvent);

#endif /* SUB_HSM_Wait_H */

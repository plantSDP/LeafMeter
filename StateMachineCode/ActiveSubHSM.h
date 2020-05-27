#ifndef SUB_HSM_ACTIVE_H
#define SUB_HSM_ACTIVE_H  

/*******************************************************************************
 * PUBLIC #INCLUDES                                                            *
 ******************************************************************************/

#include "Configure.h"   // defines architecture
#include "Arduino.h"


/*******************************************************************************
 * PUBLIC FUNCTION PROTOTYPES                                                  *
 ******************************************************************************/

uint8_t Init_SubHSM_Active(uint8_t resetFlag);

Event Run_SubHSM_Active(Event thisEvent);

#endif /* SUB_HSM_ACTIVE_H */

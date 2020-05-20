#ifndef SUB_HSM_ACTIVE_H
#define SUB_HSM_ACTIVE_H  

/*******************************************************************************
 * PUBLIC #INCLUDES                                                            *
 ******************************************************************************/

#include "Configure.h"   // defines events
#include "Arduino.h"


/*******************************************************************************
 * PUBLIC FUNCTION PROTOTYPES                                                  *
 ******************************************************************************/

uint8_t Init_SubHSM_Active(void);

Event Run_SubHSM_Active(Event thisEvent);

#endif /* SUB_HSM_ACTIVE_H */
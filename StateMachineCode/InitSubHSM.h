#ifndef SUB_HSM_INIT_H
#define SUB_HSM_INIT_H  

/*******************************************************************************
 * PUBLIC #INCLUDES                                                            *
 ******************************************************************************/

#include "Configure.h"   // defines events
#include "Arduino.h"


/*******************************************************************************
 * PUBLIC FUNCTION PROTOTYPES                                                  *
 ******************************************************************************/

uint8_t Init_SubHSM_Init(void);

Event Run_SubHSM_Init(Event thisEvent);

#endif /* SUB_HSM_INIT_H */
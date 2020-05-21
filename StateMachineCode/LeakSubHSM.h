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

uint8_t Init_SubHSM_Leak(void);

Event Run_SubHSM_Leak(Event thisEvent);

#endif /* SUB_HSM_LEAK_H */
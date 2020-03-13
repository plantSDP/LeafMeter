#ifndef SUB_HSM_RF_H
#define SUB_HSM_RF_H  

/*******************************************************************************
 * PUBLIC #INCLUDES                                                            *
 ******************************************************************************/

#include "Configure.h"   // defines events
#include "Arduino.h"


/*******************************************************************************
 * PUBLIC FUNCTION PROTOTYPES                                                  *
 ******************************************************************************/

uint8_t Init_SubHSM_RF(void);

Event Run_SubHSM_RF(Event thisEvent);

#endif /* SUB_HSM_RF_H */

#ifndef SUB_HSM_DATETIME_H
#define SUB_HSM_DATETIME_H  

/*******************************************************************************
 * PUBLIC #INCLUDES                                                            *
 ******************************************************************************/

#include "Configure.h"   // defines events
#include "Arduino.h"


/*******************************************************************************
 * PUBLIC FUNCTION PROTOTYPES                                                  *
 ******************************************************************************/

uint8_t Init_SubHSM_DateTime(void);

Event Run_SubHSM_DateTime(Event thisEvent);



#endif /* SUB_HSM_DATETIME_H */

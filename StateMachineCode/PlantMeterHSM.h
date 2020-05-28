#ifndef PLANT_HSM
#define PLANT_HSM

#include "Configure.h"
#include "EventCheckers.h"

// Hierarchical State Machine Architecture Includes
#include "InitSubHSM.h"
#include "LeakSubHSM.h"
#include "RFSubHSM.h"
#include "DateTimeSubHSM.h"
#include "ActiveSubHSM.h"
#include "WaitSubHSM.h"

/*
* Used to initialize the HSM by running it with an INIT_EVENT 
* This function should be called at the startup of the system and is used to perform one-time setup functionality
*/
uint8_t InitHSM(void);


/*
* Function that implements the HSM. 
* Events should be passed to this function in the parameter
* The events will be run through the subHSM and only passed back up if they 
* are not handled in the lower states.
*/
Event RunHSM(Event thisEvent);


#endif

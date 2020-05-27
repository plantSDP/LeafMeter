#ifndef EVENTCHECK_H
#define EVENTCHECK_H
#include "RemoteConfigure.h"
#include "Arduino.h"


// Public Definitions






/*
Sets the interval for a timer. The timer will only generate a single event. 
A timer that has not expired yet cannot be set again. 
Parameters
	timer - the number of the timer to set 
	interval - time in ms for the interval
	
Return - one or zero based on success
*/
uint8_t SetTimer(uint8_t timer, unsigned int interval);





/*
Checks for timer expirations and triggers an event if a timer has expired. Multiple timer events cannot be triggered on the same check. 
*/
Event TimerExpireCheck(void);



/*
Checks for if a new payload is available from the RF module.
*/
Event RfMessageCheck(void);



/*
Checks for if a new message is available from the BLE module. If yes, the message and length are stored in the global variables BLEMessage and BLEMessage_length;
*/
Event BLEMessageCheck(void);


#endif

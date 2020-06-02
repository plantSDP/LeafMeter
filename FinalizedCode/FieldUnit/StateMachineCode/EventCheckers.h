#ifndef EVENTCHECK_H
#define EVENTCHECK_H
#include "Configure.h"
#include "Arduino.h"


// Public Definitions


/*
typedef struct ButtonEvent{
	uint8_t ButtonFlag;
	uint16_t ButtonParams; //bitmasked button states (bit0 = btn1, bit1 = btn2...)
}ButtonEvent;
*/

/*
Initializes the event checkers. 
*/
uint8_t InitEventCheckers(void);


/*
debounces the buttons and set the flag in the struct, along with the bitmasked button states
*/
Event ButtonCheckDebounce(void);


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





#endif

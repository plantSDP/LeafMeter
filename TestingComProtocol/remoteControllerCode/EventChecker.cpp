#include "ClientConfigure.h"
#include "Arduino.h"
#include "EventChecker.h"
#include <Metro.h> //this library is used to create timers


static uint8_t timer0state = 0; //holder for if the timers are currently active
static uint8_t timer1state = 0;
static uint8_t timer2state = 0;
static Metro timer0 = Metro(1000);
static Metro timer1 = Metro(1000); 
static Metro timer2 = Metro(1000);


/*
Sets the interval for a timer. The timer will only generate a single event. 
A timer that has not expired yet cannot be set again. 
Parameters
	timer - the number of the timer to set 
	interval - time in ms for the interval
	
Return - one or zero based on success
*/
uint8_t SetTimer(uint8_t timer, unsigned int interval){
	
	
	if (timer == 0){
		if (timer0state == 0){
			//start the interval
			timer0.interval(interval); 
			timer0.reset();
			timer0state = 1;
			return 1;
		}
	} else if (timer == 1){
		if (timer1state == 0){
			//start the interval
			timer1.interval(interval); 
			timer1.reset();
			timer1state = 1;
			return 1;
		}
	} else if (timer == 2){
		if (timer2state == 0){
			//start the interval
			timer2.interval(interval); 
			timer2.reset();
			timer2state = 1;
			return 1;
		}
	}
	
	return 0;
}



/*
Checks for timer expirations and triggers an event if a timer has expired. Multiple timer events cannot be triggered on the same check. 
*/
Event TimerExpireCheck(void){
	Event returnEvent;
	if (timer0state == 1){
		if (timer0.check() == 1){
			timer0state = 0;
			returnEvent.EventType = TIMEOUT;
			returnEvent.EventParam = 0b1;
			return returnEvent;
		}
	} 
	
	
	if (timer1state == 1){
		if (timer1.check() == 1){
			timer1state = 0;
			returnEvent.EventType = TIMEOUT;
			returnEvent.EventParam = 0b10;
			return returnEvent;
		}
	} 
	
	if (timer2state == 1){
		if (timer2.check() == 1){
			timer2state = 0;
			returnEvent.EventType = TIMEOUT;
			returnEvent.EventParam = 0b100;
			return returnEvent;
		}
	} 
	
	returnEvent.EventType = NO_EVENT;
	return returnEvent;
}


/*
Checks for if a new payload is available from the RF module.
*/
Event RfMessageCheck(void){
	static uint8_t previousCheck = FALSE;
	uint8_t currentCheck = FALSE;
	Event returnEvent;
	returnEvent.EventType = NO_EVENT;
	
	if (clientInfo.transciever_state == RECIEVING){
		if (rf95.available()){
			currentCheck = TRUE;
		}else{
			currentCheck = FALSE;
		}
		if (currentCheck == TRUE && previousCheck == FALSE){
			returnEvent.EventType = RF_RECIEVE_EVENT;
			returnEvent.EventParam = 0;
		}	
	}
		
	previousCheck = currentCheck;
	return returnEvent;
		
}




			










#include "Configure.h"
#include "Arduino.h"
#include "EventCheckers.h"
#include <Metro.h> // this library is used to create timers

//=====================================================================================
// EventCheckers generates button and timeout Events based on button and timer statuses
//=====================================================================================

// Private Definitions
#define BTN4_PIN 17
#define BTN3_PIN 16
#define BTN2_PIN 15 // CHANGE THIS LATER!
#define BTN1_PIN 14 // CHANGE THIS LATER!

static uint16_t oldButtonStates = 0;	// Holds the previous button states. Default is 0, or no press.
static uint8_t buttonVals[4];			// Button debouncing array, each array address holds the previous 8 button hardware pin reads

// active timer flags
static uint8_t timer0state = 0; 	
static uint8_t timer1state = 0;
static uint8_t timer2state = 0;

static Metro timer0 = Metro(1000);		// default timer tic at 1ms
static Metro timer1 = Metro(1000); 
static Metro timer2 = Metro(1000);



/*
Initializes the event checkers - sets button pinouts

Returns TRUE
*/
uint8_t InitEventCheckers(void){
	pinMode(BTN1_PIN, INPUT);
	pinMode(BTN2_PIN, INPUT);
	pinMode(BTN3_PIN, INPUT);
	pinMode(BTN4_PIN, INPUT);
	
	return TRUE;
}


/*
Debounces the buttons and generates the corresponding EventType, along with the bitmasked button states via EventParam

Return - Event with EventType BTN_EVENT and corresponding EventParam on button press or release
*/
Event ButtonCheckDebounce(void){
	// this 16-bit int holds the new event parameter, for the BTN_EVENT this is the state of the four buttons
	uint16_t newButtonStates = 0;
	
	// Get new button state, discard oldest button state by shifting, store in buttonVals array
	// each array address holds 8 previous button states for a single button via an 8-bit integer
	
	// add other buttons here
	buttonVals[3] = buttonVals[3] << 1 | digitalRead(BTN4_PIN);		// BTN4
	buttonVals[2] = buttonVals[2] << 1 | digitalRead(BTN3_PIN);		// BTN3
	buttonVals[1] = buttonVals[1] << 1 | digitalRead(BTN2_PIN);		// BTN2
	buttonVals[0] = buttonVals[0] << 1 | digitalRead(BTN1_PIN);		// BTN1
	
	
	// Here the 16-bit integer newButtonStates is formed. 
	// If a button state persists for 8 or more times, the button state is allowed to change.
	// The least significant bits correspond to each button: 1 is button pressed, 0 is button depressed
	/* 
	With four buttons, the four least significant bits look like this:
	Button:   [BTN4] [BTN3] [BTN2] [BTN1]
	Bit:	 	3	   2	  1		 0
	*/
	
	// add more buttons buttons here
	
	if (buttonVals[3] == 0x00){
		newButtonStates &= 0b0111;
	} else if (buttonVals[3] == 0xFF){
		newButtonStates |= 0b1000;
	}
	
	if (buttonVals[2] == 0x00){
		newButtonStates &= 0b1011;
	} else if (buttonVals[2] == 0xFF){
		newButtonStates |= 0b0100;
	}
	
	if (buttonVals[1] == 0x00){
		newButtonStates &= 0b1101;
	} else if (buttonVals[1] == 0xFF){
		newButtonStates |= 0b0010;
	}
	
	if (buttonVals[0] == 0x00){
		newButtonStates &= 0b1110;
	} else if (buttonVals[0] == 0xFF){
		newButtonStates |= 0b0001;
	}
	
	
	// If there is any change to button states, return EventType BTN_EVENT & set EventParam to newButtonStates
	// If there is no change, return EventType NO_EVENT & set EventPram to oldButtonStates (default 0).
	Event returnBtnEvent;
	if (oldButtonStates != newButtonStates){
		returnBtnEvent.EventType = BTN_EVENT;
		returnBtnEvent.EventParam = newButtonStates;
	} else {
		returnBtnEvent.EventType = NO_EVENT;
		returnBtnEvent.EventParam = oldButtonStates;
	}
	oldButtonStates = newButtonStates;
	
	return returnBtnEvent;
}




/*
Sets the interval for a timer. The timer will only generate a single event. 
A timer that has not expired yet cannot be set again. 
Parameters
	timer - the number of the timer to set 
	interval - time in ms for the interval
	
Return - 1 or 0 based on success
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




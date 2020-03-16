#include "Configure.h"
#include "Arduino.h"
#include "EventCheckers.h"
#include <Metro.h> //this library is used to create timers

// Private Definitions
#define BTN4_PIN 17
#define BTN3_PIN 16
#define BTN2_PIN 15 //CHANGE THIS!
#define BTN1_PIN 14 //CHANGE THIS!

static uint8_t buttonVals[4];
static uint8_t timer0state = 0; //holder for if the timers are currently active
static uint8_t timer1state = 0;
static Metro timer0 = Metro(1000);
static Metro timer1 = Metro(1000); 



/*
Initializes the event checkers. 
*/
uint8_t InitEventCheckers(void){
	pinMode(BTN1_PIN, INPUT);
	pinMode(BTN2_PIN, INPUT);
	pinMode(BTN3_PIN, INPUT);
	pinMode(BTN4_PIN, INPUT);
	
	return 1;
}


/*
debounces the buttons and set the flag in the struct, along with the bitmasked button states
*/
Event ButtonCheckDebounce(void){
	
	static uint16_t oldButtonStates = 0;
	uint16_t newButtonStates = 0;
	
	buttonVals[3] = buttonVals[3] << 1 | digitalRead(BTN4_PIN);
	buttonVals[2] = buttonVals[2] << 1 | digitalRead(BTN3_PIN);
	buttonVals[1] = buttonVals[1] << 1 | digitalRead(BTN2_PIN);
	buttonVals[0] = buttonVals[0] << 1 | digitalRead(BTN1_PIN);
	//add other buttons here 
	
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
	
	//add other buttons here
	
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
	
	returnEvent.EventType = NO_EVENT;
	return returnEvent;
}




#include "RemoteConfigure.h"
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


/*
Checks for if a new message is available from the BLE module. If yes, the message and length are stored in the global variables BLEMessage and BLEMessage_length;
*/
Event BLEMessageCheck(void){
	
	uint8_t currentCheck = FALSE;
	Event returnEvent;
	returnEvent.EventType = NO_EVENT;
	
	uint8_t messageID; //used to hold the first byte stored in the UART buffer. This will be used to determine if an entire message has been retrieved
	
	
	uint8_t numBytes = Serial1.available(); //determine if there is data in the buffer. If yes, look at the first byte to determine the messageID
	if(numBytes > 0){
		messageID = Serial1.peek(); //this does not remove the first byte from the software buffer
		
		if (messageID == CONFIGURE_DATE_TIME){ //check all message IDs and see if a full message had been sent based on the ID. If so, put the message in a global array 
			if (numBytes >= 6){

				Serial1.readBytes(BLEMessage, 6);
				BLEMessage_length = 6;
				currentCheck = TRUE;
			}
		} else if (messageID == MEASUREMENT_REQUEST){
			if (numBytes >= 2){
				
				Serial1.readBytes(BLEMessage, 2);
				BLEMessage_length = 2;
				currentCheck = TRUE;
			}
		}else if (messageID == CONFIGURE_TOTAL_MEASUREMENTS){
			if (numBytes >= 2){
	
				Serial1.readBytes(BLEMessage, 2);
				BLEMessage_length = 2;
				currentCheck = TRUE;
			}
		}else if (messageID == CONFIGURE_CYCLE_PERIOD){
			if (numBytes >= 3){
				
				Serial1.readBytes(BLEMessage, 3);
				BLEMessage_length = 3;
				currentCheck = TRUE;
			}
		}else if (messageID == POWER_OFF_REQUEST){
			if (numBytes >= 3){
				
				Serial1.readBytes(BLEMessage, 3);
				BLEMessage_length = 3;
				currentCheck = TRUE;
			}
		}else if (messageID == UPDATE_STATUS_REQUEST){
			if (numBytes >= 1){
				
				Serial1.readBytes(BLEMessage, 1);
				BLEMessage_length = 1;
				currentCheck = TRUE;
			}
		}else if (messageID == DATA_TRANSMISSION_REQUEST){
			if (numBytes >= 2){
				
				Serial1.readBytes(BLEMessage, 2);
				BLEMessage_length = 2;
				currentCheck = TRUE;
			}
		}
	}
	
	
	
	if (currentCheck == TRUE){
		returnEvent.EventType = BLE_RECIEVE_EVENT;
		returnEvent.EventParam = 0;
	}	
	
	return returnEvent;
		
}



			










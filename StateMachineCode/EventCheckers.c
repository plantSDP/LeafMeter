#include "Configure.h"
#include "Arduino.h"
#include "EventCheckers.h"


#define BTN4_PIN 17
#define BTN3_PIN 16
#define BTN2_PIN 16 //CHANGE THIS!
#define BTN1_PIN 16 //CHANGE THIS!


static uint8_t buttonVals[4];


/*
Initializes the event checkers. 
*/
uint8_t InitEventCheckers(void){
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
	}else if (buttonVals[3] == 0xFF){
		newButtonStates |= 0b1000;
	}
	
	if (buttonVals[2] == 0x00){
		newButtonStates &= 0b1011;
	}else if (buttonVals[2] == 0xFF){
		newButtonStates |= 0b0100;
	}
	
	if (buttonVals[1] == 0x00){
		newButtonStates &= 0b1101;
	}else if (buttonVals[1] == 0xFF){
		newButtonStates |= 0b0010;
	}
	
	
	if (buttonVals[0] == 0x00){
		newButtonStates &= 0b1110;
	}else if (buttonVals[0] == 0xFF){
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
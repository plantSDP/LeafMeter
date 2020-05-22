// Our Architecture Includes
#include "Configure.h"

#include "PlantMeterHSM.h"

#include "InitSubHSM.h"
#include "LeakSubHSM.h"
#include "RFSubHSM.h"
#include "DateTimeSubHSM.h"
#include "ActiveSubHSM.h"
#include "WaitSubHSM.h"

// LCD screen include
#include <LiquidCrystal_I2C.h>

//====================================================================================================
// PlantMeterHSM implements the top-level HSM for the field unit via two functions: InitHSM and RunHSM
//====================================================================================================

//===================================================
// Private definitions
//===================================================

#define TIMER_ACTIVE_DURATION 0				// Timer 0 is used as the active duration timer
#define TIMER_ACTIVE_DURATION_PARAM 0b1

#define TIMER_WAIT_DURATION 0				// Timer 0 is used as the wait duration timer
#define TIMER_WAIT_DURATION_PARAM 0b1		

// This enum lists the names of all states in this state machine
typedef enum {
	InitPState,		// initilizing state machine pseudo state for one-time startup functions
    Initing,		// initializing the device
    LeakChecking,	// checking for a leak
	RFChecking,		// checking for RF connection
	DateTime,		// entering date and time
	Active,			// running a measurment
	Waiting,		// waiting for next measurement or user input
} HSMstates;

static HSMstates CurrentState = InitPState;		// holds the current state, default is the initial pseudostate



//===================================================
// Global Functions
//===================================================

/*
This function initializes the state machine by running it with an INIT_EVENT. 
In regards to the state machine, it transitions the machine out of the initial pseudostate and performs one-time setup functions

Parameters: none
Return: TRUE on success, FALSE on failure
*/
uint8_t InitHSM(void){
	Event thisEvent;
	thisEvent.EventType = INIT_EVENT;
	thisEvent.EventParam = 0;
	Event returnEvent = RunHSM(thisEvent);
	if (returnEvent.EventType == NO_EVENT) {
		return TRUE;
	} else {
		return FALSE;
	}
}

/* 
Contains a nested switch-case that implements the top-level state machine.

Parameters: Event thisEvent - a struct that contains an EventType and EventParameter, defined in Configure.h
Return: NO_EVENT if thisEvent is handled/consumed, or thisEvent unchanged if not consumed/handled
*/
Event RunHSM(Event thisEvent){
	uint8_t makeTransition = FALSE;			// state transition flag
	HSMstates nextState;					// holds the next state to be transitioned to
	
	switch (CurrentState){
		case InitPState:
			if (thisEvent.EventType == INIT_EVENT){
				// any initializations that should occur once on startup. This code could also go in the main function
				//Init_SubHSM_Init();
				//Init_SubHSM_Leak();
				//Init_SubHSM_RF();
				//Init_SubHSM_DateTime();
				//Init_SubHSM_Active();
				//Init_SubHSM_Wait();
				nextState = Initing;
				makeTransition = TRUE;	
			}
			break;

		case Initing:
			thisEvent = Run_SubHSM_Init(thisEvent);	// Always send events to sub-statemachine first
			
			switch (thisEvent.EventType) {
				case ENTRY_EVENT:
					Init_SubHSM_Init();					// one-time sub-state init call
					thisEvent.EventType = NO_EVENT;		// Entry event consumed
					break;
				case BTN_EVENT:
					if (thisEvent.EventParam == BTN3) {
						nextState = LeakChecking;
						makeTransition = TRUE;
					}
					break;
				default:
					break;
			}
			break;

		case LeakChecking:
			//pinMode(13, OUTPUT);
			//digitalWrite(13, HIGH); // Teensy LED test
			
			thisEvent = Run_SubHSM_Leak(thisEvent); // Always send events to sub-statemachine first 

			switch (thisEvent.EventType) {
				case ENTRY_EVENT:
					Init_SubHSM_Leak();					// one-time sub-state init call
					thisEvent.EventType = NO_EVENT;
					break;
				case BTN_EVENT:
					// continue to dateTime if RFoption disabled
					if ((thisEvent.EventParam == BTN3) && (rfOption == RF_NO)){
						nextState = DateTime;
						makeTransition = TRUE;
					
					// continue with RF diagnostic if RFoption is enabled
					} else if ((thisEvent.EventParam == BTN3) && (rfOption == RF_YES)){	
						nextState = RFChecking;
						makeTransition = TRUE;
					
					// return to previous state
					} else if (thisEvent.EventParam == BTN4){	
						nextState = Initing;
						makeTransition = TRUE;
					}
					break;
				default:
					break;
			}
			break;

		case RFChecking:
			thisEvent = Run_SubHSM_RF(thisEvent); // Always send events to sub-statemachine first 
			switch (thisEvent.EventType) {
				case ENTRY_EVENT:
					Init_SubHSM_RF();					// one-time sub-state init call
					thisEvent.EventType = NO_EVENT;
					break;
				case BTN_EVENT:
					// continue to next state, DateTime
					if (thisEvent.EventParam == BTN3){
						nextState = DateTime;
						makeTransition = TRUE;
						
					// return to previous state, LeakChecking
					} else if (thisEvent.EventParam == BTN4){
						nextState = LeakChecking;
						makeTransition = TRUE;
					}
					break;
				default:
					break;
			}
			break;

		case DateTime:
			thisEvent = Run_SubHSM_DateTime(thisEvent); // Always send events to sub-statemachine first 
			
			switch (thisEvent.EventType) {
				case ENTRY_EVENT:
					Init_SubHSM_DateTime();				// one-time sub-state init call
					thisEvent.EventType = NO_EVENT;
					break;
				case BTN_EVENT:
					// continue to next state, active
					if (thisEvent.EventParam == BTN3){
						nextState = Active;
						makeTransition = TRUE;
						
					// return to previous state if RFoption = no, leak checking
					} else if ((thisEvent.EventParam == BTN4) && (rfOption == RF_NO)){
						nextState = LeakChecking;
						makeTransition = TRUE;
						
					// return to previous state if RFoption = yes, RF checking
					} else if ((thisEvent.EventParam == BTN4) && (rfOption == RF_YES)){
						nextState = RFChecking;
						makeTransition = TRUE;
					}
					break;
				default:
					break;
			}
			break;

		case Active:
			thisEvent = Run_SubHSM_Active(thisEvent); // Always send events to sub-statemachine first 
		
			switch(thisEvent.EventType) {
				// On entry, init active duration timer, display message
				case ENTRY_EVENT:
					Init_SubHSM_Active();				// one-time sub-state init call

					SetTimer(0, ACTIVE_DURATION);
					
					sprintf(myString, "MEAS IN PROG    ");
					lcd.setCursor(0, 0); // set the cursor to column 0, line 0
					lcd.print(myString);  // Print a message to the LCD
					sprintf(myString, "BTN3 TO CANCEL  ");
					lcd.setCursor(0, 1); // set the cursor to column 0, line 1
					lcd.print(myString);  // Print a message to the LCD

					thisEvent.EventType = NO_EVENT;		// Entry event consumed
					break;
					
				// On active duration timeout, transition to waiting
				case TIMEOUT:
					if (thisEvent.EventParam == TIMER_0_PARAM) {
						Init_SubHSM_Wait();
						nextState = Waiting;
						makeTransition = TRUE;
					}
					break;
					
				case BTN_EVENT:
					if (thisEvent.EventParam == BTN3) {
						nextState = Waiting;
						makeTransition = TRUE;
					}
					break;
					
				default:
					break;
			}
			break;

		case Waiting:
			thisEvent = Run_SubHSM_Wait(thisEvent); // Runs the sub-state machine for Waiting
			
			switch(thisEvent.EventType) {
				// On entry, init wait duration timer, display message
				case ENTRY_EVENT:
					Init_SubHSM_Wait();				// one-time sub-state init call

					SetTimer(TIMER_WAIT_DURATION, period*60000);	// init wait duration timer with period. Must convert from [min] to [ms]
					
					sprintf(myString, "WAITING         ");
					lcd.setCursor(0, 0);  // set the cursor to column 0, line 0
					lcd.print(myString);  // Print a message to the LCD
					sprintf(myString, "                ");
					lcd.setCursor(0, 1);  // set the cursor to column 0, line 1
					lcd.print(myString);  // Print a message to the LCD

					thisEvent.EventType = NO_EVENT;
					break;
					
				// On wait duration timeout, transition to active
				case TIMEOUT:
					if (thisEvent.EventParam == TIMER_WAIT_DURATION_PARAM) {
						Init_SubHSM_Active();
						nextState = Active;
						makeTransition = TRUE;
					}

					break;
				default:
					break;
			}			
			break;

		default:
			break;
			
	}
	
	if (makeTransition == TRUE) { // making a state transition, send EXIT and ENTRY events to allow for special on-transition behavior
		// recursively call the current state machine with an exit event before changing states for exit behavior
		thisEvent.EventType = EXIT_EVENT;
		RunHSM(thisEvent);
		
		CurrentState = nextState;
		
		// recursively call the current state machine with an entry event after changing states for entry behavior
		thisEvent.EventType = ENTRY_EVENT;
		RunHSM(thisEvent);
		thisEvent.EventType = NO_EVENT; // Transitions are only triggered by events being handled, so the return must be NO_EVENT
	}
	return thisEvent;
}


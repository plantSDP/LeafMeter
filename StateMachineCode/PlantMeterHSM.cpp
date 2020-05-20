// Our Architecture Includes
#include "PlantMeterHSM.h"
#include "Configure.h"
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
				Init_SubHSM_Init();
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
			thisEvent = Run_SubHSM_Init(thisEvent);	// Runs the sub-state machine for Initing
			
			// Button event detected
			if (thisEvent.EventType == BTN_EVENT){
				
				// continue to next state
				if (thisEvent.EventParam == BTN3){
					Init_SubHSM_Leak();
					nextState = LeakChecking;
					makeTransition = TRUE;
				}
			}
			break;

		case LeakChecking:
			//pinMode(13, OUTPUT);
			//digitalWrite(13, HIGH); // Teensy LED test
			
			thisEvent = Run_SubHSM_Leak(thisEvent); // Runs the sub-state machine for LeakChecking

			// Button event detected
			if (thisEvent.EventType == BTN_EVENT){
				
				// continue to next state
				if ((thisEvent.EventParam == BTN3) && (rfOption == RF_NO)){			
					Init_SubHSM_DateTime();
					nextState = DateTime;
					makeTransition = TRUE;
				
				// continue with RF diagnostic
				} else if ((thisEvent.EventType == BTN3) && (rfOption == RF_YES)){	
					Init_SubHSM_RF();
					nextState = RFChecking;
					makeTransition = TRUE;
				
				// return to previous state
				} else if (thisEvent.EventType == BTN4){	
					Init_SubHSM_Init();
					nextState = Initing;
					makeTransition = TRUE;
				}
			}
			break;

		case RFChecking:
			thisEvent = Run_SubHSM_RF(thisEvent); // Runs the sub-state machine for RFChecking
			
			// Button event detected
			if (thisEvent.EventType == BTN_EVENT){
				
				// continue to next state, DateTime
				if (thisEvent.EventParam == BTN3){
					Init_SubHSM_DateTime();
					nextState = DateTime;
					makeTransition = TRUE;
					
				// return to previous state, LeakChecking
				} else if (thisEvent.EventType == BTN4){
					Init_SubHSM_Leak();
					nextState = LeakChecking;
					makeTransition = TRUE;
				}
			}
			break;

		case DateTime:
			thisEvent = Run_SubHSM_DateTime(thisEvent); // Runs the sub-state machine for DateTime
			
			// Button event detected
			if (thisEvent.EventType == BTN_EVENT){
				
				// continue to next state
				if (thisEvent.EventParam == BTN3){
					Init_SubHSM_Active();
					nextState = Active;
					makeTransition = TRUE;
					
				// return to previous state, leak checking
				} else if ((thisEvent.EventType == BTN4) && (rfOption == RF_NO)){
					Init_SubHSM_Leak();
					nextState = LeakChecking;
					makeTransition = TRUE;
					
				// return to previous state, RF checking
				} else if ((thisEvent.EventType == BTN4) && (rfOption == RF_YES)){
					Init_SubHSM_RF();
					nextState = RFChecking;
					makeTransition = TRUE;
				}
			}
			break;

		case Active:
			thisEvent = Run_SubHSM_Active(thisEvent); // Runs the sub-state machine for Active
		
			switch(thisEvent.EventType) {
				// On entry, init active duration timer, display message
				case ENTRY_EVENT:
					SetTimer(0, ACTIVE_DURATION);
					
					sprintf(myString, "MEAS IN PROG    ");
					lcd.setCursor(0, 0); // set the cursor to column 0, line 0
					lcd.print(myString);  // Print a message to the LCD
					sprintf(myString, "BTN3 TO CANCEL  ");
					lcd.setCursor(0, 1); // set the cursor to column 0, line 1
					lcd.print(myString);  // Print a message to the LCD
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
						Init_SubHSM_Wait();
						nextStte = Waiting;
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
					SetTimer(0, period*60000);
					
					sprintf(myString, "WAITING         ");
					lcd.setCursor(0, 0);  // set the cursor to column 0, line 0
					lcd.print(myString);  // Print a message to the LCD
					sprintf(myString, "                ");
					lcd.setCursor(0, 1);  // set the cursor to column 0, line 1
					lcd.print(myString);  // Print a message to the LCD
					break;
					
				// On wait duration timeout, transition to active
				case TIMEOUT:
					if (thisEvent.EventParam == TIMER_0_PARAM) {
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
		
		// recursively call the current state machine with an entry event after changing states for exit behavior
		thisEvent.EventType = ENTRY_EVENT;
		RunHSM(thisEvent);
		thisEvent.EventType = NO_EVENT;
	}
	return thisEvent;
	
	
	
}


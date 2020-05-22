// Includes
#include "ActiveSubHSM.h"
#include "Arduino.h"
#include "EventCheckers.h"

// Private Definitions
#define TIMER_DATA 1
#define TIMER_DATA_PARAM 0b10			// Timer 0 is in use as the active duration, so Timer 1 is used for data sampling

// List states here:
typedef enum {
    InitPSubState,
	State0_Failure,
    State1_StartingActive,				// Closes valves
    State2_TakingMeasurement1,			// Inits sampling timer and waits for a timeout
	State3_TakingMeasurement2,			// Takes a sample, records the data
} ActiveSubHSMStates;

// Holds current state
static ActiveSubHSMStates CurrentState = InitPSubState;

// This function runs the state machine with an INIT_EVENT
uint8_t Init_SubHSM_Active(void){
	Event thisEvent;
	thisEvent.EventType = INIT_EVENT;
	thisEvent.EventParam = 0;
	Event returnEvent = Run_SubHSM_Active(thisEvent);
	if (returnEvent.EventType == NO_EVENT) {
		return TRUE;
	} else {
		return FALSE;
	}
}

/*
This function contains the nested switch statement that implements the state machine.
It requires an Event as a parameter.
If the event is consumed/handled in the state machine, then the function returns NO_EVENT
If the event is not consumed/handled, then the event is returned unchanged.
*/
Event Run_SubHSM_Active(Event thisEvent) {
	
	uint8_t makeTransition = FALSE; // use to flag transition
	ActiveSubHSMStates nextState;

	switch (CurrentState) {
		case InitPSubState:								// If current state is initial Pseudo State
			if (thisEvent.EventType == INIT_EVENT) {	// only respond to INIT_EVENT
				nextState = State1_StartingActive;		// transition to first state
				makeTransition = TRUE;
			}
			break;
		
		case State0_Failure:
			sprintf(myString, "Sensor Failure");
			lcd.setCursor(0, 0); // set the cursor to column 0, line 0
			lcd.print(myString);  // Print a message to the LCD
			thisEvent.EventType = NO_EVENT;
			break;

		case State1_StartingActive:
			switch (thisEvent.EventType) {
				case ENTRY_EVENT:
					// Close valves, run pump
					nextState = State2_TakingMeasurement1;
					makeTransition = TRUE;
					break;
				default:
					break;
			}
			break;
		
		case State2_TakingMeasurement1:
			switch (thisEvent.EventType) {
				case ENTRY_EVENT:
					// init timer
					SetTimer(TIMER_DATA, SAMPLING_FREQ); 	// sampling frequency timer

					// Init Cozir and request data
					if (CozirInit()) {
						CozirRequestData();	// it takes around 70-100 ms for cozir to send this data. The min sampling freq is 500 ms, so this shouldn't be a prob
					} else {
						nextState = State0_Failure;
						makeTransition = TRUE;
					}
					thisEvent.EventType = NO_EVENT;
					break;
				case TIMEOUT:
					if (thisEvent.EventParam == TIMER_DATA_PARAM) {
						nextState = State3_TakingMeasurement2;
						makeTransition = TRUE;
					}
					thisEvent.EventType = NO_EVENT;
					break;
				default:
					break;
			}
			break;

		case State3_TakingMeasurement2:
			switch (thisEvent.EventType) {
				case ENTRY_EVENT:
					// init timer
					// read data
					// temporarily store data

					thisEvent.EventType = NO_EVENT;
					break;
				case TIMEOUT:
					if (thisEvent.EventParam == TIMER_DATA_PARAM) {
						nextState = State3_TakingMeasurement2;
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
		Run_SubHSM_Init(thisEvent);
		
		CurrentState = nextState;
		
		// recursively call the current state machine with an entry event after changing states for entry behavior
		thisEvent.EventType = ENTRY_EVENT;
		Run_SubHSM_Init(thisEvent);
		thisEvent.EventType = NO_EVENT;	// Transitions are only triggered by events being handled, so the return must be NO_EVENT
	}
	return thisEvent;
}
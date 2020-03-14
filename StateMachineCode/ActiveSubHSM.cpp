// Includes
#include "ActiveSubHSM.h"

// Private Definitions

// List states here:
typedef enum {
    InitPSubState,
    State1_StartingActive,
    State2_TakingMeasurement1,
	State3_TakingMeasurement2,
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
					// 
					break;
				case TIMEOUT:
					nextState = State3_TakingMeasurement2;
					makeTransition = TRUE;
					break;
				default:
					break;
			}
			break;

		case State3_TakingMeasurement2:
			switch (thisEvent.EventType) {
				case ENTRY_EVENT:
					// Open valves
					// Display results
					break;
				case BTN_EVENT:
					if (thisEvent.EventParam == BTN4) {			// Back
						nextState = State2_RFChecking;
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
		
	if (makeTransition == TRUE) { // making a state transition, send EXIT and ENTRY
		// recursively call the current state with an exit event
		thisEvent.EventType = EXIT_EVENT;
		Run_SubHSM_Active(thisEvent);
		CurrentState = nextState;
		thisEvent.EventType = ENTRY_EVENT;
		Run_SubHSM_Active(thisEvent);
	}
	return thisEvent;
}
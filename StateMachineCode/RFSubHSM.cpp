// Includes
#include "RFSubHSM.h"

// Private Definitions

// List states here:
typedef enum {
    InitPSubState,
    State1_ContinuePrompt,
    State2_RFChecking,
	State3_,
} RFSubHSMStates;

// Holds current state
static RFSubHSMStates CurrentState = InitPSubState;

// This function runs the state machine with an INIT_EVENT
uint8_t Init_SubHSM_RF(void){
	Event thisEvent;
	thisEvent.EventType = INIT_EVENT;
	thisEvent.EventParam = 0;
	Event returnEvent = Run_SubHSM_RF(thisEvent);
	if (returnEvent.EventType == NO_EVENT) {
		return TRUE;
	} else {
		return FALSE;
	}
}

Event Run_SubHSM_RF(Event thisEvent) {
	
	uint8_t makeTransition = FALSE; // use to flag transition
	InitSubHSMStates nextState;

	switch (CurrentState) {
		case InitPSubState:								// If current state is initial Pseudo State
			if (thisEvent.EventType == INIT_EVENT) {	// only respond to INIT_EVENT
				nextState = State1_ContinuePrompt;		// transition to first state
				makeTransition = TRUE;
			}
			break;

		case State1_ContinuePrompt:
			switch (thisEvent.EventType) {
				case ENTRY_EVENT:
					// Display "continue with RF connectivity check" prompt
					break;
				case BTN_EVENT:
					if (thisEvent.EventParam == BTN3) {			// Continue
						nextState = State2_RFChecking;
						makeTransition = TRUE;
					}
					break;
				default:
					break;
			}
			break;
		
		case State2_RFChecking:
			switch (thisEvent.EventType) {
				case ENTRY_EVENT:
					// Perform RF connectivity check here
					// Init a timer
					break;
				case TIMEOUT:
					nextState = State3_DisplayingResult;
					makeTransition = TRUE;
					break;
				default:
					break;
			}
			break;

		case State3_DisplayingResult:
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
		Run_SubHSM_RF(thisEvent);
		CurrentState = nextState;
		thisEvent.EventType = ENTRY_EVENT;
		Run_SubHSM_RF(thisEvent);
	}
	return thisEvent;
}

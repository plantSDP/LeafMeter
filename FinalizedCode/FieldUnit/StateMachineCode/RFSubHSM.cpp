// Includes
#include "RFSubHSM.h"

// Private Definitions

// State list without RF test
// To run the RF test, uncomment the #define in Configure.h
#ifdef NO_RF_TEST
typedef enum {
    InitPSubState,
    State1_ContinuePrompt,
} RFSubHSMStates;

#else
// List states here:
typedef enum {
    InitPSubState,
    State1_ContinuePrompt,
    State2_RFChecking,
	State3_DisplayingResult,
} RFSubHSMStates;
#endif

// Holds current state
static RFSubHSMStates CurrentState = InitPSubState;

/*
This function initializes the state machine with an INIT_EVENT. 
In regards to the state machine, it transitions the machine out of the initial pseudostate and performs one-time setup functions

Parameters: none
Return: TRUE on success, FALSE on failure
*/
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

// This macro runs the version of this substate without the RF test 
// To run the RF test version, uncomment the #define in Configure.h
#ifdef NO_RF_TEST
/*
This function contains the nested switch statement that implements the state machine.
It requires an Event as a parameter.
If the event is consumed/handled in the state machine, then the function returns NO_EVENT
If the event is not consumed/handled, then the event is returned unchanged.
*/
Event Run_SubHSM_RF(Event thisEvent) {
	
	uint8_t makeTransition = FALSE; // use to flag transition
	RFSubHSMStates nextState;

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
					// Display prompt
					sprintf(myString, "RF CHECK DONE   ");
					lcd.setCursor(0, 0);  // set the cursor to column 0, line 0
					lcd.print(myString);  // Print a message to the LCD
					sprintf(myString, "BTN3 CONTINUE   ");
					lcd.setCursor(0, 1);  // set the cursor to column 0, line 1
					lcd.print(myString);  // Print a message to the LCD

					thisEvent.EventType = NO_EVENT;
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
		Run_SubHSM_RF(thisEvent);
		
		CurrentState = nextState;
		
		// recursively call the current state machine with an entry event after changing states for entry behavior
		thisEvent.EventType = ENTRY_EVENT;
		Run_SubHSM_RF(thisEvent);
		thisEvent.EventType = NO_EVENT;	// Transitions are only triggered by events being handled, so the return must be NO_EVENT
	}
	return thisEvent;
}

// the code below implements the RF test, but is unfinished -JN 5/10/20
#else
/*
This function contains the nested switch statement that implements the state machine.
It requires an Event as a parameter.
If the event is consumed/handled in the state machine, then the function returns NO_EVENT
If the event is not consumed/handled, then the event is returned unchanged.
*/
Event Run_SubHSM_RF(Event thisEvent) {
	
	uint8_t makeTransition = FALSE; // use to flag transition
	RFSubHSMStates nextState;

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
					thisEvent.EventType = NO_EVENT;
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

					thisEvent.EventType = NO_EVENT;
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

					thisEvent.EventType = NO_EVENT;
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
#endif
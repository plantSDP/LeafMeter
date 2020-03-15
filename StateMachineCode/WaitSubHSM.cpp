// Includes
#include "WaitSubHSM.h"
#include "Arduino.h"
#include "EventCheckers.h"

// Private Definitions
#define HUM_DANGER_THRESHOLD 90
#define HUM_WARNING_THRESHOLD 80

#define PRESSURE_TEST_THRESHOLD 10 // just a placeholder for now

#define TEST_TIMER 0
#define DATA_TIMER 1

// List states here:
typedef enum {
    InitPSubState,
    State1_ContinuePrompt,
	State2_Pressurizing,
	State3_ReadingPressure,
	State4_DisplayingResult,
	State5_HumFail,
	State6_HumCheck,
	State7_HumConfirm,
} WaitSubHSMStates;

// Holds the current state
static WaitSubHSMStates CurrentState = InitPSubState;

// This function runs the state machine with an INIT_EVENT
uint8_t Init_SubHSM_Wait(void){
	Event thisEvent;
	thisEvent.EventType = INIT_EVENT;
	thisEvent.EventParam = 0;
	Event returnEvent = Run_SubHSM_Wait(thisEvent);
	if (returnEvent.EventType == NO_EVENT) {
		return TRUE;
	} else {
		return FALSE;
	}
}


static int hum = 0;
static int pressure = 0;
Event Run_SubHSM_Wait(Event thisEvent) {
	
	uint8_t makeTransition = FALSE; // use to flag transition
	WaitSubHSMStates nextState;

	switch (CurrentState) {
		case InitPSubState:								// If current state is initial Pseudo State
			if (thisEvent.EventType == INIT_EVENT){		// only respond to INIT_EVENT
				nextState = State1_ContinuePrompt;		// transition to first state
				makeTransition = TRUE;
			}
			break;

		case State1_Pressurizing:
			switch (thisEvent.EventType) {
				case ENTRY_EVENT:
					// Init timer
					// Display "Pressurizing"
					// Run pump
					// Close valves
					// Read cozir data (for hum)
					// Read pressure sensor
					break;
				case TIMEOUT:
					nextState = State2_Pressurizing;	// transitions back into itself on a data timer timeout, so new pressure can be read
					makeTransition = TRUE;				// not sure if this works
					break;
				// Transition to State3 when pressure is greater than threshold. This number is defined above.
					// Stop pump, init leak test timer
				// Transition to State5 when humidity is greater than threshold. This number is defined above.
				default:
					break;
			}
			break;
		
		case State2_ReadingPressure:
			switch (thisEvent.EventType) {
				case ENTRY_EVENT:
					// Init data collection timer
					break;
				case TIMEOUT:
					if (thisEvent.EventParam == TEST_TIMER) {			// if leak test duration is over, transition to next state
						nextState = State3_DisplayingResult;
						makeTransition = TRUE;
					} else if (thisEvent.EventParam == DATA_TIMER) {	// transitions back into itself on a data timer timeout, so new pressure can be read, like above
						nextState = State2_ReadingPressure;
						makeTransition = TRUE;
					}
					break;
				default:
					break;
			}
			break;

		case State3_RecordingResult:
			switch (thisEvent.EventType) {
				case ENTRY_EVENT:
					// Open valves
					// Record result
					nextState = State1_Pressurizing;
					makeTransition = TRUE;
					break;
				default:
					break;
			}
			break;

		case State4_HumFail:
			switch (thisEvent.EventType) {
				case ENTRY_EVENT:
					// Display failure, prompt btn press for retry
					// Open valves
					break;
				case BTN_EVENT:
					nextState = State5_HumCheck;
					makeTransition = TRUE;
					break;
				default:
					break;					
			}
			break;

		case State5_HumCheck:
			switch (thisEvent.EventType) {
				case ENTRY_EVENT:
					// Init timer
					// Read hum
					break;						
				case TIMEOUT:
					if (hum < HUM_DANGER_THRESHOLD) {
						nextState = State6_HumConfirm;
						makeTransition = TRUE;
					} else {
						nextState = State4_HumFail;
						makeTransition = TRUE;	
					}
					break;
				default:
					break;
			}			
			break;

		case State6_HumConfirm:
			switch (thisEvent.EventType) {
				case ENTRY_EVENT:
					if (hum > HUM_WARNING_THRESHOLD) {
						// Display warning
					} else {
						// Display ok
					}
				break; 
				// Continue to State2_Pressurizing
				case BTN_EVENT:
					if (thisEvent.EventParam == BTN3){
						nextState = State2_Pressurizing;
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
		Run_SubHSM_Wait(thisEvent);
		CurrentState = nextState;
		thisEvent.EventType = ENTRY_EVENT;
		Run_SubHSM_Wait(thisEvent);
	}
	return thisEvent;
}
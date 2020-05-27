// Includes
#include "LeakSubHSM.h"

// Private Definitions
#define TEST_TIMER 0
#define DATA_TIMER 1

// State list without pressure test
// To run the pressure test, uncomment the #define in Configure.h
#ifdef NO_PRESSURE_TEST
typedef enum {
    InitPSubState,
    State1_ContinuePrompt,
} LeakSubHSMStates;

// State list with pressure checking, incomplete -JN 5/10/20
#else
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
} LeakSubHSMStates;
#endif
// Holds the current state
static LeakSubHSMStates CurrentState = InitPSubState;

/*
This function initializes the state machine with an INIT_EVENT. 
In regards to the state machine, it transitions the machine out of the initial pseudostate and performs one-time setup functions

Parameters: none
Return: TRUE on success, FALSE on failure
*/
uint8_t Init_SubHSM_Leak(void){
	Event thisEvent;
	thisEvent.EventType = INIT_EVENT;
	thisEvent.EventParam = 0;
	Event returnEvent = Run_SubHSM_Leak(thisEvent);
	if (returnEvent.EventType == NO_EVENT) {
		return TRUE;
	} else {
		return FALSE;
	}
}

// This macro runs the version of this substate without the pressure test 
// To run the pressure test version, uncomment the #define in Configure.h
#ifdef NO_PRESSURE_TEST
/*
This function contains the nested switch statement that implements the state machine.
It requires an Event as a parameter.
If the event is consumed/handled in the state machine, then the function returns NO_EVENT
If the event is not consumed/handled, then the event is returned unchanged.
*/
Event Run_SubHSM_Leak(Event thisEvent) {
	
	uint8_t makeTransition = FALSE; // use to flag transition
	LeakSubHSMStates nextState;

	switch (CurrentState) {
		case InitPSubState:								// If current state is initial Pseudo State
			if (thisEvent.EventType == INIT_EVENT){		// only respond to INIT_EVENT
				nextState = State1_ContinuePrompt;		// transition to first state
				makeTransition = TRUE;
			}
			break;


		case State1_ContinuePrompt:
			switch (thisEvent.EventType) {
				case ENTRY_EVENT:
					// Display "continue with leak check" prompt
					sprintf(myString, "LEAK CHECK OFF  ");
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
		Run_SubHSM_Leak(thisEvent);
		
		CurrentState = nextState;
		
		// recursively call the current state machine with an entry event after changing states for entry behavior
		thisEvent.EventType = ENTRY_EVENT;
		Run_SubHSM_Leak(thisEvent);
		thisEvent.EventType = NO_EVENT;	// Transitions are only triggered by events being handled, so the return must be NO_EVENT
	}
	return thisEvent;
}

// the code below implements the pressure test, but is unfinished -JN 5/10/20
#else
/*
This function contains the nested switch statement that implements the state machine.
It requires an Event as a parameter.
If the event is consumed/handled in the state machine, then the function returns NO_EVENT
If the event is not consumed/handled, then the event is returned unchanged.
*/
Event Run_SubHSM_Leak(Event thisEvent) {
	
	uint8_t makeTransition = FALSE; // use to flag transition
	LeakSubHSMStates nextState;

	switch (CurrentState) {
		case InitPSubState:								// If current state is initial Pseudo State
			if (thisEvent.EventType == INIT_EVENT){		// only respond to INIT_EVENT
				nextState = State1_ContinuePrompt;		// transition to first state
				makeTransition = TRUE;
			}
			break;


		case State1_ContinuePrompt:
			switch (thisEvent.EventType) {
				case ENTRY_EVENT:
					// Display "continue with leak check" prompt
					sprintf(myString, "LEAK CHECK      ");
					lcd.setCursor(0, 0);  // set the cursor to column 0, line 0
					lcd.print(myString);  // Print a message to the LCD
					sprintf(myString, "   BTN3 CONTINUE");
					lcd.setCursor(0, 1);  // set the cursor to column 0, line 1
					lcd.print(myString);  // Print a message to the LCD
					break;
				case BTN_EVENT:
					if (thisEvent.EventParam == BTN3) {		// Continue
						nextState = State2_Pressurizing;
						makeTransition = TRUE;
					}
					break;
				default:
					break;
			}				
			break;

		case State2_Pressurizing:
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
					nextState = State2_Pressurizing;	// transitions back into itself on a data timer timeout, so new data can be read
					makeTransition = TRUE;				// not sure if this works
					break;
				// Transition to State3 when pressure is greater than threshold. This number is defined above.
					// Stop pump, init leak test timer
				// Transition to State5 when humidity is greater than threshold. This number is defined above.
				default:
					break;
			}
			break;
		
		case State3_ReadingPressure:
			switch (thisEvent.EventType) {
				case ENTRY_EVENT:
					// Init data collection timer
					break;
				case TIMEOUT:
					if (thisEvent.EventParam == TEST_TIMER) {			// if leak test duration is over, transition to next state
						nextState = State4_DisplayingResult;
						makeTransition = TRUE;
					} else if (thisEvent.EventParam == DATA_TIMER) {	// transitions back into itself on a data timer timeout, so new pressure can be read, like above
						nextState = State3_ReadingPressure;
						makeTransition = TRUE;
					}
					break;
				default:
					break;
			}
			break;

		case State4_DisplayingResult:
			switch (thisEvent.EventType) {
				case ENTRY_EVENT:
					// Open valves
					// Display results
					break;
				case BTN_EVENT:
					if (thisEvent.EventParam == BTN4) {			// Back
						nextState = State2_Pressurizing;
						makeTransition = TRUE;
					}
					break;
				default:
					break;
			}
			break;

		case State5_HumFail:
			switch (thisEvent.EventType) {
				case ENTRY_EVENT:
					// Display failure, prompt btn press for retry
					// Open valves
					break;
				case BTN_EVENT:
					nextState = State6_HumCheck;
					makeTransition = TRUE;
					break;
				default:
					break;					
			}
			break;

		case State6_HumCheck:
			switch (thisEvent.EventType) {
				case ENTRY_EVENT:
					// Init timer
					// Read hum
					break;						
				case TIMEOUT:
					if (hum < HUM_DANGER_THRESHOLD) {
						nextState = State7_HumConfirm;
						makeTransition = TRUE;
					} else {
						nextState = State5_HumFail;
						makeTransition = TRUE;	
					}
					break;
				default:
					break;
			}			
			break;

		case State7_HumConfirm:
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
		
	if (makeTransition == TRUE) { // making a state transition, send EXIT and ENTRY events to allow for special on-transition behavior
		// recursively call the current state machine with an exit event before changing states for exit behavior
		thisEvent.EventType = EXIT_EVENT;
		Run_SubHSM_Init(thisEvent);
		
		CurrentState = nextState;
		
		// recursively call the current state machine with an entry event after changing states for entry behavior
		thisEvent.EventType = ENTRY_EVENT;
		Run_SubHSM_Init(thisEvent);
		thisEvent.EventType = NO_EVENT;
	}
	return thisEvent;
}
#endif
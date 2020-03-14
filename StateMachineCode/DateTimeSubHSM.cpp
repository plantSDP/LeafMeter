// Includes
#include "DateTimeSubHSM.h"

// Private Definitions
#define MONTH_1 0
#define MONTH_2 0

#define DAY_1 0
#define DAY_2 0

#define YEAR_1 2
#define YEAR_2 0

// List states here:
typedef enum {
    InitPSubState,
    State1_MonthDigit1,
    State2_MonthDigit2,
	State3_DayDigit1,
	State4_DayDigit2,
    State5_YearDigit1,
	State6_YearDigit2,
	State7_HourDigit1,
	State8_MinuteDigit1,
	State9_MinuteDigit2,
} DateTimeSubHSMStates;

// Holds current state
static DateTimeSubHSMStates CurrentState = InitPSubState;

// This function runs the state machine with an INIT_EVENT
uint8_t Init_SubHSM_DateTime(void){
	Event thisEvent;
	thisEvent.EventType = INIT_EVENT;
	thisEvent.EventParam = 0;
	Event returnEvent = Run_SubHSM_DateTime(thisEvent);
	if (returnEvent.EventType == NO_EVENT) {
		return TRUE;
	} else {
		return FALSE;
	}
}

Event Run_SubHSM_DateTime(Event thisEvent) {
	
	uint8_t makeTransition = FALSE; // use to flag transition
	DateTimeSubHSMStates nextState;

	switch (CurrentState) {
		case InitPSubState:								// If current state is initial Pseudo State
			if (thisEvent.EventType == INIT_EVENT) {	// only respond to INIT_EVENT
				nextState = State1_MonthDigit1;			// transition to first state
				makeTransition = TRUE;
			}
			break;

		case State1_MonthDigit1:
			switch (thisEvent.EventType) {
				case ENTRY_EVENT:
					// Update Display
					break;
				case BTN_EVENT:
					if (thisEvent.EventParam == BTN1) {
						// increment digit
						// update display
					} else if (thisEvent.EventParam == BTN2) {			// Switch digits
						nextState = State2_MonthDigit2;
						makeTransition = TRUE;
					} else if (thisEvent.EventParam == BTN3) {			// Continue to Day digit 1
						nextState = State3_DayDigit1;
						makeTransition = TRUE;
					}
					break;
				default:
					break;
			}
			break;
		
		case State2_MonthDigit2:
			switch (thisEvent.EventType) {
				case ENTRY_EVENT:
					// Update Display
					break;
				case BTN_EVENT:
					if (thisEvent.EventParam == BTN1) {
						// increment digit
						// update display
					} else if (thisEvent.EventParam == BTN2) {			// Switch digits
						nextState = State1_MonthDigit1;
						makeTransition = TRUE;
					} else if (thisEvent.EventParam == BTN3) {			// Continue to Day digit 1
						nextState = State3_DayDigit1;
						makeTransition = TRUE;
					}
					break;
				default:
					break;
			}
			break;
		
		case State3_DayDigit1:
			switch (thisEvent.EventType) {
				case ENTRY_EVENT:
					// Update Display
					break;
				case BTN_EVENT:
					if (thisEvent.EventParam == BTN1) {
						// increment digit
						// update display
					} else if (thisEvent.EventParam == BTN2) {			// Switch digits
						nextState = State4_DayDigit2;
						makeTransition = TRUE;
					} else if (thisEvent.EventParam == BTN3) {			// Continue to Year digit 1
						nextState = State5_YearDigit1;
						makeTransition = TRUE;
					} else if (thisEvent.EventParam == BTN4) {			// Back to Month digit 1
						nextState = State1_MonthDigit1;
						makeTransition = TRUE;
					}
					break;
				default:
					break;
			}
			break;

		case State4_DayDigit2:
			switch (thisEvent.EventType) {
				case ENTRY_EVENT:
					// Update Display
					break;
				case BTN_EVENT:
					if (thisEvent.EventParam == BTN1) {
						// increment digit
						// update display
					} else if (thisEvent.EventParam == BTN2) {			// Switch digits
						nextState = State3_DayDigit1;
						makeTransition = TRUE;
					} else if (thisEvent.EventParam == BTN3) {			// Continue to Year digit 1
						nextState = State5_YearDigit1;
						makeTransition = TRUE;
					} else if (thisEvent.EventParam == BTN4) {			// Back to Month digit 1
						nextState = State1_MonthDigit1;
						makeTransition = TRUE;
					}
					break;
				default:
					break;
			}
			break;

		case State5_YearDigit1:
			switch (thisEvent.EventType) {
				case ENTRY_EVENT:
					// Update Display
					break;
				case BTN_EVENT:
					if (thisEvent.EventParam == BTN1) {
						// increment digit
						// update display
					} else if (thisEvent.EventParam == BTN2) {			// Switch digits
						nextState = State6_YearDigit2;
						makeTransition = TRUE;
					} else if (thisEvent.EventParam == BTN4) {			// Back to Day digit 1
						nextState = State3_DayDigit1;
						makeTransition = TRUE;
					}
					break;
				default:
					break;
			}
			
			break;

		case State6_YearDigit2:
			switch (thisEvent.EventType) {
				case ENTRY_EVENT:
					// Update Display
					break;
				case BTN_EVENT:
					if (thisEvent.EventParam == BTN1) {
						// increment digit
						// update display
					} else if (thisEvent.EventParam == BTN2) {			// Switch digits
						nextState = State5_YearDigit1;
						makeTransition = TRUE;
					} else if (thisEvent.EventParam == BTN4) {			// Back to Day digit 1
						nextState = State3_DayDigit1;
						makeTransition = TRUE;
					}
					break;
				default:
					break;
			}
			break;

		case State7_HourDigit1:
			switch (thisEvent.EventType) {
				case ENTRY_EVENT:
					// Update Display
					break;
				case BTN_EVENT:
					if (thisEvent.EventParam == BTN1) {
						// increment digit
						// update display
					} else if (thisEvent.EventParam == BTN3) {			// Continue to Minute digit 1
						nextState = State8_MinuteDigit1;
						makeTransition = TRUE;
					} else if (thisEvent.EventParam == BTN4) {			// Back to Year digit 2
						nextState = State6_YearDigit2;
						makeTransition = TRUE;
					}
					break;
				default:
					break;
			}
			break;

		case State8_MinuteDigit1:
			switch (thisEvent.EventType) {
				case ENTRY_EVENT:
					// Update Display
					break;
				case BTN_EVENT:
					if (thisEvent.EventParam == BTN1) {
						// increment digit
						// update display
					} else if (thisEvent.EventParam == BTN2) {			// Continue to Minute digit 2
						nextState = State9_MinuteDigit2;
						makeTransition = TRUE;
					} else if (thisEvent.EventParam == BTN4) {			// Back to Hour digit 1
						nextState = State7_HourDigit1;
						makeTransition = TRUE;
					}
					break;
				default:
					break;
			}
			break;

		case State9_MinuteDigit2:
			switch (thisEvent.EventType) {
				case ENTRY_EVENT:
					// Update Display
					break;
				case BTN_EVENT:
					if (thisEvent.EventParam == BTN1) {
						// increment digit
						// update display
					} else if (thisEvent.EventParam == BTN2) {			// Continue to Minute digit 1
						nextState = State8_MinuteDigit1;
						makeTransition = TRUE;
					} else if (thisEvent.EventParam == BTN4) {			// Back to Hour digit 1
						nextState = State7_HourDigit1;
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
		Run_SubHSM_DateTime(thisEvent);
		CurrentState = nextState;
		thisEvent.EventType = ENTRY_EVENT;
		Run_SubHSM_DateTime(thisEvent);
	}
	return thisEvent;
}

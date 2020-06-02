// Includes
#include "DateTimeSubHSM.h"

// Private Definitions
#define MONTH_1 0
#define MONTH_2 0

#define DAY_1 0
#define DAY_2 0

#define YEAR_1 2
#define YEAR_2 0

void PrintDate(void);
void PrintTime(void);

// List states here:
typedef enum {
    InitPSubState,
    State1_MonthDigit1,			// config of the month tens-digit
    State2_MonthDigit2,			// config of the month ones-digit
	State3_DayDigit1,			// config of the day tens-digit
	State4_DayDigit2,			// config of the day ones-digit
    State5_YearDigit1,			// config of the year tens-digit
	State6_YearDigit2,			// config of the year ones-digit
	State7_HourDigit1,			// config of the hour tens-digit
	State8_HourDigit2,			// config of the hour ones-digit
	State9_MinuteDigit1,		// config of the minute tens-digit
	State10_MinuteDigit2,		// config of the minute ones-digit AND RTC sync on exit
} DateTimeSubHSMStates;

// Holds current state
static DateTimeSubHSMStates CurrentState = InitPSubState;

/*
This function initializes the state machine with an INIT_EVENT. 
In regards to the state machine, it transitions the machine out of the initial pseudostate and performs one-time setup functions

Parameters: none
Return: TRUE on success, FALSE on failure
*/
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

/*
This function contains the nested switch statement that implements the state machine.
It requires an Event as a parameter.
If the event is consumed/handled in the state machine, then the function returns NO_EVENT
If the event is not consumed/handled, then the event is returned unchanged.
*/
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
					PrintDate();
					// blink cursor location
					lcd.setCursor(0, 1);
					// turn blinking on
					lcd.blink();

					thisEvent.EventType = NO_EVENT;
					break;
				case BTN_EVENT:
					if (thisEvent.EventParam == BTN1) {
						// increment digit
						if (month1 == 0) {
							month1 = 1;
							if (month2 > 2) {
								month2 = 2;
							}
						} else if (month1 == 1) {
							month1 = 0;
							if (month2 == 0) {
								month2 = 1;
							}
						}
						// update display
						PrintDate();
						// blink cursor location
						lcd.setCursor(0, 1);				
					} else if (thisEvent.EventParam == BTN2) {			// Switch digits
						nextState = State2_MonthDigit2;
						makeTransition = TRUE;
					} else if (thisEvent.EventParam == BTN3) {			// Continue to Day digit 1
						nextState = State3_DayDigit1;
						makeTransition = TRUE;
					}
					//thisEvent.EventType = NO_EVENT;				
					break;
				default:
					break;
			}
			break;
		
		case State2_MonthDigit2:
			switch (thisEvent.EventType) {
				case ENTRY_EVENT:
					// Update Display
					PrintDate();
					// blink cursor location
					lcd.setCursor(1, 1);

					thisEvent.EventType = NO_EVENT;
					break;
				case BTN_EVENT:
					if (thisEvent.EventParam == BTN1) {
						// increment digit
						if (month1 == 1) {
							if (month2 < 2) {
								month2 = month2 + 1;
							} else if (month2 == 2){
								month2 = 0;
							}
						} else if (month1 == 0) {
							if (month2 < 9) {
								month2 = month2 + 1;
							} else if (month2 == 9) {
								month2 = 1;
							}
						}
						// update display
						PrintDate();
						// blink cursor location
						lcd.setCursor(1, 1);
					} else if (thisEvent.EventParam == BTN2) {			// Switch digits
						nextState = State1_MonthDigit1;
						makeTransition = TRUE;
					} else if (thisEvent.EventParam == BTN3) {			// Continue to Day digit 1
						nextState = State3_DayDigit1;
						makeTransition = TRUE;
					}
					//thisEvent.EventType = NO_EVENT;
					break;
				default:
					break;
			}
			break;
		
		case State3_DayDigit1:
			switch (thisEvent.EventType) {
				case ENTRY_EVENT:
					// Update Display
					PrintDate();
					// blink cursor location
					lcd.setCursor(3, 1);

					thisEvent.EventType = NO_EVENT;
					break;
				case BTN_EVENT:
					if (thisEvent.EventParam == BTN1) {
						// increment digit
						if (day1 < 3) {
							day1 = day1 + 1;
							if (day1 == 3) {
								if (day2 > 1) {
									day2 = 1;
								}
							}
						} else if (day1 == 3) {
							day1 = 0;
							if (day2 == 0) {
								day2 = 1;
							}
						}
						// update display
						PrintDate();
					
						lcd.setCursor(3, 1); // blink cursor location
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
					thisEvent.EventType = NO_EVENT;					
					break;
				default:
					break;
			}
			break;

		case State4_DayDigit2:
			switch (thisEvent.EventType) {
				case ENTRY_EVENT:
					// Update Display
					PrintDate();
					// blink cursor location
					lcd.setCursor(4, 1);

					thisEvent.EventType = NO_EVENT;					
					break;
				case BTN_EVENT:
					if (thisEvent.EventParam == BTN1) {
						// increment digit
						if (day1 == 3) {
							if (day2 == 0) {
								day2 = 1;
							} else if (day2 == 1){
								day2 = 0;
							}
						} else if (day1 == 0) {
							if (day2 < 9) {
								day2 = day2 + 1;
							} else if (day2 == 9) {
								day2 = 1;
							}
						} else {
							if (day2 < 9) {
								day2 = day2 + 1;
							} else if (day2 == 9) {
								day2 = 0;
							}							
						}							
						// update display
						PrintDate();
						// blink cursor location
						lcd.setCursor(4, 1);							
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
					thisEvent.EventType = NO_EVENT;					
					break;
				default:
					break;
			}
			break;

		case State5_YearDigit1:
			switch (thisEvent.EventType) {
				case ENTRY_EVENT:
					// Update Display
					PrintDate();
					// blink cursor location
					lcd.setCursor(6, 1);

					thisEvent.EventType = NO_EVENT;
					break;
				case BTN_EVENT:
					if (thisEvent.EventParam == BTN1) {
						// increment digit
						if (year1 < 9) {
							year1 = year1 + 1;
						} else if (year1 == 9) {
							year1 = 0;
						}
						// update display
						PrintDate();
						// blink cursor location
						lcd.setCursor(6, 1);						
					} else if (thisEvent.EventParam == BTN2) {			// Switch digits
						nextState = State6_YearDigit2;
						makeTransition = TRUE;
					} else if (thisEvent.EventParam == BTN3) {			// Continue to Hour digit 1
						nextState = State7_HourDigit1;
						makeTransition = TRUE;
					} else if (thisEvent.EventParam == BTN4) {			// Back to Day digit 1
						nextState = State3_DayDigit1;
						makeTransition = TRUE;
					}
					thisEvent.EventType = NO_EVENT;
					break;
				default:
					break;
			}
			break;

		case State6_YearDigit2:
			switch (thisEvent.EventType) {
				case ENTRY_EVENT:
					// Update Display
					PrintDate();
					// blink cursor location
					lcd.setCursor(7, 1);

					thisEvent.EventType = NO_EVENT;
					break;
				case BTN_EVENT:
					if (thisEvent.EventParam == BTN1) {
						// increment digit
						if (year1 < 9) {
							year2 = year1 + 1;
						} else if (year1 == 9) {
							year2 = 0;
						}						
						// update display
						PrintDate();
						// blink cursor location
						lcd.setCursor(7, 1);						
					} else if (thisEvent.EventParam == BTN2) {			// Switch digits
						nextState = State5_YearDigit1;
						makeTransition = TRUE;
					} else if (thisEvent.EventParam == BTN3) {			// Continue to Hour digit 1
						nextState = State7_HourDigit1;
						makeTransition = TRUE;
					} else if (thisEvent.EventParam == BTN4) {			// Back to Day digit 1
						nextState = State3_DayDigit1;
						makeTransition = TRUE;
					}
					thisEvent.EventType = NO_EVENT;
					break;
				default:
					break;
			}
			break;

		case State7_HourDigit1:
			switch (thisEvent.EventType) {
				case ENTRY_EVENT:
					// Update Display
					PrintTime();
					// blink cursor location
					lcd.setCursor(0, 1);

					thisEvent.EventType = NO_EVENT;
					break;
				case BTN_EVENT:
					if (thisEvent.EventParam == BTN1) {
						// increment digit
						if (hour1 < 2) {
							hour1 = hour1 + 1;
							if (hour1 == 2) {
								if (hour2 > 4) {
									hour2 = 4;
								}
							}
						} else if (hour1 == 2) {
							hour1 = 0;
						}
						// update display
					PrintTime();
					// blink cursor location
					lcd.setCursor(0, 1);
					} else if (thisEvent.EventParam == BTN2) {			// Switch digits
						nextState = State8_HourDigit2;
						makeTransition = TRUE;
					} else if (thisEvent.EventParam == BTN3) {			// Continue to Minute digit 1
						nextState = State9_MinuteDigit1;
						makeTransition = TRUE;
					} else if (thisEvent.EventParam == BTN4) {			// Back to Year digit 1
						nextState = State5_YearDigit1;
						makeTransition = TRUE;
					}
					thisEvent.EventType = NO_EVENT;
					break;
				default:
					break;
			}
			break;

		case State8_HourDigit2:
			switch (thisEvent.EventType) {
				case ENTRY_EVENT:
					// Update Display
					PrintTime();
					// blink cursor location
					lcd.setCursor(1, 1);

					thisEvent.EventType = NO_EVENT;
					break;
				case BTN_EVENT:
					if (thisEvent.EventParam == BTN1) {
						// increment digit
						if (hour1 == 2) {
							if (hour2 < 4) {
								hour2 = hour2 + 1; 
							} else if (hour2 == 4) {
								hour2 = 0;
							}
						} else {
							if (hour2 < 9) {
								hour2 = hour2 + 1;
							} else if (hour2 == 9) {
								hour2 = 0;
							}
						}
						// update display
						PrintTime();
						// blink cursor location
						lcd.setCursor(1, 1);
					} else if (thisEvent.EventParam == BTN2) {			// Switch digits
						nextState = State7_HourDigit1;
						makeTransition = TRUE;
					} else if (thisEvent.EventParam == BTN3) {			// Continue to Minute digit 1
						nextState = State9_MinuteDigit1;
						makeTransition = TRUE;
					} else if (thisEvent.EventParam == BTN4) {			// Back to Year digit 1
						nextState = State5_YearDigit1;
						makeTransition = TRUE;
					}
					thisEvent.EventType = NO_EVENT;
					break;
				default:
					break;
			}
			break;

		case State9_MinuteDigit1:
			switch (thisEvent.EventType) {
				case ENTRY_EVENT:
					// Update Display
					PrintTime();
					// blink cursor location
					lcd.setCursor(3, 1);

					thisEvent.EventType = NO_EVENT;
					break;
				case BTN_EVENT:
					if (thisEvent.EventParam == BTN1) {
						// increment digit
						if (min1 < 5) {
							min1 = min1 + 1;
						} else if (min1 == 5) {
							min1 = 0;
						}
						// update display
						PrintTime();
						// blink cursor location
						lcd.setCursor(3, 1);
					} else if (thisEvent.EventParam == BTN2) {			// Switch digits
						nextState = State10_MinuteDigit2;
						makeTransition = TRUE;
					} else if (thisEvent.EventParam == BTN4) {			// Back to Hour digit 1
						nextState = State7_HourDigit1;
						makeTransition = TRUE;
					}
					//thisEvent.EventType = NO_EVENT;
					break;
				default:
					break;
			}
			break;

		case State10_MinuteDigit2:
			switch (thisEvent.EventType) {
				case ENTRY_EVENT:
					// Update Display
					PrintTime();
					// blink cursor location
					lcd.setCursor(4, 1);

					thisEvent.EventType = NO_EVENT;
					break;
				case BTN_EVENT:
					if (thisEvent.EventParam == BTN1) {
						// increment digit
						if (min2 < 9) {
							min2 = min2 + 1;
						} else if (min2 == 9) {
							min2 = 0;
						}
						// update display
						PrintTime();
						// blink cursor location
						lcd.setCursor(4, 1);
					} else if (thisEvent.EventParam == BTN2) {			// Switch digits
						nextState = State9_MinuteDigit1;
						makeTransition = TRUE;
					} else if (thisEvent.EventParam == BTN4) {			// Back to Hour digit 1
						nextState = State7_HourDigit1;
						makeTransition = TRUE;
					}
					//thisEvent.EventType = NO_EVENT;
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
		Run_SubHSM_DateTime(thisEvent);
		
		CurrentState = nextState;
		
		// recursively call the current state machine with an entry event after changing states for entry behavior
		thisEvent.EventType = ENTRY_EVENT;
		Run_SubHSM_DateTime(thisEvent);
		thisEvent.EventType = NO_EVENT;	// Transitions are only triggered by events being handled, so the return must be NO_EVENT
	}
	return thisEvent;
}

//====================================
// Private functions
//====================================
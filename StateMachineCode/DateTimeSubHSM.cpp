// Includes
#include "DateTimeSubHSM.h"
#include "Arduino.h"
#include "EventCheckers.h"

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
	State8_HourDigit2,
	State9_MinuteDigit1,
	State10_MinuteDigit2,
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

// MM/DD/YY
static uint8_t month1 = 0;
static uint8_t month2 = 1;

static uint8_t day1   = 0;
static uint8_t day2   = 1;

static uint8_t year1  = 2;
static uint8_t year2  = 0;

// HH:MM
static uint8_t hour1  = 0;
static uint8_t hour2  = 0;

static uint8_t min1   = 0;
static uint8_t min2   = 0
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
					thisEvent.EventType = NO_EVENT;					
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
					thisEvent.EventType = NO_EVENT;
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
					break;
				case BTN_EVENT:
					if (thisEvent.EventParam == BTN1) {
						// increment digit
						if (year1 < 9) {
							year1 = year + 1;
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
					break;
				case BTN_EVENT:
					if (thisEvent.EventParam == BTN1) {
						// increment digit
						if (year1 < 9) {
							year2 = year + 1;
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
					} else if (thisEvent.EventParam == BTN4) {			// Back to Year digit 2
						nextState = State6_YearDigit2;
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
					} else if (thisEvent.EventParam == BTN4) {			// Back to Year digit 2
						nextState = State6_YearDigit2;
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
					thisEvent.EventType = NO_EVENT;
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
					thisEvent.EventType = NO_EVENT;
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


// Private functions

// Prints the date in MM/DD/YY form.
void PrintDate() {
	sprintf(myString, "Enter MM/DD/YY      ");
	lcd.setCursor(0, 0); // set the cursor to column 0, line 0
	lcd.print(myString);  // Print a message to the LCD
	sprintf(myString, "%1d%1d/%1d%1d/%1d%1d      ", month1, month2, day1, day2, year1, year2);
	lcd.setCursor(0, 1); // set the cursor to column 0, line 0
	lcd.print(myString);  // Print a message to the LCD
}

// Prints the time in HH:MM form.
void PrintTime() {
	sprintf(myString, "Enter time HH:MM");
	lcd.setCursor(0, 0); // set the cursor to column 0, line 0
	lcd.print(myString);  // Print a message to the LCD
	sprintf(myString, "%1d%1d:%1d%1d           ", hour1, hour2, min1, min2);
	lcd.setCursor(0, 1); // set the cursor to column 0, line 0
	lcd.print(myString);  // Print a message to the LCD
}
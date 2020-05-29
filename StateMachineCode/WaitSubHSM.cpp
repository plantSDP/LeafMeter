// Includes
#include "WaitSubHSM.h"

// Private Definitions
#define TEST_TIMER 0
#define DATA_TIMER 1

// State list without pressure test
// To run the pressure test, uncomment the #define in Configure.h
#ifdef NO_PRESSURE_TEST
// List state names here:
typedef enum {
    InitPSubState,
	State1_RecordingData,		// record data to SD card
	State2_Idle,				// remain idle, but listen for RF communications (if enabled) and button events
	State3_ConfigPeriod,		// local period configuration
	State4_ConfigNumCycles,		// local numCycles configuration
	State5_ConfigRF,			// local RF option configuration
	State6_M1,					// local month config
	State7_M2,
	State8_D1,					// local day config
	State9_D2,
	State10_Y1,					// local year config
	State11_Y2,
	State12_H1,					// local hour config
	State13_H2,
	State14_min1,				// local minute config
	State15_min2
} WaitSubHSMStates;

// State list with pressure checking, incomplete -JN 5/10/20
#else
// List state names here:
typedef enum {
    InitPSubState,
	State1_Pressurizing,
	State2_ReadingPressure,
	State3_RecordingResult,
	State4_HumFail,
	State5_HumCheck,
	State6_HumConfirm
} WaitSubHSMStates;
#endif
// Holds the current state
static WaitSubHSMStates CurrentState = InitPSubState;


/*
This function initializes the state machine with an INIT_EVENT. 
In regards to the state machine, it transitions the machine out of the initial pseudostate and performs one-time setup functions

Parameters: uint8_t resetFlag, if true: current state will be reset to the starting pseudostate, if false, current state is unchanged. Used to start a new waiting cycle
Return: TRUE on success, FALSE on failure
*/
uint8_t Init_SubHSM_Wait(uint8_t resetFlag){
	if (resetFlag == TRUE) {				// used to reset the Waiting state machine to the starting pseudostate
		CurrentState = InitPSubState;
	}

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

// This macro runs the version of this substate without the pressure test 
// To run the pressure test version, uncomment the #define in Configure.h
#ifdef NO_PRESSURE_TEST
/*
This function contains the nested switch statement that implements the state machine.
It requires an Event as a parameter.
If the event is consumed/handled in the state machine, then the function returns NO_EVENT
If the event is not consumed/handled, then the event is returned unchanged.
*/
Event Run_SubHSM_Wait(Event thisEvent) {
	uint8_t makeTransition = FALSE;
	WaitSubHSMStates nextState;

	switch (CurrentState) {
		case InitPSubState:								// If current state is initial Pseudo State
			if (thisEvent.EventType == INIT_EVENT){		// only respond to INIT_EVENT
				nextState = State1_RecordingData;		// transition to first state
				makeTransition = TRUE;
			}
			break;

		case State1_RecordingData:
			
			switch (thisEvent.EventType) {
				case ENTRY_EVENT:
					// record data to SD card, transition to idle
					nextState = State2_Idle;
					makeTransition = TRUE;					

					thisEvent.EventType = NO_EVENT;
					break;
				// case TIMEOUT:
				// 	break;
				default:
					break;
			}
			break;
		
		case State2_Idle:
			// listen for RF, if enabled
			// respond to RF, if received
			switch (thisEvent.EventType) {
				case ENTRY_EVENT:
					// Display status on LCD
					sprintf(myString, "Idling          ");
					lcd.setCursor(0, 0);  // set the cursor to column 0, line 0
					lcd.print(myString);  // Print a message to the LCD
					sprintf(myString, "BTN3 CONFIG     ");
					lcd.setCursor(0, 1);  // set the cursor to column 0, line 1
					lcd.print(myString);  // Print a message to the LCD

					thisEvent.EventType = NO_EVENT;
					break;
				case BTN_EVENT:
					if (thisEvent.EventParam == BTN3) {			// on BTN3 event, go to local configuration chain
						nextState = State3_ConfigPeriod;
						makeTransition = TRUE;
					}
					break;
				default:
					break;
			}
			break;
		
		// local config
		case State3_ConfigPeriod:
			switch (thisEvent.EventType) {
				case ENTRY_EVENT:
					// Display prompt
					sprintf(myString, "Min btwn meas:");
					lcd.setCursor(0, 0);  // set the cursor to column 0, line 0
					lcd.print(myString);  // Print a message to the LCD
					sprintf(myString, "          %3d        ", period);
					lcd.setCursor(0, 1);  // set the cursor to column 0, line 1
					lcd.print(myString);  // Print a message to the LCD
					// BTN1 || BTN2 increments
				    lcd.setCursor(12, 1);
					lcd.blink();

					thisEvent.EventType = NO_EVENT;
					break;
				case BTN_EVENT:
					if (thisEvent.EventParam == BTN1) {
						// increment periodParam
						if (period < 300){
							period = period + 1;
						}
						// update display
						sprintf(myString, "Min btwn meas,");
						lcd.setCursor(0, 0);  // set the cursor to column 0, line 0
						lcd.print(myString);  // Print a message to the LCD
						sprintf(myString, "          %3d     ", period);
						lcd.setCursor(0, 1);  // set the cursor to column 0, line 1
						lcd.print(myString);  // Print a message to the LCD
						lcd.setCursor(12, 1);

					} else if (thisEvent.EventParam == BTN2) {
						// decrement periodParam
						if (period > 10){
							period = period - 1;
						}
						
						// update display
						sprintf(myString, "Min btwn meas,");
						lcd.setCursor(0, 0);  // set the cursor to column 0, line 0
						lcd.print(myString);  // Print a message to the LCD
						sprintf(myString, "          %3d     ", period);
						lcd.setCursor(0, 1);  // set the cursor to column 0, line 1
						lcd.print(myString);  // Print a message to the LCD
						lcd.setCursor(12, 1);
						
					} else if (thisEvent.EventParam == BTN3) {		// Continue
						nextState = State4_ConfigNumCycles;
						makeTransition = TRUE;

					} else if (thisEvent.EventParam == BTN4) {		// Back
						nextState = State2_Idle;
						makeTransition = TRUE;

					}
					thisEvent.EventType = NO_EVENT;
					break;
				case EXIT_EVENT:
					lcd.noBlink();
					thisEvent.EventType = NO_EVENT;
					break;
				default:
					break;
			}
			break;
		
		case State4_ConfigNumCycles:
			switch (thisEvent.EventType) {
				case ENTRY_EVENT:
					// Display prompt
					sprintf(myString, "# of meas:      ");
					lcd.setCursor(0, 0);  // set the cursor to column 0, line 0
					lcd.print(myString);  // Print a message to the LCD
					sprintf(myString, "          %3d   ", numCycles);
					lcd.setCursor(0, 1);  // set the cursor to column 0, line 1
					lcd.print(myString);  // Print a message to the LCD
					// BTN1 || BTN2 increments
				    lcd.setCursor(12, 1);
					lcd.blink();

					thisEvent.EventType = NO_EVENT;
					break;
				case BTN_EVENT:
					if (thisEvent.EventParam == BTN1) {
						// increment numCycles
						if (numCycles < 11){
							numCycles = numCycles + 1;
						}
						// update display
						sprintf(myString, "# of meas:      ");
						lcd.setCursor(0, 0);  // set the cursor to column 0, line 0
						lcd.print(myString);  // Print a message to the LCD
						sprintf(myString, "          %3d   ", numCycles);
						lcd.setCursor(0, 1);  // set the cursor to column 0, line 1
						lcd.print(myString);  // Print a message to the LCD
						lcd.setCursor(12, 1);

					} else if (thisEvent.EventParam == BTN2) {
						// decrement numCycles
						if (numCycles > 1){
							numCycles = numCycles - 1;
						}
						
						// update display
						sprintf(myString, "# of meas:      ");
						lcd.setCursor(0, 0);  // set the cursor to column 0, line 0
						lcd.print(myString);  // Print a message to the LCD
						sprintf(myString, "          %3d   ", numCycles);
						lcd.setCursor(0, 1);  // set the cursor to column 0, line 1
						lcd.print(myString);  // Print a message to the LCD
						lcd.setCursor(12, 1);
						
					} else if (thisEvent.EventParam == BTN3) {		// Continue
						nextState = State5_ConfigRF;
						makeTransition = TRUE;

					} else if (thisEvent.EventParam == BTN4) {		// Back
						nextState = State3_ConfigPeriod;
						makeTransition = TRUE;

					}
					thisEvent.EventType = NO_EVENT;
					break;
				case EXIT_EVENT:
					lcd.noBlink();
					thisEvent.EventType = NO_EVENT;
					break;
				default:
					break;
			}
			break;
		
		case State5_ConfigRF:
			switch (thisEvent.EventType) {
				case ENTRY_EVENT:
					// Display prompt
					sprintf(myString, "Set RF option:");
					lcd.setCursor(0, 0);  // set the cursor to column 0, line 0
					lcd.print(myString);  // Print a message to the LCD
					
					sprintf(myString, "NO       YES          ");
					lcd.setCursor(0, 1);  // set the cursor to column 0, line 1
					lcd.print(myString);  // Print a message to the LCD
					
					lcd.setCursor(0, 1);
					lcd.blink();
					// BTN1 yes, BTN2 no, default is no

					thisEvent.EventType = NO_EVENT;
					break;
				case BTN_EVENT:
					if (thisEvent.EventParam == BTN1) {
						rfOption = 1;
						sprintf(myString, "Set RF option:");
						lcd.setCursor(0, 0);  // set the cursor to column 0, line 0
						lcd.print(myString);  // Print a message to the LCD
					
						sprintf(myString, "NO       YES          ");
						lcd.setCursor(0, 1);  // set the cursor to column 0, line 1
						lcd.print(myString);  // Print a message to the LCD
						
						lcd.setCursor(9, 1);

					} else if (thisEvent.EventParam == BTN2) {
						rfOption = 0;
						sprintf(myString, "Set RF option:");
						lcd.setCursor(0, 0); // set the cursor to column 0, line 0
						lcd.print(myString);  // Print a message to the LCD
					
						sprintf(myString, "NO       YES          ");
						lcd.setCursor(0, 1); // set the cursor to column 0, line 1
						lcd.print(myString);  // Print a message to the LCD
						
						lcd.setCursor(0, 1);

					} else if (thisEvent.EventParam == BTN3) {		// Continue
					    lcd.noBlink();
						nextState = State6_M1;
						makeTransition = TRUE;

					} else if (thisEvent.EventParam == BTN4) {		// Back
					    lcd.noBlink();
						nextState = State4_ConfigNumCycles;
						makeTransition = TRUE;

					}
					thisEvent.EventType = NO_EVENT;
					break;
				default:
					break;
			}
			break;
		
		case State6_M1:
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
						nextState = State7_M2;
						makeTransition = TRUE;
					} else if (thisEvent.EventParam == BTN3) {			// Continue to Day digit 1
						nextState = State8_D1;
						makeTransition = TRUE;
					} else if (thisEvent.EventParam == BTN4) {			// Back to Config RF
						nextState = State5_ConfigRF;
						makeTransition = TRUE;
					}
					thisEvent.EventType = NO_EVENT;					
					break;
				default:
					break;
			}
			break;
		
		case State7_M2:
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
						nextState = State6_M1;
						makeTransition = TRUE;
					} else if (thisEvent.EventParam == BTN3) {			// Continue to Day digit 1
						nextState = State8_D1;
						makeTransition = TRUE;
					} else if (thisEvent.EventParam == BTN4) {			// Back to Config RF
						nextState = State5_ConfigRF;
						makeTransition = TRUE;
					}
					thisEvent.EventType = NO_EVENT;
					break;
				default:
					break;
			}
			break;

		case State8_D1:
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
						nextState = State9_D2;
						makeTransition = TRUE;
					} else if (thisEvent.EventParam == BTN3) {			// Continue to Year digit 1
						nextState = State10_Y1;
						makeTransition = TRUE;
					} else if (thisEvent.EventParam == BTN4) {			// Back to Month digit 1
						nextState = State6_M1;
						makeTransition = TRUE;
					}
					thisEvent.EventType = NO_EVENT;					
					break;
				default:
					break;
			}
			break;
		
		case State9_D2:
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
						nextState = State8_D1;
						makeTransition = TRUE;
					} else if (thisEvent.EventParam == BTN3) {			// Continue to Year digit 1
						nextState = State10_Y1;
						makeTransition = TRUE;
					} else if (thisEvent.EventParam == BTN4) {			// Back to Month digit 1
						nextState = State6_M1;
						makeTransition = TRUE;
					}
					thisEvent.EventType = NO_EVENT;					
					break;
				default:
					break;
			}
			break;
		
		case State10_Y1:
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
						nextState = State11_Y2;
						makeTransition = TRUE;
					} else if (thisEvent.EventParam == BTN3) {			// Continue to Hour digit 1
						nextState = State12_H1;
						makeTransition = TRUE;
					} else if (thisEvent.EventParam == BTN4) {			// Back to Day digit 1
						nextState = State8_D1;
						makeTransition = TRUE;
					}
					thisEvent.EventType = NO_EVENT;
					break;
				default:
					break;
			}
			break;
		
		case State11_Y2:
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
						nextState = State10_Y1;
						makeTransition = TRUE;
					} else if (thisEvent.EventParam == BTN3) {			// Continue to Hour digit 1
						nextState = State12_H1;
						makeTransition = TRUE;
					} else if (thisEvent.EventParam == BTN4) {			// Back to Day digit 1
						nextState = State8_D1;
						makeTransition = TRUE;
					}
					thisEvent.EventType = NO_EVENT;
					break;
				default:
					break;
			}
			break;

		case State12_H1:
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
						nextState = State13_H2;
						makeTransition = TRUE;
					} else if (thisEvent.EventParam == BTN3) {			// Continue to Minute digit 1
						nextState = State14_min1;
						makeTransition = TRUE;
					} else if (thisEvent.EventParam == BTN4) {			// Back to Year digit 1
						nextState = State10_Y1;
						makeTransition = TRUE;
					}
					thisEvent.EventType = NO_EVENT;
					break;
				default:
					break;
			}
			break;

		case State13_H2:
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
						nextState = State12_H1;
						makeTransition = TRUE;
					} else if (thisEvent.EventParam == BTN3) {			// Continue to Minute digit 1
						nextState = State14_min1;
						makeTransition = TRUE;
					} else if (thisEvent.EventParam == BTN4) {			// Back to Year digit 1
						nextState = State10_Y1;
						makeTransition = TRUE;
					}
					thisEvent.EventType = NO_EVENT;
					break;
				default:
					break;
			}
			break;

		case State14_min1:
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
						nextState = State15_min2;
						makeTransition = TRUE;
					} else if (thisEvent.EventParam == BTN4) {			// Back to Hour digit 1
						nextState = State12_H1;
						makeTransition = TRUE;
					}
					thisEvent.EventType = NO_EVENT;
					break;
				default:
					break;
			}
			break;

		case State15_min2:
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
						nextState = State14_min1;
						makeTransition = TRUE;
					} else if (thisEvent.EventParam == BTN3) {			// finished with configuration, sync RTC & continue back to idle
						SyncRTC(min1, min2, hour1, hour2, day1, day2, month1, month2, year1, year2);
						nextState = State2_Idle;
						makeTransition = TRUE;
					} else if (thisEvent.EventParam == BTN4) {			// Back to Hour digit 1
						nextState = State12_H1;
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

	if (makeTransition == TRUE) { // making a state transition, send EXIT and ENTRY events to allow for special on-transition behavior
		// recursively call the current state machine with an exit event before changing states for exit behavior
		thisEvent.EventType = EXIT_EVENT;
		Run_SubHSM_Wait(thisEvent);
		
		CurrentState = nextState;
		
		// recursively call the current state machine with an entry event after changing states for entry behavior
		thisEvent.EventType = ENTRY_EVENT;
		Run_SubHSM_Wait(thisEvent);
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
Event Run_SubHSM_Wait(Event thisEvent) {
	
	uint8_t makeTransition = FALSE; // use to flag transition
	WaitSubHSMStates nextState;

	switch (CurrentState) {
		case InitPSubState:								// If current state is initial Pseudo State
			if (thisEvent.EventType == INIT_EVENT){		// only respond to INIT_EVENT
				nextState = State1_Pressurizing;		// transition to first state
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
					nextState = State2_ReadingPressure;	// transitions back into itself on a data timer timeout, so new pressure can be read
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
						nextState = State3_RecordingResult;
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
				// Continue to State1_Pressurizing
				case BTN_EVENT:
					if (thisEvent.EventParam == BTN3){
						nextState = State1_Pressurizing;
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

//====================================
// Private functions
//====================================
// Includes
#include "WaitSubHSM.h"
#include "Arduino.h"
#include "EventCheckers.h"

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

// This function runs the state machine with an INIT_EVENT
// Returns TRUE if succesful, FALSE if failed
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
				nextState = State1_Pressurizing;		// transition to first state
				makeTransition = TRUE;
			}
			break;

		case State1_RecordingData:
			// record data to SD card
			break;
		
		case State2_Idle:
			// Display message to LCD
			switch (thisEvent.EventType) {
				case ENTRY_EVENT:
					// Display prompt
					sprintf(myString, "Idling          ");
					lcd.setCursor(0, 0);  // set the cursor to column 0, line 0
					lcd.print(myString);  // Print a message to the LCD
					sprintf(myString, "                ");
					lcd.setCursor(0, 1);  // set the cursor to column 0, line 1
					lcd.print(myString);  // Print a message to the LCD
					// BTN1 || BTN2 increments
				    lcd.setCursor(12, 1);
					lcd.blink();
					break;
				case BTN_EVENT:
					if (thisEvent.EventParam == BTN3) {
						nextState = State3_ConfigPeriod;
						makeTransition = TRUE;
					}
					break;
				default:
					break;
			}

			// listen for RF, if enabled
			// respond to RF, if received
			// on BTN3, go to local configuration chain
			break;
		
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
			break;
		
		case State7_M2:
			break;

		case State8_D1:
			break;
		
		case State9_D2:
			break;
		
		case State10_Y1:
			break;
		
		case State11_Y2:
			break;

		case State12_H1:
			break;

		case State13_H2:
			break;

		case State14_min1:
			break;

		case State15_min2:
			break;

		default:
			break;
	}

	if (makeTransition == TRUE) { // making a state transition, send EXIT and ENTRY events to allow for special on-transition behavior
		// recursively call the current state machine with an exit event before changing states for exit behavior
		thisEvent.EventType = EXIT_EVENT;
		Run_SubHSM_Init(thisEvent);
		
		CurrentState = nextState;
		
		// recursively call the current state machine with an entry event after changing states for exit behavior
		thisEvent.EventType = ENTRY_EVENT;
		Run_SubHSM_Init(thisEvent);
		thisEvent.EventType = NO_EVENT;
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
#endif
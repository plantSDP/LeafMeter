// Includes
#include "InitSubHSM.h"
#include "Arduino.h"
#include "EventCheckers.h"

// Private definitions
#define HUM_DANGER_THRESHOLD 90
#define HUM_WARNING_THRESHOLD 80
#define RF_NO 0
#define RF_YES 1

// List states here:
typedef enum {
    InitPSubState,
    State1_Starting,
    State2_HumConfirm,
    State3_HumFail,
    State4_HumCheck,
    State5_SettingPeriod,
	State6_SettingRF,
	State7_LifetimeDisplay,
} InitSubHSMStates;

// Holds the current state
static InitSubHSMStates CurrentState = InitPSubState;

// This function runs the state machine with an INIT_EVENT
uint8_t Init_SubHSM_Init(void){
	Event thisEvent;
	thisEvent.EventType = INIT_EVENT;
	thisEvent.EventParam = 0;
	Event returnEvent = Run_SubHSM_Init(thisEvent);
	if (returnEvent.EventType == NO_EVENT) {
		return TRUE;
	} else {
		return FALSE;
	}
}


static int hum = 0;					// holds humidity measurement
static int rfOption = RF_NO;		// sets rfOption, default is NO (0)
Event Run_SubHSM_Init(Event thisEvent) {
	
	uint8_t makeTransition = FALSE; // use to flag transition
	InitSubHSMStates nextState;


	switch (CurrentState) {
		case InitPSubState:								// If current state is initial Pseudo State
			if (thisEvent.EventType == INIT_EVENT){		// only respond to INIT_EVENT
				nextState = State1_Starting;			// transition to first state
				makeTransition = TRUE;
			}
			break;


		case State1_Starting:
			switch (thisEvent.EventType) {
				case ENTRY_EVENT:
					// Display Hello
					// Init timer
					SetTimer(0, 5000);
					// Open valves
					// Run Pump
					// request Cozir Data
					break;
				case TIMEOUT:
					if (thisEvent.EventParam == 0b10){
						digitalWrite(13,LOW);
					}
					
					if (hum < HUM_DANGER_THRESHOLD) {
						nextState = State2_HumConfirm;
						makeTransition = TRUE;
					} else {
						nextState = State3_HumFail;
						makeTransition = TRUE;	
					}
					break;
				default:
					break;
			}				
			break;

		case State2_HumConfirm:
			switch (thisEvent.EventType) {
				case ENTRY_EVENT:
					if (hum > HUM_WARNING_THRESHOLD) {
						// Display warning
					} else {
						// Display ok
					}
				break; 
				case BTN_EVENT:
					if (thisEvent.EventParam == BTN3) {			// Continue
						nextState = State5_SettingPeriod;
						makeTransition = TRUE;
					}
					break;
				default:
					break;
			}
			break;
				
		case State3_HumFail:
			switch (thisEvent.EventType) {
				case ENTRY_EVENT:
					// Display failure, prompt any btn press for retry
					break;
				case BTN_EVENT:
					nextState = State4_HumCheck;				// Continue
					makeTransition = TRUE;
					break;
				default:
					break;					
			}
			break;
			
		case State4_HumCheck:
			switch (thisEvent.EventType) {
				case ENTRY_EVENT:
					// Init timer
					// Read hum
					break;						
				case TIMEOUT:
					if (hum < HUM_DANGER_THRESHOLD) {
						nextState = State2_HumConfirm;
						makeTransition = TRUE;
					} else {
						nextState = State3_HumFail;
						makeTransition = TRUE;	
					}
					break;
				default:
					break;
			}								
			break;
				
		case State5_SettingPeriod:
			switch (thisEvent.EventType) {
				case ENTRY_EVENT:
					// Display prompt
					// Display current period
					// BTN1 || BTN2 increments
					break;
				case BTN_EVENT:
					if (thisEvent.EventParam == BTN1) {
						// increment periodParam1
						// update display

					} else if (thisEvent.EventParam == BTN2) {
						// increment periodParam2
						// update display

					} else if (thisEvent.EventParam == BTN3) {		// Continue
						nextState = State6_SettingRF;
						makeTransition = TRUE;

					} else if (thisEvent.EventParam == BTN4) {		// Back
						nextState = State4_HumCheck;
						makeTransition = TRUE;

					}
					break;
				default:
					break;
			}								
			break;
				
		case State6_SettingRF:
			switch (thisEvent.EventType) {
				case ENTRY_EVENT:
					// Display prompt
					// BTN1 yes, BTN2 no, default is no
					break;
				case BTN_EVENT:
					if (thisEvent.EventParam == BTN1) {
						rfOption = 1;

					} else if (thisEvent.EventParam == BTN2) {
						rfOption = 0;

					} else if (thisEvent.EventParam == BTN3) {		// Continue
						nextState = State7_LifetimeDisplay;
						makeTransition = TRUE;

					} else if (thisEvent.EventParam == BTN4) {		// Back
						nextState = State5_SettingPeriod;
						makeTransition = TRUE;

					}
					break;
				default:
					break;
			}					
				
			break;
				
		case State7_LifetimeDisplay:
			switch (thisEvent.EventType) {
				case ENTRY_EVENT:
					// Calculate lifetime
					// Display lifetime				
					break;
				case BTN_EVENT:
					if (thisEvent.EventParam == BTN4) {
						nextState = State5_SettingPeriod;
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
		Run_SubHSM_Init(thisEvent);
		CurrentState = nextState;
		thisEvent.EventType = ENTRY_EVENT;
		Run_SubHSM_Init(thisEvent);
	}
	return thisEvent;
}
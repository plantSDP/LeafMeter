#include "InitSubHSM.h"


#define HUM_DANGER_THRESHOLD 90
#define HUM_WARNING_THRESHOLD 80

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



static InitSubHSMStates CurrentState = InitPSubState;



uint8_t Init_SubHSM_Init(void){
	Event returnEvent;
	CurrentState = InitPSubState;
	returnEvent = Run_SubHSM_Init(INIT_EVENT);
	if (returnEvent.EventType == NO_EVENT){
		return TRUE;
	}
	return FALSE;
}


static int hum = 0;
Event Run_SubHSM_Init(Event ThisEvent) {
	uint8_t makeTransition = FALSE; // use to flag transition
	InitSubHSMStates nextState;
	static 

	
	switch (CurrentState) {
			case InitPSubState: // If current state is initial Pseudo State
				if (ThisEvent.EventType == INIT)// only respond to ES_Init
				{
					// now put the machine into the actual initial state
					nextState = State1_Starting;
					makeTransition = TRUE;

				}
				break;


			case State1_Starting:
			
				switch (ThisEvent.EventType) {
					case ENTRY_EVENT:
						printf("\r\nState1: Initing \r\n");
						// Display Hello
						// Init timer
						// Open valves
						// Run Pump
						// request Cozir Data
					break;
					case TIMEOUT:
						if (hum < HUM_DANGER_THRESHOLD) {
							nextState = State2_HumConfirm;
							makeTransition = TRUE;
						} else {
							nextState = State3_HumFail
							makeTransition = TRUE;	
						}
						break;
						
					default:
						break;
				}				
				break;

			case State2_HumConfirm:

				switch (ThisEvent.EventType) {
					case ENTRY_EVENT:
						if (hum > HUM_WARNING_THRESHOLD) {
							// Display warning
						} else {
							// Display ok
						}
					break; 
				
					// Continue
					case BTN3:
						nextState = State5_SettingPeriod;
						makeTransition = TRUE;
						break;

					default:
						break;
				}
				break;
				
			case State3_HumFail:
				// Display failure, prompt btn press for retry
				
				switch {
				
					// Continue
					case BTN3:
						nextState = State4_HumCheck;
						makeTransition = TRUE;
						break;

					default:
						break;					
				}
				break;
			
			case State4_HumCheck:

				
				switch (ThisEvent.EventType) {
					case ENTRY_EVENT:
						// Init timer
						// Read hum
						break;						
					case TIMEOUT:
						if (humOk == TRUE) {
							nextState = State2_HumConfirm;
							makeTransition = TRUE;
						} else {
							nextState = State3_HumFail
							makeTransition = TRUE;	
						}
						break;
						
					default:
						break;
				}								
				break;
				
			case State5_SettingPeriod:

				
				switch (ThisEvent.EventType) {
					case ENTRY_EVENT:
						// Display prompt
						// Display current period
						// BTN1 || BTN2 increments
						break;
				
					// Continue
					case BTN3:
						nextState = State6_SettingRF;
						makeTransition = TRUE;	
						break;
					
					// Back
					case BTN4:
						nextState = State4_HumCheck;
						makeTransition = TRUE;
						break;
						
					default:
						break;
				}								
				break;
				
			case State6_SettingRF:
				// Display prompt
				// BTN1 yes
				// BTN2 no
				
				switch (ThisEvent.EventType) {
					
					// Continue
					case BTN3:
						nextState = State7_LifetimeDisplay;
						makeTransition = TRUE;	
						break;
						
					// Back
					case BTN4:
						nextState = State5_SettingPeriod;
						makeTransition = TRUE;
						break;
						
					default:
						break;
				}					
				
				break;
				
			case State7_LifetimeDisplay:
				// Calculate lifetime
				// Display lifetime

				switch (ThisEvent.EventType) {
						
					// Back
					case BTN4:
						nextState = State5_SettingPeriod;
						makeTransition = TRUE;
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
		Run_SubHSM_Init(EXIT_EVENT); 
		CurrentState = nextState;
		Run_SubHSM_Init(ENTRY_EVENT);
	}
}
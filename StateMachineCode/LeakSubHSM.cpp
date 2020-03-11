#include "LeakSubHSM.h"


#define HUM_DANGER_THRESHOLD 90
#define HUM_WARNING_THRESHOLD 80

typedef enum {
    InitPSubState,
    State1_Starting,

} LeakSubHSMStates;

static LeakSubHSMStates CurrentState = InitPSubState;



uint8_t Init_SubHSM_Leak(void){
	Event thisEvent;
	thisEvent.EventType = INIT_EVENT;
	thisEvent.EventParam = 0;
	Event returnEvent = Run_SubHSM_Init(thisEvent);
	if (returnEvent.EventType == NO_EVENT){
		return TRUE;
	} else {
		return FALSE;
	}
}


static int hum = 0;
Event Run_SubHSM_Leak(Event thisEvent) {
	
	uint8_t makeTransition = FALSE; // use to flag transition
	InitSubHSMStates nextState;


	switch (CurrentState) {
		case InitPSubState: // If current state is initial Pseudo State
			if (thisEvent.EventType == INIT_EVENT){// only respond to ES_Init
				// now put the machine into the actual initial state
				nextState = State1_Starting;
				makeTransition = TRUE;
			}
			break;


		case State1_Starting:
			switch (thisEvent.EventType) {
				case ENTRY_EVENT:
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
						nextState = State3_HumFail;
						makeTransition = TRUE;	
					}
					break;
						
				default:
					pinMode(13, OUTPUT);
					digitalWrite(13, HIGH);
					thisEvent.EventType = NO_EVENT;
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
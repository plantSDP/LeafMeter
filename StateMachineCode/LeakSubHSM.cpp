#include "LeakSubHSM.h"


#define HUM_DANGER_THRESHOLD 90
#define HUM_WARNING_THRESHOLD 80

#define PRESSURE_TEST_THRESHOLD 10 // just a placeholder for now

typedef enum {
    InitPSubState,
    State1_Starting,

} LeakSubHSMStates;

static LeakSubHSMStates CurrentState = InitPSubState;


// This function runs the state machine with an INIT_EVENT
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
static int pressure= 0;
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


		case State1_ContinuePrompt:
			switch (thisEvent.EventType) {
				case ENTRY_EVENT:
					// Display "continue with leak check" prompt
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
					// Display "Pressurizing"
					// Run pump
					// Close valves
					// Read cozir data
					break;
				// Transition to State3 when pressure reaches testing threshold. This number is defined above.
				// Transition to State5 when humidity is greater than threshold. This number is defined above.
			}
		
		case State3_ReadingPressure:
			break;

		case State4_DisplayingResult:
			break;

		case State5_HumDanger:
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
#include "PlantMeterHSM.h"
#include "Configure.h"
#include "InitSubHSM.h"


typedef enum {
	InitPState, //initilizing state maching pseudo state
    Initing, //initializing the device
    PressureChecking, //checking for a leak
	RFChecking, //checking for RF connection
	Active, // Running a measurment
	Waiting, //waiting for next measurement or user input
} HSMstates;


static HSMstates CurrentState = InitPState;

uint8_t InitHSM(void){
	Event thisEvent;
	thisEvent.EventType = INIT_EVENT;
	thisEvent.EventParam = 0;
	Event returnEvent = RunHSM(thisEvent);
	if (returnEvent.EventType == NO_EVENT){
		return TRUE;
	}else{
		return FALSE;
	}
	
}




Event RunHSM(Event thisEvent){
	
	uint8_t makeTransition = FALSE;
	HSMstates nextState;
	
	switch (CurrentState){
		case InitPState:
			if (thisEvent.EventType == INIT_EVENT){
				//any initializations that should start. Maybe some blocking code. This code could also go in the main function 
				Init_SubHSM_Init();
				nextState = Initing;
				makeTransition = TRUE;	
			}
			
			break;

		case Initing:
		    pinMode(13, OUTPUT);
			digitalWrite(13, HIGH);
			thisEvent = Run_SubHSM_Init(thisEvent);
			if (thisEvent.EventType == BTN3){
				nextState = PressureChecking;
				makeTransition = TRUE;
			}
			break;
		case PressureChecking:
			//pinMode(13, OUTPUT);
			//digitalWrite(13, HIGH);
			break;
		case RFChecking:
			break;
		case Active:
			break;
		case Waiting:
			break;
			
	}
	if (makeTransition == TRUE){
		thisEvent.EventType = EXIT_EVENT;
		RunHSM(thisEvent);
		CurrentState = nextState;
		thisEvent.EventType = ENTRY_EVENT;
		RunHSM(thisEvent);
	}
	return thisEvent;
	
	
	
}


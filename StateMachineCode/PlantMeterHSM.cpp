#include "PlantMeterHSM.h"
#inlcude "Configure.h"


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
	Event ThisEvent;
	ThisEvent.EventType = init;
	ThisEvent.EventParam = 0;
	if (RunHSM(ThisEvent)){
		return TRUE;
	}else{
		return FALSE;
	}
}




uint8_t RunHSM(Event thisEvent){
	
	uint8_t makeTransition = FALSE;
	HSMstates nextState;
	
	switch (currentState){
		case InitPstate:
			if (thisEvent.EventType != )
		break;
		case Initing:
		break;
		case PressureChecking:
		break;
		case RFChecking:
		break;
		case Active:
		break;
		case Waiting:
		break;
	}
	
	
	
}


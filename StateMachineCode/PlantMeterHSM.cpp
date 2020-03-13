#include "PlantMeterHSM.h"
#include "Configure.h"
#include "InitSubHSM.h"
#include <LiquidCrystal_I2C.h>
#include <Wire.h>

typedef enum {
	InitPState,		// initilizing state machine pseudo state
    Initing,		// initializing the device
    LeakChecking,	// checking for a leak
	RFChecking,		// checking for RF connection
	DateTime,		// entering date and time
	Active,			// running a measurment
	Waiting,		// waiting for next measurement or user input
} HSMstates;



static HSMstates CurrentState = InitPState;
char myString2[50]; 

uint8_t InitHSM(void){
	
	  sprintf(myString2, "Hello please");
  lcd.setCursor(0, 0); // set the cursor to column 0, line 0
  lcd.print(myString2);  // Print a message to the LCD
	Event thisEvent;
	thisEvent.EventType = INIT_EVENT;
	thisEvent.EventParam = 0;
	Event returnEvent = RunHSM(thisEvent);
	if (returnEvent.EventType == NO_EVENT) {
		return TRUE;
	} else {
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
			thisEvent = Run_SubHSM_Init(thisEvent);
			if (thisEvent.EventType == BTN_EVENT){
				if (thisEvent.EventParam == BTN3) {
					nextState = LeakChecking;
					makeTransition = TRUE;
				}
			}
			break;

		case LeakChecking:
			//pinMode(13, OUTPUT);
			//digitalWrite(13, HIGH);
			break;
		case RFChecking:
			break;
		case DateTime:
			break;
		case Active:
			break;
		case Waiting:
			break;
			
	}

	if (makeTransition == TRUE) {
		thisEvent.EventType = EXIT_EVENT;
		RunHSM(thisEvent);
		CurrentState = nextState;
		thisEvent.EventType = ENTRY_EVENT;
		RunHSM(thisEvent);
	}
	return thisEvent;
	
	
	
}


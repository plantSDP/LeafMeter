// Includes
#include "ActiveSubHSM.h"

// Private Definitions
#define TIMER_DATA 1
#define TIMER_DATA_PARAM 0b10			// Timer 0 is in use as the active duration, so Timer 1 is used for data sampling

File dataFile;				// used to open a new datafile on the SD card

// List states here:
typedef enum {
    InitPSubState,
	State0_Failure,
    State1_StartingActive,				// Closes valves
    State2_TakingMeasurement1,			// Inits sampling timer and waits for a timeout
	State3_TakingMeasurement2,			// Takes a sample, records the data
} ActiveSubHSMStates;

// Holds current state
static ActiveSubHSMStates CurrentState = InitPSubState;

/*
This function initializes the state machine with an INIT_EVENT. 
In regards to the state machine, it transitions the machine out of the initial pseudostate and performs one-time setup functions

Parameters: uint8_t resetFlag, if true: current state will be reset to the starting pseudostate, if false, current state is unchanged. Used to start a new active cycle
Return: TRUE on success, FALSE on failure
*/
uint8_t Init_SubHSM_Active(uint8_t resetFlag){
	if (resetFlag == TRUE) {
		CurrentState = InitPSubState;		// special modification for the Active sub-statemachine, used to reset the current state to the starting pseudostate.
	}
	
	Event thisEvent;
	thisEvent.EventType = INIT_EVENT;
	thisEvent.EventParam = 0;
	Event returnEvent = Run_SubHSM_Active(thisEvent);
	if (returnEvent.EventType == NO_EVENT) {
		return TRUE;
	} else {
		return FALSE;
	}
}

/*
This function contains the nested switch statement that implements the state machine.
It requires an Event as a parameter.
If the event is consumed/handled in the state machine, then the function returns NO_EVENT
If the event is not consumed/handled, then the event is returned unchanged.
*/
Event Run_SubHSM_Active(Event thisEvent) {
	
	uint8_t makeTransition = FALSE; // use to flag transition
	ActiveSubHSMStates nextState;	// use to indicate next state

	switch (CurrentState) {
		case InitPSubState:								// If current state is initial Pseudo State
			if (thisEvent.EventType == INIT_EVENT) {	// only respond to INIT_EVENT
				nextState = State1_StartingActive;		// transition to first state
				makeTransition = TRUE;
			}
			break;
		
		case State0_Failure:
			sprintf(myString, "Sensor Failure");
			lcd.setCursor(0, 0); // set the cursor to column 0, line 0
			lcd.print(myString);  // Print a message to the LCD
			thisEvent.EventType = NO_EVENT;
			break;

		case State1_StartingActive:
			switch (thisEvent.EventType) {
				case ENTRY_EVENT:
					// Close valves, run pump
					
					dataArrayAddress = 0;	// reset the data array address for a new measurement cycle
					numSamples = 0;			// reset the number of samples taken

					nextState = State2_TakingMeasurement1;
					makeTransition = TRUE;
					break;
				default:
					break;
			}
			break;
		
		case State2_TakingMeasurement1:
			switch (thisEvent.EventType) {
				case ENTRY_EVENT:
					// init timer
					SetTimer(TIMER_DATA, SAMPLING_FREQ); 	// sampling frequency timer

					Cozir_Request_Data();	// it takes around 70-100 ms for cozir to send this data. The min sampling freq is 500 ms, so this shouldn't be a prob

					thisEvent.EventType = NO_EVENT;
					break;
				case TIMEOUT:
					if (thisEvent.EventParam == TIMER_DATA_PARAM) {
						nextState = State3_TakingMeasurement2;
						makeTransition = TRUE;
					}
					thisEvent.EventType = NO_EVENT;
					break;
				default:
					break;
			}
			break;

		case State3_TakingMeasurement2:
			switch (thisEvent.EventType) {
				case ENTRY_EVENT:
					Cozir_NewDataAvailable();
					pressureSensor.readSensor();

					// read data
					co2 = Cozir_Get_Co2();
					hum = Cozir_Get_Rh();
					temp = Cozir_Get_Temp();

					pres = pressureSensor.getPressure_Pa();

					lux = GetLux(lightSensor.getFullLuminosity());

					// record data into data arrays
					co2Data[dataArrayAddress] = co2;
					// humData[dataArrayAddress] = hum;
					// tempData[dataArrayAddress] = temp;
					// presData[dataArrayAddress] = pres;
					// luxData[dataArrayAddress] = lux;

					// increment data array address
					if (dataArrayAddress < MAX_SAMPLES_PER_CYCLE) {
						dataArrayAddress = dataArrayAddress + 1;
					}
					
					// create data string for SD card .txt file
					char dataString[50];
					sprintf(dataString, "%04d\t%02d\t%03d\t%06d\t%06u\n", co2, hum, temp, pres, lux);
					Serial.println(dataString);

					dataFile = SD.open(fileName, FILE_WRITE);
					// if the file is available, write the data string to it:
					if (dataFile) {
						dataFile.println(dataString);
						dataFile.close();
						Serial.println("wrote to datafile");
						// print to the serial port too:
						// Serial.println(myString);
					} else {
						dataFile.close();
						Serial.println("failed to write to datafile");
					}
					numSamples = numSamples + 1;		// keep a record of how many samples are recorded to the .txt file

					thisEvent.EventType = NO_EVENT;
					break;
				case TIMEOUT:
					if (thisEvent.EventParam == TIMER_DATA_PARAM) {
						nextState = State3_TakingMeasurement2;
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
		Run_SubHSM_Active(thisEvent);
		
		CurrentState = nextState;
		
		// recursively call the current state machine with an entry event after changing states for entry behavior
		thisEvent.EventType = ENTRY_EVENT;
		Run_SubHSM_Active(thisEvent);
		thisEvent.EventType = NO_EVENT;	// Transitions are only triggered by events being handled, so the return must be NO_EVENT
	}
	return thisEvent;
}

//=============================
// Private Functions
//=============================

  
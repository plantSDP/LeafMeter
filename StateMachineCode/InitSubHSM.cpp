// Includes
#include "InitSubHSM.h"

//===================================================================================================================
// InitSubHSM implements the top-level HSM for the field unit via two functions: Init_SubHSM_Init and Run_SubHSM_Init
//===================================================================================================================

//=====================
// Private definitions
//=====================

// This enum lists the names of all states in this state machine
typedef enum {
    InitPSubState,					// initial pseudostate to implement one-time startup behavior, the light and pressure sensors are initialized here
	State0_Failure,					// l or p sensor initialization failure 
    State1_Starting,				// Co2 sensor startup
    State2_HumConfirm,				// take humidity reading, display results
    State3_HumFail,					// humidity too high, warn user to alleviate and prompt humidity reread
    State4_HumCheck,				// humidity rereading, display results
    State5_SettingPeriod,			// prompt user to set period between measurements
	State6_SettingNumCycles,		// prompt user to set number of measurement cycles
	State7_SettingRF,				// prompt user to set RF option
	State8_LifetimeDisplay,			// calculate and display expected lifetime
} InitSubHSMStates;

// Holds the current state
static InitSubHSMStates CurrentState = InitPSubState;

/*
This function initializes the state machine with an INIT_EVENT. 
In regards to the state machine, it transitions the machine out of the initial pseudostate and performs one-time setup functions

Parameters: none
Return: TRUE on success, FALSE on failure
*/
uint8_t Init_SubHSM_Init(uint8_t resetFlag){
	if (resetFlag == TRUE) {
		CurrentState = InitPSubState;		// special modification to the init substate machine, used to reset the current state in when the device has completed all cycles
	}

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

//static unsigned int hum = 0;		// holds humidity measurement
//static int rfOption = RF_NO;		// sets rfOption, default is NO (0)
//static int period = 60;				// holds value for period in between measurements


/* 
Contains a nested switch-case that implements the state machine.

Parameters: Event thisEvent - a struct that contains an EventType and EventParameter, defined in Configure.h
Return: NO_EVENT if thisEvent is handled/consumed, return thisEvent unchanged if thisEvent not consumed 
*/
Event Run_SubHSM_Init(Event thisEvent) {
	
	uint8_t makeTransition = FALSE; // use to flag transition
	InitSubHSMStates nextState;


	switch (CurrentState) {
		case InitPSubState:								// If current state is initial Pseudo State
			if (thisEvent.EventType == INIT_EVENT){		// only respond to INIT_EVENT
				nextState = State1_Starting;			// transition to first state
				
			    if (lightSensor.begin() <= 0){			// Light sensor error checking
					nextState = State0_Failure;
				} else {
					lightSensor.setGain(TSL2591_GAIN_LOW);
					lightSensor.setTiming(TSL2591_INTEGRATIONTIME_100MS);
				}
				
				if (pressureSensor.begin() < 0){		// Pressure sensor error checking
					nextState = State0_Failure;
				} else {
					pressureSensor.setNormalMode();
				}
				makeTransition = TRUE;					// Always transition out of the initial pseudostate
			}
			break;
		
		case State0_Failure:
			sprintf(myString, "Failure      ");
			lcd.setCursor(0, 0); // set the cursor to column 0, line 0
			lcd.print(myString);  // Print a message to the LCD	
			thisEvent.EventType = NO_EVENT;
			break;

		case State1_Starting:
			switch (thisEvent.EventType) {
				case ENTRY_EVENT:
					// Display Hello
					sprintf(myString, "Co2 Flux Meter  ");
					lcd.setCursor(0, 0); // set the cursor to column 0, line 0
					lcd.print(myString);  // Print a message to the LCD
					sprintf(myString, "Starting...     ");
					lcd.setCursor(0, 1); // set the cursor to column 0, line 1
					lcd.print(myString);  // Print a message to the LCD

					// Init timer
					SetTimer(1, 10000); // cozir warmup, 10 sec
					
					// Open valves
					// Run Pump
					thisEvent.EventType = NO_EVENT;
					break;
				case TIMEOUT:
					if (thisEvent.EventParam == TIMER_0_PARAM) {			// If Timer 0 has timed out, Cozir has sent initial hum data after the three sensor initializations
						Cozir_NewDataAvailable();
						hum = Cozir_Get_Rh();
						if (hum < HUM_DANGER_THRESHOLD) {
							nextState = State2_HumConfirm;
							makeTransition = TRUE;
						} else {
							nextState = State3_HumFail;
							makeTransition = TRUE;	
						}
					} else if (thisEvent.EventParam == TIMER_1_PARAM) {		// if Timer 1 has timed out, 10sec cozir warmup is done
						if ((Cozir_Init()) && (lightSensor.begin()) && (pressureSensor.begin() && SD.begin(SD_CHIP_SELECT))) {	// initializations for the three sensors
							// Request humidity data from Cozir
							Cozir_Request_Data();
							SetTimer(0, 100); 		// It takes around 70-100ms for the Cozir to send data after a request

							// tsl startup settings
							lightSensor.setGain(TSL2591_GAIN_LOW);
							lightSensor.setTiming(TSL2591_INTEGRATIONTIME_100MS);

							// bme280 startup settings
							pressureSensor.setNormalMode();

						} else {
							nextState = State0_Failure;						// if any sensors fail, transition to failure state
							makeTransition = TRUE;
						}
					}
					thisEvent.EventType = NO_EVENT;
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
						sprintf(myString, "HUM WARNING, %d      ", hum);
						lcd.setCursor(0, 0);  // set the cursor to column 0, line 0
						lcd.print(myString);  // Print a message to the LCD	
						sprintf(myString, "BTN3 CONTINUE           ");
						lcd.setCursor(0, 1);  // set the cursor to column 0, line 1
						lcd.print(myString);  // Print a message to the LCD							
					} else {
						// Display ok
						sprintf(myString, "HUM OK, %dRH          ", hum);
						lcd.setCursor(0, 0);  // set the cursor to column 0, line 0
						lcd.print(myString);  // Print a message to the LCD
					    sprintf(myString, "   BTN3 CONTINUE      ");
						lcd.setCursor(0, 1);  // set the cursor to column 0, line 1
						lcd.print(myString);  // Print a message to the LCD
					}
					thisEvent.EventType = NO_EVENT;
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
					sprintf(myString, "HUM DANGER, %dRH        ", hum);
					lcd.setCursor(0, 0); // set the cursor to column 0, line 0
					lcd.print(myString);  // Print a message to the LCD
					sprintf(myString, "BTN3 CONTINUE         ");
					lcd.setCursor(0, 1); // set the cursor to column 0, line 0
					lcd.print(myString);  // Print a message to the LCD

					thisEvent.EventType = NO_EVENT;
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
					sprintf(myString, "Checking         ");
					lcd.setCursor(0, 0); 					// set the cursor to column 0, line 0
					lcd.print(myString);  					// Print a message to the LCD
					sprintf(myString, "      Humidity   ");
					lcd.setCursor(0, 1); 					// set the cursor to column 0, line 0
					lcd.print(myString);  					// Print a message to the LCD
					
					SetTimer(0, 5000);		// Init timer
					Cozir_Request_Data();	// requests sensor for new data

					thisEvent.EventType = NO_EVENT;
					break;						
				case TIMEOUT:
					Cozir_NewDataAvailable();
					hum = Cozir_Get_Rh();
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
						nextState = State6_SettingNumCycles;
						makeTransition = TRUE;

					} else if (thisEvent.EventParam == BTN4) {		// Back
						nextState = State4_HumCheck;
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
			
		case State6_SettingNumCycles:
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
						nextState = State7_SettingRF;
						makeTransition = TRUE;

					} else if (thisEvent.EventParam == BTN4) {		// Back
						nextState = State5_SettingPeriod;
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
				
		case State7_SettingRF:
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
						nextState = State8_LifetimeDisplay;
						makeTransition = TRUE;

					} else if (thisEvent.EventParam == BTN4) {		// Back
					    lcd.noBlink();
						nextState = State6_SettingNumCycles;
						makeTransition = TRUE;

					}
					thisEvent.EventType = NO_EVENT;
					break;
				default:
					break;
			}					
			break;
				
		case State8_LifetimeDisplay:
			switch (thisEvent.EventType) {
				case ENTRY_EVENT:
					// Calculate lifetime
					// Display lifetime
					sprintf(myString, "Est. Lifetime:        ");
					lcd.setCursor(0, 0);  // set the cursor to column 0, line 0
					lcd.print(myString);  // Print a message to the LCD
					sprintf(myString, "    x [Hours]     ");
					lcd.setCursor(0, 1);  // set the cursor to column 0, line 1
					lcd.print(myString);  // Print a message to the LCD

					thisEvent.EventType = NO_EVENT;
					break;
				case BTN_EVENT:
					if (thisEvent.EventParam == BTN4) {
						nextState = State7_SettingRF;
						makeTransition = TRUE;
						thisEvent.EventType = NO_EVENT;
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
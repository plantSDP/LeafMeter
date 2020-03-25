// Includes
#include "InitSubHSM.h"
#include "Arduino.h"
#include "Configure.h"
#include "EventCheckers.h"



// Private definitions
#define HUM_DANGER_THRESHOLD 90
#define HUM_WARNING_THRESHOLD 80
#define RF_NO 0
#define RF_YES 1

// List states here:
typedef enum {
    InitPSubState,
	State0_Failure,
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

static unsigned int hum = 0;		// holds humidity measurement
static int rfOption = RF_NO;		// sets rfOption, default is NO (0)
static int period = 60;				// holds value for period in between measurements
Event Run_SubHSM_Init(Event thisEvent) {
	
	uint8_t makeTransition = FALSE; // use to flag transition
	InitSubHSMStates nextState;


	switch (CurrentState) {
		case InitPSubState:								// If current state is initial Pseudo State
			if (thisEvent.EventType == INIT_EVENT){		// only respond to INIT_EVENT
				nextState = State1_Starting;			// transition to first state
			    if (lightSensor.begin() <= 0){
					nextState = State0_Failure;
				} else {
					lightSensor.setGain(TSL2591_GAIN_LOW);
					lightSensor.setTiming(TSL2591_INTEGRATIONTIME_100MS);
				}
				if (pressureSensor.begin() < 0){
					nextState = State0_Failure;
				} else {
					pressureSensor.setNormalMode();
				}
	

				
				makeTransition = TRUE;
			}
			break;
		
		case State0_Failure:
			sprintf(myString, "Sensor Failure");
			lcd.setCursor(0, 0); // set the cursor to column 0, line 0
			lcd.print(myString);  // Print a message to the LCD	
			thisEvent.EventType = NO_EVENT;
			break;

		case State1_Starting:
			switch (thisEvent.EventType) {
				case ENTRY_EVENT:
					// Display Hello
					sprintf(myString, "Starting...");
					lcd.setCursor(0, 0); // set the cursor to column 0, line 0
					lcd.print(myString);  // Print a message to the LCD
					// Init timer
					SetTimer(1, 10000); // cozir
					
					// Open valves
					// Run Pump
					// request Cozir Data
					
					break;
				case TIMEOUT:
					if (thisEvent.EventParam == TIMER_0_PARAM) {
						Cozir_NewDataAvailable();
						hum = Cozir_Get_Rh();
						if (hum < HUM_DANGER_THRESHOLD) {
							nextState = State2_HumConfirm;
							makeTransition = TRUE;
						} else {
							nextState = State3_HumFail;
							makeTransition = TRUE;	
						}
					} else if (thisEvent.EventParam == TIMER_1_PARAM) {	
						sprintf(myString, "timer 1");
						Serial.println(myString);					
						if (Cozir_Init() == 0){
							nextState = State0_Failure;
							makeTransition = TRUE;
						} else {
							Cozir_Request_Data();
							SetTimer(0, 100); //It takes around 70-100ms for the Cozir to send data 
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
						lcd.setCursor(0, 0); // set the cursor to column 0, line 0
						lcd.print(myString);  // Print a message to the LCD	
						sprintf(myString, "BTN3 CONTINUE           ");
						lcd.setCursor(0, 1); // set the cursor to column 0, line 0
						lcd.print(myString);  // Print a message to the LCD							
					} else {
						// Display ok
						sprintf(myString, "HUM OK, %d          ", hum);
						lcd.setCursor(0, 0); // set the cursor to column 0, line 0
						lcd.print(myString);  // Print a message to the LCD
					    sprintf(myString, "   BTN3 CONTINUE      ");
						lcd.setCursor(0, 1); // set the cursor to column 0, line 0
						lcd.print(myString);  // Print a message to the LCD
					}
				break; 
				case BTN_EVENT:
					if (thisEvent.EventParam == BTN3) {			// Continue
						nextState = State5_SettingPeriod;
						makeTransition = TRUE;
					}
					thisEvent.EventType = NO_EVENT;
					break;
				default:
					break;
			}
			break;
				
		case State3_HumFail:
			switch (thisEvent.EventType) {
				case ENTRY_EVENT:
					// Display failure, prompt any btn press for retry
					sprintf(myString, "HUM DANGER, %d        ", hum);
					lcd.setCursor(0, 0); // set the cursor to column 0, line 0
					lcd.print(myString);  // Print a message to the LCD
					sprintf(myString, "BTN3 CONTINUE         ");
					lcd.setCursor(0, 1); // set the cursor to column 0, line 0
					lcd.print(myString);  // Print a message to the LCD
					break;
				case BTN_EVENT:
					nextState = State4_HumCheck;				// Continue
					makeTransition = TRUE;
					thisEvent.EventType = NO_EVENT;
					break;
				default:
					break;					
			}
			break;
			
		case State4_HumCheck:
			switch (thisEvent.EventType) {
				case ENTRY_EVENT:
					// Init timer
					sprintf(myString, "Checking         ");
					lcd.setCursor(0, 0); // set the cursor to column 0, line 0
					lcd.print(myString);  // Print a message to the LCD
					sprintf(myString, "      Humidity   ");
					lcd.setCursor(0, 1); // set the cursor to column 0, line 0
					lcd.print(myString);  // Print a message to the LCD
					SetTimer(0, 5000);
					Cozir_Request_Data();
					// Read hum

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
					thisEvent.EventType = NO_EVENT;
					break;
				default:
					break;
			}								
			break;
				
		case State5_SettingPeriod:
			switch (thisEvent.EventType) {
				case ENTRY_EVENT:
					// Display prompt
					sprintf(myString, "Min btwn meas,");
					lcd.setCursor(0, 0); // set the cursor to column 0, line 0
					lcd.print(myString);  // Print a message to the LCD
					sprintf(myString, "          %3d        ", period);
					lcd.setCursor(0, 1); // set the cursor to column 0, line 0
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
						lcd.setCursor(0, 0); // set the cursor to column 0, line 0
						lcd.print(myString);  // Print a message to the LCD
						sprintf(myString, "          %3d     ", period);
						lcd.setCursor(0, 1); // set the cursor to column 0, line 0
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
						lcd.setCursor(0, 1);  // set the cursor to column 0, line 0
						lcd.print(myString);  // Print a message to the LCD
						lcd.setCursor(12, 1);
					} else if (thisEvent.EventParam == BTN3) {		// Continue
						nextState = State6_SettingRF;
						makeTransition = TRUE;

					} else if (thisEvent.EventParam == BTN4) {		// Back
						nextState = State4_HumCheck;
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
				
		case State6_SettingRF:
			switch (thisEvent.EventType) {
				case ENTRY_EVENT:
					// Display prompt
					sprintf(myString, "Set RF option:");
					lcd.setCursor(0, 0); // set the cursor to column 0, line 0
					lcd.print(myString);  // Print a message to the LCD
					
					sprintf(myString, "NO       YES          ");
					lcd.setCursor(0, 1); // set the cursor to column 0, line 0
					lcd.print(myString);  // Print a message to the LCD
					
					lcd.setCursor(0, 1);
					lcd.blink();
					// BTN1 yes, BTN2 no, default is no
					break;
				case BTN_EVENT:
					if (thisEvent.EventParam == BTN1) {
						rfOption = 1;
						sprintf(myString, "Set RF option:");
						lcd.setCursor(0, 0); // set the cursor to column 0, line 0
						lcd.print(myString);  // Print a message to the LCD
					
						sprintf(myString, "NO       YES          ");
						lcd.setCursor(0, 1); // set the cursor to column 0, line 0
						lcd.print(myString);  // Print a message to the LCD
						
						lcd.setCursor(9, 1);

					} else if (thisEvent.EventParam == BTN2) {
						rfOption = 0;
						sprintf(myString, "Set RF option:");
						lcd.setCursor(0, 0); // set the cursor to column 0, line 0
						lcd.print(myString);  // Print a message to the LCD
					
						sprintf(myString, "NO       YES          ");
						lcd.setCursor(0, 1); // set the cursor to column 0, line 0
						lcd.print(myString);  // Print a message to the LCD
						
						lcd.setCursor(0, 1);

					} else if (thisEvent.EventParam == BTN3) {		// Continue
					    lcd.noBlink();
						nextState = State7_LifetimeDisplay;
						makeTransition = TRUE;

					} else if (thisEvent.EventParam == BTN4) {		// Back
					    lcd.noBlink();
						nextState = State5_SettingPeriod;
						makeTransition = TRUE;

					}
					thisEvent.EventType = NO_EVENT;
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
					sprintf(myString, "Est. Lifetime:        ");
					lcd.setCursor(0, 0); // set the cursor to column 0, line 0
					lcd.print(myString);  // Print a message to the LCD
					sprintf(myString, "    x [Hours]     ");
					lcd.setCursor(0, 1); // set the cursor to column 0, line 0
					lcd.print(myString);  // Print a message to the LCD
					break;
				case BTN_EVENT:
					if (thisEvent.EventParam == BTN4) {
						nextState = State6_SettingRF;
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
		
	if (makeTransition == TRUE) { // making a state transition, send EXIT and ENTRY
		// recursively call the current state with an exit event
		thisEvent.EventType = EXIT_EVENT;
		Run_SubHSM_Init(thisEvent);
		CurrentState = nextState;
		thisEvent.EventType = ENTRY_EVENT;
		Run_SubHSM_Init(thisEvent);
		thisEvent.EventType = NO_EVENT;
	}
	return thisEvent;
}
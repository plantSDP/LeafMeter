// IDE and compiler: Arduino

#ifndef CONFIGURE_H			// header shield, protects against multiple #includes
#define CONFIGURE_H


#include "Arduino.h"				// contains declarations for hardware mapping pins, etc. for Arduino microprocessors
#include "LiquidCrystal_I2C.h"		// lcd screen library, specifically I2C
#include <Adafruit_Sensor.h>		// contains declarations for Adafruit sensors
#include "Adafruit_TSL2591.h"		// light sensor

// This library was modified from an existing sensor library
#include "BME280.h"					// Pressure sensor

// This library is created by us
#include "CozirLib.h"				// Co2 sensor

//===================================================================================================================
// Configure.h holds global definitions, structs, objects, needed by other parts of the framework (HSM, EventCheckers)
//===================================================================================================================

//===================================
// Public Definitions & Global Things
//===================================
#define TRUE 1
#define FALSE 0

// Global Objects
extern char myString[50]; 				// for screen printing I2C
extern LiquidCrystal_I2C lcd;

extern BME280 pressureSensor;			// for pressure sensor I2C
extern Adafruit_TSL2591 lightSensor;	// for light sensor I2C

// Button EventParam Definitions used by the HSM
#define NO_PRESS	0b0
#define BTN1		0b1
#define BTN2		0b10
#define BTN3		0b100
#define BTN4		0b1000

// Timer EventParam Definitions used by the HSM
#define TIMER_0_PARAM 			0b1		
#define TIMER_1_PARAM 			0b10	 


// Humidity thresholds used by HSM, measured in RH
#define HUM_DANGER_THRESHOLD 	90
#define HUM_WARNING_THRESHOLD 	80
static unsigned int hum = 0;		// holds humidity measurement


// RF options used by HSM
#define RF_YES 	1
#define RF_NO 	0
static int rfOption = RF_NO;		// sets rfOption, default is NO (0)

// Active and Waiting Periods
#define ACTIVE_DURATION 600000		// duration of active cycle in [ms] (default 600000[ms] = 10[min])
static int period = 60;				// holds value for period in between measurements in [min], default is 60[min]

// Number of measurement cycles
static int numCycles = 1;			// holds value for number of measurement cycles, default is 1

// Listing of all possible event types for the state machine. Add new events here.
typedef enum {
	NO_EVENT,						// Used when events are consumed or when no event occurs
	
	ENTRY_EVENT,					// Used on transition from one state to another, mainly for on-transition behavior
	EXIT_EVENT,
	
	INIT_EVENT,						// Used once on state machine initialization, all state machines transition out of the initial pseudostate with this event
	
	BTN_EVENT,						// Used when any button status changes are detected
	TIMEOUT							// Used when any timer expires
} eventTypes;

/*
The Event struct holds the EventType: an identifier defined in the enum above, and EventParam: an additional 16-bit identifier
These events will be the arguments and returns for the HSM and sub-HSM functions
The event checkers generate these events with a specific EventType and EventParam
*/
typedef struct Events{
	eventTypes EventType;
	int EventParam;
}Event;


#endif

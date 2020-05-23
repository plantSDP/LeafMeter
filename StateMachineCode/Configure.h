#ifndef CONFIGURE_H			// header shield, protects against multiple #includes
#define CONFIGURE_H

#include "Arduino.h"				// contains declarations for hardware mapping pins, etc. for Arduino microprocessors

#include "LiquidCrystal_I2C.h"		// lcd screen library, specifically I2C

#include <Adafruit_Sensor.h>		// contains definitions/declarations for Adafruit sensors
#include "Adafruit_TSL2591.h"		// Light sensor

#include "ds3231.h"					// library for the RTC

// This library was modified from an existing sensor library
#include "BME280.h"					// Pressure sensor

// This library is created by the Plant CO2 Flux Meter Team 2019-20 for the CozIR-A 2000PPM CO2+RH/T Sensor
#include "CozirLib.h"				// Co2 sensor

//====================================================================================================================
// Configure.h holds global definitions, structs, objects, needed by other parts of the framework (HSM, EventCheckers)
//====================================================================================================================

//===================================
// Public Definitions & Global Things
//===================================
#define TRUE 1
#define FALSE 0

// Special debugging/development defines
#define NO_PRESSURE_TEST // turns off pressure testing, affects LeakSubHSM and WaitSubHSM
						 // as of 5/10/2020, pressure testing has not been implemented fully and if NO_PRESSURE_TEST is undefined the resulting code is unstable

#define NO_RF_TEST		 // turns off RF testing, affects RFSubHSM
						 // as of 5/10/2020, RF testing has not been implemented fully, and if NO_RF_TEST is undefined the resulting code is unstable

// Global Hardware Objects
extern char myString[50]; 				// Initializes a string for storing messages to be printed to LCD screen
extern LiquidCrystal_I2C lcd;			// Initializes global LCD screen class

extern BME280 pressureSensor;			// Initializes global BME280 class
extern Adafruit_TSL2591 lightSensor;	// Initializes global TSL2591 class

// Button EventParam Definitions used by the HSM, used to differentiate BTN_EVENT
#define NO_PRESS			0b0
#define BTN1				0b1
#define BTN2				0b10
#define BTN3				0b100
#define BTN4				0b1000

// Timer EventParam Definitions used by the HSM, used to differentiate TIMEOUT events
#define TIMER_0_PARAM 		0b1	
#define TIMER_1_PARAM 		0b10
#define TIMER_2_PARAM		0b100

// Humidity thresholds used by HSM, measured in RH
#define HUM_DANGER_THRESHOLD 	90
#define HUM_WARNING_THRESHOLD 	80

// Pressure thresholds used by HSM, measured in Pa
#define PRESSURE_TEST_THRESHOLD 10 	// just a placeholder for now

// RF options used by HSM
#define RF_YES 	1
#define RF_NO 	0
static int rfOption = RF_NO;		// sets rfOption, default is NO (0)

// Active and Waiting Periods, Sampling Frequency
#define ACTIVE_DURATION 600000		// duration of active cycle in [ms], default 600000[ms] = 10[min]
static int period = 60;				// holds value for period in between measurements in [min], default is 60[min] but can be user-defined
#define SAMPLING_FREQ 550			// sampling frequency in [ms], the minimum is 500[ms]
#define MAX_SAMPLES_PER_CYCLE 120	// ACTIVE_DURATION / MIN_SAMPLING_FREQ

// Number of measurement cycles
static int numCycles = 1;			// holds value for number of measurement cycles, default is 1

// Global microclimate variables
static unsigned int co2 = 0;		// holds CO2 measurement in ppm from Cozir, default is 0

static int hum = 0;					// holds humidity measurement in RH from BME280, default is 0
static int temp = 0;				// holds temperature mesurement in C from BME280, default is 0
static int pres = 0;				// holds pressure measurement in Pa from BME280, default is 0

static unsigned int lux = 0;		// holds light measurement in lux from TSL2591, default is 0

// Global microclimate data arrays
extern char co2Data[MAX_SAMPLES_PER_CYCLE];			// these arrays hold microclimate data for one active measurement cycle
extern char humData[MAX_SAMPLES_PER_CYCLE];			
extern char tempData[MAX_SAMPLES_PER_CYCLE];
extern char presData[MAX_SAMPLES_PER_CYCLE];
extern char luxData[MAX_SAMPLES_PER_CYCLE];

// Global date & time variables
static uint8_t month1 = 0;	// MM/DD/YY, default 01/01/20
static uint8_t month2 = 1;

static uint8_t day1   = 0;
static uint8_t day2   = 1;

static uint8_t year1  = 2;
static uint8_t year2  = 0;

static uint8_t hour1  = 0;	// HH:MM millitary time, default 00:00
static uint8_t hour2  = 0;

static uint8_t min1   = 0;
static uint8_t min2   = 0;

struct ts rtcDateTimeStruct;		// RTC struct, holds date and time values and is used for setting/retrieving this data from the RTC

// All possible event types for the HSM. Add new events here.
typedef enum {
	NO_EVENT,						// Used when events are consumed or when no event occurs
	
	ENTRY_EVENT,					// Used on transition from one state to another, mainly for on-transition behavior
	EXIT_EVENT,
	
	INIT_EVENT,						// Used once on state machine initialization, all state machines transition out of the initial pseudostate with this event
	
	BTN_EVENT,						// Used when any button status changes are detected
	TIMEOUT,						// Used when any timer expires
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

#endif		// closes the header shield
#ifndef CONFIGURE_H			// header shield, protects against multiple #includes
#define CONFIGURE_H

#include "Arduino.h"				// contains declarations for hardware mapping pins, etc. for Arduino microprocessors
#include "string.h"

#include "LiquidCrystal_I2C.h"		// lcd screen library, specifically I2C

#include <Adafruit_Sensor.h>		// contains definitions/declarations for Adafruit sensors
#include "Adafruit_TSL2591.h"		// Light sensor

#include "ds3231.h"					// library for the RTC

#include "SPI.h"
#include <SD.h>						// Teensy's library for SD card, allows creation, writing, and deletion of folders and files

// This library was modified from an existing sensor library
#include "BME280.h"					// Pressure sensor

// This library is created by the Plant CO2 Flux Meter Team 2019-20 for the CozIR-A 2000PPM CO2+RH/T Sensor
#include "CozirLib.h"				// Co2 sensor

//====================================================================================================================
// Configure.h holds global declarations needed by other parts of the framework (HSM, EventCheckers)
//====================================================================================================================

//===============================================
// Global Declarations & Public Structs/Functions
//===============================================
#define TRUE 1
#define FALSE 0

// Special debugging/development defines
#define NO_PRESSURE_TEST // turns off pressure testing, affects LeakSubHSM and WaitSubHSM
						 // as of 5/10/2020, pressure testing has not been implemented fully and if NO_PRESSURE_TEST is undefined the resulting code is unstable

#define NO_RF_TEST		 // turns off RF testing, affects RFSubHSM
						 // as of 5/10/2020, RF testing has not been implemented fully, and if NO_RF_TEST is undefined the resulting code is unstable

// Global Objects
extern char myString[32]; 				// Declares a string for storing messages to be printed to LCD screen
extern LiquidCrystal_I2C lcd;			// Declares global LCD screen class

extern char fileName[16];				// Declares a string for storing the unique data file name for each measurement period
extern char metaFileName[16];			// Declares a string for storing the unique metadata file name for each measurement period

extern BME280 pressureSensor;			// Declares global BME280 class
extern Adafruit_TSL2591 lightSensor;	// Declares global TSL2591 class

// Button EventParam Definitions used by the HSM, used to differentiate BTN_EVENTs
#define NO_PRESS			0b0
#define BTN1				0b1
#define BTN2				0b10
#define BTN3				0b100
#define BTN4				0b1000

// Timer EventParam Definitions used by the HSM, used to differentiate TIMEOUT events
#define TIMER_0_PARAM 		0b1	
#define TIMER_1_PARAM 		0b10
#define TIMER_2_PARAM		0b100

// Humidity thresholds for used by HSM, measured in RH
#define HUM_DANGER_THRESHOLD 	90
#define HUM_WARNING_THRESHOLD 	80

// Pressure thresholds used by HSM, measured in Pa
#define PRESSURE_TEST_THRESHOLD 10 	// just a placeholder for now

// RF options used by HSM
#define RF_YES 	1
#define RF_NO 	0
extern uint8_t rfOption;			// delcaration: holds rfOption, default is NO (0)

// Active and Waiting Periods, Sampling Frequency
#define ACTIVE_DURATION 600000		// duration of active cycle in [ms], default 600000[ms] = 10[min]
#define SAMPLING_FREQ 550			// sampling frequency in [ms], the minimum is 500[ms]
#define MAX_SAMPLES_PER_CYCLE 1200	// ACTIVE_DURATION / MIN_SAMPLING_FREQ
extern uint8_t period;				// holds value for period in between measurements in [min], default is 60[min] but can be user-defined

// Number of measurement cycles
extern uint8_t numCycles;			// declaration: holds value for number of measurement cycles, default is 1
extern uint8_t numCyclesCompleted;  // declaration: holds value for number of measurement cycles complete, default is 0

// Number of samples taken
extern uint8_t numSamples;			// declaration: holds value of the number of samples taken in a single active meas cycle, default is 0

// Global microclimate variables declaration
extern uint16_t co2;			// holds CO2 measurement in ppm from CozIR, default is 0
extern uint8_t hum;				// holds humidity measurement in RH from CozIR, default is 0
extern int8_t temp;				// holds temperature mesurement in C from CozIR, default is 0

extern int pres;				// holds pressure measurement in Pa from BME280, default is 0

extern uint16_t lux;			// holds light measurement in lux from TSL2591, default is 0

// Global microclimate data arrays declaration
extern uint16_t co2Data[MAX_SAMPLES_PER_CYCLE/5];			// this array holds co2 data for one active measurement cycle, used for digital filtering

extern uint8_t dataArrayAddress;

// Global date & time variables declaration
extern uint8_t month1;	// MM/DD/YY, default 01/01/20
extern uint8_t month2;

extern uint8_t day1;
extern uint8_t day2;

extern uint8_t year1;
extern uint8_t year2;

extern uint8_t hour1;	// HH:MM, default 00:00
extern uint8_t hour2;

extern uint8_t min1;
extern uint8_t min2;

extern ts rtcDateTimeStruct;		// declares RTC struct, holds date and time values and is used for setting/retrieving this data from the RTC

// SD card global definitions
#define SD_CHIP_SELECT 4

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

// Prints date in MM/DD/YY format to LCD screen
void PrintDate(void);

// Prints time in HH:MM format to LCD screen
void PrintTime(void);

// Syncs the RTC. Requires 8bit values for each minute, hour, day, month, and year digits.
void SyncRTC(uint8_t min1, uint8_t min2, uint8_t hour1, uint8_t hour2, uint8_t day1, uint8_t day2, uint8_t month1, uint8_t month2, uint8_t year1, uint8_t year2);

/*
Calculates lux value for luminosity input from light sensor
Requires a uint32_t luminosity parameter
Returns a uint16_t lux value
*/
uint16_t GetLux(uint32_t lum);

#endif		// closes the header shield
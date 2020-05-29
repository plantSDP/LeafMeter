#include "Configure.h"

// Global Variable/Array/Struct **definitions**. Declarations are in the .h file.
char myString[50];

char fileName[33];                  // global strings for filenames
char metaFileName[38];

uint8_t rfOption = RF_NO;       // sets rfOption, default is NO (0)

uint8_t period = 60;            // holds value for period in between measurements in [min], default is 60[min] but can be user-defined
uint8_t numCycles = 1;          // holds value for number of measurement cycles, default is 1
uint8_t numSamples = 0;         // holds value of the number of samples taken in a single active meas cycle, default is 0

uint16_t co2 = 0;		// holds CO2 measurement in ppm from CozIR, default is 0
uint8_t hum = 0;		// holds humidity measurement in RH from CozIR, default is 0
int8_t temp = 0;	    // holds temperature mesurement in C from CozIR, default is 0
int pres = 0;			// holds pressure measurement in Pa from BME280, default is 0
uint16_t lux = 0;		// holds light measurement in lux from TSL2591, default is 0

uint16_t co2Data[MAX_SAMPLES_PER_CYCLE/5];			// holds co2 data for one meas cycle

uint8_t dataArrayAddress;


uint8_t month1 = 0;	// MM/DD/YY, default 01/01/20
uint8_t month2 = 1;

uint8_t day1 = 0;
uint8_t day2 = 1;

uint8_t year1 = 2;
uint8_t year2 = 0;

uint8_t hour1 = 0;	// HH:MM millitary time, default 00:00
uint8_t hour2 = 0;

uint8_t min1 = 0;
uint8_t min2 = 0;

ts rtcDateTimeStruct; // RTC struct, holds date and time values and is used for setting/retrieving this data from the RTC

//==============================
// Public Function definitions
//==============================

// Prints the date to the LCD screen in MM/DD/YY form.
void PrintDate(void) {
	sprintf(myString, "Enter MM/DD/YY      ");
	lcd.setCursor(0, 0); // set the cursor to column 0, line 0
	lcd.print(myString);  // Print a message to the LCD
	sprintf(myString, "%1d%1d/%1d%1d/%1d%1d      ", month1, month2, day1, day2, year1, year2);
	lcd.setCursor(0, 1); // set the cursor to column 0, line 0
	lcd.print(myString);  // Print a message to the LCD
}

// Prints the time to the LCD screen in HH:MM form.
void PrintTime(void) {
	sprintf(myString, "Enter time HH:MM");
	lcd.setCursor(0, 0); // set the cursor to column 0, line 0
	lcd.print(myString);  // Print a message to the LCD
	sprintf(myString, "%1d%1d:%1d%1d           ", hour1, hour2, min1, min2);
	lcd.setCursor(0, 1); // set the cursor to column 0, line 0
	lcd.print(myString);  // Print a message to the LCD
}

// Synchronizes the RTC, requires the 10 date/time digits determined by the user
void SyncRTC(uint8_t min1, uint8_t min2, uint8_t hour1, uint8_t hour2, uint8_t day1, uint8_t day2, uint8_t month1, uint8_t month2, uint8_t year1, uint8_t year2) {
	uint8_t min;
	uint8_t hour;
	uint8_t day;
	uint8_t month;
	uint8_t year;

	min 	= (min1*10) + min2;
	hour 	= (hour1*10) + hour2;
	day 	= (day1*10) + day2;
	month	= (month1*10) + month2;
	year 	= 2000 + (year1*10) + year2;

	rtcDateTimeStruct.min 	= min;		// minute
	rtcDateTimeStruct.hour 	= hour;		// hour
	rtcDateTimeStruct.mday 	= day;		// day of the month
	rtcDateTimeStruct.mon 	= month;	// month
	rtcDateTimeStruct.year 	= year;		// year

	DS3231_set(rtcDateTimeStruct);		// sync the RTC
}

/*
Calculates lux value for luminosity input from light sensor
Requires a uint32_t luminosity parameter
Returns a uint16_t lux value
*/
uint16_t GetLux(uint32_t lum) {
	//lum comes from tsl.getFullLuminosity();
	uint16_t ir, full, lux;
  	
	ir = lum >> 16;
  	full = lum & 0xFFFF;
  	lux = lightSensor.calculateLux(full,ir);
	
	return lux;
}
#include "Configure.h"

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
	uint16_t year;

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

    char outString[50];
    sprintf(outString, "RTC set at %02d/%02d/%04d %02d:%02d", rtcDateTimeStruct.mday, rtcDateTimeStruct.mon, rtcDateTimeStruct.year, rtcDateTimeStruct.hour, rtcDateTimeStruct.min);
    Serial.println(outString);

	sprintf(outString, "%d%d:%d%d", hour1, hour2, min1, min2);

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
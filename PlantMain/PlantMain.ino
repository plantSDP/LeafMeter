// Our Architecture header includes			
// contains global declarations+definitions for objects, structs, arrays, & variables used by the rest of the architecture
#include "Configure.h"

// HSM & its substates
#include "PlantMeterHSM.h"  // top level of the HSM

#include "ActiveSubHSM.h"
#include "DateTimeSubHSM.h"
#include "InitSubHSM.h"
#include "LeakSubHSM.h"
#include "RFSubHSM.h"
#include "WaitSubHSM.h"

// Button and HSM timer event checkers
#include "EventCheckers.h"

// Boilerplate includes for Arduino, Adafruit sensors, I2C communication, and LCD screen
#include "Arduino.h"
#include <Adafruit_Sensor.h>
//#include <Wire.h>
#include "i2c_t3.h"
#include <LiquidCrystal_I2C.h>

// Constructions
LiquidCrystal_I2C lcd(0x27, 16, 2); // constructs an lcd class, set the LCD address to 0x27 for a 16 chars and 2 line display

BME280 pressureSensor(Wire,0x77);	// Constructor for the BME280 pressure sensor
Adafruit_TSL2591 lightSensor = Adafruit_TSL2591(2591);  // light sensor definition

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

// Holds new event from the eventcheckers
static Event newEvent;

void setup() {
  // put your setup code here, to run once:
  
  // Teensy LED
  // pinMode(13,OUTPUT);
  // digitalWrite(13,HIGH); // HIGH means LED on


  //DS3231_init(DS3231_CONTROL_INTCN);  // initialize RTC module

  lcd.init();  		  // initialize the lcd
  lcd.backlight();  // open the backlight for the lcd
  
  InitEventCheckers();  // Initialize event checkers, sets hardware pins for buttons
  InitHSM();            // Initialize HSM, transitions out of pseudostate and performs one-time startup functions
}

void loop() {
  // put your main code here, to run repeatedly:
  
  // Update button event status
  newEvent = ButtonCheckDebounce();
  if (newEvent.EventType != NO_EVENT){	// if new, send it to the state machine
    RunHSM(newEvent);
  }
  
  // Update timer expiration status
  newEvent = TimerExpireCheck();
  if (newEvent.EventType != NO_EVENT){	// if new, send it to the state machine
    RunHSM(newEvent);
  }
}

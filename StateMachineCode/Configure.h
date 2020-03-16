#ifndef CONFIGURE_H
#define CONFIGURE_H

#include "Arduino.h"
#include "LiquidCrystal_I2C.h"
#include <Adafruit_Sensor.h>
#include "Adafruit_TSL2591.h"
#include "BME280.h"
#include "CozirLib.h"

// Public Definitions & Global Things

#define TRUE 1
#define FALSE 0

extern char myString[50]; // for screen printing

//added this to try and give global access to this object
extern LiquidCrystal_I2C lcd;
extern BME280 pressureSensor;
extern Adafruit_TSL2591 lightSensor;

#define NO_PRESS	0
#define BTN1		1
#define BTN2		2
#define BTN3		4
#define BTN4		8

#define TIMER_0_PARAM 0b1
#define TIMER_1_PARAM 0b10



//the possible events for the state machine. Add new events to this enum. 
typedef enum {
	NO_EVENT,
	ENTRY_EVENT,
	EXIT_EVENT,
	INIT_EVENT,
	BTN_EVENT,
	TIMEOUT} eventTypes;

//struct to hold event types and parameter.
//this struct will be the argument for the state machine functions, and the event checkers generate these events. 
typedef struct Events{
	eventTypes EventType;
	int EventParam;
}Event;





#endif

// Our Architecture header includes
#include <Configure.h>			// contains global definitions, objects, structs used by the rest of the architecture
#include "Configure.h"

// HSM & its substates
#include <PlantMeterHSM.h>		// top level of the HSM
#include "PlantMeterHSM.h"

#include <ActiveSubHSM.h>
#include <DateTimeSubHSM.h>
#include <EventCheckers.h>
#include <InitSubHSM.h>
#include <LeakSubHSM.h>
#include <RFSubHSM.h>
#include <WaitSubHSM.h>

#include "EventCheckers.h"

// Boilerplate includes for Arduino, Adafruit sensors, I2C communication, and LCD screen
#include "Arduino.h"
#include <Adafruit_Sensor.h>
//#include <Wire.h>
#include "i2c_t3.h"
#include <LiquidCrystal_I2C.h>

// Declarations and Initializations
BME280 pressureSensor(Wire,0x77);
Adafruit_TSL2591 lightSensor  = Adafruit_TSL2591(2591);
LiquidCrystal_I2C lcd(0x27, 16, 2); // set the LCD address to 0x27 for a 16 chars and 2 line display
static Event newEvent;
char myString[50]; 



void setup() {
  // put your setup code here, to run once:
  
  pinMode(13,OUTPUT);
  digitalWrite(13,HIGH);
  lcd.init();  		// initialize the lcd
  lcd.backlight();  // open the backlight for the lcd
  
  InitEventCheckers();
  InitHSM();
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

#include "EventCheckers.h"
#include "PlantMeterHSM.h"
#include "Configure.h"
#include "Arduino.h"
//#include <Wire.h>
#include "i2c_t3.h"
#include <LiquidCrystal_I2C.h>

BME280 pressureSensor(Wire,0x77);
Adafruit_TSL2591 lightSensor  = Adafruit_TSL2591(2591);
LiquidCrystal_I2C lcd(0x27, 16, 2); // set the LCD address to 0x27 for a 16 chars and 2 line display
static Event newEvent;
char myString[50]; 

void setup() {
  pinMode(13,OUTPUT);
  digitalWrite(13,HIGH);
  lcd.init();  //initialize the lcd
  lcd.backlight();  //open the backlight
  // put your setup code here, to run once:
  InitEventCheckers();
  InitHSM();
  

}

void loop() {
  // put your main code here, to run repeatedly:
  newEvent = ButtonCheckDebounce();
  if (newEvent.EventType != NO_EVENT){
    RunHSM(newEvent);
  }
  
  newEvent = TimerExpireCheck();
  if (newEvent.EventType != NO_EVENT){
    RunHSM(newEvent);
  }
  
}

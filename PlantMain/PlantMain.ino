#include "EventCheckers.h"
#include "PlantMeterHSM.h"
#include "Configure.h"
#include "Arduino.h"
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2); // set the LCD address to 0x27 for a 16 chars and 2 line display
static Event newEvent;
char myString[50]; 

void setup() {
  
  lcd.init();  //initialize the lcd
  lcd.backlight();  //open the backlight
  // put your setup code here, to run once:
  InitHSM();
  InitEventCheckers();

//  sprintf(myString, "Hello");
//  lcd.setCursor(0, 0); // set the cursor to column 0, line 0
//  lcd.print(myString);  // Print a message to the LCD
}

void loop() {
  // put your main code here, to run repeatedly:
  newEvent = ButtonCheckDebounce();
  if (newEvent.EventType != NO_EVENT){
    RunHSM(newEvent);
  }
  
}

#include "EventCheckers.h"
#include "PlantMeterHSM.h"
#include "Configure.h"
#include "Arduino.h"


static Event newEvent;

void setup() {
  // put your setup code here, to run once:
  InitHSM();
  InitEventCheckers();
}

void loop() {
  // put your main code here, to run repeatedly:
  newEvent = ButtonCheckDebounce();
  if (newEvent.EventType != NO_EVENT){
    RunHSM(newEvent);
  }
  
}

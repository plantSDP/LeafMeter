#include "Arduino.h"
const uint8_t inletValveDirection1 = 6;
const uint8_t inletValveDirection2 = 7;
const uint8_t outletValveDirection1 = 8;
const uint8_t outletValveDirection2 = 9;
const uint8_t valveEnable = 3;
const uint8_t pumpEnable = 22;

void setup() {
  // put your setup code here, to run once:
  pinMode(inletValveDirection1, OUTPUT);
  pinMode(inletValveDirection2, OUTPUT);
  pinMode(outletValveDirection1, OUTPUT);
  pinMode(outletValveDirection2, OUTPUT);
  pinMode(valveEnable, OUTPUT);
  pinMode(pumpEnable, OUTPUT);



  //To set the inlet valve in one position
  digitalWrite(inletValveDirection1, HIGH);
  digitalWrite(inletValveDirection2, LOW);
  digitalWrite(valveEnable, HIGH); //leave this high for 250ms
  digitalWrite(valveEnable, LOW);


  //to set the inlet valve in the other position
  digitalWrite(inletValveDirection1, LOW);
  digitalWrite(inletValveDirection2, HIGH);
  digitalWrite(valveEnable, HIGH); //leave this high for 250ms
  digitalWrite(valveEnable, LOW);

  //to set the outlet valve in one position
  digitalWrite(outletValveDirection1, HIGH);
  digitalWrite(outletValveDirection2, LOW);
  digitalWrite(valveEnable, HIGH); //leave this high for 250ms
  digitalWrite(valveEnable, LOW);


  //to set the outlet valve in the other position
  digitalWrite(outletValveDirection1, LOW);
  digitalWrite(outletValveDirection2, HIGH);
  digitalWrite(valveEnable, HIGH); //leave this high for 250ms
  digitalWrite(valveEnable, LOW);

  //to adjust the pump use this function
  analogWrite(pumpEnable, 255); //the second argument can be adjusted between 0-255 to change the duty cycle with resolution of 3.9%

}

void loop() {
  // put your main code here, to run repeatedly:

}

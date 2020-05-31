#include <ServerConfigure.h>
#include <ServerEventChecker.h>
#include <ServerFSM.h>
#include "Arduino.h"
#include <RH_RF95.h>
#include <RHHardwareSPI1.h>
#include <SPI.h>

int led = 16;
RH_RF95 rf95(10, 2, hardware_spi1);
RF_module_info serverInfo;
Recieved_info info_recieved;
Field_info fieldInfo;
static Event newEvent;
//char myString[50]; 
uint8_t test; //using this to send a message

void setup() {
  pinMode(led,OUTPUT);
  digitalWrite(led,LOW);
  info_recieved.payloadLength = RH_RF95_MAX_MESSAGE_LEN;
  SPI1.setMISO(5); // May also be 5
  SPI1.setMOSI(21); // May also be 21
  SPI1.setSCK(20); 
  SPI1.begin();

  
  Serial.begin(9600);
  while (!Serial) ; // Wait for serial port to be available
  Serial.println("Beginning...");

  if (!rf95.init()){
    Serial.println("RF init failed");
  }
  Serial.println("RF init success");
  
  rf95.setFrequency(915);
  Init_ServerFSM();

  digitalWrite(led,HIGH);
}

void loop() {
  // put your main code here, to run repeatedly:

  newEvent = TimerExpireCheck();
  if (newEvent.EventType != NO_EVENT){
    Run_ServerFSM(newEvent);
  }

  newEvent = RfMessageCheck();
  if (newEvent.EventType != NO_EVENT){
    Run_ServerFSM(newEvent);
  }
  
}

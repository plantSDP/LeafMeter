#include "EventChecker.h"
#include "ClientFSM.h"
#include "ClientConfigure.h"
#include "Arduino.h"
#include <RH_RF95.h>
#include <RHHardwareSPI1.h>
#include <SPI.h>


RH_RF95 rf95(10, 2, hardware_spi1);
RF_module_info clientInfo;
Recieved_info info_recieved;
static Event newEvent;
char myString[50]; 
uint8_t test; //using this to send a message

void setup() {
  info_recieved.payloadLength = RH_RF95_MAX_MESSAGE_LEN;
  pinMode(16,OUTPUT);
  digitalWrite(16,HIGH);
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
  Init_ClientFSM();

  Event testEvent;
  testEvent.EventType = TRANSMIT_REQUEST_EVENT;
  clientInfo.payloadIsData = TRUE;
  Run_ClientFSM(testEvent);
}

void loop() {
  // put your main code here, to run repeatedly:

  newEvent = TimerExpireCheck();
  if (newEvent.EventType != NO_EVENT){
    Run_ClientFSM(newEvent);
  }

  newEvent = RfMessageCheck();
  if (newEvent.EventType != NO_EVENT){
    Run_ClientFSM(newEvent);
  }
  
}

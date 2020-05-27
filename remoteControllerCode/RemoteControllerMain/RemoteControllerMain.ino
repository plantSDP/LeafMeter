#include "EventChecker.h"
#include "ClientFSM.h"
//#include "RemoteFSM.h" //reinclude if we need an entire state machine
#include "RemoteConfigure.h"
#include "Arduino.h"
#include "RH_RF95.h"
#include "RHHardwareSPI1.h"
#include "SPI.h"
#include "HM10Lib.h"


RH_RF95 rf95(10, 2, hardware_spi1);
RF_module_info clientInfo;
Recieved_info info_recieved;
static Event newEvent;
uint8_t holder[10]; 
uint8_t test; //using this to send a message
uint8_t BLEMessage[255];
uint8_t BLEMessage_length;


void setup() {
  info_recieved.payloadLength = RH_RF95_MAX_MESSAGE_LEN;
  pinMode(16,OUTPUT);
  digitalWrite(16,HIGH);
  SPI1.setMISO(5); // May also be 5
  SPI1.setMOSI(21); // May also be 21
  SPI1.setSCK(20); 
  SPI1.begin();

  BLE_Init();
  rf95.init();
  rf95.setFrequency(915);
  
  Init_ClientFSM();
  //Init_RemoteFSM(); //This was meant to be a state machine to handle a BLE event, but it was actually really simple so its just in the main loop. If more complicated actions need to happen, use this state machine
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


  newEvent = BLEMessageCheck();
  if (newEvent.EventType != NO_EVENT){
    //Run_RemoteFSM(newEvent); //was going to be used but not anymore. If more complicated actions are needed, use this state machine
    if (clientInfo.transciever_state == TRANSMITTING){
      //?NEED TO ADD logic HERE FOR WHEN SETUP MODE IS BEING REQUESTED
        memcpy(clientInfo.payloadToSend, BLEMessage, BLEMessage_length); //copies the BLE message into the array for sending via LoRa
        clientInfo.payloadToSend_length = BLEMessage_length;
        newEvent.EventType = TRANSMIT_REQUEST_EVENT;
        Run_ClientFSM(newEvent); //this will transmit the message to the field unit and start the acknowledgment process
    } 
  }
}

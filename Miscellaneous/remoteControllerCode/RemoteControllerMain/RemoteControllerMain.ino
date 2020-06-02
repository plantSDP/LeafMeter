/*
 * This is the main arduino file for the Remote controller. This file should be in the path \Documents\Arduino\RemoteControllerMain.
 * This code is written in C++ but makes minimal use of oop features. It is intended to be used with the Arduino IDE with Teensyduino(An add-on which provides support for Teensy boards)
 * Instructions for installing Teensyduino can be found here https://www.pjrc.com/teensy/td_download.html . This code is meant for use with the TeensyLC microcontroller board.
 */
#include "EventChecker.h"
#include "ClientFSM.h"
#include "RemoteConfigure.h"
#include "Arduino.h"
#include "RH_RF95.h"
#include "RHHardwareSPI1.h"
#include "SPI.h"
#include "HM10Lib.h"

//declare various global variables / objects. The definitions of these are found in "RemoteConfigure.h"
//These are essential for communication with peripheral devices, and maintaining the state of the communication protocol.
RH_RF95 rf95(10, 2, hardware_spi1); //This is the object for the LoRa module. This instance allows for easy configuration and communication with the module. See "RH_RF95.h" for details
RF_module_info clientInfo; // This struct is used to hold 
Recieved_info info_recieved;
static Event newEvent;
uint8_t holder[10]; 
uint8_t test; //using this to send a message
uint8_t BLEMessage[255];
uint8_t BLEMessage_length;


void setup() { //This code runs a single time at the beginning of the program.
  Serial.begin(9600);
  while (!Serial) ; // Wait for serial port to be available
  Serial.println("Remote Controller Startup");
  info_recieved.payloadLength = RH_RF95_MAX_MESSAGE_LEN; //indicate that the entire payload length may be copied when using the recv() method. 
  SPI1.setMISO(5); //set the pins for the SPI hardware used with the LoRa module. 
  SPI1.setMOSI(21); 
  SPI1.setSCK(20); 
  SPI1.begin(); //enable the SPI hardware

  BLE_Init(); //Configure the UART hardware used with the BLE module
  rf95.init();//Initialize the LoRa module library / hardware
  rf95.setFrequency(915);
  
  Init_ClientFSM();
  
}

void loop() { //This code is called continuously in main after the setup() function. This code polls for new events and responds to these event when they are detected

  newEvent = TimerExpireCheck();       //If one of the timers has expired, the state machine for the remote controller is called with the event type as an argument. 
  if (newEvent.EventType != NO_EVENT){ //Based on the current state of the state machine, a different action will be taken for the particular event type.
    Run_ClientFSM(newEvent);
  }

  newEvent = RfMessageCheck();         //Same as above but this checks for if a packet has been recieved by the LoRa module.
  if (newEvent.EventType != NO_EVENT){
    Run_ClientFSM(newEvent);
  }


  newEvent = BLEMessageCheck();         //check if a complete BLE message has been recieved by the BLE modules and sent via UART
  if (newEvent.EventType != NO_EVENT){  //If yes and the remote controller is not currenlty in the middle of a transaction, different actions will take place.
    Serial.println("Bluetooth EVENT"); 
    if (clientInfo.transciever_state == TRANSMITTING){ 
      
      if (BLEMessage[0] == SET_SETUP_MODE){     //if the message is to set the remote controller to setup mode, run the state machine with a setup event, send an acknowledgment to the GUI
        newEvent.EventType = LINK_SETUP_EVENT;  //the setup mode temporarily makes the remote controller the server in the communication protocol to allow the user to test the RF connection 
        Run_ClientFSM(newEvent);                //while they are in the field. This event should be sent before the user leaves the location of the remote controller to set up the field unit.
        holder[0] = SET_SETUP_MODE_ACK;
        BLE_send(holder, 1);
        
      } else if (BLEMessage[0] == END_SETUP_MODE){  //if the message is to end the setup mode, run the state machine with a end setup event, send an acknowledgment to the GUI
        newEvent.EventType = END_LINK_SETUP_EVENT;  //this will put the remote controller back as the client, and allow for messages to be relayed to the field unit
        Run_ClientFSM(newEvent);
        holder[0] = END_SETUP_MODE_ACK;
        BLE_send(holder, 1); 
        
      }else{ //relay the message to the field unit by copying the BLE message into the RF payload struct member and calling the state machine with a transmit request event
        memcpy(clientInfo.payloadToSend, BLEMessage, BLEMessage_length); //copies the BLE message into the array for sending via LoRa
        clientInfo.payloadToSend_length = BLEMessage_length;
        newEvent.EventType = TRANSMIT_REQUEST_EVENT;
        Run_ClientFSM(newEvent); //this will transmit the message to the field unit and start the acknowledgment process
      } 
    }
  }
}

/*
 Written in C++ but does not make large use of OOP functionallity. 
 This code is intended to be used with the Arduino IDE in conjunction with the file 'RemoteControllerMain.ino'
 
 This is the header file which links all of the code related to the remote controller. 
 The primary purpose of this code is to hold enum and struct declarations needed for the polling of events, 
 and state machine responses to those events. There are also global variable/object declarations here. The definition of these 
 variables/objects are in 'RemoteControllerMain.ino'. These globals are used communicate information between files efficiently. 
 
 Written by CO2 leaf meter
*/


#ifndef REMOTECONFIGURE_H
#define REMOTECONFIGURE_H


#include "Arduino.h" 		//This gives access to a variety of functions and #defines that ease development. (Part of the native arduino toolchain)
#include <RH_RF95.h> 		//The library we are using to communicate with the LoRa RF modules (Comes with the Teensyduino add-op for the arduino IDE). 
#include <RHHardwareSPI1.h> //This header allows the SPI1 hardware to be used with the LoRa modules instead of the SPI0 hardware which is used with the SD card module (Comes with the Teensyduino add-op for the arduino IDE). 
#include "MessageIDs.h"		//This holds the enum which is used for message type identification (Written by CO2 leaf meter)
#include "HM10Lib.h"		//This allows message to be sent to the BLE module (Written by CO2 leaf meter)

// Public Definitions & Global Things

#define TRUE 1
#define FALSE 0
#define TRANSMITTING 0 //this define and the one below it are used with the 'RF_module_info' struct below to indicate this unit is awaiting a message or ready to transmit 
#define RECIEVING 1
#define NUM_CYCLES_MAX 32



//globally accessible objects, structs, and variables. 
extern RH_RF95 rf95; //LoRa transciever object. Defined in RH_RF95.h
extern uint8_t test; //using this to send a message 
extern uint8_t BLEMessage[255]; //an array to store the message sent by the bluetooth module. 
extern uint8_t BLEMessage_length; //the length of the message stored in the array above.
extern uint8_t holder[10]; //used to store random data. Info stored in here is volatile. 

#define TIMER_0_PARAM 0b1 //These defines are used to indicate which timer has expired when a timeout event occurs. 
#define TIMER_1_PARAM 0b10
#define TIMER_2_PARAM 0b100

//the possible events which can be sent to the state machine. Some of these event are self generated by the state machine while other are based on polling various conditions
typedef enum {
	NO_EVENT,
	ENTRY_EVENT,
	EXIT_EVENT,
	INIT_EVENT,
	BTN_EVENT,
	TIMEOUT,
	RF_RECIEVE_EVENT,
	TRANSMIT_REQUEST_EVENT,
	LINK_SETUP_EVENT,
	END_LINK_SETUP_EVENT,
	BLE_RECIEVE_EVENT,
} eventTypes;

//struct to hold event types and parameter. This struct will be the argument for the state machine functions
typedef struct Events{ //As of 5/25/20 the paramter is only ever used with timer events to indicate which timer has expired.  
	eventTypes EventType;
	int EventParam;
}Event;



typedef struct RF_module_info{	//this struct is used to store information about the rf module.
	uint8_t transciever_state;	//In particular it should be used to ensure the transciever chip is in the correct state for 
	uint8_t retry_number;	    //the non-stateless protocol, and to keep track of retries as well as previous payloads for retransmission
	uint16_t total_retries;
	uint8_t payloadIsData; 
	uint8_t payloadToSend[256];
	uint8_t payloadToSend_length;
	uint8_t previousPayload[256];
	uint8_t previousPayload_length;
	int8_t lastRSSI;
} RF_module_info;


//this struct is used to store all of the most recent information recieved from the server (field unit). 
typedef struct Recieved_info{
	uint8_t raw_payload[RH_RF95_MAX_MESSAGE_LEN]; //the most recent recieved payload without the header. 
	uint8_t payloadLength; //this will hold the length of the payload. Use this and the member above with the recv() function to get data from the transciever. 
						   //Before calling recv, this member should be set to RH_RF95_MAX_MESSAGE_LEN to indicate the entire payload may be copied
	uint8_t messageID; // most recent messageID as defined in 'MessageIDs.h'
	uint8_t measurement_confirmed; //indicator for if the field device is going to take an on-demand measurement
	uint8_t remaining_cycles;
	uint16_t time_till_powerOff;
	uint16_t time_left_battery;
	uint8_t num_measurments_taken;
	uint8_t timestamps[2][NUM_CYCLES_MAX]; //2D array to store the timestamps of each measurment. timestamps[0][x] has the hour, timestamps[1][x] has the corresponding minute
	uint16_t time_till_nextCycle;
	uint16_t sample_period; //the time in milliseconds between samples
	uint16_t flux_estimate;
	uint8_t data_packets_total;
	uint8_t data_packet_current;
	uint8_t num_sample_points; //how many points of data are being transmitted in the current packet
	int8_t recent_temp[32];
	uint8_t recent_humidity[32];
	uint16_t recent_CO2[32];
	uint32_t recent_pressure[32];
	uint32_t recent_light[32];
} Recieved_info;


//additional globals
extern RF_module_info clientInfo; 
extern Recieved_info info_recieved;

#endif

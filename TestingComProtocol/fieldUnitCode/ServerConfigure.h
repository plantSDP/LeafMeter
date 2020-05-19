#ifndef CONFIGURE_H
#define CONFIGURE_H


#include "Arduino.h"
#include <RH_RF95.h>
#include <RHHardwareSPI1.h>
#include "MessageIDs.h"

// Public Definitions & Global Things

#define TRUE 1
#define FALSE 0
#define TRANSMITTING 0
#define RECIEVING 1
#define NUM_CYCLES_MAX 32

extern char myString[50]; // for screen printing

//globally accessible objects, structs, and variables. 
extern RH_RF95 rf95; //LoRa transciever object. Defined in RH_RF95.h
extern uint8_t test; //using this to send a message 


#define TIMER_0_PARAM 0b1
#define TIMER_1_PARAM 0b10
#define TIMER_2_PARAM 0b100

//the possible events for the state machine. Add new events to this enum. 
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
} eventTypes;

//struct to hold event types and parameter.
//this struct will be the argument for the state machine functions, and the event checkers generate these events. 
typedef struct Events{
	eventTypes EventType;
	int EventParam;
}Event;


//this struct is used to store information about the rf module.
//In particular it should be used to ensure the transciever chip is in the correct state for 
//the non-stateless protocol, and to keep track of retries, and previous payloads for retransmission
typedef struct RF_module_info{
	uint8_t totalDataPackets;
	uint8_t currentDataPacket;
	uint8_t payloadToSend[256];
	uint8_t payloadToSend_length;
	uint8_t previousPayload[256];
	uint8_t previousPayload_length;
	int8_t lastRSSI;
} RF_module_info;


//this struct is used to store all of the most recent information recieved from the server. 
typedef struct Recieved_info{
	uint8_t raw_payload[RH_RF95_MAX_MESSAGE_LEN]; //the most recent recieved payload without the header. 
	uint8_t payloadLength; //this will hold the length of the payload. Use this and the member above with the recv() function to get data from the transciever
	uint8_t messageID; // most recent messageID
	uint8_t day;
	uint8_t month;
	uint8_t year;
	uint8_t hour;
	uint8_t minute;
	uint8_t data_transmit_indicator;
	uint8_t new_total_measurements;
	uint16_t new_min_between_measurements;
	uint16_t time_till_powerOff;
	uint8_t requested_measurement_number; //the measurement cycle number thats data is to be transmitted
} Recieved_info;


//struct to hold crucial information on the status of the field unit 
typedef struct Field_info{
	uint8_t total_measurements;
	uint8_t num_measurements_taken;
	uint8_t timestamps[2][NUM_CYCLES_MAX]; //2D array to store the timestamps of each measurment. timestamps[0][x] has the hour, timestamps[1][x] has the corresponding minute
	uint16_t numSamples[NUM_CYCLES_MAX]; //stores how many sample points each measurment cycle had. To be used to calculate how many packets will be needed to transmit 
	uint16_t min_between_measurements;	
	uint16_t time_till_powerOff;
	uint16_t time_left_battery;
	uint16_t time_till_next_measurment; 
} Field_info;

//additional globals
extern RF_module_info serverInfo; //the struct used to keep track of the transciever state and retries. Defined in this file. 
extern Recieved_info info_recieved;
extern Field_info fieldInfo;
#endif



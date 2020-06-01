// This file is meant to be used for the field unit. It handles the logic for sending and and recieving messages over the 915Mhz link. 
// When messages are recieved, the information encoded in the payload is decoded and stored in members of a global struct 'recievedInfo' which is defined in ClientConfigure.h. 
// To send messages, first the desired payload and the length should be stored in the global struct 'clientInfo'. If the message is a measurment data request, the member 'payloadIsData' should be set to TRUE. 
// call the function 'Run_ClientFSM()' with a 'TRANSMIT_REQUEST_EVENT' to send this payload. The packet will only be sent if the current state of the remote controller is not in the middle of handling a transaction. 
// currently there is no way to queue packets to be sent. 
// THIS IS THE COMMENT FOR CLIENTFSM? NEEDS TO BE UPDATED?

// Includes
#include "ServerFSM.h"
#include "Arduino.h"
#include "ServerConfigure.h"
#include "ServerEventChecker.h"



// Private definitions
#define ALLOWED_RETRIES 2 //number of retries before giving up transmission and warning the user
#define RETRY_PERIOD_MAX 1700 //twice the round trip ping of a 255 byte payload (ms)
#define RETRY_PERIOD_MIN 250 //twice the round trip ping of a 16 byte payload (ms)

// List states here:
typedef enum {
    InitPState,
	State0_Listening,
    State1_TransmittingData,
	State2_TestingLink,
} ServerFSMStates;

// Holds the current state
static ServerFSMStates CurrentState = InitPState;

// This function runs the state machine with an INIT_EVENT
uint8_t Init_ServerFSM(void){
	Event thisEvent;
	thisEvent.EventType = INIT_EVENT;
	thisEvent.EventParam = 0;
	Event returnEvent = Run_ServerFSM(thisEvent);
	if (returnEvent.EventType == NO_EVENT) {
		return TRUE;
	} else {
		return FALSE;
	}
}

Event Run_ServerFSM(Event thisEvent) {
	
	uint8_t makeTransition = FALSE; // use to flag transition
	ServerFSMStates nextState;
	int i,j;	// these variables are used to iterate through payload data during decoding and to copy data between two arrays which hold transmitted payloads

	
	switch (CurrentState) {
		case InitPState:								// If current state is initial Pseudo State
			if (thisEvent.EventType == INIT_EVENT){		// only respond to INIT_EVENT
				fieldInfo.total_measurements = 24; //initialize some information about the field unit. This should be updated in the main state machine. 
				fieldInfo.num_measurements_taken = 0;
				fieldInfo.min_between_measurements = 60;
				serverInfo.lastRSSI = 0;
				nextState = State0_Listening;				
				makeTransition = TRUE;
			}
			break;
		
		case State0_Listening: 					//This is the most common state. In here, the server is always waiting for a packet to decode. After decoding, an acknowledgmenet is sent to the client. 
			switch(thisEvent.EventType){		//If the client asks for measurement data, the state will change to 
				case ENTRY_EVENT:
					Serial.println("Entered Listening State"); //here for testing
					thisEvent.EventType = NO_EVENT;
					break;
					
				case RF_RECIEVE_EVENT:		
					Serial.print("Field Unit Received Message..."); //here for testing
					info_recieved.payloadLength = RH_RF95_MAX_MESSAGE_LEN; //this is needed to indicate that the entire payload may be copied
					rf95.recv(info_recieved.raw_payload, &info_recieved.payloadLength); //get the payload from the module 
					info_recieved.messageID = info_recieved.raw_payload[0];
					
					if (info_recieved.messageID == CONFIGURE_DATE_TIME){ 
						info_recieved.day = info_recieved.raw_payload[1]; //save the data sent in the global struct
						info_recieved.month = info_recieved.raw_payload[2];
						info_recieved.year = info_recieved.raw_payload[3];
						info_recieved.hour = info_recieved.raw_payload[4];
						info_recieved.minute = info_recieved.raw_payload[5];
						Serial.print("ConfigureDateTime: "); //here for testing
						Serial.print(info_recieved.day); //here for testing
						Serial.print("/");
						Serial.print(info_recieved.month);
						Serial.print("/");
						Serial.print(info_recieved.year);
						Serial.print("_");
						Serial.print(info_recieved.hour);
						Serial.print(":");
						Serial.println(info_recieved.minute);
						
						serverInfo.payloadToSend[0] = CONFIGURE_DATE_TIME_ACK;	//transmit the ack
						serverInfo.payloadToSend_length = 1;
						rf95.send(serverInfo.payloadToSend, serverInfo.payloadToSend_length);
						//memcpy(serverInfo.previousPayload, serverInfo.payloadToSend, serverInfo.payloadToSend_length); //copies the sent payload into previousPayload. THIS IS ONLY NEEDED WHEN TRANSMITTING MEASUREMENT DATA
						//serverInfo.previousPayload_length = serverInfo.payloadToSend_length; // copy the length 
						//Do something in state machine?
						
					} else if (info_recieved.messageID == MEASUREMENT_REQUEST){
						info_recieved.data_transmit_indicator = info_recieved.raw_payload[1]; 
						//?determine if the the on demand measurment is going to be accepted somehow. Also do something with this indicator. I think we should give up on real-time transmission as were short on time.
						Serial.print("Measurement Request"); //here for testing
						Serial.println(info_recieved.data_transmit_indicator);

						serverInfo.payloadToSend[0] = MEASUREMENT_REQUEST_ACK;	//transmit the ack
						serverInfo.payloadToSend[1] = 1; //? this set value should represent if the on-demand measurement will be taken
						serverInfo.payloadToSend_length = 2;
						rf95.send(serverInfo.payloadToSend, serverInfo.payloadToSend_length);
						
						
					} else if (info_recieved.messageID == CONFIGURE_TOTAL_MEASUREMENTS){
						info_recieved.new_total_measurements = info_recieved.raw_payload[1]; //?might need to do something here. Depends how main state machine is set up. 
						fieldInfo.total_measurements = info_recieved.new_total_measurements;
						Serial.print("ConfigureTotalMeasurments"); //here for testing
						Serial.println(fieldInfo.total_measurements);
						
						if ((fieldInfo.total_measurements - fieldInfo.num_measurements_taken) > NUM_CYCLES_MAX){
							serverInfo.payloadToSend[1] = 0;
						} else{
							serverInfo.payloadToSend[1] = fieldInfo.total_measurements - fieldInfo.num_measurements_taken;
						}
						serverInfo.payloadToSend[0] = CONFIGURE_TOTAL_MEASUREMENTS_ACK;
						serverInfo.payloadToSend_length = 2;
						rf95.send(serverInfo.payloadToSend, serverInfo.payloadToSend_length);
						
					} else if (info_recieved.messageID == CONFIGURE_CYCLE_PERIOD){
						info_recieved.new_min_between_measurements = ((uint16_t)info_recieved.raw_payload[1]<<8) | (info_recieved.raw_payload[2]); // there is a shift by 8 because shorts are sent Big Endian. '|' logical OR to recover the 16 bit value
						fieldInfo.min_between_measurements = info_recieved.new_min_between_measurements; //? do something here probably
						Serial.print("ConfigureCyclePeriod"); //here for testing
						Serial.println(fieldInfo.min_between_measurements);
						
						serverInfo.payloadToSend[0] = CONFIGURE_CYCLE_PERIOD_ACK;
						serverInfo.payloadToSend_length = 1;
						rf95.send(serverInfo.payloadToSend, serverInfo.payloadToSend_length);
					
					} else if (info_recieved.messageID == POWER_OFF_REQUEST) {
						info_recieved.time_till_powerOff = ((uint16_t)info_recieved.raw_payload[1]<<8) | (info_recieved.raw_payload[2]);
						fieldInfo.time_till_powerOff = info_recieved.time_till_powerOff; // ?theres probably some stuff that needs to happen here. Set timer? calculate the real time to power off?
						Serial.print("Power Off Request"); //here for testing
						Serial.println(fieldInfo.time_till_powerOff);
						
						serverInfo.payloadToSend[0] = POWER_OFF_REQUEST_ACK;
						serverInfo.payloadToSend[1] = fieldInfo.time_till_powerOff >> 8;
						serverInfo.payloadToSend[2] = fieldInfo.time_till_powerOff & 0xff;
						serverInfo.payloadToSend_length = 3;
						rf95.send(serverInfo.payloadToSend, serverInfo.payloadToSend_length);
						
					} else if (info_recieved.messageID == UPDATE_STATUS_REQUEST){
						Serial.println("UpdateStatusRequest"); //here for testing						
						
						serverInfo.payloadToSend[0] = UPDATE_STATUS_REQUEST_ACK;
						//?update the parameter fieldInfo.time_left_battery with the time left
						serverInfo.payloadToSend[1] = fieldInfo.time_left_battery >> 8;
						serverInfo.payloadToSend[2] = fieldInfo.time_left_battery & 0xff;
						serverInfo.payloadToSend[3] = fieldInfo.num_measurements_taken;
						
						j = 4;
						for (i = 0; i < fieldInfo.num_measurements_taken; i++){ // iterate through timestamps and store them in the payload.
							serverInfo.payloadToSend[j++] = fieldInfo.timestamps[0][i]; // store the hour
							serverInfo.payloadToSend[j++] = fieldInfo.timestamps[1][i]; // store the minute 
						}
						fieldInfo.time_till_next_measurment = 1; //?needs to actually be determined. 
						serverInfo.payloadToSend[j++] = fieldInfo.time_till_next_measurment >> 8;
						serverInfo.payloadToSend[j++] = fieldInfo.time_till_next_measurment & 0xff;
						serverInfo.payloadToSend_length = j;
						rf95.send(serverInfo.payloadToSend, serverInfo.payloadToSend_length);
						
					} else if (info_recieved.messageID == DATA_TRANSMISSION_REQUEST){
						Serial.print("TransmitDataRequest"); //here for testing
						
						info_recieved.requested_measurement_number = info_recieved.raw_payload[1];
						Serial.println(info_recieved.requested_measurement_number); //here for testing
						serverInfo.payloadToSend[0] = DATA_TRANSMISSION_INITIAL;//?load up the payload according to definition in MessageIDs.h
						//?this is going to require alot of work. Need to define how files are stored. I think we should have a place to store how many samples were taken for each cycle. Thatll make it easy to calculate how many packets will need to be sent. 
						serverInfo.totalDataPackets = 2; //?load the total number of packets
						serverInfo.currentDataPacket = 1;
						serverInfo.payloadToSend[10] = serverInfo.totalDataPackets;
						serverInfo.payloadToSend[11] = serverInfo.currentDataPacket;
						serverInfo.payloadToSend_length = 12;//?calculate the payload length. 
						rf95.send(serverInfo.payloadToSend, serverInfo.payloadToSend_length);
						if (serverInfo.totalDataPackets > 1){
							nextState = State1_TransmittingData;
							makeTransition = TRUE;
						}
						memcpy(serverInfo.previousPayload, serverInfo.payloadToSend, serverInfo.payloadToSend_length); //copies the sent payload into previousPayload.
						serverInfo.previousPayload_length = serverInfo.payloadToSend_length; // copy the length
						
					}else if (info_recieved.messageID == DATA_TRANSMISSION_NACK){
						Serial.println("DataTransmissionNack");
						
						rf95.send(serverInfo.previousPayload, serverInfo.previousPayload_length);
					}else{
						Serial.print("Failed to identify message id");
						Serial.println(info_recieved.messageID);
					}
					break;
					
				case LINK_SETUP_EVENT:
					nextState = State2_TestingLink;
					makeTransition = TRUE;
					break;
					
				default:
					break;
				
			}
			break;


		case State1_TransmittingData: 
			switch (thisEvent.EventType) {
				case ENTRY_EVENT:
						Serial.println("Entered Data Transmit State");
					break;
					
				case RF_RECIEVE_EVENT:				
					Serial.print("Got message: ");
					info_recieved.payloadLength = RH_RF95_MAX_MESSAGE_LEN; //this is needed to indicate that the entire payload may be copied
					rf95.recv(info_recieved.raw_payload, &info_recieved.payloadLength);
					info_recieved.messageID = info_recieved.raw_payload[0];
					
					if (info_recieved.messageID == DATA_TRANSMISSION_NACK){
						Serial.println("DataTransmissionNack");
						
						rf95.send(serverInfo.previousPayload, serverInfo.previousPayload_length);
						
					} else if (info_recieved.messageID == DATA_TRANSMISSION_ACK){ 
						Serial.println("DataTransmissionAck");
					
						serverInfo.payloadToSend[0] = DATA_TRANSMISSION;//?load up the payload according to definition in MessageIDs.h 
						serverInfo.currentDataPacket++;
						serverInfo.payloadToSend_length = 3;//?calculate the payload length. 
						rf95.send(serverInfo.payloadToSend, serverInfo.payloadToSend_length);
						memcpy(serverInfo.previousPayload, serverInfo.payloadToSend, serverInfo.payloadToSend_length); //copies the sent payload into previousPayload.
						serverInfo.previousPayload_length = serverInfo.payloadToSend_length; // copy the length
						
						if (serverInfo.currentDataPacket >= serverInfo.totalDataPackets){ //sending last packet of transaction. Move back to listening state. 
							nextState = State0_Listening;
							makeTransition = TRUE;
						}
						
					} else {
						Serial.println("failure");
						
						//?this means there was some serious failure in the communication protocol
						nextState = State0_Listening;
						makeTransition = TRUE;
					}
					
					break;
			
					
				default:
					break;
			}				
			break;	
			
		case State2_TestingLink:	//This state is used when setting up the RF connection. A complementary state is on the client side, allowing for the RSSI to be checked by the user.
			switch(thisEvent.EventType){ //To use this state, a LINK_SETUP_EVENT should be passed to this function. This will send out a ping message, and if a pong is recieved the RSSI will be stored
				case ENTRY_EVENT:		 //in the serverInfo global struct. There are no timeouts so it is up to the main code to determine if a pong cannot be retrieved. To retry a ping, LINK_SETUP_EVENT
					Serial.println("Entered TestingLink"); //can be sent again. To leave this setup mode the fuction needs to be ran with a END_LINK_SETUP_EVENT event. 
					
					serverInfo.payloadToSend[0] = PING;
					serverInfo.payloadToSend_length = 1;
					rf95.send(serverInfo.payloadToSend, serverInfo.payloadToSend_length);
					break;
					
				case RF_RECIEVE_EVENT:
					Serial.print("Got Pong...");
					info_recieved.payloadLength = RH_RF95_MAX_MESSAGE_LEN; //this is needed to indicate that the entire payload may be copied
					rf95.recv(info_recieved.raw_payload, &info_recieved.payloadLength);
					info_recieved.messageID = info_recieved.raw_payload[0];		
						
					if (info_recieved.messageID == PONG){
						serverInfo.lastRSSI = rf95.lastRssi(); //save RSSI in info struct
						Serial.println(serverInfo.lastRSSI);						
					}
					
					break;
				
				case LINK_SETUP_EVENT:
					serverInfo.payloadToSend[0] = PING;
					serverInfo.payloadToSend_length = 1;
					rf95.send(serverInfo.payloadToSend, serverInfo.payloadToSend_length);
					
					break;
				
				case END_LINK_SETUP_EVENT:
					//?change back to listening
					nextState = State0_Listening;
					makeTransition = TRUE;
					break;
				
				default:
					break;
			}
			
			break;
	
			
		default:
			break;
	}
		
	if (makeTransition == TRUE) { // making a state transition, send EXIT and ENTRY
		// recursively call the current state with an exit event
		thisEvent.EventType = EXIT_EVENT;
		Run_ServerFSM(thisEvent);
		CurrentState = nextState;
		thisEvent.EventType = ENTRY_EVENT;
		Run_ServerFSM(thisEvent);
		thisEvent.EventType = NO_EVENT;
	}
	return thisEvent;
}
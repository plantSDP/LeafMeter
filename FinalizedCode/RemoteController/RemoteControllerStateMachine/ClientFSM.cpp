/*
  Written in C++ but does not make large use of OOP functionallity. 
  This code is intended to be used with the Arduino IDE in conjunction with the file 'RemoteControllerMain.ino'
  
  This file is meant to be used for the remote controller. It handles the logic for sending and and recieving messages over the 915Mhz link to the field unit. 
  When messages are recieved, the information encoded in the payload is decoded and stored in members of a global struct 'recievedInfo' which is defined in ClientConfigure.h. 
  To send messages, first the desired payload and the length should be stored in the global struct 'clientInfo'. If the message is a measurment data request, the member 'payloadIsData' should be set to TRUE. 
  Call the function 'Run_ClientFSM()' with a 'TRANSMIT_REQUEST_EVENT' to send this payload. The packet will only be sent if the current state of the remote controller is not in the middle of handling a transaction. 
  currently there is no way to queue packets to be sent. 
  
  Written by CO2 leaf meter
*/



#include "Arduino.h"   //This gives access to a variety of functions and #defines that ease development. (Part of the native arduino toolchain)
#include "ClientFSM.h" //contains protoypes for the functions in this file (Written by CO2 leaf meter)
#include "RemoteConfigure.h" //This gives access to enums, structs, globals which are needed (Written by CO2 leaf meter)
#include "EventChecker.h"    //This gives access to event checking functions. In particular it allows timers to be set (Written by CO2 leaf meter)



// These defines are used with the retry capability of our protocol. The retry periods are used to set timers when packets are transmitted. If these timers expire, a retransmission 
// will take place unless there have been too many retries. 
#define ALLOWED_RETRIES 2 //number of retries before giving up transmission and warning the user
#define RETRY_PERIOD_MAX 1700 //twice the round trip ping of a 255 byte payload (ms)
#define RETRY_PERIOD_MIN 250 //twice the round trip ping of a 16 byte payload (ms)


typedef enum { //The list of possible states for the porotocol state machine. 
    InitPState, 				//This is used for any initialization at the start of the program. This state should be left immediately and not returned to.
	State0_Idle,				//In this state, the remote controller is waiting to transmit a packet and is not listening for messages from the field unit
    State1_WaitingForACK,		//In this state, a packet has been sent and the remote controller is expecting a response from the field unit
    State2_GettingData,			//This state is used when the remote is expecting measurment data to be transmitted by the field unit.
	State3_TestingLink,			//This state is used when setting up the field unit to test the link strength.
} ClientFSMStates;

// Holds the current state
static ClientFSMStates CurrentState = InitPState;

// This function runs the state machine with an INIT_EVENT to initialize the state machine. It should only be called once. When called the state machine is set to Idle
uint8_t Init_ClientFSM(void){
	Event thisEvent;
	thisEvent.EventType = INIT_EVENT;
	thisEvent.EventParam = 0;
	Event returnEvent = Run_ClientFSM(thisEvent);
	if (returnEvent.EventType == NO_EVENT) {
		return TRUE;
	} else {
		return FALSE;
	}
}



/*
	This is the main function of this state machine. It takes an event (type and parameter) as an input. Based on the current state, the events will be handled differently. 
	The state machine waits in an idle state until it is prompted to transmit a message by a TRANSMIT_REQUEST_EVENT. Once transmitted, the state will change to either wait for 
	an ack, or a measurment data packet depending on if the transmitted message was a data request. If awaiting an ack, once a packet is correctly recieved, it relays the message 
	to the GUI via the bluetooth module. It will parse the response, store it locally in the 'recieved_info' struct but as of 5/30/20 does nothing with that parsed data. If no response
	is recieved after the maximum number of retries, an error message is sent to the GUI. 
	
	If the transmitted message to the field unit was a request for measurment data, the state will change from Idle to GettingData. When a data packet is recieved, it is parsed 
	and is checked if it is the last packet to be sent. If not it will send an acknowldegment to indicate it is ready for the next packet. If it is the last packet, the state 
	will return to idle and no acknowldegment is needed. In the case that a packet is not recieved in an appriate amount of time, a non-acknowlengment will be sent to the field unit,
	indicating a request for retransmission of the most recent packet. 
	
	Finally if the current state is idle, a LINK_SETUP_EVENT can be used to change states to testingLink. Here the remote controller will wait for a ping message, then respond
	with a pong message. This state transitions back to idle on a END_LINK_SETUP_EVENT. 
	
	5/30/20 KNOWN EDGE CASE: While recieving measurment data, if an ack is not heard by the field unit, if the following nack is recieved, it will send the wrong data packet.
*/
Event Run_ClientFSM(Event thisEvent) {
	
	uint8_t makeTransition = FALSE; // use to flag transition
	ClientFSMStates nextState;
	int i,j;	// these variables are used to iterate through payload data during decoding and to copy data between two arrays which hold transmitted payloads

	
	switch (CurrentState) {
		case InitPState:								// If current state is initial Pseudo State
			if (thisEvent.EventType == INIT_EVENT){		// only respond to INIT_EVENT
				clientInfo.payloadIsData = FALSE;
				clientInfo.retry_number = 0;
				clientInfo.total_retries = 0;
				clientInfo.transciever_state = TRANSMITTING;
				nextState = State0_Idle;				// transition to first state where we wait for a command to transmit
				makeTransition = TRUE;
			}
			break;
		
		case State0_Idle: //In this state, the transciever is in its idle state and awaits a request to send a packet. 
			switch(thisEvent.EventType){
				case ENTRY_EVENT:
					clientInfo.transciever_state = TRANSMITTING;
					thisEvent.EventType = NO_EVENT;
					break;
					
				case TRANSMIT_REQUEST_EVENT:		//transmissions are made by storing the desired payload into the transciever struct calling this function with this event while in this state
					rf95.send(clientInfo.payloadToSend, clientInfo.payloadToSend_length);
					clientInfo.transciever_state = RECIEVING; //this is set to start the event checker, thereby putting the chip into reciever mode as soon as transmission ends
					
					for (i = 0; i < clientInfo.payloadToSend_length; i++){ //keep a copy of the most recent sent payload.
						clientInfo.previousPayload[i] = clientInfo.payloadToSend[i];
					}
					clientInfo.previousPayload_length = clientInfo.payloadToSend_length;
					
					if (clientInfo.payloadIsData == TRUE){
							SetTimer(2, RETRY_PERIOD_MAX);
							nextState = State2_GettingData;
							makeTransition = TRUE;
					}else{
						SetTimer(2, RETRY_PERIOD_MIN);
						nextState = State1_WaitingForACK;
						makeTransition = TRUE;
					}
					break;
				
				case LINK_SETUP_EVENT:	//move to setup state to test RSSI. Must set the client to be a reciever to await a ping message
					clientInfo.transciever_state = RECIEVING;
					nextState = State3_TestingLink;
					makeTransition = TRUE;
					break;
					
				default:
					break;
				
			}
			break;

		case State1_WaitingForACK: //If the packet being sent is a control/configuration, the client waits for an ack and decodes the response message. If a timeout ocurrs, a rentransmission will be sent a finite number of times. 
			switch (thisEvent.EventType) {
				case ENTRY_EVENT:
						clientInfo.retry_number = 0;
					break;
					
				case RF_RECIEVE_EVENT:				
					info_recieved.payloadLength = RH_RF95_MAX_MESSAGE_LEN; //this is needed to indicate that the entire payload may be copied
					rf95.recv(info_recieved.raw_payload, &info_recieved.payloadLength);
					clientInfo.transciever_state = TRANSMITTING; //set back to transmitter. All this does is disable the event checker so the chip stays in idle mode. Also allows for new BLE message 
					info_recieved.messageID = info_recieved.raw_payload[0];
					BLE_send(info_recieved.raw_payload, info_recieved.payloadLength); //*relay message to GUI
					

					if(info_recieved.messageID == ID_INVALID){              //Here is where the decoding happens. Each of these cases will likely send an event to the main state machine. To understand what each message indicates, look in MessageIDs.h						
						nextState = State0_Idle;
						makeTransition = TRUE;
						
					}else if (info_recieved.messageID == CONFIGURE_DATE_TIME_ACK){
						Serial.println("Received: Config Date/Time");
						nextState = State0_Idle;
						makeTransition = TRUE;
						
					}else if (info_recieved.messageID == MEASUREMENT_REQUEST_ACK){
						Serial.println("Received: Measurement request");
						info_recieved.measurement_confirmed = info_recieved.raw_payload[1];
						nextState = State0_Idle;
						makeTransition = TRUE;
						
					}else if (info_recieved.messageID == CONFIGURE_TOTAL_MEASUREMENTS_ACK){
						Serial.println("Received: Config Total Measurements");
						info_recieved.remaining_cycles = info_recieved.raw_payload[1];
						nextState = State0_Idle;
						makeTransition = TRUE;
						
					}else if (info_recieved.messageID == CONFIGURE_CYCLE_PERIOD_ACK){
						Serial.println("Received: Config Cycle Period");
						nextState = State0_Idle;
						makeTransition = TRUE;
						
					}else if (info_recieved.messageID == POWER_OFF_REQUEST_ACK){
						Serial.println("Received: Power Off Req");
						info_recieved.time_till_powerOff = ((uint16_t)info_recieved.raw_payload[1]<<8) | (info_recieved.raw_payload[2]); // there is a shift by 8 because shorts are sent Big Endian. '|' logical OR to recover the 16 bit value
						nextState = State0_Idle;
						makeTransition = TRUE;				
						
					}else if (info_recieved.messageID == UPDATE_STATUS_REQUEST_ACK){
						Serial.println("Received: Update Status Request");
						info_recieved.time_left_battery = ((uint16_t)info_recieved.raw_payload[1]<<8) | (info_recieved.raw_payload[2]);
						info_recieved.num_measurments_taken = info_recieved.raw_payload[3];
						
						j = 4; //this is used as a payload pointer
						for (i = 0; i < info_recieved.num_measurments_taken; i++){ //this loop iterates through the all the timestamps and stores the hours/minutes into the global array
							info_recieved.timestamps[0][i] = info_recieved.raw_payload[j++];	   // 'i' is used to iterate through the samples, and 'j' is used to hold the current pointer to the payload
							info_recieved.timestamps[1][i] = info_recieved.raw_payload[j++];							
						}
						
						info_recieved.time_till_nextCycle = ((uint16_t)info_recieved.raw_payload[j]<<8) + (info_recieved.raw_payload[j+1]);
	
						nextState = State0_Idle;
						makeTransition = TRUE;
						
					}else	{
						info_recieved.messageID = ID_INVALID;
						nextState = State0_Idle;
						makeTransition = TRUE;

					}
					break;
					
				case TIMEOUT:
					if (clientInfo.retry_number < ALLOWED_RETRIES){
						Serial.println("retrying");
						rf95.send(clientInfo.previousPayload, clientInfo.previousPayload_length);
						clientInfo.transciever_state = RECIEVING;
						SetTimer(2, RETRY_PERIOD_MIN);
						clientInfo.retry_number++;
						clientInfo.total_retries++;
					} else{
						Serial.println("retries exceeded, msg to server failure");
						holder[0] = ID_INVALID; //*send an invalid ID so that the user can be warned that there has been a failure
						BLE_send(holder,1);
						clientInfo.retry_number = 0;
						nextState = State0_Idle;
						makeTransition = TRUE;
					}
					break;
					
				default:
					break;
			}				
			break;	



		case State2_GettingData: 	//if the client is requesting data, packets will continue being sent by the server with acks from the client as repeated starts. Timeouts and retries apply here as well. The first message from the field unit contains metadata 
			switch(thisEvent.EventType){
				case ENTRY_EVENT:
					clientInfo.retry_number = 0;
					break;
					
				case RF_RECIEVE_EVENT:						
					info_recieved.payloadLength = RH_RF95_MAX_MESSAGE_LEN; //this is needed to indicate that the entire payload may be copied
					rf95.recv(info_recieved.raw_payload, &info_recieved.payloadLength);
					info_recieved.messageID = info_recieved.raw_payload[0];

					if (info_recieved.messageID == DATA_TRANSMISSION_INITIAL){//decode first payload which contains metadata 
						//for now going to skip the timestamp data because we should already have that information through update.? 
						info_recieved.sample_period = ((uint16_t)info_recieved.raw_payload[6]<<8) + (info_recieved.raw_payload[7]);
						info_recieved.flux_estimate = ((uint16_t)info_recieved.raw_payload[8]<<8) + (info_recieved.raw_payload[9]);
						info_recieved.data_packets_total = info_recieved.raw_payload[10];
						info_recieved.data_packet_current = info_recieved.raw_payload[11];
						info_recieved.num_sample_points = info_recieved.raw_payload[12]; //get number of samples for this packet. 
							
						j = 13; //pointer for payload index
						for (i = 0; i < info_recieved.num_sample_points; i++){ //loop to get all sample points
							info_recieved.recent_temp[i] = info_recieved.raw_payload[j++];
							info_recieved.recent_humidity[i] = info_recieved.raw_payload[j++];
							info_recieved.recent_CO2[i] = ((uint16_t)info_recieved.raw_payload[j]<<8) + (info_recieved.raw_payload[j+1]);
							j = j + 2;
							info_recieved.recent_pressure[i] = ((uint32_t)info_recieved.raw_payload[j]<<24) + ((uint32_t)info_recieved.raw_payload[j+1]<<16) + ((uint32_t)info_recieved.raw_payload[j+2]<<8) + ((uint32_t)info_recieved.raw_payload[j+3]);
							j = j + 4;
							info_recieved.recent_light[i] = ((uint32_t)info_recieved.raw_payload[j]<<24) + ((uint32_t)info_recieved.raw_payload[j+1]<<16) + ((uint32_t)info_recieved.raw_payload[j+2]<<8) + ((uint32_t)info_recieved.raw_payload[j+3]);
							j = j + 4;
						}
						//DO SOMETHING NOW THAT THE DATA HAS BEEN STORED?
						
						if (info_recieved.data_packets_total == info_recieved.data_packet_current){ //if for some reason only one packet is going to be sent, return to idle
							nextState = State0_Idle;
							makeTransition = TRUE;	
						} else {
							clientInfo.payloadToSend[0] = DATA_TRANSMISSION_ACK; //load the payload to send an ack back to the field unit
							clientInfo.payloadToSend_length = 1;
							rf95.send(clientInfo.payloadToSend, clientInfo.payloadToSend_length);
							clientInfo.transciever_state = RECIEVING; //this is set to start the event checker, thereby putting the chip into reciever mode as soon as transmission ends
							SetTimer(2, RETRY_PERIOD_MAX);
							clientInfo.retry_number = 0;
						}
						
						for (i = 0; i < clientInfo.payloadToSend_length; i++){ //keep a copy of the most recent sent payload.
						clientInfo.previousPayload[i] = clientInfo.payloadToSend[i];
						}
						clientInfo.previousPayload_length = clientInfo.payloadToSend_length;
						
					} else if (info_recieved.messageID == DATA_TRANSMISSION){ //if the recieved message is not the first packet of measurment data, store data in the info_recieved struct. Send an ack if its not the last packet.
						info_recieved.data_packets_total = info_recieved.raw_payload[1];
						info_recieved.data_packet_current = info_recieved.raw_payload[2];
						info_recieved.num_sample_points = info_recieved.raw_payload[3]; //get number of samples for this packet. 
							
						j = 4; //pointer for payload index
						for (i = 0; i < info_recieved.num_sample_points; i++){ //loop to get all sample points
							info_recieved.recent_temp[i] = info_recieved.raw_payload[j++];
							info_recieved.recent_humidity[i] = info_recieved.raw_payload[j++];
							info_recieved.recent_CO2[i] = ((uint16_t)info_recieved.raw_payload[j]<<8) + (info_recieved.raw_payload[j+1]);
							j = j + 2;
							info_recieved.recent_pressure[i] = ((uint32_t)info_recieved.raw_payload[j]<<24) + ((uint32_t)info_recieved.raw_payload[j+1]<<16) + ((uint32_t)info_recieved.raw_payload[j+2]<<8) + ((uint32_t)info_recieved.raw_payload[j+3]);
							j = j + 4;
							info_recieved.recent_light[i] = ((uint32_t)info_recieved.raw_payload[j]<<24) + ((uint32_t)info_recieved.raw_payload[j+1]<<16) + ((uint32_t)info_recieved.raw_payload[j+2]<<8) + ((uint32_t)info_recieved.raw_payload[j+3]);
							j = j + 4;
						}

						if (info_recieved.data_packets_total == info_recieved.data_packet_current){ //check if this is the last packet. If it is, dont send ack and go back to idle
							nextState = State0_Idle;
							makeTransition = TRUE;	
						} else {
							clientInfo.payloadToSend[0] = DATA_TRANSMISSION_ACK; //load the payload to send an ack back to the field unit
							clientInfo.payloadToSend_length = 1;
							rf95.send(clientInfo.payloadToSend, clientInfo.payloadToSend_length);
							clientInfo.transciever_state = RECIEVING; //this is set to start the event checker, thereby putting the chip into reciever mode as soon as transmission ends
							SetTimer(2, RETRY_PERIOD_MAX);
							clientInfo.retry_number = 0;
							
							for (i = 0; i < clientInfo.payloadToSend_length; i++){ //keep a copy of the most recent sent payload.
							clientInfo.previousPayload[i] = clientInfo.payloadToSend[i];
							}
							clientInfo.previousPayload_length = clientInfo.payloadToSend_length;
						}
						
					} else{ //if the message ID is invalid for this state, go back to idle and warn the user? ADD NOTE TO DATAFILE THAT DATA IS WRONG?
						holder[0] = ID_INVALID; //*send an invalid ID so that the user can be warned that there has been a failure
						BLE_send(holder,1);
						nextState = State0_Idle;
						makeTransition = TRUE;	
					}
					
					break;
					
				case TIMEOUT:
					if (clientInfo.retry_number < ALLOWED_RETRIES){ //check if retry count is exceeded. If not retransmit the most recent packet and restart the timer. 
						clientInfo.payloadToSend[0] = DATA_TRANSMISSION_NACK; //load the payload to send an ack back to the field unit
						clientInfo.payloadToSend_length = 1;
						rf95.send(clientInfo.payloadToSend, clientInfo.payloadToSend_length);
						clientInfo.transciever_state = RECIEVING;
						SetTimer(2, RETRY_PERIOD_MAX);
						clientInfo.retry_number++;
						clientInfo.total_retries++;
					} else{ //retry count has been exceeded
						holder[0] = ID_INVALID; //*send an invalid ID so that the user can be warned that there has been a failure
						BLE_send(holder,1);
						clientInfo.retry_number = 0;
						nextState = State0_Idle;
						makeTransition = TRUE;
					}
					break;
					
				case EXIT_EVENT:
					clientInfo.payloadIsData = FALSE; //set this flag low as it has been handled. 
					clientInfo.retry_number = 0;
					clientInfo.transciever_state = TRANSMITTING; //all this does is disable the event checker for RF messages and allow for a new BLE message
					break;
					
				default:
					break;
				
			}
			break;			
		
		case State3_TestingLink:
			switch (thisEvent.EventType){
				case ENTRY_EVENT:
					//?send BLE message? light some LED? Ideally let user know somehow that it is in the correct mode
					break;
					
				case RF_RECIEVE_EVENT:
					info_recieved.payloadLength = RH_RF95_MAX_MESSAGE_LEN; 
					rf95.recv(info_recieved.raw_payload, &info_recieved.payloadLength); 
					info_recieved.messageID = info_recieved.raw_payload[0];
					
					if(info_recieved.messageID == PING){
						clientInfo.payloadToSend[0] = PONG; //load the payload to send an ack back to the field unit
						clientInfo.payloadToSend_length = 1;
						rf95.send(clientInfo.payloadToSend, clientInfo.payloadToSend_length);
					}
					
					break;
					
				case END_LINK_SETUP_EVENT:	
					//?send BLE message? light some LED? Ideally let user know somehow that it is in the correct mode
					nextState = State0_Idle;	// transciever_state will be set to transmitting upon entry to Idle state
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
		Run_ClientFSM(thisEvent);
		CurrentState = nextState;
		thisEvent.EventType = ENTRY_EVENT;
		Run_ClientFSM(thisEvent);
		thisEvent.EventType = NO_EVENT;
	}
	return thisEvent;
}
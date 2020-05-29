#ifndef MESSAGEIDS_H
#define MESSAGEIDS_H


#define COMMAND_TIMEOUT_PERIOD 500
#define DATA_TIMEOUT_PERIOD 1500


//Messages are sent in big endian

typedef enum {
    ID_INVALID = 0, 					// Invalid Packet, used as an error return from functions 
	
	PING,								// No payload. To be used to test connectivity [FIELD -> REMOTE]
	
	PONG,								// No payload. To be used to test connectivity [REMOTE -> FIELD]
	
    CONFIGURE_DATE_TIME, 				// Contains the day (char), month (char), year (char), hour (char), minute (char) [GUI -> REMOTE -> FIELD]
	
    CONFIGURE_DATE_TIME_ACK, 			// No payload but indicates the messsage was recieved [FIELD -> REMOTE -> GUI]
	
	MEASUREMENT_REQUEST, 				// Contains indication of if data should be transmitted in real time (char) [GUI -> REMOTE -> FIELD]
	
	MEASUREMENT_REQUEST_ACK,			// Contains indication of if request has been accepted or not (char) [FIELD -> REMOTE -> GUI]
	
	CONFIGURE_TOTAL_MEASUREMENTS,		// Contains new number of requested measurements (char) [GUI -> REMOTE -> FIELD]
	
	CONFIGURE_TOTAL_MEASUREMENTS_ACK,	// Contains remaining number of measurements (char) [FIELD -> REMOTE -> GUI]
	
	CONFIGURE_CYCLE_PERIOD,				// Contains period in minutes between measurement cycles (short) [GUI -> REMOTE -> FIELD]
	
	CONFIGURE_CYCLE_PERIOD_ACK,			// No payload but indicates the messsage was recieved [FIELD -> REMOTE -> GUI]	
	
	POWER_OFF_REQUEST,					// Contains time in minutes until desired power off (short) [GUI -> REMOTE -> FIELD]
	
	POWER_OFF_REQUEST_ACK,				// Returned time until power off (short) [FIELD -> REMOTE -> GUI]
	
	UPDATE_STATUS_REQUEST,				// No payload. Requests an update from the field unit [GUI -> REMOTE -> FIELD]
	
	UPDATE_STATUS_REQUEST_ACK,			/* Contains estimated life of battery in minutes (short), # of measurments taken (char), 
										   timestamps of meaurements (hour = char, minute = char), time until next measurment (short) [FIELD -> REMOTE -> GUI] */
										
	DATA_TRANSMISSION_REQUEST, 			// Contains which number measurement cycle to transmit (char)[GUI -> REMOTE -> FIELD]
	
	DATA_TRANSMISSION_INITIAL, 			/* Contains the day (char), month (char), year (char), hour (char), minute (char), sample period (short), flux estimate (short), 
										   Then the typical info for DATA_TRANSMISSION [FIELD -> REMOTE]*/
										   
	DATA_TRANSMISSION,					/* Contains total packets to be sent (char), current packet number (char), number of samples in the packet (char), 
										   temperature (char), humidity (char), CO2 ppm(short),  pressure (int), lux (int), repeat last 5 till end of payload [FIELD -> REMOTE]*/
	
	DATA_TRANSMISSION_ACK, 				// The expected reponse for DATA_TRANSMISSION_INITIAL and DATA_TRANSMISSION [REMOTE -> FIELD]
	
	DATA_TRANSMISSION_NACK,				// The message to be sent if DATA_TRANSMISSION_INITIAL or DATA_TRANSMISSION is not recieved in time. [REMOTE -> FIELD]
	
	
	
	//==============================================================================================================================
	
	
	//commands sent between GUI and Remote controller only 
	
	
	//==============================================================================================================================
	
	COMMUNICATION_FAILURE				// Used to indicate communication with the field unit failed [REMOTE -> GUI]
	
	SET_SETUP_MODE,						// The message to be sent to put the remote controller into RF setup mode. [GUI -> REMOTE]
	
	SET_SETUP_MODE_ACK,					// No payload but indicates the messsage was recieved. [REMOTE -> GUI]
	
	END_SETUP_MODE,						// The message to be sent to unset the remote controller from RF setup mode. [GUI -> REMOTE]
	
	END_SETUP_MODE_ACK,					// No payload but indicates the messsage was recieved. [REMOTE -> GUI]
	
	
} MessageIDs;


#endif	/* MESSAGEIDS_H */
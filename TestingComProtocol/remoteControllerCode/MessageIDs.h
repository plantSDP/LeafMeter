#ifndef MESSAGEIDS_H
#define MESSAGEIDS_H


#define COMMAND_TIMEOUT_PERIOD 500
#define DATA_TIMEOUT_PERIOD 1500


//Messages are sent in big endian

typedef enum {
    ID_INVALID = 0, 					// Invalid Packet, used as an error return from functions 
	
    CONFIGURE_DATE_TIME, 				// Contains the day (char), month (char), year (char), hour (char), minute (char) [SENT BY REMOTE]
	
    CONFIGURE_DATE_TIME_ACK, 			// No payload but indicates the messsage was recieved [SENT BY FIELD]
	
	MEASUREMENT_REQUEST, 				// Contains indication of if data should be transmitted in real time (char) [SENT BY REMOTE]
	
	MEASUREMENT_REQUEST_ACK,			// Contains indication of if request has been accepted or not (char) [SENT BY FIELD]
	
	CONFIGURE_TOTAL_MEASUREMENTS,		// Contains new number of requested measurements (char) [SENT BY REMOTE]
	
	CONFIGURE_TOTAL_MEASUREMENTS_ACK,	// Contains remaining number of measurements (char) [SENT BY FIELD]
	
	CONFIGURE_CYCLE_PERIOD,				// Contains period in minutes between measurement cycles (short) [SENT BY REMOTE]
	
	CONFIGURE_CYCLE_PERIOD_ACK,			// No payload but indicates the messsage was recieved [SENT BY FIELD]	
	
	POWER_OFF_REQUEST,					// Contains time in minutes until desired power off (short) [SENT BY REMOTE]
	
	POWER_OFF_REQUEST_ACK,				// Returned time until power off (short) [SENT BY FIELD]
	
	UPDATE_STATUS_REQUEST,				// No payload [SENT BY REMOTE]
	
	UPDATE_STATUS_REQUEST_ACK,			/* Contains estimated life of battery in minutes (short), # of measurments taken (char), 
										   timestamps of meaurements (hour = char, minute = char), time until next measurment (short) [SENT BY FIELD] */
										
	DATA_TRANSMISSION_REQUEST, 			// Contains which number measurement cycle to transmit (char)[SENT BY REMOTE]
	
	DATA_TRANSMISSION_INITIAL, 			/* Contains the day (char), month (char), year (char), hour (char), minute (char), sample period (short), flux estimate (short), 
										   Then the typical info for DATA_TRANSMISSION [SENT BY FIELD]*/
										   
	DATA_TRANSMISSION,					/* Contains total packets to be sent (char), current packet number (char), number of samples in the packet (char), 
										   temperature (char), humidity (char), CO2 ppm(short),  pressure (int), lux (int), repeat last 5 till end of payload [SENT BY FIELD]*/
	
	DATA_TRANSMITTION_ACK, 				// Contains the packet number recieved (char). The expected reponse for DATA_TRANSMISSION_INITIAL and DATA_TRANSMISSION [SENT BY REMOTE]
						
} MessageIDs;


#endif	/* MESSAGEIDS_H */
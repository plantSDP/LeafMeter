//  messageID_enum.swift
//  PlantCO2Meter
//
//  Created by Sam Bower on 5/29/20.
//  Copyright © 2020 Sam Bower. All rights reserved.
//

enum messageID: UInt8{
    
    case ID_INVALID = 0                     // Invalid Packet, used as an error return from functions
    
    case PING                               // No payload. To be used to test connectivity [FIELD -> REMOTE]
    
    case PONG                                // No payload. To be used to test connectivity [REMOTE -> FIELD]
    
    case CONFIGURE_DATE_TIME                 // Contains the day (char), month (char), year (char), hour (char), minute (char) [GUI -> REMOTE -> FIELD]
    
    case CONFIGURE_DATE_TIME_ACK             // No payload but indicates the messsage was recieved [FIELD -> REMOTE -> GUI]
    
    case MEASUREMENT_REQUEST                 // Contains indication of if data should be transmitted in real time (char) [GUI -> REMOTE -> FIELD]
    
    case MEASUREMENT_REQUEST_ACK            // Contains indication of if request has been accepted or not (char) [FIELD -> REMOTE -> GUI]
    
    case CONFIGURE_TOTAL_MEASUREMENTS        // Contains new number of requested measurements (char) [GUI -> REMOTE -> FIELD]
    
    case CONFIGURE_TOTAL_MEASUREMENTS_ACK    // Contains remaining number of measurements (char) [FIELD -> REMOTE -> GUI]
    
    case CONFIGURE_CYCLE_PERIOD               // Contains period in minutes between measurement cycles (short) [GUI -> REMOTE -> FIELD]
    
    case CONFIGURE_CYCLE_PERIOD_ACK            // No payload but indicates the messsage was recieved [FIELD -> REMOTE -> GUI]
    
    case POWER_OFF_REQUEST                    // Contains time in minutes until desired power off (short) [GUI -> REMOTE -> FIELD]
    
    case POWER_OFF_REQUEST_ACK                // Returned time until power off (short) [FIELD -> REMOTE -> GUI]
    
    case UPDATE_STATUS_REQUEST                // No payload. Requests an update from the field unit [GUI -> REMOTE -> FIELD]
    
    case UPDATE_STATUS_REQUEST_ACK            /* Contains estimated life of battery in minutes (short), # of measurments taken (char),
     timestamps of meaurements (hour = char, minute = char), time until next measurment (short) [FIELD -> REMOTE -> GUI] */
    
    case DATA_TRANSMISSION_REQUEST             // Contains which number measurement cycle to transmit (char)[GUI -> REMOTE -> FIELD]
    
    case DATA_TRANSMISSION_INITIAL             /* Contains the day (char), month (char), year (char), hour (char), minute (char), sample period (short), flux estimate (short),
     Then the typical info for DATA_TRANSMISSION [FIELD -> REMOTE]*/
    
    case DATA_TRANSMISSION                    /* Contains total packets to be sent (char), current packet number (char), number of samples in the packet (char),
     temperature (char), humidity (char), CO2 ppm(short),  pressure (int), lux (int), repeat last 5 till end of payload [FIELD -> REMOTE]*/
    
    case DATA_TRANSMISSION_ACK                 // The expected reponse for DATA_TRANSMISSION_INITIAL and DATA_TRANSMISSION [REMOTE -> FIELD]
    
    case DATA_TRANSMISSION_NACK                // The message to be sent if DATA_TRANSMISSION_INITIAL or DATA_TRANSMISSION is not recieved in time. [REMOTE -> FIELD]
    
    //commands sent between GUI and Remote controller only
    
    
    case COMMUNICATION_FAILURE                // Used to indicate communication with the field unit failed [REMOTE -> GUI]
    
    case SET_SETUP_MODE                        // The message to be sent to put the remote controller into RF setup mode. [GUI -> REMOTE]
    
    case SET_SETUP_MODE_ACK                    // No payload but indicates the messsage was recieved. [REMOTE -> GUI]
    
    case END_SETUP_MODE                        // The message to be sent to unset the remote controller from RF setup mode. [GUI -> REMOTE]
    
    case END_SETUP_MODE_ACK                    // No payload but indicates the messsage was recieved. [REMOTE -> GUI]
}

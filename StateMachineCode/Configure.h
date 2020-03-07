#ifndef CONFIGURE_H
#define CONFIGURE_H

#include "Arduino.h"

#define TRUE 1
#define FALSE 0

#define NO_PRESS	0
#define BTN1		1
#define BTN2		2
#define BTN3		3
#define BTN4		4


//the possible events for the state machine. Add new events to this enum. 
typedef enum {
	NO_EVENT,
	ENTRY_EVENT,
	EXIT_EVENT,
	INIT_EVENT,
	BTN_EVENT,
	TIMEOUT} eventTypes;

//struct to hold event types and parameter.
//this struct will be the argument for the state machine functions, and the event checkers generate these events. 
typedef struct Events{
	eventTypes EventType;
	int EventParam;
}Event;



#endif

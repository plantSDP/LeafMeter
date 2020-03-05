#ifndef CONFIGURE_H
#define CONFIGURE_H


//the possible events for the state machine. Add new events to this enum. 
typedef enum {
	NO_EVENT,
	ENTRY_EVENT,
	EXIT_EVENT,
	INIT,
	BTN1,
	BTN2,
	BTN3,
	BTN4,
	TIMEOUT} eventTypes;

//struct to hold event types and parameter.
//this struct will be the argument for the state machine functions, and the event checkers generate these events. 
typedef struct Events{
	eventTypes EventType,
	uint16_t EventParam,
}Event;



#endif

#ifndef EVENTCHECK_H
#define EVENTCHECK_H
#include "Configure.h"
#include "Arduino.h"

#ifdef __cplusplus // This was added to fix compilation error for "name mangling"
extern "C" {
#endif

/*
typedef struct ButtonEvent{
	uint8_t ButtonFlag;
	uint16_t ButtonParams; //bitmasked button states (bit0 = btn1, bit1 = btn2...)
}ButtonEvent;
*/

/*
Initializes the event checkers. 
*/
uint8_t InitEventCheckers(void);


/*
debounces the buttons and set the flag in the struct, along with the bitmasked button states
*/
Event ButtonCheckDebounce(void);



#ifdef __cplusplus //closes the above fix for "name mangling"
}
#endif

#endif

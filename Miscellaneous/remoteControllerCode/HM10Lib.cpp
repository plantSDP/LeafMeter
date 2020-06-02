//HM10 library
#include "Arduino.h"


/*
Initializes the UART for communication with the module
*/
void BLE_Init(void){
	//set up Serial1 for Uart communication 
	Serial1.setTX(1);
	Serial1.setRX(0);
	Serial1.begin(9600, SERIAL_8N1);
	delay(150); //delay to avoid sending a message too quickly
}

/*
wrapper function to send a series of bytes of lenght len from an array to the bluetooth module. 
*/
void BLE_send(uint8_t* msg, uint8_t len){ 
	Serial1.write(msg, len);
}


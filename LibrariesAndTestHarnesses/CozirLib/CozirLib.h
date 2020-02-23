#ifndef Test_H
#define Test_H


#include "Arduino.h"
#define TEST 200


typedef struct Data{
	unsigned int co2;
	int temp;
	unsigned int rh;
}Data;



/*
Initializes the sensor for polling mode communication.
*/
uint8_t Cozir_Init(void);


/*
Calibrates the sensor to set the current reading as 400 ppm.
*/
uint8_t Cozir_Calibrate(void);


/*
update members of data struct 
*/
uint8_t Cozir_NewDataAvailable(void);


/*
request data from sensor
*/
void Cozir_Request_Data(void);



/*
returns co2 concentration of most recent read 
*/
unsigned int Cozir_Get_Co2(void);

/*
returns temp of most recent read 
*/
int Cozir_Get_Temp(void);

/*
returns humidity of most recent read  
*/
unsigned int Cozir_Get_Rh(void);


#endif
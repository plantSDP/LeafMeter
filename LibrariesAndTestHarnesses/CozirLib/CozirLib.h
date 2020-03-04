#ifndef Test_H
#define Test_H


#include "Arduino.h"
#define TEST 200


typedef struct CozirData{
	unsigned int co2;
	int temp;
	unsigned int rh;
}CozirData;



/*
Initializes the sensor for polling mode communication.
*/
uint8_t Cozir_Init(void);


/*
Calibrates the sensor to set the current reading as 400 ppm.
*/
uint8_t Cozir_Calibrate(void);


/*
 *Checks if the COzir has sent new data. If it has, store the data in the module struct.
 *This function should only be called if the serial buffer is empty or receiving data 
 *from Cozir_Request_Data(). Calling this function outside this context, will give bad 
 *data and ruin whatever other response the COzir was attempting to send. 
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
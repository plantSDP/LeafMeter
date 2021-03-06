#include "CozirLib.h"

#define OUTPUT_FIELDS 4162 //This is the decimal code to have the sensor output humidity, temperature, and unfiltered CO2
static CozirData data;
static char inString[100];
static unsigned int inStringPointer = 0;
static char outString[20];

static CozirData data2;
static char inString2[100];
static unsigned int inStringPointer2 = 0;
static char outString2[20];

static uint8_t dataRetrieveFlag = 0;
static uint8_t dataRetrieveFlag2 = 0;

/*
Initializes the sensor for polling mode communication. Blocking code to ensure transmission

Pin 0 is RX
Pin 1 is TX

Cozir sensor works on 9600 baud with 8 data bits, one stop bit. No parity
*/
uint8_t Cozir_Init(void){
	//set up Serial1 for Uart communication with COZIR
	Serial1.setTX(1);
	Serial1.setRX(0);
	Serial1.begin(9600, SERIAL_8N1);
	delay(150);//without this delay, the sensor will think that a reset is a start bit and will return a '?' when given a command immediately. 
	
	
	//set sensor to polling mode
	Serial1.print("K 2\r\n");
	delay(150);//include delay to ensure response is recieved 
	
	//while (Serial.available() < 8);
	uint8_t j = 0;
	while (Serial1.available() > 0){
		inString[j] = Serial1.read();
		j++;
	}
	
	if (inString[1] != 'K'){
		Serial.println(inString);
		return 0;
	}
	
	sprintf(outString, "M %d\r\n", OUTPUT_FIELDS);
	Serial1.print(outString); //The sensor stores setting in NVM so this actually shouldnt be called every power up.
	delay(150);
	j = 0;
	while (Serial1.available() > 0){
		inString[j] = Serial1.read();
		j++;
	}
	return 1;
	
}


/*
Calibrates the sensor to set the current reading as 400 ppm.
NOT IMPLEMENTED YET
*/
uint8_t Cozir_Calibrate(void){
	if (dataRetrieveFlag == 0){
		Serial1.print("G\r\n"); //requests the sensor to calibrate to 400ppm
		
		delay(150);//include delay to ensure response is recieved 
	
		//while (Serial.available() < 8);
		uint8_t j = 0;
		while (Serial1.available() > 0){
			inString[j] = Serial1.read();
			j++;
		}
	
		if (inString[1] != 'G'){
			Serial.println(inString);
			return 0;
		}
		
		return 1;
	}
	
	return 0;
	
}



/*
request data from sensor
*/
void Cozir_Request_Data(void){
	Serial1.print("Q\r\n"); //requests the sensor to send the data. 
	dataRetrieveFlag = 1;
}


/*
update members of data struct 
*/
uint8_t Cozir_NewDataAvailable(void){
	
	while (Serial1.available() > 0){
		inString[inStringPointer] = Serial1.read();
		inStringPointer++;
		if (inString[inStringPointer - 1] == '\n'){
			break;
		}
	}
	
	if ((inStringPointer > 0) && (inString[inStringPointer - 1] == '\n')){
		
		inStringPointer = 0;
		
		//convert the string into integer values and store them in the struct
		//may need to subtract 1 from each index
		data.co2 = inString[23] - '0';
		data.co2 += (inString[22] - '0') * 10; //subtract 48 to get decimal value
		data.co2 += (inString[21] - '0') * 100;
		data.co2 += (inString[20] - '0') * 1000;
		data.co2 += (inString[19] - '0') * 10000;

		data.temp = (inString[15] - '0');
		data.temp += ((inString[14] - '0') * 10);
		data.temp += ((inString[13] - '0') * 100);
		data.temp += ((inString[12] - '0') * 1000);
		data.temp += ((inString[11] - '0') * 10000);
		data.temp = (data.temp - 1000) / 10;

		data.rh = (inString[7] - '0');
		data.rh += ((inString[6] - '0') * 10);
		data.rh += ((inString[5] - '0') * 100);
		data.rh += ((inString[4] - '0') * 1000);
		data.rh += ((inString[3] - '0') * 10000);
		data.rh = data.rh / 10;
		
		dataRetrieveFlag = 0;
		return 1;
		
	}else{
		return 0;
	}
	
}



/*
returns co2 concentration of most recent read 
*/
unsigned int Cozir_Get_Co2(void){
	return data.co2;
}


/*
returns temp of most recent read 
*/
int Cozir_Get_Temp(void){
	return data.temp;
}

/*
returns humidity of most recent read  
*/
unsigned int Cozir_Get_Rh(void){
	return data.rh;
}




/* 
code for if using serial 2
*/

uint8_t Cozir2_Init(void){
	Serial2.setTX(10);
	Serial2.setRX(9);
	Serial2.begin(9600, SERIAL_8N1);
	delay(150);

	Serial2.print("K 2\r\n");
	delay(150);

	uint8_t j = 0;
	while (Serial2.available() > 0){
		inString2[j] = Serial2.read();
		j++;
	}
	
	if (inString2[1] != 'K'){
		Serial.println(inString);
		return 0;
	}
	
	sprintf(outString2, "M %d\r\n", OUTPUT_FIELDS);
	Serial2.print(outString2);
	delay(150);
	j = 0;
	while (Serial2.available() > 0){
		inString2[j] = Serial2.read();
		j++;
	}
	return 1;
	
}




void Cozir2_Request_Data(void){
	Serial2.print("Q\r\n"); 
	dataRetrieveFlag2 = 1;
}



uint8_t Cozir2_NewDataAvailable(void){
	
	while (Serial2.available() > 0){
		inString2[inStringPointer2] = Serial2.read();
		inStringPointer2++;
		if (inString2[inStringPointer2 - 1] == '\n'){
			break;
		}
	}
	
	if ((inStringPointer2 > 0) && (inString2[inStringPointer2 - 1] == '\n')){
		
		inStringPointer2 = 0;

		data2.co2 = inString2[23] - '0';
		data2.co2 += (inString2[22] - '0') * 10; 
		data2.co2 += (inString2[21] - '0') * 100;
		data2.co2 += (inString2[20] - '0') * 1000;
		data2.co2 += (inString2[19] - '0') * 10000;

		data2.temp = (inString2[15] - '0');
		data2.temp += ((inString2[14] - '0') * 10);
		data2.temp += ((inString2[13] - '0') * 100);
		data2.temp += ((inString2[12] - '0') * 1000);
		data2.temp += ((inString2[11] - '0') * 10000);
		data2.temp = (data2.temp - 1000) / 10;

		data2.rh = (inString2[7] - '0');
		data2.rh += ((inString2[6] - '0') * 10);
		data2.rh += ((inString2[5] - '0') * 100);
		data2.rh += ((inString2[4] - '0') * 1000);
		data2.rh += ((inString2[3] - '0') * 10000);
		data2.rh = data2.rh / 10;
		
		dataRetrieveFlag2 = 0;
		return 1;
		
	}else{
		return 0;
	}
	
}




unsigned int Cozir2_Get_Co2(void){
	return data2.co2;
}



int Cozir2_Get_Temp(void){
	return data2.temp;
}


unsigned int Cozir2_Get_Rh(void){
	return data2.rh;
}

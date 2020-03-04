#include "CozirLib.h"

#define OUTPUT_FIELDS 4162 //This is the decimal code to have the sensor output humidity, temperature, and unfiltered CO2
static CozirData data;
static char inString[100];
static unsigned int inStringPointer = 0;
static char outString[20];

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
request data from sensor
*/
void Cozir_Request_Data(void){
	Serial1.print("Q\r\n"); //requests the sensor to send the data. 
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


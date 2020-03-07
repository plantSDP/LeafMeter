#include "CozirLib.h"
#include "SPI.h"
//#include "SdCard.h"

void setup() {
  // put your setup code here, to run once:

    Serial.begin(9600);
   while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
  Serial.println("Initializing COzIR");
  delay(10000);
  uint8_t errorCheck = Cozir_Init();
  if(errorCheck == 0){
    Serial.println("Failure on Serial1");
  }else if (errorCheck == 1){
    Serial.println("Success on Serial1");
  }else {
    Serial.println("Serious Failure");
  }
  errorCheck = Cozir_Calibrate();
  if(errorCheck == 0){
    Serial.println("Calibration Failure");
  }else if (errorCheck == 1){
    Serial.println("Calibration Success");
  }else {
    Serial.println("Serious Failure");
  }

//  errorCheck = Cozir2_Init();
//  if(errorCheck == 0){
//    Serial.println("Failure on Serial2");
//  }else if (errorCheck == 1){
//    Serial.println("Success on Serial2");
//  }else {
//    Serial.println("Serious Failure");
//  }

}

elapsedMillis sinceLastReading;
elapsedMillis curTime;
CozirData cozirData;
CozirData cozirData2;
char rxVal = 0;
char myString[100];
unsigned int holder;

//comment out this loop if using two sensors
void loop() {
  if (sinceLastReading > 550){
    sinceLastReading = 0;
    Cozir_Request_Data();
  }

  if(Cozir_NewDataAvailable() == 1){
    cozirData.co2 = Cozir_Get_Co2();
    cozirData.temp = Cozir_Get_Temp();
    cozirData.rh = Cozir_Get_Rh();
    holder = curTime;
    //sprintf(myString, "%d\t%d\t%d", cozirData.co2, cozirData.temp, cozirData.rh);
    sprintf(myString, "%d\t%d\t%d\t%d", holder, cozirData.co2, cozirData.temp, cozirData.rh);
    Serial.println(myString);
  }
  

}

//comment either this loop or the one above depending on if one or two sensors are being used
//this loop will only print give co2 data and time in ms
//void loop() {
//  if (sinceLastReading > 550){
//    sinceLastReading = 0;
//    Cozir_Request_Data();
//    Cozir2_Request_Data();
//  }
//
//  if(Cozir_NewDataAvailable() == 1 && Cozir2_NewDataAvailable() == 1){
//    cozirData.co2 = Cozir_Get_Co2();
//    cozirData.temp = Cozir_Get_Temp();
//    cozirData.rh = Cozir_Get_Rh();
//
//    cozirData2.co2 = Cozir2_Get_Co2();
//    cozirData2.temp = Cozir2_Get_Temp();
//    cozirData2.rh = Cozir2_Get_Rh();
//    
//    holder = curTime;
//    sprintf(myString, "%d\t%d\t%d", holder, cozirData.co2, cozirData2.co2);
//    Serial.println(myString);
//  }
//  
//
//}

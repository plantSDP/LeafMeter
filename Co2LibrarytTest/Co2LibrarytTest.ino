#include "CozirLib.h"
//#include "SdCard.h"

void setup() {
  // put your setup code here, to run once:

    Serial.begin(9600);
   while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
Serial.println("Initializing COzIR");
  uint8_t errorCheck = Cozir_Init();
  if(errorCheck == 0){
    Serial.println("Failure");
  }else if (errorCheck == 1){
    Serial.println("Success");
  }else {
    Serial.println("Fuck");
  }

//  Serial1.setTX(1);
//  Serial1.setRX(0);
//  Serial1.begin(9600, SERIAL_8N1);
//  delay(3000);
//
//  Serial1.print("K 2\r\n");
//  delay(500);
//  Serial1.print("z\r\n");

}

elapsedMillis sinceLastReading;
elapsedMillis curTime;
Data cozirData;
char rxVal = 0;
char myString[100];
unsigned int holder;
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

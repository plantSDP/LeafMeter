#include "CozirLib.h"
#include "SPI.h"
//#include "SdCard.h"
#include <SD.h>

#define TOTAL_TIME 36000000

const int chipSelect = 4;

char folderName[8] = "folder";

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

  Serial.print("Initializing SD card...");

  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    while (1);
  }
  Serial.println("card initialized.");
}

elapsedMillis sinceLastReading;
elapsedMillis curTime;
CozirData cozirData;
char myString[100];
unsigned int holder;

char fileName[9] = "data.txt";

//comment out this loop if using two sensors
void loop() {
  if (sinceLastReading > 550){
    sinceLastReading = 0;
    Cozir_Request_Data();
  }
  if (curTime < TOTAL_TIME) { // 3 hrs = 10,800,000 ms
    if(Cozir_NewDataAvailable() == 1){
      cozirData.co2 = Cozir_Get_Co2();
      holder = curTime;
      // sprintf(myString, "%d\t%d\t%d", cozirData.co2, cozirData.temp, cozirData.rh);
      sprintf(myString, "%d", cozirData.co2);
      // Serial.println(myString);
  
      // open the file. note that only one file can be open at a time,
      // so you have to close this one before opening another.
      File dataFile = SD.open(fileName, FILE_WRITE);
  
      // if the file is available, write to it:
      if (dataFile) {
        dataFile.println(myString);
        dataFile.close();
        // print to the serial port too:
        Serial.println(myString);
      }
      // if the file isn't open, pop up an error:
      else {
        Serial.println("error opening data.txt");
      }
    }
  } else if (curTime == TOTAL_TIME){
    Serial.println("DONE");
  }
}

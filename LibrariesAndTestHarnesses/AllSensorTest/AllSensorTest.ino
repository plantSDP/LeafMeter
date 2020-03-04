#include <Adafruit_Sensor.h>
#include "Adafruit_TSL2591.h"
#include "BME280.h"
#include "CozirLib.h"


BME280 bme280Sensor(Wire,0x77);
Adafruit_TSL2591 tsl = Adafruit_TSL2591(2591);
uint32_t lum;
uint16_t ir, full;
uint16_t lux;
char myString[150];
elapsedMillis curTime;
elapsedMicros microTime;
unsigned int time1, time2, elapsedTime;
int pressureRead, tempRead, humidRead;
CozirData cozirData;

void setup() {
  Serial.begin(115200);
  while(!Serial){}
  Serial.println("Starting CO2 Meter Sensor Test");
  delay(10000);

  time1 = curTime;
  if (tsl.begin()) {
    elapsedTime = curTime - time1;
    sprintf(myString, "Found light sensor. Initialization time:  %d ms", elapsedTime);
    Serial.println(myString);
    tsl.setGain(TSL2591_GAIN_LOW);
    tsl.setTiming(TSL2591_INTEGRATIONTIME_100MS);
  } else {
    Serial.println("Light Sensor Not Found...");
    while (1);
  }

  time1 = curTime;
  if (bme280Sensor.begin() < 0) {
    Serial.println("Pressure Sensor Not Found...");
    while(1);
  } else {
    elapsedTime = curTime - time1;
    sprintf(myString, "Found pressure sensor. Initialization time:  %d ms", elapsedTime);
    Serial.println(myString);
    bme280Sensor.setNormalMode();
  }
 

  time1 = curTime;
  uint8_t errorCheck = Cozir_Init();
  if(errorCheck == 0){
    Serial.println("COzIR Not Found...");
    while(1);
  }else if (errorCheck == 1){
    elapsedTime = curTime - time1;
    sprintf(myString, "Found COzIR sensor. Initialization time:  %d ms", elapsedTime);
    Serial.println(myString);
    Cozir_Request_Data();
  }
  

}

void loop() {

  //read from light sensor and calculate lux
  time1 = curTime;
  lum = tsl.getFullLuminosity();
  ir = lum >> 16;
  full = lum & 0xFFFF;
  lux = tsl.calculateLux(full,ir);

  //read from pressure sensor and get data values
  bme280Sensor.readSensor();
  pressureRead = bme280Sensor.getPressure_Pa();
  tempRead = bme280Sensor.getTemperature_C();
  humidRead = bme280Sensor.getHumidity_RH();

  //read from COzIR sensor and request new data
  if(Cozir_NewDataAvailable() == 1){
    cozirData.co2 = Cozir_Get_Co2();
    Cozir_Request_Data();
  }
  
  
  
  elapsedTime = curTime - time1;
  sprintf(myString, "CO2: %d   Lux: %u,   Pressure: %d,   Temp: %d,   RH: %d,   Elapsed time:  %d", cozirData.co2, lux, pressureRead, tempRead, humidRead, elapsedTime);
  Serial.println(myString);

  delay(500);

}

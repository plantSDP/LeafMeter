/*
 * This is a test harness to check the pressure sensor basic functions. It will initialize the sensor 
 * and calculate the time it took to perform various actions. The readings from the sensor are output in
 * tsv format in this order (time[ms], pressure[Pa], temperature [C], humidity [RH%]).
 */

#include "BME280.h"

/* A BME280 object with I2C address 0x77 (SDO to Vdd) */
/* on Teensy I2C bus 0 */
BME280 bme280Sensor(Wire,0x77);
elapsedMillis curTime;
elapsedMicros microTime;
unsigned int time1, time2;
char myString[100];
int pressureRead, tempRead, humidRead;
unsigned int msTime;

void setup() {
  // serial to display data
  Serial.begin(115200);
  while(!Serial){}

  // begin communication with BME280 and set to default sampling, iirc, and standby settings
  //calculate time it took to initialize and print to terminal
  time1 = curTime;
  if (bme280Sensor.begin() < 0) {
    Serial.println("Error communicating with sensor, check wiring and I2C address");
    while(1){}
  }
  time2 = curTime;
  sprintf(myString, "Initialization Time: %d ms", (time2 - time1));
  Serial.println(myString);

  //calculate time it took to set sleep mode and print to terminal
  time1 = curTime;
  bme280Sensor.setSleepMode();
  time2 = curTime;
  sprintf(myString, "Setting sleep mode time: %d ms", (time2 - time1));
  Serial.println(myString);

 //calculate time it took to set normal mode and print to terminal
  time1 = curTime;
  bme280Sensor.setNormalMode();
  time2 = curTime;
  sprintf(myString, "Setting normal mode time: %d ms", (time2 - time1));
  Serial.println(myString);

//calculate time it took to read data registers, calibrate, and print to terminal
  time1 = microTime;
  bme280Sensor.readSensor();
  time2 = microTime;
  sprintf(myString, "Reading data register time: %d us", (time2 - time1));
  Serial.println(myString);

  pressureRead = bme280Sensor.getPressure_Pa();
  tempRead = bme280Sensor.getTemperature_C();
  humidRead = bme280Sensor.getHumidity_RH();

  sprintf(myString, "Pressure: %d [Pa]   Temp: %d [C]   RH: %d[%%RH]", pressureRead, tempRead, humidRead);
  Serial.println(myString);

  
  while(1); //comment this line for continuous measurements to be printed to the terminal in tsv format
}

void loop() {
  
  // read the sensor
  bme280Sensor.readSensor();

  pressureRead = bme280Sensor.getPressure_Pa();
  tempRead = bme280Sensor.getPressure_Pa();
  humidRead = bme280Sensor.getPressure_Pa();
  static unsigned int timeOffset = curTime;
  msTime = curTime - timeOffset;
  sprintf(myString, "%d\t%d\t%d\t%d", msTime, pressureRead, tempRead, humidRead);
  Serial.println(myString);
  delay(500);
}

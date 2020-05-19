//

#include <SPI.h>
#include <RH_RF95.h>
#include <RHHardwareSPI1.h>
#include <MessageIDs.h>

// Singleton instance of the radio driver
RH_RF95 rf95(10, 2, hardware_spi1);

int led = 16;
uint8_t data[50];
uint8_t datalength;
uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
uint8_t len = RH_RF95_MAX_MESSAGE_LEN;

void setup() 
{    
  pinMode(led,OUTPUT);
  SPI1.setMISO(5); // May also be 5
  SPI1.setMOSI(21); // May also be 21
  SPI1.setSCK(20); 
  SPI1.begin();
  rf95.init();
  rf95.setFrequency(915);

  //send configure date time 
  data[0] = CONFIGURE_DATE_TIME;
  data[1] = 1;
  data[2] = 2;
  data[3] = 3;
  data[4] = 4;
  data[5] = 5;
  datalength = 6;
  rf95.send(data, datalength);
  rf95.waitPacketSent();
  while (!rf95.available());
  len = RH_RF95_MAX_MESSAGE_LEN;
  rf95.recv(buf, &len);


  //send measurement Request
  data[0] = MEASUREMENT_REQUEST;
  data[1] = 1;
  datalength = 2;
  rf95.send(data, datalength);
  rf95.waitPacketSent();
  while (!rf95.available());
  len = RH_RF95_MAX_MESSAGE_LEN;
  rf95.recv(buf, &len);

  //send configure total measurements
  data[0] = CONFIGURE_TOTAL_MEASUREMENTS;
  data[1] = 8;
  datalength = 2;
  rf95.send(data, datalength);
  rf95.waitPacketSent();
  while (!rf95.available());
  len = RH_RF95_MAX_MESSAGE_LEN;
  rf95.recv(buf, &len);

  //send configure cycle period
  data[0] = CONFIGURE_CYCLE_PERIOD;
  data[1] = 0xff;
  data[2] = 0xff;
  datalength = 3;
  rf95.send(data, datalength);
  rf95.waitPacketSent();
  while (!rf95.available());
  len = RH_RF95_MAX_MESSAGE_LEN;
  rf95.recv(buf, &len);

  //send power off request
  data[0] = POWER_OFF_REQUEST;
  data[1] = 0xAA;
  data[2] = 0xAA;
  datalength = 3;
  rf95.send(data, datalength);
  rf95.waitPacketSent();
  while (!rf95.available());
  len = RH_RF95_MAX_MESSAGE_LEN;
  rf95.recv(buf, &len);

  //send update status request
  data[0] = UPDATE_STATUS_REQUEST;
  datalength = 1;
  rf95.send(data, datalength);
  rf95.waitPacketSent();
  while (!rf95.available());
  len = RH_RF95_MAX_MESSAGE_LEN;
  rf95.recv(buf, &len);
  
  //send data transmit request
  data[0] = DATA_TRANSMISSION_REQUEST;
  data[1] = 1;
  datalength = 2;
  rf95.send(data, datalength);
  rf95.waitPacketSent();
  while (!rf95.available());
  len = RH_RF95_MAX_MESSAGE_LEN;
  rf95.recv(buf, &len);

  //send data transmit request
  data[0] = DATA_TRANSMISSION_NACK;
  data[1] = 1;
  datalength = 2;
  rf95.send(data, datalength);
  rf95.waitPacketSent();
  while (!rf95.available());
  len = RH_RF95_MAX_MESSAGE_LEN;
  rf95.recv(buf, &len);

  //send data transmit ACK
  data[0] = DATA_TRANSMISSION_ACK;
  data[1] = 1;
  datalength = 2;
  rf95.send(data, datalength);
  rf95.waitPacketSent();
  while (!rf95.available());
  len = RH_RF95_MAX_MESSAGE_LEN;
  rf95.recv(buf, &len);
  
  digitalWrite(led,HIGH);
}

void loop()
{
  if (rf95.available())
  {

  }
}

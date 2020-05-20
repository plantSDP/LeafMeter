
#include <SPI.h>
#include <RH_RF95.h>
#include <RHHardwareSPI1.h>
#include <MessageIDs.h>

RH_RF95 rf95(10, 2, hardware_spi1);

int counter = 0;

void setup() 
{
  SPI1.setMISO(5); // May also be 5
  SPI1.setMOSI(21); // May also be 21
  SPI1.setSCK(20); 
  SPI1.begin();
  rf95.init();
  rf95.setFrequency(915);
}

void loop()
{
  if (rf95.available())
  {
    // Should be a message for us now   
    uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);
    
    if (rf95.recv(buf, &len)){
      uint8_t data[30];
      data[0] = DATA_TRANSMISSION_INITIAL;
      uint16_t timeleft;
      timeleft = 255;
      uint16_t timeTillCycle = 7;
      data[1] = timeleft >> 8;
      data[2] = timeleft & 0xff;
      data[3] = 5;
      data[4] = 1;
      data[5] = 23;
      data[6] = 1;
      data[7] = 1;
      data[8] = 7;
      data[9] = 59;
      data[10] = 1;
      data[11] = 1;
      data[12] = 1;
      data[13] = 2;
      data[14] = 3;
      data[15] = 4;
      data[16] = 4;
      data[17] = 1;
      data[18] = 1;
      data[19] = 1;
      data[20] = 1;
      data[21] = 2;
      data[22] = 2;
      data[23] = 2;
      data[24] = 2;
      if(counter >= 1){
          rf95.send(data, 25);
          rf95.waitPacketSent();
      }
      counter++;
    }
  }
}

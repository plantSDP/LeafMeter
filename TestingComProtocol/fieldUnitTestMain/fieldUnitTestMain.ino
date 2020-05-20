// rf95_server.pde
// -*- mode: C++ -*-
// Example sketch showing how to create a simple messageing server
// with the RH_RF95 class. RH_RF95 class does not provide for addressing or
// reliability, so you should only use RH_RF95  if you do not need the higher
// level messaging abilities.
// It is designed to work with the other example rf95_client
// Tested with Anarduino MiniWirelessLoRa, Rocket Scream Mini Ultra Pro with
// the RFM95W, Adafruit Feather M0 with RFM95

#include <SPI.h>
#include <RH_RF95.h>
#include "MessageIDs.h"

// Singleton instance of the radio driver
RH_RF95 rf95;
//RH_RF95 rf95(5, 2); // Rocket Scream Mini Ultra Pro with the RFM95W
//RH_RF95 rf95(8, 3); // Adafruit Feather M0 with RFM95 

// Need this on Arduino Zero with SerialUSB port (eg RocketScream Mini Ultra Pro)
//#define Serial SerialUSB

int led = 9;
int counter = 0;

void setup() 
{
  // Rocket Scream Mini Ultra Pro with the RFM95W only:
  // Ensure serial flash is not interfering with radio communication on SPI bus
//  pinMode(4, OUTPUT);
//  digitalWrite(4, HIGH);

  pinMode(led, OUTPUT);     
  //Serial.begin(9600);
  //while (!Serial) ; // Wait for serial port to be available
  //if (!)//{
    //Serial.println("init failed");  
 // } else{
    //Serial.println("init success"); 
  //}
  rf95.init();
  rf95.setFrequency(915);

  // Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on

  // The default transmitter power is 13dBm, using PA_BOOST.
  // If you are using RFM95/96/97/98 modules which uses the PA_BOOST transmitter pin, then 
  // you can set transmitter powers from 5 to 23 dBm:
//  driver.setTxPower(23, false);
  // If you are using Modtronix inAir4 or inAir9,or any other module which uses the
  // transmitter RFO pins and not the PA_BOOST pins
  // then you can configure the power transmitter power for -1 to 14 dBm and with useRFO true. 
  // Failure to do that will result in extremely low transmit powers.
//  driver.setTxPower(14, true);
}

void loop()
{
  if (rf95.available())
  {
    // Should be a message for us now   
    uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);
    if (rf95.recv(buf, &len))
    {
      digitalWrite(led, HIGH);
//      RH_RF95::printBuffer("request: ", buf, len);
      //Serial.print("got request: ");
      //Serial.println((char*)buf);
//      Serial.print("RSSI: ");
//      Serial.println(rf95.lastRssi(), DEC);
      
      // Send a reply
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
      //Serial.println("Sent a reply");
       digitalWrite(led, LOW);
    }
    else
    {
      //Serial.println("recv failed");
    }
  }
}

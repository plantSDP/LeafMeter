#ifndef SdCard_H
#define SdCard_H


#include "Arduino.h"


/*
 * Initializes SD card for use
 * return 0 on failure to initialize. 1 on Success
 */
 uint8_t SDcard_INIT(void);
 
 
 /*
 * creates a directory in on the SD card
 */
uint8_t SDcard_MakeDir(char dirName[]);
 
 
 /*
 * Opens file or creates it if it does not exist
 */
uint8_t SDcard_OpenFile(char filename[]);
 
 
 /*
 * Writes the string to file.
 * Argument must be a correctly formatted, null terminated string
 */
uint8_t SDcard_WriteString(char addedString[]);



#endif
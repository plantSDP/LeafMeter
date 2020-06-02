#include <SD.h>
#include <SPI.h>

#define OPEN 1
#define CLOSED 0

File myFile;


const int chipSelect = 4;//wont work if this is changed. 
char myFileNameAndPath[35];//used to store the string for the file name and path
static char folderName[16];//name of the directory to be made / used 
static uint8_t fileStatus = CLOSED;
/*
* Initializes SD card for use
* returns 0 on failure, 1 on success
*/
uint8_t SDcard_INIT(char filename[]){
	if (!SD.begin(chipSelect)) {
		return 0;
	}else {
		return 1;
	}

}



 /*
 * creates a directory on the SD card
 * if the folder already exists, return is 0.
 * if the folder was created, returns 1
 */
uint8_t SDcard_MakeDir(char dirName[]){
	if (SD.exists(dirName)){
		return 0;
	}
	strcpy(folderName, dirName);
	SD.mkdir(folderName);
	return 1;
}




/*
* creates a new file in the current directory
* if file is open or already exists, returns 0. Returns 1 on success
*/
uint8_t SDcard_OpenFile(char filename[]){
	if (fileStatus == OPEN){
		return 0;
	}
	char tempFileName;
	sprintf(tempFileName, "%s/%s", folderName, filename);
	if (SD.exists(tempFileName)){
		return 0;
	}
	strcpy(myFileNameAndPath, tempFileName);
	myFile = SD.open(myFileNameAndPath, FILE_WRITE);
	fileStatus = OPEN;
	return 1;
}



 /*
 * Writes the string to file.
 * Argument must be a correctly formatted, null terminated string
 */
uint8_t  SDcard_WriteString(char addedString[]){
	if(myFile){
		myFile.println(addedString);
		return 1;
	}
	return 0;

 }
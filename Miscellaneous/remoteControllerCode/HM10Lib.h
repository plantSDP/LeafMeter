#ifndef HM_10_LIB 
#define HM_10_LIB

/*
Initializes the UART for communication with the module
*/
void BLE_Init(void);


/*
wrapper function to send a series of bytes of lenght n from an array to the bluetooth module. 
*/
void BLE_send(uint8_t* msg, uint8_t len);

#endif
//This code will print the voltage on pin 15 when pin 16 in high. 
//Pin 17 will trigger the onboard LED

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#define BUFFERLENGTH 64
#define BUFFERSHIFT 6

LiquidCrystal_I2C lcd(0x27, 16, 2); // set the LCD address to 0x27 for a 16 chars and 2 line display

uint8_t pot = A1;
uint8_t button0 = 16;
uint8_t button1 = 17;
uint8_t led = 13;
unsigned int potValue[BUFFERLENGTH];
unsigned int potSum;
uint8_t potValPtr = 0;
char myString[100];
unsigned int counter = 0;
elapsedMillis curTime;
unsigned int displayTime = 0;
unsigned int buttonTime = 0;
unsigned int potTime = 0;


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  analogReadResolution(10);
  pinMode(led, OUTPUT);
  pinMode(button0, INPUT);
  pinMode(button1, INPUT);

  lcd.init();  //initialize the lcd
  lcd.backlight();  //open the backlight
}

void loop() {

  if ((curTime - buttonTime) > 100){
    buttonTime = curTime;

      if (digitalRead(button0)){
        counter = counter + 1;
      }

      if (digitalRead(button1)){
        counter = counter + 10;
        digitalWrite(led, HIGH);
      } else {
         digitalWrite(led, LOW);
      }
  }




  if ((curTime - potTime) > 20){
    potTime = curTime;
    potSum = potSum - potValue[potValPtr];
    potValue[potValPtr] = analogRead(pot);
    potSum = potSum + potValue[potValPtr];
    potValPtr = (potValPtr + 1) % BUFFERLENGTH;
  }

  


  if ((curTime - displayTime) > 100){
    displayTime = curTime;

      sprintf(myString, "Counter = %d", counter);
      lcd.setCursor(0, 0); // set the cursor to column 0, line 0
      lcd.print(myString);  // Print a message to the LCD

      sprintf(myString, "Pot = %4d", (potSum >> BUFFERSHIFT)  & 0x03FB);
      lcd.setCursor(0, 1); // set the cursor to column 0, line 0
      lcd.print(myString);
  }
  
}

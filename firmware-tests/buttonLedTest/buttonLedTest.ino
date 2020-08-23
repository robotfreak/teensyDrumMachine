/**
 *  @file
 *  @Author Septillion (https://github.com/sseptillion)
 *  @date 2016-05-20
 *  @brief Example how to use buttonRead8()
 *  
 *  buttonRead8() works the same as buttonRead() but works on all 
 *  pins simultaneous. You can give a mask as parameter to decide which 
 *  pins are affected (aka, made high for a short periode) during 
 *  a buttonRead8(). 
 *  
 *  The mask can also be set with setButtonMask(mask). Calling buttonRead8() 
 *  after that will be the same as calling buttonRead8(mask).
 *  
 *  ButtonRead8() can also be used for pins that are input only.
 *  
 *  In this example pin 0 of the PCF8574 (address 0x20) is used to blink a 
 *  LED but at the same time it can be used to toggle the onboard LED of 
 *  the Arduino.
 *  
 *  Pin 1 is also toggle but isn't affected by the buttonRead8();
 *  
 *  NOTE: The button will affect the LED on the same pin. The led will light up 
 *  no matter the set output state when you press the button.
 */

#include <PCF8574.h>
#include <Wire.h>
//#include <WS2812Serial.h>
#include <Adafruit_NeoPixel.h>

/* Neopixel Defines */
const int numled = 20;
const int pin = 1;

//byte drawingMemory[numled*3];         //  3 bytes per LED
//DMAMEM byte displayMemory[numled*12]; // 12 bytes per LED

//WS2812Serial leds(numled, displayMemory, drawingMemory, pin, WS2812_GRB);
Adafruit_NeoPixel leds(numled, pin, NEO_GRB);

#define RED    0xFF0000
#define GREEN  0x00FF00
#define BLUE   0x0000FF
#define YELLOW 0xFFFF00
#define PINK   0xFF1088
#define ORANGE 0xE05800
#define WHITE  0xFFFFFF

/* Port Expander Defines */

PCF8574 pcf1(0x20);
PCF8574 pcf2(0x21);
PCF8574 pcf3(0x22);

const byte onboardLed = 13;

unsigned int blinkMillis;
unsigned int buttonMillis;

void setup() {
  Serial.begin(115200);
  pcf1.begin();
  pcf2.begin();
  pcf3.begin();
  leds.begin();
  
  pinMode(onboardLed, OUTPUT);
  
}

void loop() {
  static bool state;
  unsigned int currentMillis = millis();
  int color = WHITE;
  
  //Limit button read to 20 times a second
  //Fast enough for most buttons 
  //but this way you don't have a dimmer output because it's blanked during button read
  //a read takes 460us t 16Mhz Arduino and normal I2C speed.
  if(currentMillis - buttonMillis >= 50){
    buttonMillis = currentMillis;
    
    byte input1 = pcf1.read8();
    byte input2 = pcf2.read8();
    byte input3 = pcf3.read8();

    leds.clear();
    for(int i=0; i<8; i++) {
      if ((input1 & (1 << i)) == 0)
        leds.setPixelColor(i, leds.Color(255, 255, 255));
    }
    for(int i=0; i<8; i++) {
      if ((input2 & (1 << i)) == 0)
        leds.setPixelColor(i+8, leds.Color(255, 255, 255));
    }
    for(int i=0; i<8; i++) {
      if ((input3 & (1 << i)) == 0)
        leds.setPixelColor(i+16, leds.Color(255, 255, 255));
    }
    leds.show();

    Serial.print(input1, BIN);
    Serial.print(", ");
    Serial.print(input2, BIN);
    Serial.print(", ");
    Serial.println(input3, BIN);
  }
}

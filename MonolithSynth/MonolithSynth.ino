
#include <ILI9341_t3.h>
#include <font_Arial.h> // from ILI9341_t3

#include <PCF8574.h>

#include <WS2812Serial.h>
//#include <Adafruit_NeoPixel.h>

/* Neopixel Defines */
const int numled = 20;
const int pin = 1;

#define RED    0xFF0000
#define GREEN  0x00FF00
#define BLUE   0x0000FF
#define YELLOW 0xFFFF00
#define PINK   0xFF1088
#define ORANGE 0xE05800
#define WHITE  0xFFFFFF
#define BLACK  0x000000

byte drawingMemory[numled*3];         //  3 bytes per LED
DMAMEM byte displayMemory[numled*12]; // 12 bytes per LED

WS2812Serial leds(numled, displayMemory, drawingMemory, pin, WS2812_GRB);
//Adafruit_NeoPixel leds(numled, pin, NEO_GRB);


#include <Bounce.h>
#include "AudioSampleKick1.h"
#include "AudioSampleHat1.h"
#include "AudioSampleHat2.h"
#include "AudioSampleHat3.h"
#include "AudioSampleHat4.h"
#include "AudioSampleHat5.h"
#include "AudioSamplePerc1.h"
#include "AudioSamplePerc2.h"
#include "AudioSamplePerc3.h"
#include "AudioSamplePerc4.h"
#include "AudioSamplePerc5.h"
#include "AudioSampleSnare1.h"
#include "AudioSampleSnare2.h"
#include "AudioSampleSnare3.h"
#include "AudioSampleSnare4.h"
#include "AudioSampleSnare5.h"
#include "AudioSampleClap1.h"
#include "AudioSampleClap2.h"
#include "AudioSampleClap3.h"

const unsigned int *kickList[1] = {AudioSampleKick1};
const unsigned int *snareList[5] = {AudioSampleSnare1, AudioSampleSnare2, AudioSampleSnare3, AudioSampleSnare4, AudioSampleSnare5};
const unsigned int *clapList[3] = {AudioSampleClap1, AudioSampleClap2, AudioSampleClap3};
const unsigned int *hatList[5] = {AudioSampleHat1, AudioSampleHat2, AudioSampleHat3, AudioSampleHat4, AudioSampleHat5};
const unsigned int *percList[5] = {AudioSamplePerc1, AudioSamplePerc2, AudioSamplePerc3, AudioSamplePerc4, AudioSamplePerc5};

#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

// GUItool: begin automatically generated code
AudioPlayMemory          sound1;         //xy=3710,1576
AudioPlayMemory          sound0;         //xy=3711,1526
AudioPlayMemory          sound2;         //xy=3714,1622
AudioPlayMemory          sound3;         //xy=3717,1672
AudioPlayMemory          sound4;         //xy=3886,1666
AudioMixer4              mix1;           //xy=3892,1597
AudioMixer4              mix2;           //xy=4039,1615
AudioSynthWaveformSine   sine1;          //xy=4043,1692
AudioMixer4              mix3;           //xy=4230,1604
AudioOutputI2S           i2s2;           //xy=4378,1627
AudioAnalyzeFFT256       fft256_1;       //xy=4402,1772
AudioAnalyzePeak         peak1;          //xy=4403,1706
AudioConnection          patchCord1(sound1, 0, mix1, 1);
AudioConnection          patchCord2(sound0, 0, mix1, 0);
AudioConnection          patchCord3(sound2, 0, mix1, 2);
AudioConnection          patchCord4(sound3, 0, mix1, 3);
AudioConnection          patchCord5(sound4, 0, mix2, 1);
AudioConnection          patchCord6(mix1, 0, mix2, 0);
AudioConnection          patchCord7(mix2, 0, mix3, 1);
AudioConnection          patchCord8(sine1, 0, mix3, 3);
AudioConnection          patchCord9(mix3, 0, i2s2, 0);
AudioConnection          patchCord10(mix3, 0, i2s2, 1);
AudioConnection          patchCord11(mix3, peak1);
AudioConnection          patchCord12(mix3, fft256_1);
AudioControlSGTL5000     audioShield;    //xy=4426,1517
// GUItool: end automatically generated code

// SPI TFT definitions
#define TFT_DC       5
#define TFT_CS       4
#define TFT_RST      2  // 255 = unused, connect to 3.3V
ILI9341_t3 tft = ILI9341_t3(TFT_CS, TFT_DC, TFT_RST);

/* Port Expander Defines */

PCF8574 pcf1(0x20);
PCF8574 pcf2(0x21);
PCF8574 pcf3(0x22);

// Global Clock
int rightTiming = 4;
int leftTiming = 1;
elapsedMillis sinceTempo;
elapsedMillis sinceThreshold;
elapsedMillis sinceTouch;
elapsedMillis sinceShift;
elapsedMillis sinceVolume;
elapsedMillis sinceRightTrigger;
elapsedMillis sinceTouchRead;
int clockDuration;
int timingValues[3] = {4, 8, 16};
int drumDivider = 8;
boolean modeShift = false;

//Ben variables
float tempo = 80;
int stepCount;
byte row[5];
boolean selectedSample[5];
int rowOn[5];
int lastOn;
int totalOn;
int offThreshold = 10;
byte drumTable[5] = {0b01001111, 0b10010000, 0b11100110, 0b11010101, 0b11111111};
boolean GPIOOnline = 1;
int buttonMapping[20] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19};
boolean currentButtons[64];
boolean lastButtons[64];
int sequence=0;

// Two useful functions
float mapfloat(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

float mtof(int note) {     // Outputs a float freq based on incoming MIDI note. How to use: mtof(noteValue)
  return (440.0f * exp2f((float)(note - 69) * 0.0833333f));
}

void setup() {
  Serial.begin(38400);

  analogReadResolution(10);
  // Global audio stuff
  AudioMemory(50);
  audioShield.enable();
  audioShield.volume(0);

  // Test Oscillator
  sine1.amplitude(0.5);
  sine1.frequency(440);

  // TFT initialisation
  tft.begin();
  tft.fillScreen(ILI9341_BLACK);
  tft.setTextColor(ILI9341_YELLOW);
  tft.setFont(Arial_24);
  tft.setRotation(3);
  //tft.setTextSize(3);
  tft.setCursor(40, 8);
  tft.println("FFT Meter");


  // Wire
  pcf1.begin();
  pcf2.begin();
  pcf3.begin();

  // Neopixel Setup
  leds.begin();

  // Drums mixer
  mix1.gain(0, 2.0); // Drum ch. 1
  mix1.gain(1, 0.7); // Drum ch. 2
  mix1.gain(2, 0.6); // Drum ch. 3
  mix1.gain(3, 1.0); // Drum ch. 4
  mix2.gain(0, 0.5); // Drum mixer 1 daisy chain ==> drum mixer 2
  mix2.gain(1, 0.8); // Drum ch. 5

  // Master Mixer
  mix3.gain(0, 0.6); // Ross
  mix3.gain(1, 0.7); // Ben
  mix3.gain(2, 0.45); // Darcy
  mix3.gain(3, 0); // Test Oscillator  //connected to DRC test scales

}



void led(unsigned int lednum, unsigned int value)
{
  if (GPIOOnline == 0) return;
  switch(value) {
    case WHITE: 
      leds.setPixelColor(lednum, leds.Color(255,255,255));
    break;
    case RED: 
      leds.setPixelColor(lednum, leds.Color(0,255,0));
    break;
    case BLUE: 
      leds.setPixelColor(lednum, leds.Color(0,0,255));
    break;
    default:
      leds.setPixelColor(lednum, leds.Color(0,0,0));
    break;
  }
  leds.show();

}


uint16_t button_state[1];
uint16_t button_high2low[1];
uint16_t button_state2[1];
uint16_t button_high2low2[1];
uint16_t button_state3[1];
uint16_t button_high2low3[1];

void buttons_update(void)
{
  byte state[3];
  state[0] = pcf1.read8();
  state[1] = pcf2.read8();
  state[2] = pcf3.read8();

  for (int e = 0; e < 3; e++)
  {
    for (int i = 0; i < 8; i++)
    {
      lastButtons[i + 8 * e] = currentButtons[i + 8 * e];
      currentButtons[i + 8 * e] = bitRead(state[e], i);
    }
  }
  if (currentButtons[16] == 0)
  {
    if (modeShift == true) {
      modeShift = false;
      led(16, BLACK);
    }
    else {
      modeShift = true;  
      led(16, RED);
    }
  }
}

void loop() {

  GPIOOnline = 1;
  buttons_update();
  do_center_panel();
  peakMeter();
  if (sinceVolume > 25)
  {
    float volSetting;
    //volSetting = float(analogRead(A1)) / 1023.*0.4 + 0.6; // loud: for public performance
    volSetting=float(analogRead(A1))/1023.*0.4+0.2; // quiet: for home testing
    audioShield.volume(volSetting);
    sinceVolume = 0;
  }
}

elapsedMillis msecs;

void peakMeter() {
  //if (msecs > 15) {
    if (peak1.available() ) {
      msecs = 0;
      float leftNumber = peak1.read();
      //Serial.print(leftNumber);
      //Serial.println();

      // draw the verticle bars
      int height = leftNumber * 240;
      tft.fillRect(60, 280 - height, 40, height, ILI9341_GREEN);
      tft.fillRect(60, 280 - 240, 40, 240 - height, ILI9341_BLACK);
      // a smarter approach would redraw only the changed portion...

      // draw numbers underneath each bar
      tft.setFont(Arial_14);
      tft.fillRect(60, 284, 40, 16, ILI9341_BLACK);
      tft.setCursor(60, 284);
      tft.print(leftNumber);
    }
  //}
}
 
void do_center_panel(void)  //Bens Sequencer
{
  leftTiming = timingValues[map(analogRead(2), 0, 1023, 2, 0)] * 2; // Division amounts
  for (int i = 0; i < 16; i++)
  {
    if (currentButtons[buttonMapping[i]] < lastButtons[buttonMapping[i]])
    {
      if (modeShift == true) 
        bitWrite(row[(i/8)+2], i % 8, !bitRead(row[(i/8)+2], i % 8));
      else 
        bitWrite(row[i/8], i % 8, !bitRead(row[i/8], i % 8));
      led(buttonMapping[i], WHITE);
      sinceTouch = 0;
    }
  }
  lastOn = totalOn;
  totalOn = 0;
  for (int i = 0; i < 5; i++)
    rowOn[i] = 0;
  for (int r = 0; r < 5; r++)
  {
    for (int c = 0; c < 8; c++)
    {
      if (bitRead(row[r], c))
      {
        rowOn[r]++;
        totalOn++;
      }
    }
  }
  float diff;
  diff = analogRead(A3) - tempo; //was A16, changed to A17 for DRC test
  tempo = tempo + diff / 40;
  int finalTempo = tempo / 10  + 80;
  if (sinceTempo >= (15000 / (finalTempo * 8)))
  {
    if (stepCount / drumDivider % 256 == 0)
    {
      int randomSample;
      randomSample = random(5);
      if (randomSample == 3)
        selectedSample[3] = random(3);
      else
        selectedSample[randomSample] = random(5);
    }
    if (sinceTouch > 30000 && totalOn < offThreshold && stepCount % 64 == 0)
    {
      for (int r = 0; r < 5; r++)
      {
        for (int c = 0; c < 8; c++)
        {
          if (totalOn == offThreshold)
            bitClear(row[r], c);
            led(buttonMapping[r*8+c], BLACK);
        }
      }
      if (totalOn < offThreshold)
      {
        for (int w = 0; w < offThreshold; w++)
        {
          int mapStep;
          int mapRow;
          mapStep = random(8);
          mapRow = random(5);
          //	bitWrite(row[mapRow],mapStep,bitRead(drumTable[mapRow],mapStep));
        }
      }
    }
    if (totalOn >= offThreshold)
    {
      if (lastOn != totalOn)
        sinceThreshold = 0;
      if (sinceThreshold > 400)
      {
        int changeRow = 0;
        int selectedRow;
        int randomStep;
        changeRow = max(rowOn[0], rowOn[1]);
        changeRow = max(changeRow, rowOn[2]);
        changeRow = max(changeRow, rowOn[3]);
        changeRow = max(changeRow, rowOn[4]);
        for (int s = 0; s < 5; s++)
        {
          if (changeRow == rowOn[s])
            selectedRow = s;
        }
        randomStep = random(8);
        if (sinceTouch > 4000) {
          bitClear(row[selectedRow], randomStep);
          led(buttonMapping[selectedRow*8+randomStep], BLACK);
        }
        sinceThreshold = 0;
      }
    }
    /*
    	#include "AudioSampleDt_kick.h"
    	#include "AudioSampleDt_openhat.h"
    	#include "AudioSampleDt_snare.h"
    	#include "AudioSampleDt_tamborine.h"
    	#include "AudioSampleDt_tom01.h"
    */
    if (stepCount % drumDivider == 0)
    {
      leds.clear();
      led((sequence), BLUE);
      sequence++;
      if (sequence >= drumDivider) sequence = 0;
      if (bitRead(row[0], stepCount / drumDivider % 8))
      {
        sound4.play(percList[selectedSample[4]]);
        Serial.write('4');
      }
      if (bitRead(row[1], stepCount / drumDivider % 8))
      {
        sound2.play(hatList[selectedSample[2]]);
        Serial.write('2');
      }
      if (bitRead(row[2], stepCount / drumDivider % 8))
      {
        sound1.play(snareList[selectedSample[1]]);
        Serial.write('1');
      }
      if (bitRead(row[3], stepCount / drumDivider % 8))
      {
        sound3.play(clapList[selectedSample[3] % 3]);
        Serial.write('3');
      }
      if (bitRead(row[4], stepCount / drumDivider % 8))
      {
        sound0.play(kickList[0]);
        Serial.write('0');
      }
    }
    stepCount++;
    clockDuration = sinceTempo;
    //Serial.println(tempo);
    sinceTempo = sinceTempo - clockDuration;
  }
  // button step sequencer stuff goes here
}

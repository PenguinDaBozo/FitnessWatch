#include <Wire.h>
#include "Hardware.h"
#include "Screen.cpp"

#define NUM_SCREEN = 3


Screen screen(&tft, &RTC);

enum {SCREEN_MAIN = 0, 
      SCREEN_FIT = 1, 
      SCREEN_SW = 2, 
      SCREEN_WEATHER = 3, 
      SCREEN_COMPASS = 4,
      SCREEN_SETTING = 5,
      };

int currentScreen = SCREEN_MAIN;
bool needFullRedraw = true;

void setup() {
  Serial.begin(115200);

  Wire.begin(SDA, SCL);
  SPI.begin(TFT_SCK, -1, TFT_MOSI);
  
  RTC.begin();
  tft.begin();
  
  pinMode(LFT_BUTTON, INPUT);
  pinMode(RGT_BUTTON, INPUT);

  tft.fillScreen(GC9A01A_BLACK);
  tft.setTextWrap(false);

  currentScreen = SCREEN_MAIN;
  needFullRedraw = true;
  
}

void loop() {
  handleButtons();
  
  screen.update();
  screen.draw();
}

void handleButtons() {
  if(digitalRead(LFT_BUTTON) == HIGH) {
    screen.prevScreen();
  }
  if(digitalRead(RGT_BUTTON) == HIGH) {
    screen.nextScreen();
  }
}




// unsigned long thirdScreen() {
//   tft.fillScreen(GC9A01A_GREEN);
//   unsigned long start = micros();
//   tft.setCursor(30,50);
//   tft.setTextColor(GC9A01A_BLACK);  tft.setTextSize(5);
//   tft.println("Stop watch page");
//   return micros() - start;
// }






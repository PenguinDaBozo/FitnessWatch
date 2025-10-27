#include "FitScreen.h"

FitScreen::FitScreen(Adafruit_GCA01A* display, Adafruit_MPU6050* sensor, DS1307* clock)
: tft(display), mpu(sensor), rtc(clock) {
  fitDirtySteps = fitDirtyHR = fitDirtyFlights = false;
  needFullRedraw = true;
  nextPollMs = 0;
  lastSteps = lastHR = lastFlights = -1 
}

FitScreen::update(){
  int steps = 10000;
  int heartRate = 50;
  int flights = 11;

  if(needFullRedraw || steps != lastSteps) {
    lastSteps = steps;
    needFullRedraw = true;
  }
  if(needFullRedraw || heartRate != lastHR) {
    lastHR = heartRate;
    needFullRedraw = true;
  }
  if(needFullRedraw || flights != lastFlights) {
    lastFlights = flights;
    needFullRedraw = true;
  }
}

FitScreen::draw(){
  if(!fitDirtySteps || !fitDirtyHR || !fitDirtyFlights || !needFullRedraw) return;
  tft.setCursor(30,50);
  tft.setTextColor(GC9A01A_BLACK);  tft.setTextSize(5);
  tft.println("Fitness page");
  
}

FitScreen::forceRedraw(){
  needFullRedraw = true; 
}

#include "Screen.h"

Screen::Screen(Adafruit_GC9A01A* display, Adafruit_MPU6050* mpu, DS1307* clock)
: tft(display), mpu(mpu), rtc(clock),
  mainScreen(display, clock),
  fitScreen(display, mpu, clock) {
    currentScreen = SCREEN_MAIN;
}

void Screen::invalidateScreen(){
  needFullRedraw = true;
}
  
void Screen::nextScreen() {
  currentScreen = static_cast<Page>((currentScreen + 1) % NUM_SCREEN);
  invalidateScreen();
}

void Screen::prevScreen() {
  currentScreen = static_cast<Page>((currentScreen - 1 + NUM_SCREEN) % NUM_SCREEN);
  invalidateScreen();
}

void Screen::update() {
  switch(currentScreen) {
    case SCREEN_MAIN: mainScreen.update(); break;
    case SCREEN_FIT: fitScreen.update(); break;
  }
}
void Screen::draw() {
  switch(currentScreen) {
    case SCREEN_MAIN: mainScreen.draw(); break;
    case SCREEN_FIT: fitScreen.draw(); break;
  }
}

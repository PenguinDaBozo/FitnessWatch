#include "Screen.h"

Screen::Screen(Adafruit_GC9A01A* display, Adafruit_MPU6050* mpu, DS1307* clock)
: tft(display), mpu(mpu), rtc(clock),
  mainScreen(display, clock),
  fitScreen(display, mpu) {
    currentScreen = SCREEN_MAIN;
}

Screen::invalidateScreen(){
  needFullRedraw = true;
}
  
Screen::nextScreen() {
  currentScreen = static_cast<Page>((currentScreen + 1) % NUM_SCREEN);
  invalidatePage();
}

Screen::prevScreen() {
  currentScreen = static_cast<Page>((currentScreen - 1 + NUM_SCREEN) % NUM_SCREEN);
  invalidatePage();
}

Screen::update() {
  switch(currentScreen) {
    case SCREEN_MAIN: mainScreen.update();
    case SCREEN_FIT: fitScreen.update();
  }
}
Screen::draw() {
  switch(currentScreen) {
    case SCREEN_MAIN: mainScreen.draw();
    case SCREEN_FIT: fitScreen.draw();
  }
}

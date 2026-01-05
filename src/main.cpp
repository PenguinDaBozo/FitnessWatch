#include "Hardware.h"
#include "sensors.h"
#include "MainScreen.h"
#include "BreathingScreen.h"
#include "Stopwatch.h"

#define NUM_SCREEN 3


LGFX tft;
LGFX_Sprite img(&tft);


const size_t EE_SIZE= 512;
bool invalidateScreen = false;

bool lastUpState;
bool lastDownState;
bool screenChanged = !HIGH;

unsigned long lastTimeUpButtonStateChanged = millis();
unsigned long lastTimeDownButtonStateChanged = millis();
unsigned long debounceDuration = 50;

unsigned long now;

unsigned long lastStepsUpdate = 0;
unsigned long lastTimeUpdate = 0;
unsigned long lastWeatherUpdate = 0;
unsigned long lastHeartRateUpdate = 0;

void setup() {
  Serial.begin(115200);
  Wire.begin(SDA, SCL);
  Wire.setClock(100000);
  #if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
    Wire.begin();
  #elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
    Fastwire::setup(400, true);
  #endif

  pinMode(UP_BUTTON, INPUT_PULLUP);
  pinMode(DOWN_BUTTON, INPUT_PULLUP);
  pinMode(BUTTON_START, INPUT_PULLUP);
  pinMode(BUTTON_STOP, INPUT_PULLUP);
  pinMode(BUZZER, OUTPUT);

  lastUpState = digitalRead(UP_BUTTON);
  lastDownState = digitalRead(DOWN_BUTTON);

  screenInit();
  mpu6050Init();
  wifiInit();
  ds1307Init();
  
  max30102Init();

  Serial.println("Setup complete");
  
}

void handleButtons() {
    if(millis() - lastTimeUpButtonStateChanged >= debounceDuration){
      bool readingUp = digitalRead(UP_BUTTON);
      if(readingUp != lastUpState){
        lastTimeUpButtonStateChanged = millis();
        lastUpState = readingUp;
        if (readingUp == !HIGH) {
          if(screenChanged == !HIGH) {
            state.currentScreen = (state.currentScreen - 1 + NUM_SCREEN) % NUM_SCREEN;

            // main screen
            invalidateScreen = true;

            // box breathing 
            breathingState = MENU_STATE;


            // stop watch
            staticDrawn = false;
            isStart = true;
            count = false;
          } else {
            screenChanged == !HIGH;
          }
        }

      }
    }
  if(millis() - lastTimeDownButtonStateChanged >= debounceDuration){
    bool readingDown = digitalRead(DOWN_BUTTON);
    if(readingDown != lastDownState){
      lastDownState = readingDown;
      if (readingDown == !HIGH) {
        if(screenChanged == !HIGH) {
          state.currentScreen = (state.currentScreen + 1) % NUM_SCREEN;

          // main screen
          invalidateScreen = true;

          // box breathing screen
          breathingState = MENU_STATE;

          // stop watch
          staticDrawn = false;
          isStart = true;
          count = false;
          
        } else {
          screenChanged == !HIGH;
        }
      }
    }
  
    

    }

}


void loop() {
  now = millis();
    handleButtons();

    if(now - lastStepsUpdate >=20 || state.isFirstRun) {
      updateSteps();
      lastStepsUpdate = now;
    }
    if(now - lastTimeUpdate >=1000 || state.isFirstRun) {
      updateTime();
      lastTimeUpdate = now;
    }
    if(now - lastWeatherUpdate >=300000 || state.isFirstRun) {
      updateWeather();
      lastWeatherUpdate = now;
    }
    if(now - lastHeartRateUpdate >=1000 || state.isFirstRun) {
      updateHeartRate();
      lastHeartRateUpdate = now;
    }
    switch(state.currentScreen){
      case 0:
        mainScreenDraw();
        break;
      case 1:
        updateBoxBreathing();
        drawBoxBreathing();
        break;
      case 2:
        updateStopWatch();
        drawStopWatch();
        break;
    }  
}




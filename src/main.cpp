#include "Hardware.h"
#include "MainScreen.h"
#include "BreathingScreen.h"
#include "Stopwatch.h"

#include "Roboto_10pt7b.h"
#include "Baloo2_46pt7b.h"

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



// enum {SCREEN_MAIN = 0, 
//       SCREEN_FIT = 1, 
//       SCREEN_SW = 2, 
//       SCREEN_WEATHER = 3, 
//       SCREEN_COMPASS = 4,
//       SCREEN_SETTING = 5,
//       };

int currentScreen = 0;

void setup() {
  Serial.begin(115200);
  Wire.begin(SDA, SCL);
  Wire.setClock(100000);
  pinMode(UP_BUTTON, INPUT_PULLUP);
  pinMode(DOWN_BUTTON, INPUT_PULLUP);
  pinMode(BUTTON_START, INPUT_PULLUP);
  pinMode(BUTTON_STOP, INPUT_PULLUP);
  pinMode(BUZZER, OUTPUT);

  lastUpState = digitalRead(UP_BUTTON);
  lastDownState = digitalRead(DOWN_BUTTON);

  screenInit();

  Serial.println("Setup complete");
  // currentScreen = SCREEN_MAIN;
  
}

void handleButtons() {
    if(millis() - lastTimeUpButtonStateChanged >= debounceDuration){
      bool readingUp = digitalRead(UP_BUTTON);
      if(readingUp != lastUpState){
        lastTimeUpButtonStateChanged = millis();
        lastUpState = readingUp;
        if (readingUp == !HIGH) {
          if(screenChanged == !HIGH) {
            currentScreen = (currentScreen - 1 + NUM_SCREEN) % NUM_SCREEN;

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
          currentScreen = (currentScreen + 1) % NUM_SCREEN;

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
    handleButtons();
    switch(currentScreen){
      case 0:
        mainScreenUpdate();
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




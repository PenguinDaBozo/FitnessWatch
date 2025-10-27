#pragma once
#include "MainScreen.h"
#include "FitScreen.h"

class Screen {
    public:
        Screen(Adafruit_GC9A01A* display, DS1307* rtc);

        void nextScreen();
        void prevScreen();

        void update();
        void draw();

    private:
        enum Page {SCREEN_MAIN, SCREEN_FIT, NUM_SCREEN};
        Page currentScreen;

        MainScreen mainScreen;
        FitScreen fitScreen;

        Adafruit_GC9A01A* tft;
        DS1307* rtc;

        bool needFullRedraw;

        void invalidateScreen();
};
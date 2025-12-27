#pragma once
#include <I2Cdev.h>
#include <MPU6050.h>
#include <RTClib.h>
#include <EEPROM.h>
#include <Wire.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <cstring>
#include "Adafruit_GFX.h"
#include <LovyanGFX.hpp>

class LGFX : public lgfx::LGFX_Device {
    lgfx::Panel_GC9A01 panel;
    lgfx::Bus_SPI bus;

    public:

    LGFX(){
        auto b = bus.config();
        b.spi_host = SPI2_HOST;
        b.spi_mode = 0;
        b.freq_write = 40000000;
        b.pin_mosi = 6;
        b.pin_miso = -1;
        b.pin_sclk = 7;
        b.pin_dc   = 10;
        bus.config(b);
        panel.setBus(&bus);

        auto p = panel.config();
        p.pin_rst = 5;
        p.pin_cs  = 9;
        p.panel_width = 240;
        p.panel_height = 240;
        p.rgb_order = true;
        p.invert = true;
        panel.config(p);

        setPanel(&panel);
    }
};

// I2C connections
#define SDA 15
#define SCL 16

// buttons
#define UP_BUTTON 12
#define DOWN_BUTTON 21
#define BUTTON_START 39
#define BUTTON_STOP 38

#define BUZZER 14

extern LGFX tft;
extern LGFX_Sprite img;
extern bool invalidateScreen;

struct State {
    int currentScreen;

    int month, date, dayOfWeek;
    int hour, minute;

    int steps;

    float temp;
    char description[8];

    bool farenheitMode;
    bool isFirstRun;
    bool dirtyDate, dirtyTime, daylightSaving;
    bool dirtyWeather;
};

extern State state;



// box breathing state
extern int countdownBreathing;

enum BreathingState{
  MENU_STATE,
  COUNTDOWN_STATE,
  SIMULATION_STATE
};
extern BreathingState breathingState;

// countdown state
extern const GFXfont Baloo2_Bold46pt7b;
extern const GFXfont Roboto_Black10pt7b;
extern bool staticDrawn;
extern bool count;
extern bool isStart;


void screenInit();


#pragma once
#include "Adafruit_GC9A01A.h"
#include <I2C_RTC.h>
#include "Adafruit_MPU6050.h"

// I2C connections
#define SDA 19
#define SCL 22

// TFT display
#define TFT_MOSI 13
#define TFT_SCK 14
#define TFT_DC 25
#define TFT_CS 26
#define TFT_RST 27

// Gyroscope + Accelerometer
#define MPU_XDA 
#define MPU_XCL
#define MPU_ADO
#define MPU_INT 


// Buttons
#define LFT_BUTTON 20
#define RGT_BUTTON 21

extern Adafruit_GC9A01A tft;
extern Adafruit_MPU6050 mpu;
extern DS1307 RTC;

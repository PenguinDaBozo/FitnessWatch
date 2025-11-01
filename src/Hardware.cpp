#include "Hardware.h"
#include <Wire.h>
#include <SPI.h>

Adafruit_GC9A01A tft(TFT_CS, TFT_DC);
Adafruit_MPU6050 mpu;
DS1307 RTC;

Adafruit_Sensor *accel_sensor = nullptr;
Adafruit_Sensor *gyro_sensor = nullptr;
Adafruit_Sensor *temp_sensor = nullptr;

void HardwareInit() {
    Wire.begin(SDA, SCL);
    SPI.begin(TFT_SCK, -1, TFT_MOSI);

    mpu.begin();
    RTC.begin();
    tft.begin();
    tft.setRotation(0);
    tft.fillScreen(GC9A01A_BLACK);
    tft.setTextWrap(false);

    pinMode(LFT_BUTTON, INPUT);
    pinMode(RGT_BUTTON, INPUT);
    
    accel_sensor = mpu.getAccelerometerSensor();
    gyro_sensor = mpu.getGyroSensor();
    temp_sensor = mpu.getTemperatureSensor();
}


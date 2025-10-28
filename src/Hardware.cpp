#include "Hardware.h"

Adafruit_GC9A01A tft(TFT_CS, TFT_DC);
Adafruit_MPU6050 mpu;
Adafruit_Sensor *accel_sensor = mpu.getAccelerometerSensor();
Adafruit_Sensor *gyro_sensor = mpu.getGyroSensor();
Adafruit_Sensor *temp_sensor = mpu.getTemperatureSensor();
DS1307 RTC;



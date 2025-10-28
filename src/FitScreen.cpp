#include "FitScreen.h"

FitScreen::FitScreen(Adafruit_GC9A01A* display, Adafruit_MPU6050* sensor, DS1307* clock)
: tft(display), mpu(sensor), rtc(clock) {
    accel_sensor = mpu.getAccelerometerSensor();
    accel_sensor->printSensorDetails();
    gyro_sensor = mpu.getGyroSensor();
    gyro_sensor->printSensorDetails();

    fitDirtySteps = fitDirtyHR = fitDirtyFlights = false;
    needFullRedraw = true;
    nextPollMs = 0;
    lastSteps = lastHR = lastFlights = -1;
}

FitScreen::update(){
  uint32_t t = nowMs();
  if(t < nextPollMs && !needFullRedraw) return;
  nextPollMs = t + 250;

  if(!mpu.begin()) return;
  
  sensors_event_t accel, gyro;
  accel_sensor->getEvent(&accel);
  gyro_sensor->getEvent(&gyro);
  
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

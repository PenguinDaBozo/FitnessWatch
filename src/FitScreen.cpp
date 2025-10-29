#include "FitScreen.h"

FitScreen::FitScreen(Adafruit_GC9A01A* display, Adafruit_MPU6050* sensor, DS1307* clock)
: tft(display), mpu(sensor), rtc(clock) 
{   mpu->setHighPassFilter(MPU6050_HIGHPASS_0_63HZ);
    mpu->setMotioinDetectionThreshold(2);
    mpu->setMotionDetectionDuration(20);
    mpu->setInterruptPinLatch(true);
    mpu->setInterruptPinPolarity(true);
    mpu->setMotionInterrupt(true);

    EEPROM.begin(sizeof(int));
    EEPROM.get(0, steps);

    //fitDirtySteps = fitDirtyHR = fitDirtyFlights = false;
    needFullRedraw = true;
    accMagnitudePrev = -1;
    nextPollMs = 0;
    steps = 0;
    lastSteps = -1;
}

FitScreen::update(){
  uint32_t t = nowMs();
  if(t < nextPollMs && !needFullRedraw) return;
  nextPollMs = t + 250;

  if(!mpu->getMotionInterruptStatus()||!rtc->isRunning()) return;
  
  sensors_event_t a;
  mpu->getEvent(&a, NULL, NULL);

  int day = rtc->getDay();

  if(day != lastDay){
    lastDay = day;
    newDayReset();
  }

  int16_t accelX = a.acceleration.x;
  int16_t accelY = a.acceleration.y;
  int16_t accelZ = a.acceleration.z;

  float accX = accelX / 16384.0;
  float accY = accelY / 16384.0;
  float accZ = accelZ / 16384.0;

  float magnitude = sqrt(accX * accX +  accY * accY + accZ * accZ);

  if(accMagnitudePrev > magnitude + 0.1 && accMagnitudePrev > 1.5){
    steps++;
    saveSteps();
  } else {
    accMagnitudePrev = magnitude;
  }
  
  if(needFullRedraw || steps != lastSteps) {
    lastSteps = steps;
    needFullRedraw = true;
  }
  // if(needFullRedraw || heartRate != lastHR) {
  //   lastHR = heartRate;
  //   needFullRedraw = true;
  // }
  // if(needFullRedraw || flights != lastFlights) {
  //   lastFlights = flights;
  //   needFullRedraw = true;
  // }
}

FitScreen::draw(){
  if(!fitDirtySteps || !needFullRedraw) return;
  // tft.setCursor(30,50);
  // tft.setTextColor(GC9A01A_BLACK);  tft.setTextSize(5);
  // tft.println("Fitness page");
  
}

FitScreen::forceRedraw(){
  needFullRedraw = true; 
}

FitScreen::saveSteps(){
  EEPROM.put(0, steps);
  EEPROM.commit();
}

FitScreen::newDayReset(){
  for(int i = 0; i < EEPROM.length(); i++){
    EEPROM.write(i, 0);
  }
  EEPROM.commit();
  steps = 0;
}
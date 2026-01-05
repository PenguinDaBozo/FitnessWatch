#include "Hardware.h"

void mpu6050Init();
void max30102Init();
void ds1307Init();
void wifiInit();

void updateSteps();
void updateTime();
void updateWeather();
void updateHeartRate();


void tryNtpSync();
void saveSteps();
void newDayReset();
long dcRemove(long x);
long bandpass(long x);
boolean detectPeak(long x);
byte median(byte num[4]);
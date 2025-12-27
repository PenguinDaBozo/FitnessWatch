#include "Hardware.h"

// sensors
RTC_DS1307 rtc;
MPU6050 mpu(0x69);


// wifi
String url = "https://api.openweathermap.org/data/2.5/weather?";
String APIkey = "ed2bd981cb54db5ad049bd34731c385b";

String lat = "40.724378";
String lon = "-73.904365";

// float temp;
// char description[8] = {0};
// bool farenheitMode = true; 
// bool dirtyWeather = false


// mpu6050
#define OUTPUT_READABLE_ACCELGYRO 

int16_t ax, ay, az;
int16_t gx, gy, gz;

unsigned long nextPollMs = 0;
unsigned long lastStepUpdate = 0;
const unsigned long stepInterval = 5000;
// int steps = 0; 
long accMagnitudePrev = -1;


// rtc
// bool mainDirtyDate = false, mainDirtyTime = false, dayLightSaving = false;
// int lastDayOfTheWeek = -1, lastDay = -1, lastMonth = -1, lastHour = -1, lastMinute = -1;

String month_name = "";
String day = "";

const char* ssid = "MySpectrumWiFic8-2G";
const char* password = "moderntiger313"; // expect 0..6


void saveSteps(){
    EEPROM.put(0, state.steps);
    EEPROM.commit();
    Serial.printf("Steps saved to EEPROM: %d\n", state.steps);

}

void newDayReset(){
    for(int i = 0; i < 10; i++){
      EEPROM.write(i, 0);
    } 
    EEPROM.commit();
    state.steps = 0;
}



// extra
// bool isFirstRun = true;


// initialization
void mpu6050Init() {
  mpu.initialize();
  Serial.println("Testing MPU6050 connection...");
  if(mpu.testConnection() == false){
    Serial.println("MPU6050 connection failed");
    while (true);
  }
  else {
    Serial.println("MPU6050 connection successful");
  }

  EEPROM.begin(sizeof(int));
  EEPROM.get(0, state.steps);
  Serial.printf("Steps loaded from EEPROM: %d\n", state.steps);

  Serial.println("Updating internal sensor offsets...\n");
  mpu.setXAccelOffset(0);
  mpu.setYAccelOffset(0);
  mpu.setZAccelOffset(0);
  mpu.setXGyroOffset(0);
  mpu.setYGyroOffset(0);
  mpu.setZGyroOffset(0);
}

void ds1307Init() {
  rtc.begin();
  if(!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    while (1) delay(10);
  } else {
    Serial.println("RTC found");
  }
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

  
}

void wifiInit(){
      WiFi.begin(ssid, password);

      while(WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
      }

      Serial.println("");
      Serial.print("IP Address: ");
      Serial.print(WiFi.localIP());
}

// update
void updateSteps() {
  DateTime now = rtc.now();
  int day = now.day();

  if(day != state.date && !state.isFirstRun){
      state.date = day;
      newDayReset();
  }

  mpu.getMotion6(&ax, &ay, &az, &gz, &gy, &gz);


  #ifdef OUTPUT_BINARY_ACCELGYRO
      Serial.write((uint8_t)(ax >> 8)); Serial.write((uint8_t)(ax & 0xFF));
      Serial.write((uint8_t)(ay >> 8)); Serial.write((uint8_t)(ay & 0xFF));
      Serial.write((uint8_t)(az >> 8)); Serial.write((uint8_t)(az & 0xFF));
      Serial.write((uint8_t)(gx >> 8)); Serial.write((uint8_t)(gx & 0xFF));
      Serial.write((uint8_t)(gy >> 8)); Serial.write((uint8_t)(gy & 0xFF));
      Serial.write((uint8_t)(gz >> 8)); Serial.write((uint8_t)(gz & 0xFF));
  #endif
      float accX = ax / 16384.0;
      float accY = ay / 16384.0;
      float accZ = az / 16384.0;

      float magnitude = sqrt(accX * accX + accY * accY + accZ * accZ);

      if (accMagnitudePrev > magnitude + 0.1 && accMagnitudePrev > 1.5) {
      state.steps++;
      saveSteps();
      Serial.printf("Step detected! Total steps: %d\n", state.steps);
      }
      accMagnitudePrev = magnitude;
}

void updateTime() {
  DateTime now = rtc.now();
    
    int month = now.month();
    int day = now.day();;
    int year = now.year();

    int dayOfTheWeek = now.dayOfTheWeek();
    int hour = state.daylightSaving? now.hour()+1: now.hour();
    int minute = now.minute();
    int second = now.second();

    Serial.println(month);
    Serial.println(day);
    Serial.print(year);
    Serial.print(dayOfTheWeek);
    Serial.println(hour);
    Serial.println(minute);
    Serial.println(second);

    if(dayOfTheWeek != state.dayOfWeek || day != state.date || month != state.month){
        state.dayOfWeek = dayOfTheWeek; state.date = day; state.month = month;
        state.dirtyDate = true;
    }

    if(hour != state.hour || minute != state.minute){
        state.hour = hour;
        state.minute = minute; 
        state.dirtyTime = true;
    }

    static int prevSecond = -1;
    if (second == 0 && prevSecond != 0 && minute != state.minute)
    {
        state.minute = minute; 
        state.hour = hour;
        state.dirtyTime = true;
    }
    prevSecond = second;
}

void updateWeather(){
    HTTPClient http;

    http.begin(url + "lat=" + lat + "&lon=" + lon + "&units=metric&appid=" + APIkey);

    int httpCode = http.GET();

    if(httpCode > 0){
        if(state.isFirstRun || state.minute%30==0){
        WiFiClient& stream = http.getStream();

        StaticJsonDocument<256> filter;
        filter["weather"][0]["icon"] = true;
        filter["main"]["temp"] = true;

        StaticJsonDocument<512> doc;

        auto err = deserializeJson(doc, stream, DeserializationOption::Filter(filter));
        if (err) {
        Serial.println(err.c_str());
        return;
        }
        const char* icon = doc["weather"][0]["icon"].as<const char*>();
        if (icon == nullptr) {
        Serial.println("ICON IS NULL — JSON did NOT contain weather[0].icon");
        }

        strncpy(state.description, icon, sizeof(state.description) - 1);
        state.description[sizeof(state.description) - 1] = '\0';
        state.temp = doc["main"]["temp"].as<float>(); // Celsius

        state.dirtyWeather = true;
        }   
    
    http.end();
    }
}

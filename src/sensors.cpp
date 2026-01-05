#include "Hardware.h"
#include "heartRate.h"

#define MIN_IR 50000
#define PEAK_THRESHOLD 800


// sensors
RTC_DS1307 rtc;
MPU6050 mpu(0x69);
MAX30105 max30102;


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

const byte RATE_SIZE = 4;
byte rates[RATE_SIZE];
byte rateSpot = 0;
long lastBeat = 0;
bool peak = false; 

float beatsPerMinute;
int beatAvg;

// rtc
// bool mainDirtyDate = false, mainDirtyTime = false, dayLightSaving = false;
// int lastDayOfTheWeek = -1, lastDay = -1, lastMonth = -1, lastHour = -1, lastMinute = -1;

String month_name = "";
String day = "";

const char* ssid = "MySpectrumWiFic8-2G";
const char* password = "moderntiger313"; // expect 0..6

bool timeSynced = false;
unsigned long lastTimeSync = 0;
const unsigned long TIME_SYNC_INTERVAL = 3600000; 

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

void max30102Init(){
  if(!max30102.begin(Wire, I2C_SPEED_FAST)) {
    Serial.println("MAX30102 was not found. Please check wiring/power.");
    while (1);
  }
  max30102.setup(); 
  max30102.setPulseAmplitudeRed(0x0A);
  max30102.setPulseAmplitudeGreen(0x0A);
  Serial.println("MAX30102 initialized");
}

void ds1307Init() {
  if(!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    while (1) delay(10);
  } else {
    Serial.println("RTC found");
  }
  if(!rtc.isrunning()) {
    Serial.println("RTC is NOT running, setting the time!");
    
  }
  
  
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

void tryNtpSync(){
    static bool ntpConfigured = false;
    if(!ntpConfigured){
        configTime(0, 0, "pool.ntp.org", "time.nist.gov");
        ntpConfigured = true;
    }

    struct tm timeinfo;
    if(!getLocalTime(&timeinfo, 1000)){
        Serial.println("Failed to obtain time");
        return;
    }
    rtc.adjust(DateTime(
        timeinfo.tm_year + 1900,
        timeinfo.tm_mon + 1,
        timeinfo.tm_mday,
        timeinfo.tm_hour,
        timeinfo.tm_min,
        timeinfo.tm_sec
    ));
    Serial.println("RTC time synchronized with NTP");
    timeSynced = true;
}

long dcRemove(long x) {
  static long dc = 0;
  dc = dc + ((x - dc) >> 4);
  return x - dc;
}

long bandpass(long x){
  static long lp = 0;
  lp = lp + ((x-lp)>>2);
  return lp;
}

boolean detectPeak(long x){
  static long prev = 0;
  static long prevDiff = 0;

  long diff = x - prev;
  bool peak = (prevDiff > 0) && (diff <= 0) && (x > PEAK_THRESHOLD);
  prevDiff = diff;
  prev = x;
  return peak;
}

byte median(byte num[RATE_SIZE]){
  byte sorted[RATE_SIZE];
  memcpy(sorted, num, sizeof(sorted));
  for (byte i = 0; i < RATE_SIZE - 1; i++) {
    for (byte j = i + 1; j < RATE_SIZE; j++) {
      if (sorted[j] < sorted[i]) {
        byte temp = sorted[i];
        sorted[i] = sorted[j];
        sorted[j] = temp;
      }
    }
  }
  return sorted[RATE_SIZE / 2];
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
      state.dirtySteps = true;
      Serial.printf("Step detected! Total steps: %d\n", state.steps);
      }
      accMagnitudePrev = magnitude;
}

void updateTime() {
  

  DateTime now = rtc.now();
  // if(state.isFirstRun) {
  //   rtc.adjust(DateTime(2025, 12, 31, 2, 51, 43));
  // }

  // if(WiFi.status() == WL_CONNECTED && millis() - lastTimeSync > TIME_SYNC_INTERVAL || state.isFirstRun){
  //   tryNtpSync();
  // }

    if(now.month() != state.month || now.day() != state.date ||now.dayOfTheWeek() != state.dayOfWeek){
      state.month = now.month();
      state.date = now.day();
      state.dayOfWeek = now.dayOfTheWeek();
      state.dirtyDate = true;
    }

    if(now.hour() != state.hour || now.minute() != state.minute){
      // state.hour = state.daylightSaving? local.tm_hour+1: local.tm_hour;
      state.hour = now.hour();
      state.minute = now.minute();
      state.dirtyTime = true;
    }
    // Serial.println("State.month: " + String(state.month));
    // Serial.println("State.date: " + String(state.date));
    // Serial.print("State.dayOfWeek: " + String(state.dayOfWeek));
    // Serial.println("State.hour: " + String(state.hour));
    // Serial.println("State.minute: " + String(state.minute));
    // Serial.println("Now.second(): " + String(now.second()));
}

void updateWeather(){
    HTTPClient http;

    http.begin(url + "lat=" + lat + "&lon=" + lon + "&units=metric&appid=" + APIkey);

    int httpCode = http.GET();
    Serial.println("HTTP code: " + String(httpCode));
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
    Serial.print("Weather updated: ");
    
    http.end();
    }
}

void updateHeartRate(){
  Serial.println("Updating heart rate...");
  // if(!max30102.available()) return;
  long irValue = max30102.getIR();
  Serial.println("IR Value: " + String(irValue));

    // long ac = dcRemove(irValue);
    // long filtered = bandpass(ac);

  if(checkForBeat(irValue)){
      Serial.println("BEAT");
      long delta = millis() - lastBeat;
      lastBeat = millis();

      float beatsPerMinute = 60 / (delta / 1000.0);

      if(beatsPerMinute < 255 && beatsPerMinute > 20){
        rates[rateSpot++] = (byte)beatsPerMinute;
        rateSpot %= RATE_SIZE;

        state.bpm = median(rates);
        state.dirtyHeartRate = true;
        
      }
  }

  Serial.println("IR Value: " + String(irValue));
  Serial.println("Heartrate: " + String(state.bpm));
  
}




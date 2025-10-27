#include "MainScreen.h"

MainScreen::MainScreen(Adafruit_GC9A01A* display, DS1307* clock)
: tft(display), rtc(clock) {
    mainDirtyDate = mainDirtyTime = false;
    needFullRedraw = true;
    nextPollMs = 0;
    lastWeek = lastDate = lastMonth = lastHour = lastMinute = -1;
}

const char* MainScreen::dayName(int week){
  switch(week){
    case 1: day = "Sun";
    case 2: day = "Mon";
    case 3: day = "Tue";
    case 4: day = "Wed";
    case 5: day = "Thu";
    case 6: day = "Fri";
    case 7: day = "Sat";
  }
}

const char* MainScreen::monthName(int month){
  switch(month){
    case 1: month_name = "Jan";
    case 2: month_name = "Feb";
    case 3: month_name = "Mar";
    case 4: month_name = "Apr";
    case 5: month_name = "May";
    case 6: month_name = "June";
    case 7: month_name = "July";
    case 8: month_name = "Aug";
    case 9: month_name = "Sept";
    case 10: month_name = "Oct";
    case 11: month_name = "Nov";
    case 12: month_name = "Dec";
  }
}

void MainScreen::update() {
  uint32_t t = nowMs();
  if(t < nextPollMs && !needFullRedraw) return;
  nextPollMs = t + 250;

  if(!RTC.isRunning()) return;

  int week = RTC->getWeek();
  int date = RTC->getDay();
  int hours = RTC->getHours();
  int minutes = RTC->getMinutes();
  int month = RTC->getMonth();

  if(needFullRedraw || week != lastWeek || date != lastDate || month != lastMonth){
    lastWeek = week; lastDate = date; lastMonth = month;
    mainDirtyDate = true;
  }

  if(needFullRedraw || hours != lastHour || minutes != lastMinute){
    lastHour = hours; lastMinute = minutes;

  }
}
    
void MainScreen::draw() {
  if(!mainDirtyDate && !mainDirtyTime  && !needFullRedraw) return;
  
  int cx = tft->width()/2;
  int cy = tft->height()/2;

  if(needFullRedraw) {
    tft->fillScreen(GC9A01A_BLACK);
  }

  if(mainDirtyDate || needFullRedraw){
    tft->fillRect(0, cy - 60, tft.width(), 24, GC9A01A_BLACK);
    tft->setTextColor(GC9A01A_WHITE);  
    tft->setTextSize(2);
    tft->setCursor(cx - 60, cy - 50);
    tft->print(dayName(lastWeek)); 
    tft->print(" ");
    tft->print(monthName(lastMonth)); 
    tft->print(" ");
    tft->print(lastDate);
    mainDirtyDate = false;
  }

  if(mainDirtyTime || needFullRedraw){
    tft->fillRect(0, cy - 20, tft->width(), 40, GC9A01A_BLACK);
    tft->setTextColor(GC9A01A_WHITE); 
    tft->setTextSize(3);
    tft->setCursor(cx - 40,cy - 30);
    tft->print(lastHour); 
    tft->print(":");
    if(lastMinute < 10) tft->print("0");
    tft->print(lastMinute);
    mainDirtyTime = false;
  }
  needFullRedraw = false;
}

void MainScreen::forceRedraw(){
  needFullRedraw = true;
}


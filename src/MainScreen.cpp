#include "Hardware.h"
#include "MainScreen.h"

MainScreen::MainScreen(Adafruit_GC9A01A* display, DS1307* clock)
: tft(display), rtc(clock) {
    mainDirtyDate = mainDirtyTime = false;
    needFullRedraw = true;
    nextPollMs = 0;
    lastWeek = lastDate = lastMonth = lastHour = lastMinute = -1;
}

void MainScreen::dayName(int week){
  switch(week){
    case 1: day = "Sun"; break;
    case 2: day = "Mon"; break;
    case 3: day = "Tue"; break;
    case 4: day = "Wed"; break;
    case 5: day = "Thu"; break;
    case 6: day = "Fri"; break;
    case 7: day = "Sat"; break;
  }
}

void MainScreen::monthName(int month){
  switch(month){
    case 1: month_name = "Jan"; break;
    case 2: month_name = "Feb"; break;
    case 3: month_name = "Mar"; break;
    case 4: month_name = "Apr"; break;
    case 5: month_name = "May"; break;
    case 6: month_name = "June"; break;
    case 7: month_name = "July"; break;  
    case 8: month_name = "Aug"; break;
    case 9: month_name = "Sept"; break;
    case 10: month_name = "Oct"; break;
    case 11: month_name = "Nov"; break;
    case 12: month_name = "Dec"; break;
  }
}

void MainScreen::update() {
  uint32_t t = nowMs();
  if(t < nextPollMs && !needFullRedraw) return;
  nextPollMs = t + 250;

  if(!RTC.isRunning()) return;

  int week = RTC.getWeek();
  int date = RTC.getDay();
  int hours = RTC.getHours();
  int minutes = RTC.getMinutes();
  int month = RTC.getMonth();

  if(needFullRedraw || week != lastWeek || date != lastDate || month != lastMonth){
    lastWeek = week; lastDate = date; lastMonth = month;
    mainDirtyDate = true;
  }

  if(needFullRedraw || hours != lastHour || minutes != lastMinute){
    lastHour = hours; lastMinute = minutes; 
    mainDirtyTime = true;
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
    tft->fillRect(0, cy - 60, tft->width(), 24, GC9A01A_BLACK);
    tft->setTextColor(GC9A01A_WHITE);  
    tft->setTextSize(2);
    tft->setCursor(cx - 60, cy - 50);
    dayName(lastWeek);
    tft->print(day); 
    tft->print(" ");
    monthName(lastMonth);
    tft->print(month_name); 
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


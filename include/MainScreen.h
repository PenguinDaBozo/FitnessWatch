#include <Hardware.cpp>

class MainScreen {
    public:
        MainScreen(Adafruit_GC9A01A* display, DS1307* clock);

        void update();
        void draw();
        void forceRedraw();

    private:
        Adafruit_GC9A01A* tft;
        DS1307* rtc;

        bool mainDirtyDate, mainDirtyTime, needFullRedraw;
        unsigned long nextPollMs;
        int lastWeek, lastDate, lastMonth, lastHour, lastMinute;
        String month_name;
        String day;

        void dayName(int week);
        void monthName(int month);
        static inline uint32_t nowMs() { return (uint32_t)millis();}
};
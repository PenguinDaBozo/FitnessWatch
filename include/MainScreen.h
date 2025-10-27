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
        String month_name, day;

        static const char* dayName(int week);
        static const char* monthName(int month);
        static inline uint32_t nowMs() { return (uint32_t)millis();}
};
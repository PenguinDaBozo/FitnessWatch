class FitScreen {
    public:
        FitScreen(Adafruit_GC9A01A* display, Adafruit_MPU6050* mpu, DS1307* clock);
        void update();
        void draw();
        void forceRedraw();

    private:
        DS1307* rtc;
        Adafruit_GC9A01A* tft;
        Adafruit_MPU6050* mpu;

        bool fitDirtySteps;
        unsigned nextPollMs;
        int lastSteps, lastDay;
        int steps;
        long accMagnitudePrev;
        bool needFullRedraw;
        

        static inline uint32_t nowMs() { return (uin32_t)millis();}
        void saveSteps();
        void newDayReset();



}
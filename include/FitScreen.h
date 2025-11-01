#include <Hardware.h>

class FitScreen {
    public:
        FitScreen(Adafruit_GC9A01A* display, Adafruit_MPU6050* mpu, DS1307* clock);
        void update();
        void draw();
        void forceRedraw();

    private:
        Adafruit_GC9A01A* tft;
        DS1307* rtc;
        Adafruit_MPU6050* mpu;

        bool fitDirtySteps;
        unsigned nextPollMs;
        int lastSteps, lastDay;
        int steps;
        long accMagnitudePrev;
        bool needFullRedraw;
        
        void saveSteps();
        void newDayReset();
        static inline uint32_t nowMs() { return (uint32_t)millis();}
};

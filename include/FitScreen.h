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

        Adafruit_Sensor *accel_sensor;
        Adafruit_Sensor *gyro_sensor;

        bool fitDirtySteps, fitDirtyHR, fitDirtyFlights;
        unsigned nextPollMs;
        int lastSteps, lastHR, lastFlights;
        bool needFullRedraw;
        

        static inline uint32_t nowMs() { return (uin32_t)millis();}



}
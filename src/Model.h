#include "Arduino.h"

#ifndef CLIONTEST_MODEL_H
#define CLIONTEST_MODEL_H

class Model {
public:
    short setPointTemperature = 20;
    float currentTemperature = 0;
    float currentHumidity = 0;

    struct SensorReading {
        unsigned long when;
        float temperature;
        float humidity;
    };

    String lastTimeText;

    bool isTimeForNewReading(unsigned long now);
    bool isTimeForNtpServerSync(unsigned long now);
    bool didJustChange(unsigned long when, float temperature, float humidity);
    bool didTimeTextChange(int month, int day, int weekday, int hour, int minute);
    bool isCold();

private:
    SensorReading lastReading;
    unsigned long lastSyncedTimeWithNtpServer;
    
    const float tempDiff = 0.05f;
    const float humDiff = 0.5f;
};

#endif //CLIONTEST_MODEL_H

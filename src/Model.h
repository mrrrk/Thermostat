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

    bool isTimeForNewReading(unsigned long now);

    //void newReading(unsigned long when, float temperature, float humidity);

    bool didJustChange(unsigned long when, float temperature, float humidity);

    bool isCold();

private:
    SensorReading lastReading;

    const float tempDiff = 0.05f;
    const float humDiff = 0.5f;
};

#endif //CLIONTEST_MODEL_H

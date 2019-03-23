#include "Arduino.h"

#ifndef CLIONTEST_MODEL_H
#define CLIONTEST_MODEL_H

class Model {
public:
    int16_t setPointTemperature = 20;
    float currentTemperature = 0;
    float currentHumidity = 0;
};

#endif //CLIONTEST_MODEL_H

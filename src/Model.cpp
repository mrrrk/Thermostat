#include "Model.h"

bool Model::isTimeForNewReading(unsigned long now) {
    return now - lastReading.when > 1000;
}

bool Model::didJustChange(unsigned long when, float temperature, float humidity) {
    // Using these diff values gives us a hysteresis effect to stop 
    // the UI and relay state from jumping about too much.
    bool isDifferent = 
        fabsf(lastReading.temperature - temperature) > tempDiff || 
        fabsf(lastReading.humidity - humidity) > humDiff;
    if(isDifferent) {
        this->currentTemperature = temperature;
        this->currentHumidity = humidity;
    }
    lastReading.humidity = humidity;
    lastReading.temperature = temperature;
    lastReading.when = when;
    return isDifferent;
}

bool Model::isCold() {
    return this->setPointTemperature - this->currentTemperature > tempDiff;
}
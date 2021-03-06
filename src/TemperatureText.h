#include <Adafruit_ILI9341.h>
#include "Arduino.h"
#include "Point.h"
#include <Fonts/FreeSansBold24pt7b.h>

#ifndef CLIONTEST_TEMPERATURETEXT_H
#define CLIONTEST_TEMPERATURETEXT_H

class TemperatureText {
private:
    Adafruit_ILI9341* tft;
    short x;
    short y;
    short colour;
    unsigned char decimalPlaces;

    void drawDegreeCircle();

public:
    TemperatureText(Adafruit_ILI9341* tft, short x, short y, short colour, unsigned char decimalPlaces);
    float temperature;
    void render();
};

#endif //CLIONTEST_TEMPERATURETEXT_H

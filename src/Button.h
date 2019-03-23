#include <Adafruit_ILI9341.h>
#include <Adafruit_STMPE610.h>

#include "Arduino.h"
#include "Point.h"


class Button {

private:
    Adafruit_ILI9341* tft;
    int16_t direction; // 1 or -1 = up or down
    int16_t x1;
    int16_t y1;
    int16_t x2;
    int16_t y2;
    TS_Point lastTouchPoint;
    unsigned long lastTouchWhen;

    void drawArrow();
    
public:

    Button(Adafruit_ILI9341* tft, int16_t id, int16_t x1, int16_t y1, int16_t x2, int16_t y2);

    // function pointer as callback when button clicked
    void (*clicked)(int16_t, unsigned long);

    void render();

    bool isInBounds(TS_Point point);

    void processTouch(TS_Point point, unsigned long when);

    void checkTouch(unsigned long when);
};

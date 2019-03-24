#include <Adafruit_ILI9341.h>
#include <Adafruit_STMPE610.h>

#include "Arduino.h"
#include "Point.h"


class Button {

private:
    Adafruit_ILI9341* tft;
    short direction; // 1 or -1 = up or down
    short x1;
    short y1;
    short x2;
    short y2;
    TS_Point lastTouchPoint;
    unsigned long lastTouchWhen;

    void drawArrow();
    
public:

    Button(Adafruit_ILI9341* tft, short id, short x1, short y1, short x2, short y2);

    // function pointer as callback when button clicked
    void (*clicked)(short, unsigned long);

    void render();

    bool isInBounds(TS_Point point);

    void processTouch(TS_Point point, unsigned long when);

    void checkTouch(unsigned long when);
};

#include "Arduino.h"
#include "Button.h"
#include "TemperatureText.h"
#include "Model.h"
#include <Adafruit_ILI9341.h>
#include <Adafruit_STMPE610.h>
#include <Fonts/FreeSans9pt7b.h>

// Color definitions
#define BLACK    0x0000
#define BLUE     0x001F
#define RED      0xF800
#define GREEN    0x07E0
#define CYAN     0x07FF
#define MAGENTA  0xF81F
#define YELLOW   0xFFE0 
#define WHITE    0xFFFF

class View {

private:
    Adafruit_ILI9341* tft;

    void drawFlame(bool shouldShow);

public:

    Model* model;

    Button* upButton;
    Button* downButton;
    TemperatureText* setPointTemperatureText;
    TemperatureText* currentTemperatureText;

    View(Adafruit_ILI9341* tft, Model* model);
    
    void processTouch(TS_Point point, unsigned long when);

    void checkTouch(unsigned long when);

    void refresh();

    void drawText(short x, short y, short colour, String text);
};

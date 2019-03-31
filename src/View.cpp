#include "View.h"

// constuctor
View::View(Adafruit_ILI9341* tft, Model* model) {
    this->tft = tft;
    this->model = model;
  
    this->upButton = new Button(tft, 1, 210, 15, 305, 115);
    this->downButton = new Button(tft, -1, 210, 130, 305, 230);
    this->setPointTemperatureText = new TemperatureText(tft, 10, 40, WHITE, 0);
    this->currentTemperatureText = new TemperatureText(tft, 10, 155, RED, 1);
}

void View::processTouch(TS_Point point, unsigned long when) {
    this->downButton->processTouch(point, when);
    this->upButton->processTouch(point, when);
}

void View::checkTouch(unsigned long when) {
    this->downButton->checkTouch(when);
    this->upButton->checkTouch(when);
}

void View::refresh() {
    this->setPointTemperatureText->temperature = this->model->setPointTemperature;
    this->currentTemperatureText->temperature = this->model->currentTemperature;
    this->setPointTemperatureText->render();
    this->currentTemperatureText->render();
    this->drawFlame(this->model->isCold());
    this->drawText9pt(10,15, WHITE, "Set point");
    this->drawText12pt(10,100, WHITE, this->model->lastTimeText);
    this->drawText9pt(10,130, RED, "Current temperature");
    String humidText = String(this->model->currentHumidity, 0);
    humidText = "Humidity " + humidText + "%";
    this->drawText12pt(10,215, RED, humidText);
}

void View::drawFlame(bool shouldShow) {
    const short x = 140;
    const short y = 32;
    if(!shouldShow) {
        tft->fillRect(x, y, 32, 32, BLACK);
        return;
    }
    const static uint8_t flameYellow[] PROGMEM = {
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00,
            0x00, 0x02, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x03, 0x80, 0x00, 0x00, 0x01, 0xe0, 0x00,
            0x00, 0x01, 0xf0, 0x00, 0x00, 0x00, 0xf8, 0x00, 0x00, 0x00, 0xf8, 0x00, 0x00, 0x00, 0xfc, 0x00,
            0x00, 0x00, 0xfc, 0x00, 0x00, 0x41, 0xf8, 0x00, 0x00, 0x7f, 0xf8, 0x00, 0x00, 0x7f, 0xf0, 0x00,
            0x00, 0x7f, 0x82, 0x00, 0x00, 0x3f, 0x9c, 0x00, 0x00, 0x1f, 0xfc, 0x00, 0x00, 0x0f, 0xf0, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    };
    const static PROGMEM uint8_t flameRed[] PROGMEM = {
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0x00, 0x00, 0x00, 0xc0, 0x00, 0x00, 0x01, 0xe0, 0x00,
            0x00, 0x01, 0xe0, 0x00, 0x00, 0x07, 0xe0, 0x00, 0x00, 0x0f, 0xe0, 0x00, 0x00, 0x1f, 0xc0, 0x00,
            0x00, 0x1f, 0xc0, 0x80, 0x00, 0x1f, 0xc0, 0xc0, 0x00, 0x3f, 0xe0, 0xe0, 0x02, 0x3f, 0xe1, 0xe0,
            0x06, 0x3f, 0xf3, 0xe0, 0x0e, 0x1f, 0xff, 0xf0, 0x0e, 0x1f, 0xff, 0xf0, 0x0e, 0x3f, 0xff, 0xf0,
            0x1f, 0x3f, 0xff, 0xf8, 0x1f, 0xff, 0xff, 0xf8, 0x1f, 0xff, 0xff, 0xf8, 0x1f, 0xff, 0xff, 0xf8,
            0x1f, 0xff, 0xff, 0xf8, 0x1f, 0xff, 0xff, 0xf0, 0x0f, 0xff, 0xff, 0xf0, 0x0f, 0xff, 0xff, 0xf0,
            0x0f, 0xff, 0xff, 0xe0, 0x07, 0xff, 0xff, 0xe0, 0x03, 0xff, 0xff, 0xc0, 0x03, 0xff, 0xff, 0x80,
            0x01, 0xff, 0xff, 0x00, 0x00, 0x7f, 0xfe, 0x00, 0x00, 0x3f, 0xfc, 0x00, 0x00, 0x07, 0xe0, 0x00
    };
    tft->drawBitmap(x, y, flameRed, 32, 32, RED);
    tft->drawBitmap(x, y, flameYellow, 32, 32, YELLOW);
}

void View::drawText9pt(short x, short y, short colour, String text) {
    tft->setTextSize(1);
    tft->setTextColor(colour);
    tft->setFont(&FreeSans9pt7b);
    tft->fillRect(x - 2, y - 4, 180, 18, BLACK);
    tft->setCursor(x, y + 10);
    tft->print(text);
}

void View::drawText12pt(short x, short y, short colour, String text) {
    tft->setTextSize(1);
    tft->setTextColor(colour);
    tft->setFont(&FreeSans12pt7b);
    tft->fillRect(x - 2, y - 6, 180, 20, BLACK);
    tft->setCursor(x, y + 10);
    tft->print(text);
}
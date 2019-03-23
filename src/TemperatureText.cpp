#include "TemperatureText.h"

TemperatureText::TemperatureText(Adafruit_ILI9341* tft, int16_t x, int16_t y, int16_t colour, unsigned char decimalPlaces) {
    this->tft = tft;
    this->x = x;
    this->y = y;
    this->colour = colour;
    this->decimalPlaces = decimalPlaces;
}

void TemperatureText::render() {
    tft->setTextSize(1);
    tft->setTextColor(this->colour);
    tft->setFont(&FreeSansBold24pt7b);

    String text = String(this->temperature, this->decimalPlaces);
    text = String(text + "  C");

    int16_t  boundsX, boundsY;
    uint16_t boundsW, boundsH;
    tft->getTextBounds(text, x, y, &boundsX, &boundsY, &boundsW, &boundsH);

    //tft->fillRect(this->x - 2, this->y - 2, boundsW + 4, boundsH + 4, 0);
    tft->fillRect(this->x - 2, this->y - 2, boundsW + 30, boundsH + 4, 0);

    tft->fillCircle(this->x + boundsW - 35, this->y + 6, 6, this->colour);
    tft->fillCircle(this->x + boundsW - 35, this->y + 6, 2, 0);

    tft->setCursor(this->x, this->y + boundsH);
    tft->print(text);
}
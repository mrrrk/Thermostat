#include "Button.h"

Button::Button(Adafruit_ILI9341* tft, int16_t direction, int16_t x1, int16_t y1, int16_t x2, int16_t y2){
    this->tft = tft;
    this->lastTouchWhen = 0;
    this->direction = direction;    
    this->x1 = x1;
    this->x2 = x2;
    this->y1 = y1;
    this->y2 = y2;
}

void Button::render() {
    // to do
    int16_t width = this->x2 - this->x1;
    int16_t height = this->y2 - this->y1;

    int16_t bg = this->lastTouchWhen > 0 ? 0x630C : 0x0000;

    //http://www.barth-dev.de/online/rgb565-color-picker/
    tft->fillRoundRect(this->x1, this->y1, width, height, 4, bg);
    tft->drawRoundRect(this->x1, this->y1, width, height, 4, 0xFFE0);

    this->drawArrow();
}

void Button::drawArrow() {
    int16_t width = this->x2 - this->x1;
    int16_t height = this->y2 - this->y1;
    int16_t uw = width / 6; // unit = sixth of width
    int16_t uh = height / 6; // unit = sixth of height
    int16_t i;
    struct Point points[4] = {{ 3 * uw, uh }, { uw, 3 * uh }, { 5 * uw, 3 * uh }, { 2 * uw, 3 * uh }};
    for(i = 0; i < 4; i++){
        // flip y coords if down
        if(this->direction == -1) {
            points[i].y = height - points[i].y;
        }
        //offsets
        points[i].x = points[i].x + this->x1; 
        points[i].y = points[i].y + this->y1;
    }
    tft->fillTriangle(points[0].x, points[0].y, points[1].x, points[1].y, points[2].x, points[2].y, 0xFFE0);
    tft->fillRect(points[3].x, points[3].y, uw * 2, this-> direction * (uh * 2), 0xFFE0);
}

bool Button::isInBounds(TS_Point point) {
    bool result = point.x > this->x1 && point.x < this->x2 && point.y > this->y1 && point.y < this->y2;
    // callback...
    //if(result && this->clicked) {
    //    this->clicked(this->id);
    //}
    return result;
}



void Button::processTouch(TS_Point point, unsigned long when) {
    if(this->isInBounds(point)){
        boolean shouldRender = this->lastTouchWhen == 0; 
        this->lastTouchPoint = point;
        this->lastTouchWhen = when;
        if(shouldRender) this->render();
    }
}

void Button::checkTouch(unsigned long when) {
    // if last touch > 50ms (???) ago then clicked
    unsigned long diff = when - this->lastTouchWhen;
    if(this->lastTouchWhen > 0 && diff > 50 && this->clicked != NULL) {
        this->clicked(this->direction, diff);
        this->lastTouchWhen = 0;
        this->lastTouchPoint = TS_Point(); // ??? memory leaks?
        this->render();
    }
}

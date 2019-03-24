#include "Button.h"

Button::Button(Adafruit_ILI9341* tft, short direction, short x1, short y1, short x2, short y2){
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
    short width = this->x2 - this->x1;
    short height = this->y2 - this->y1;

    short bg = this->lastTouchWhen > 0 ? 0x630C : 0x0000;

    //http://www.barth-dev.de/online/rgb565-color-picker/
    tft->fillRoundRect(this->x1, this->y1, width, height, 4, bg);
    tft->drawRoundRect(this->x1, this->y1, width, height, 4, 0xFFE0);

    this->drawArrow();
}

void Button::drawArrow() {
    short width = this->x2 - this->x1;
    short height = this->y2 - this->y1;
    short uw = width / (short)6; // unit = sixth of width
    short uh = height / (short)6; // unit = sixth of height
    short i;
    struct Point points[4] = {{ (short)3 * uw, uh }, { uw, (short)3 * uh }, { (short)5 * uw, (short)3 * uh }, { (short)2 * uw, (short)3 * uh }};
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

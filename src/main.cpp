#include "main.h"

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);
Adafruit_STMPE610 touchScreen = Adafruit_STMPE610(STMPE_CS);
DHT dht(DHTPIN, DHTTYPE);

View* view;
Model* model;

void setup() {
    // initialize digital pin LED_BUILTIN as an output.
    pinMode(13, OUTPUT);

    

    Serial.begin(115200);
    while (!Serial) {
        ; // wait for serial port to connect. Needed for native USB port only
    }
    Serial.println("hello...");

    //tft = Adafruit_ILI9341(TFT_CS, TFT_DC);
    tft.begin();
    tft.fillScreen(ILI9341_BLACK);
    tft.setRotation(1);


    //touchScreen = Adafruit_STMPE610(STMPE_CS);
    touchScreen.begin();

    model = new Model();
    //Button* upButton = new Button(&tft, UP_ID, 200, 15, 305, 115);
    //Button* downButton = new Button(&tft, DOWN_ID, 200, 130, 305, 230);
    view = new View(&tft, model);

    view->upButton->clicked = &onClicked;
    view->downButton->clicked = &onClicked;
    view->upButton->render();
    view->downButton->render();

    view->refresh();

    dht.begin();
}

void onClicked(short direction, unsigned long diff) {
    Serial.print("CLICKED ");
    Serial.println(direction);
    Serial.print("  diff= ");
    Serial.println(diff);

    model->setPointTemperature = model->setPointTemperature + direction;

    view->refresh();
}

//void readSensor() {
//     model->newReading(millis(), dht.readTemperature(false), dht.readHumidity());
// }

// the loop function runs over and over again forever
void loop() {
   
    // check every second and refresh if changed
    if(model->isTimeForNewReading(millis())) {
        Serial.println(" check sensor... ");
        // read sensor
        if(model->didJustChange(millis(), dht.readTemperature(false), dht.readHumidity())) {
            view->refresh();
            Serial.println("reading changed");
        }
    }

    while(!touchScreen.bufferEmpty()) {
        TS_Point point = touchScreen.getPoint();

        // swap x and y because of rotated screen
        short y = point.x;
        point.x = point.y;
        point.y = y;

        // map to screen coords
        point.x = map(point.x, TS_MINX, TS_MAXX, 0, tft.width());
        point.y = map(point.y, TS_MINY, TS_MAXY, 0, tft.height());

        Serial.print("Touch x=");
        Serial.print(point.x);
        Serial.print(" y=");
        Serial.println(point.y);
        view->processTouch(point, millis());
    }
    view->checkTouch(millis());

    //digitalWrite(LED_BUILTIN, HIGH);
    //delay(250);
    //digitalWrite(LED_BUILTIN, LOW);
    //delay(250);
}

#include "Arduino.h"
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <Adafruit_STMPE610.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>

#include "Model.h"
#include "View.h"

#define STMPE_CS 32
#define TFT_CS   15
#define TFT_DC   33
#define SD_CS    14

// to map touch coords
#define TS_MINX 150
#define TS_MINY 130
#define TS_MAXX 3800
#define TS_MAXY 4000

#define DHTTYPE DHT22
#define DHTPIN 27     // what digital pin we're connected to

void onClicked(int16_t direction, unsigned long diff);

struct SensorReading {
    unsigned long when;
    float temperature;
    float humidity;
};

SensorReading readSensor();
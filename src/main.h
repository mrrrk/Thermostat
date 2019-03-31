#include "Arduino.h"
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <Adafruit_STMPE610.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <WiFi.h>
#include <WiFiUdp.h>
//#include <NTPClient.h>
#include <Time.h>

#include "Model.h"
#include "View.h"
#include "Auth.h" // contains wifi access info - not for github!

#define STMPE_CS 32
#define TFT_CS   15
#define TFT_DC   33
#define SD_CS    14

// to map touch coords
#define TS_MINX 150
#define TS_MINY 130
#define TS_MAXX 3800
#define TS_MAXY 4000

// screen and touch screen instances
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);
Adafruit_STMPE610 touchScreen = Adafruit_STMPE610(STMPE_CS);

// temp + humid sensor
#define DHTTYPE DHT22
#define DHTPIN 27     // what digital pin we're connected to
DHT dht(DHTPIN, DHTTYPE);

// NTP and UDP stuff
WiFiUDP ntpUDP;
static const char ntpServerName[] = "uk.pool.ntp.org";
const int timeZone = 1; // BST
unsigned int localPort = 8888; // local port to listen for UDP packets
const int NTP_PACKET_SIZE = 48; // NTP time is in the first 48 bytes of message
byte packetBuffer[NTP_PACKET_SIZE]; //buffer to hold incoming & outgoing packets

// instances of view and model
View* view;
Model* model;

// ----


// ---- main function prototypes 

void onClicked(short direction, unsigned long diff);

void readSensor();

time_t getNtpTime();

void sendNTPpacket(IPAddress &address);
#include "main.h"

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

    // wifi
    Serial.print("Connecting to ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
         delay(500);
         Serial.print(".");
    }

    // initialise NTP and time library
    ntpUDP.begin(localPort);
    time_t t = getNtpTime();
    setTime(t);   
}

// the loop function runs over and over again forever
void loop() {
   
    // check every second and refresh if changed
    if(model->isTimeForNewReading(millis())) {
        Serial.println(" check sensor... ");
        // read sensor and check time
        if(model->didJustChange(millis(), dht.readTemperature(false), dht.readHumidity())
            || model->didTimeTextChange(month(), day(), weekday(), hour(), minute())) {

            view->refresh();
            Serial.println("reading or time changed");
        }

        // Serial.print("TIME: "); 
        // Serial.print(hour()); 
        // Serial.print(":");
        // Serial.print(minute());  
        // Serial.print(":");
        // Serial.println(second()); 


    }

    // periodically sync time with NTP server
    if(model->isTimeForNtpServerSync(millis())) {
        time_t t = getNtpTime();
        if(t > 0) {
            setTime(t);
            Serial.println("updated time"); 
        }
    }

    // process touches
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

void onClicked(short direction, unsigned long diff) {
    Serial.print("CLICKED ");
    Serial.println(direction);
    Serial.print("  diff= ");
    Serial.println(diff);

    model->setPointTemperature = model->setPointTemperature + direction;

    view->refresh();
}

time_t getNtpTime() {
  IPAddress ntpServerIP; // NTP server's ip address

  while (ntpUDP.parsePacket() > 0) ; // discard any previously received packets
  Serial.println("Transmit NTP Request");
  // get a random server from the pool
  WiFi.hostByName(ntpServerName, ntpServerIP);
  Serial.print(ntpServerName);
  Serial.print(": ");
  Serial.println(ntpServerIP);
  sendNTPpacket(ntpServerIP);
  uint32_t beginWait = millis();
  while (millis() - beginWait < 1500) {
    int size = ntpUDP.parsePacket();
    if (size >= NTP_PACKET_SIZE) {
      Serial.println("Receive NTP Response");
      ntpUDP.read(packetBuffer, NTP_PACKET_SIZE);  // read packet into the buffer
      unsigned long secsSince1900;
      // convert four bytes starting at location 40 to a long integer
      secsSince1900 =  (unsigned long)packetBuffer[40] << 24;
      secsSince1900 |= (unsigned long)packetBuffer[41] << 16;
      secsSince1900 |= (unsigned long)packetBuffer[42] << 8;
      secsSince1900 |= (unsigned long)packetBuffer[43];
      return secsSince1900 - 2208988800UL + timeZone * SECS_PER_HOUR;
    }
  }
  Serial.println("No NTP Response :-(");
  return 0; // return 0 if unable to get the time
}

// send an NTP request to the time server at the given address
void sendNTPpacket(IPAddress &address) {
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12] = 49;
  packetBuffer[13] = 0x4E;
  packetBuffer[14] = 49;
  packetBuffer[15] = 52;
  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:
  ntpUDP.beginPacket(address, 123); //NTP requests are to port 123
  ntpUDP.write(packetBuffer, NTP_PACKET_SIZE);
  ntpUDP.endPacket();
}
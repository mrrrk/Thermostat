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

    WiFi.disconnect();
    WiFi.mode(WIFI_STA);
    
    delay(500);

    // Examples of different ways to register wifi events
    WiFi.onEvent(WiFiEvent);
    WiFi.onEvent(WiFiGotIP, WiFiEvent_t::SYSTEM_EVENT_STA_GOT_IP);
    WiFiEventId_t eventID = WiFi.onEvent([](WiFiEvent_t event, WiFiEventInfo_t info){
        Serial.print("WiFi lost connection. Reason: ");
        Serial.println(info.disconnected.reason);
    }, WiFiEvent_t::SYSTEM_EVENT_STA_DISCONNECTED);

    scanWifi();
    delay(100);

    connectWiFi();

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

wl_status_t connectWiFi() {
    if(WiFi.status() == WL_CONNECTED) {
        Serial.print("connectWiFi - Already connected.");
    }
    //WiFi.disconnect();
    Serial.print("connectWiFi - connecting to ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);
    wl_status_t wifiStatus;
    int i;
    for(i = 0; i < 5; i++){
        if ((wifiStatus = WiFi.status()) == WL_CONNECTED) {
            Serial.print("connectWiFi - connected to ");
            Serial.println(ssid);
            break;
        }
        Serial.print("connectWiFi - nothing yet: ");
        Serial.println(wifiStatusText(wifiStatus));
        delay(1000);
    }
    return wifiStatus;
}

time_t getNtpTime() {
    if(WiFi.status() != WL_CONNECTED) {
        Serial.println("getNtpTime - not connected to wifi.");
        connectWiFi();
    }
    if(WiFi.status() != WL_CONNECTED) {
        Serial.println("getNtpTime - still not connected to wifi.  Give up.");
        return 0;
    }

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

        time_t epochUTC = secsSince1900 - 2208988800UL; 

        //Add an hour if it's BST
        return isBST(epochUTC) ? epochUTC + SECS_PER_HOUR : epochUTC;
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

bool isBST(time_t t) {
    int m = month(t);
    if(m < 3 || m > 10) return false;
    if(m > 3 && m < 10) return true;
    // it must be March or October here...
    int d = day(t);
    int y = year(t); 
    int s = lastSunday(y, m);
    int h;
    if(m == 3) {
        if(d < s) return false;
        if(d > s) return true;
        h = hour(t);
        return h >= 2; // it's Sunday when the clocks change
    }
    if(m == 10) {
        if(d < s) return true;
        if(d > s) return false;
        h = hour(t);
        return h < 2; // it's Sunday when the clocks change
    }
    return true;
}

// from https://rosettacode.org/wiki/Find_the_last_Sunday_of_each_month#C
int lastSunday(int y, int forMonth) {
    int days[] = {31,29,31,30,31,30,31,31,30,31,30,31};
    int m, w;
    days[1] -= (y % 4) || (!(y % 100) && (y % 400));
    w = y * 365 + 96 * (y - 1) / 400 + 4; // no idea what's going on here
    //w = y * 365 + 97 * (y - 1) / 400 + 4;
    for(m = 0; m < 12; m++) {
        w = (w + days[m]) % 7;
        if(m + 1 == forMonth) return days[m] - w;
    }
    return 0;
}

String translateEncryptionType(wifi_auth_mode_t encryptionType) {
    switch (encryptionType) {
        case (WIFI_AUTH_OPEN): return "Open";
        case (WIFI_AUTH_WEP): return "WEP";
        case (WIFI_AUTH_WPA_PSK): return "WPA_PSK";
        case (WIFI_AUTH_WPA2_PSK): return "WPA2_PSK";
        case (WIFI_AUTH_WPA_WPA2_PSK): return "WPA_WPA2_PSK";
        case (WIFI_AUTH_WPA2_ENTERPRISE): return "WPA2_ENTERPRISE";
    }
    return "???";
}

void scanWifi() {
    Serial.println("scan start");

    // WiFi.scanNetworks will return the number of networks found
    int n = WiFi.scanNetworks();
    Serial.println("scan done");
    if (n == 0) {
        Serial.println("no networks found");
    } else {
        Serial.print(n);
        Serial.println(" networks found");
        for (int i = 0; i < n; ++i) {
            // Print SSID and RSSI for each network found
            Serial.print(WiFi.SSID(i));

            // -- bssid is mac address?  array of bytes...
            Serial.print(" / ");
            //Serial.print(WiFi.BSSID(i));
            Serial.print(WiFi.BSSIDstr(i));
            Serial.print("  strength: ");
            Serial.print(WiFi.RSSI(i));
            //Serial.println("  ");

            Serial.print("  enc type: ");
            Serial.println(translateEncryptionType(WiFi.encryptionType(i)));
            //Serial.println((WiFi.encryptionType(i) == ENC_TYPE_NONE) ? " " : "*");
            delay(10);
        }
    }
    Serial.println("");
}

const char* wifiStatusText(wl_status_t status) {
  switch (status) {
    case WL_NO_SHIELD: return "WL_NO_SHIELD";
    case WL_IDLE_STATUS: return "WL_IDLE_STATUS";
    case WL_NO_SSID_AVAIL: return "WL_NO_SSID_AVAIL";
    case WL_SCAN_COMPLETED: return "WL_SCAN_COMPLETED";
    case WL_CONNECTED: return "WL_CONNECTED";
    case WL_CONNECT_FAILED: return "WL_CONNECT_FAILED";
    case WL_CONNECTION_LOST: return "WL_CONNECTION_LOST";
    case WL_DISCONNECTED: return "WL_DISCONNECTED";
  }
}

void WiFiEvent(WiFiEvent_t event) {
    Serial.printf("[WiFi-event] event: %d\n", event);

    switch (event) {
        case SYSTEM_EVENT_WIFI_READY: 
            Serial.println("WiFi interface ready");
            break;
        case SYSTEM_EVENT_SCAN_DONE:
            Serial.println("Completed scan for access points");
            break;
        case SYSTEM_EVENT_STA_START:
            Serial.println("WiFi client started");
            break;
        case SYSTEM_EVENT_STA_STOP:
            Serial.println("WiFi clients stopped");
            break;
        case SYSTEM_EVENT_STA_CONNECTED:
            Serial.println("Connected to access point");
            break;
        case SYSTEM_EVENT_STA_DISCONNECTED:
            Serial.println("Disconnected from WiFi access point");
            break;
        case SYSTEM_EVENT_STA_AUTHMODE_CHANGE:
            Serial.println("Authentication mode of access point has changed");
            break;
        case SYSTEM_EVENT_STA_GOT_IP:
            Serial.print("Obtained IP address: ");
            Serial.println(WiFi.localIP());
            break;
        case SYSTEM_EVENT_STA_LOST_IP:
            Serial.println("Lost IP address and IP address is reset to 0");
            break;
        case SYSTEM_EVENT_STA_WPS_ER_SUCCESS:
            Serial.println("WiFi Protected Setup (WPS): succeeded in enrollee mode");
            break;
        case SYSTEM_EVENT_STA_WPS_ER_FAILED:
            Serial.println("WiFi Protected Setup (WPS): failed in enrollee mode");
            break;
        case SYSTEM_EVENT_STA_WPS_ER_TIMEOUT:
            Serial.println("WiFi Protected Setup (WPS): timeout in enrollee mode");
            break;
        case SYSTEM_EVENT_STA_WPS_ER_PIN:
            Serial.println("WiFi Protected Setup (WPS): pin code in enrollee mode");
            break;
        case SYSTEM_EVENT_AP_START:
            Serial.println("WiFi access point started");
            break;
        case SYSTEM_EVENT_AP_STOP:
            Serial.println("WiFi access point  stopped");
            break;
        case SYSTEM_EVENT_AP_STACONNECTED:
            Serial.println("Client connected");
            break;
        case SYSTEM_EVENT_AP_STADISCONNECTED:
            Serial.println("Client disconnected");
            break;
        case SYSTEM_EVENT_AP_STAIPASSIGNED:
            Serial.println("Assigned IP address to client");
            break;
        case SYSTEM_EVENT_AP_PROBEREQRECVED:
            Serial.println("Received probe request");
            break;
        case SYSTEM_EVENT_GOT_IP6:
            Serial.println("IPv6 is preferred");
            break;
        case SYSTEM_EVENT_ETH_START:
            Serial.println("Ethernet started");
            break;
        case SYSTEM_EVENT_ETH_STOP:
            Serial.println("Ethernet stopped");
            break;
        case SYSTEM_EVENT_ETH_CONNECTED:
            Serial.println("Ethernet connected");
            break;
        case SYSTEM_EVENT_ETH_DISCONNECTED:
            Serial.println("Ethernet disconnected");
            break;
        case SYSTEM_EVENT_ETH_GOT_IP:
            Serial.println("Obtained IP address");
            break;
        default: break;
    }
}

void WiFiGotIP(WiFiEvent_t event, WiFiEventInfo_t info) {
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(IPAddress(info.got_ip.ip_info.ip.addr));
}
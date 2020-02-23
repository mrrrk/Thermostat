#include "Model.h"

// todo - put this an a separate file?
const char monthName0[] PROGMEM = "";
const char monthName1[] PROGMEM = "Jan";
const char monthName2[] PROGMEM = "Feb";
const char monthName3[] PROGMEM = "Mar";
const char monthName4[] PROGMEM = "Apr";
const char monthName5[] PROGMEM = "May";
const char monthName6[] PROGMEM = "Jun";
const char monthName7[] PROGMEM = "Jul";
const char monthName8[] PROGMEM = "Aug";
const char monthName9[] PROGMEM = "Sep";
const char monthName10[] PROGMEM = "Oct";
const char monthName11[] PROGMEM = "Nov";
const char monthName12[] PROGMEM = "Dec";
const PROGMEM char * const PROGMEM monthNames[] = {
    monthName0, monthName1, monthName2, monthName3,
    monthName4, monthName5, monthName6, monthName7,
    monthName8, monthName9, monthName10, monthName11,
    monthName12
};
const char dayName0[] PROGMEM = "";
const char dayName1[] PROGMEM = "Mon";
const char dayName2[] PROGMEM = "Tue";
const char dayName3[] PROGMEM = "Wed";
const char dayName4[] PROGMEM = "Thu";
const char dayName5[] PROGMEM = "Fri";
const char dayName6[] PROGMEM = "Sat";
const char dayName7[] PROGMEM = "Sun";
const PROGMEM char * const PROGMEM dayNames[] = {
    dayName0, dayName7,dayName1, dayName2, dayName3,
    dayName4, dayName5, dayName6
};

bool Model::isTimeForNewReading(unsigned long now) {
    //return now - lastReading.when > 1000;
    return now - lastReading.when > 2000;
}

bool Model::isTimeForNtpServerSync(unsigned long now) {
    //bool isTime = now - lastSyncedTimeWithNtpServer > 10 * 1000;
    bool isTime = now - lastSyncedTimeWithNtpServer > 20 * 60 * 1000;
    if(isTime) {
        lastSyncedTimeWithNtpServer = now;
    }
    return isTime;
}

bool Model::didJustChange(unsigned long when, float temperature, float humidity) {
    // Using these diff values gives us a hysteresis effect to stop 
    // the UI and relay state from jumping about too much.
    bool isDifferent = 
        fabsf(lastReading.temperature - temperature) > tempDiff || 
        fabsf(lastReading.humidity - humidity) > humDiff;
    if(isDifferent) {
        this->currentTemperature = temperature;
        this->currentHumidity = humidity;
    }
    lastReading.humidity = humidity;
    lastReading.temperature = temperature;
    lastReading.when = when;
    return isDifferent;
}

// this is really view code!
bool Model::didTimeTextChange(int month, int day, int weekday, int hour, int minute) {
    String dateText = String(day);
    //dateText = dayNames[weekday] + " " + dateText + " " + monthNames[month];
    dateText = String(dayNames[weekday]) + " " + dateText + " " + monthNames[month];
    String hoursText = hour < 10 ? "0" + String(hour) : String(hour);
    String minutesText = minute < 10 ? "0" + String(minute) : String(minute);
    String text = dateText + " " + hoursText + ":" + minutesText;
    bool isDifferent = this->lastTimeText != text;
    this->lastTimeText = text;
    return isDifferent;
}

bool Model::isCold() {
    return this->setPointTemperature - this->currentTemperature > tempDiff;
}
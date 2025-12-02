#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <RTClib.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <pgmspace.h>

// Pin definitions
#define OLED_RESET -1
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define DS18B20_PIN D5
#define SWITCH_PIN D3
#define WIFI_SWITCH_PIN D6

// WiFi credentials — CHANGE THESE
const char* ssid = "Redmi 13 5G";
const char* password = "19902005";

// NTP Client Setup
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 19800, 60000); // IST = +5:30

// Instances
RTC_DS3231 rtc;
OneWire oneWire(DS18B20_PIN);
DallasTemperature sensors(&oneWire);
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// ----- PROGMEM strings -----
const char days0[] PROGMEM = "Sun";
const char days1[] PROGMEM = "Mon";
const char days2[] PROGMEM = "Tue";
const char days3[] PROGMEM = "Wed";
const char days4[] PROGMEM = "Thu";
const char days5[] PROGMEM = "Fri";
const char days6[] PROGMEM = "Sat";
const char * const days_table[] PROGMEM = { days0, days1, days2, days3, days4, days5, days6 };

const char m0[] PROGMEM = "JAN";
const char m1[] PROGMEM = "FEB";
const char m2[] PROGMEM = "MAR";
const char m3[] PROGMEM = "APR";
const char m4[] PROGMEM = "MAY";
const char m5[] PROGMEM = "JUN";
const char m6[] PROGMEM = "JUL";
const char m7[] PROGMEM = "AUG";
const char m8[] PROGMEM = "SEP";
const char m9[] PROGMEM = "OCT";
const char m10[] PROGMEM = "NOV";
const char m11[] PROGMEM = "DEC";
const char * const months_table[] PROGMEM = { m0, m1, m2, m3, m4, m5, m6, m7, m8, m9, m10, m11 };

// --- enums & state ---
enum ScreenMode { SHOW_TIME, SHOW_TEMP };

struct ClockState {
    DateTime rtcTime;
    unsigned long lastRTCUpdateMs;
    float temperatureC;
    unsigned long lastTempMs;
    bool colonVisible;
    ScreenMode mode;
    bool screenDirty;
};

ClockState state = {
    DateTime(2000,1,1,0,0,0),
    0,
    DEVICE_DISCONNECTED_C,
    0,
    true,
    SHOW_TIME,
    true
};

// NEW: for soft clock
unsigned long lastTickMs = 0;

// Debounce vars
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 50;
bool lastButtonState = LOW;
bool buttonState = LOW;

// display tracking
int lastDisplayedHour = -1;
int lastDisplayedMinute = -1;
bool lastDisplayedColon = false;
int lastDisplayedDay = -1;
int lastDisplayedMonth = -1;
int lastDisplayedYear = -1;
float lastDisplayedTemp = -9999.0;

// intervals
const unsigned long RTC_UPDATE_INTERVAL = 30000UL;
const unsigned long TEMP_UPDATE_INTERVAL = 1000UL;
const unsigned long COLON_BLINK_INTERVAL = 1000UL;

bool wifiSyncInProgress = false;

void readProgmemStr(const char * const table[], uint8_t index, char *buf, size_t bufSize) {
    const char *ptr = (const char*)pgm_read_ptr(&(table[index]));
    strncpy_P(buf, ptr, bufSize);
    buf[bufSize - 1] = '\0';
}

void showTimeDate();
void showDS18B20Data();
void syncTimeFromWiFi();
const char* dayOfTheWeek(uint8_t day);
const char* monthName(uint8_t month);


void setup() {
    pinMode(SWITCH_PIN, INPUT_PULLUP);
    pinMode(WIFI_SWITCH_PIN, INPUT_PULLUP);

    Wire.begin();

    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { while(1); }
    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);

    // ---- smooth 4 bouncing dots animation ----
    int dotY = SCREEN_HEIGHT / 2;
    int dotXStart = 30;
    int spacing = 20;
    unsigned long startTime = millis();
    const int bounceHeight = 10;
    const int framesPerDot = 6;
    const int frameDelay = 50;
    while (millis() - startTime < 2000) {
        for (int dot = 0; dot < 4; dot++) {
            for (int f = 0; f < framesPerDot; f++) {
                display.clearDisplay();
                for (int i = 0; i < 4; i++) {
                    int yOffset = 0;
                    if (i == dot) yOffset = -bounceHeight * sin(PI * f / framesPerDot);
                    display.fillCircle(dotXStart + i*spacing, dotY + yOffset, 4, SSD1306_WHITE);
                }
                display.display();
                delay(frameDelay);
            }
        }
    }

    display.clearDisplay();
    display.display();

    if (!rtc.begin()) { while (1); }
    sensors.begin();
    sensors.setResolution(11);

    display.ssd1306_command(SSD1306_SETCONTRAST);
    display.ssd1306_command(26);

    state.lastRTCUpdateMs = 0;
    state.lastTempMs = 0;
    state.colonVisible = true;
    state.mode = SHOW_TIME;
    state.screenDirty = true;

    lastDisplayedHour = -1;

    // NEW: initialize soft clock tick
    lastTickMs = millis();
}

// ================= MAIN LOOP =================
void loop() {
    unsigned long nowMs = millis();

    if (digitalRead(WIFI_SWITCH_PIN) == LOW && !wifiSyncInProgress) {
        wifiSyncInProgress = true;
        syncTimeFromWiFi();
        wifiSyncInProgress = false;
        state.screenDirty = true;
    }

    if (wifiSyncInProgress) return;

    int reading = digitalRead(SWITCH_PIN);
    if (reading != lastButtonState) lastDebounceTime = nowMs;
    if ((nowMs - lastDebounceTime) > debounceDelay) {
        if (reading != buttonState) {
            buttonState = reading;
            if (buttonState == LOW) {
                state.mode = (state.mode == SHOW_TIME) ? SHOW_TEMP : SHOW_TIME;
                state.screenDirty = true;
            }
        }
    }
    lastButtonState = reading;

    static unsigned long lastColonToggle = 0;
    if (nowMs - lastColonToggle >= COLON_BLINK_INTERVAL) {
        lastColonToggle = nowMs;
        state.colonVisible = !state.colonVisible;
    }

    // ----------- RTC READ (every 30s) -----------
    if (nowMs - state.lastRTCUpdateMs >= RTC_UPDATE_INTERVAL || state.lastRTCUpdateMs == 0) {
        state.rtcTime = rtc.now();
        state.lastRTCUpdateMs = nowMs;

        // NEW: reset ticker because time jumped to accurate RTC time
        lastTickMs = nowMs;

        state.screenDirty = true;
    }

    // ----------- SOFT CLOCK TICK (every second) -----------
    if (nowMs - lastTickMs >= 1000) {
        int secPassed = (nowMs - lastTickMs) / 1000;
        state.rtcTime = state.rtcTime + TimeSpan(secPassed);
        lastTickMs += secPassed * 1000;
        state.screenDirty = true;
    }

    // ----------- Temperature read -----------
    if (nowMs - state.lastTempMs >= TEMP_UPDATE_INTERVAL) {
        sensors.requestTemperatures();
        state.temperatureC = sensors.getTempCByIndex(0);
        state.lastTempMs = nowMs;
        if (state.mode == SHOW_TEMP) state.screenDirty = true;
    }

    if (state.mode == SHOW_TEMP) showDS18B20Data();
    else showTimeDate();

    delay(0);
}

// ---------- Display time and date ----------
void showTimeDate() {
    DateTime now = state.rtcTime;
    int hour = now.hour();
    int minute = now.minute();
    int day = now.day();
    int month = now.month();
    int year = now.year();

    if (state.screenDirty ||
        hour != lastDisplayedHour ||
        minute != lastDisplayedMinute ||
        state.colonVisible != lastDisplayedColon ||
        day != lastDisplayedDay ||
        month != lastDisplayedMonth ||
        year != lastDisplayedYear) {

        display.fillRect(0, 0, SCREEN_WIDTH, 40, SSD1306_BLACK);
        display.setTextSize(3);
        display.setTextColor(SSD1306_WHITE);
        display.setCursor(15, 10);

        if (state.colonVisible) display.printf("%02d:%02d", hour, minute);
        else display.printf("%02d %02d", hour, minute);

        display.fillRect(0, 40, SCREEN_WIDTH, 24, SSD1306_BLACK);
        display.setTextSize(1);
        display.setCursor(10, 50);

        char dowBuf[4];
        char monBuf[4];
        readProgmemStr(days_table, now.dayOfTheWeek(), dowBuf, sizeof(dowBuf));
        readProgmemStr(months_table, month - 1, monBuf, sizeof(monBuf));

        display.printf("%s, %02d %s %04d", dowBuf, day, monBuf, year);
        display.display();

        lastDisplayedHour = hour;
        lastDisplayedMinute = minute;
        lastDisplayedColon = state.colonVisible;
        lastDisplayedDay = day;
        lastDisplayedMonth = month;
        lastDisplayedYear = year;

        state.screenDirty = false;
    }
}

// ---------- Display DS18B20 temperature ----------
void showDS18B20Data() {
    if (state.screenDirty || fabs(state.temperatureC - lastDisplayedTemp) >= 0.01) {
        display.fillRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, SSD1306_BLACK);

        display.setTextSize(2);
        display.setTextColor(SSD1306_WHITE);
        display.setCursor(20, 0);
        display.println("DS18B20");

        display.setCursor(20, 30);
        if (state.temperatureC == DEVICE_DISCONNECTED_C) display.print("----");
        else {
            display.printf("%.2f", state.temperatureC);
            display.print((char)248);
            display.print("C");
        }

        display.display();

        lastDisplayedTemp = state.temperatureC;
        state.screenDirty = false;
    }
}

// ---------- Fetch time from NTP ----------
void syncTimeFromWiFi() {
    display.fillRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, SSD1306_BLACK);
    display.setTextSize(1);
    display.setCursor(10, 20);
    display.println("Connecting WiFi...");
    display.display();

    WiFi.disconnect(true);
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);

    unsigned long start = millis();
    const unsigned long timeout = 10000UL;
    while (WiFi.status() != WL_CONNECTED && (millis() - start) < timeout) delay(100);

    if (WiFi.status() != WL_CONNECTED) {
        display.fillRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, SSD1306_BLACK);
        display.setCursor(10, 20);
        display.println("WiFi Failed");
        display.display();
        delay(1500);
        WiFi.disconnect(true);
        WiFi.mode(WIFI_OFF);
        return;
    }

    display.fillRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, SSD1306_BLACK);
    display.setCursor(10, 20);
    display.println("Connected.");
    display.display();

    timeClient.begin();
    if (!timeClient.forceUpdate()) {
        display.fillRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, SSD1306_BLACK);
        display.setCursor(10, 20);
        display.println("NTP Failed");
        display.display();
        delay(1500);
        timeClient.end();
        WiFi.disconnect(true);
        WiFi.mode(WIFI_OFF);
        return;
    }

    time_t raw = timeClient.getEpochTime();
    struct tm *ti = localtime(&raw);
    rtc.adjust(DateTime(1900 + ti->tm_year, ti->tm_mon + 1, ti->tm_mday,
                        ti->tm_hour, ti->tm_min, ti->tm_sec));

    state.rtcTime = rtc.now();
    state.lastRTCUpdateMs = millis();

    // NEW: reset tick counter because time jumped
    lastTickMs = millis();

    state.screenDirty = true;

    display.fillRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, SSD1306_BLACK);
    display.setCursor(10, 20);
    display.println("Time Fetched!");
    display.display();
    delay(1200);

    timeClient.end();
    WiFi.disconnect(true);
    WiFi.mode(WIFI_OFF);
}

const char* dayOfTheWeek(uint8_t day) {
    static char buf[4];
    readProgmemStr(days_table, day, buf, sizeof(buf));
    return buf;
}

const char* monthName(uint8_t month) {
    static char buf[4];
    readProgmemStr(months_table, month - 1, buf, sizeof(buf));
    return buf;
}

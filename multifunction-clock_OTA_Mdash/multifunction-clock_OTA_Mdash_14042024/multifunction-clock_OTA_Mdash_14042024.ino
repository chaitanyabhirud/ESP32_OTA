#include <WiFi.h>
#include <WifiMulti.h>
#include <Adafruit_NeoPixel.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <TimeLib.h>
#include "RTClib.h"
#include <DHT.h>
#include <Adafruit_Sensor.h>
#include <cmath>
#include <mDash.h>

#define PIXEL_PER_SEGMENT 16  // Number of LEDs in each Segment
#define PIXEL_DIGITS 4        // Number of connected Digits
#define PIXEL_PIN 5           // GPIO Pin
#define PIXEL_DASH 1          // Binary segment
#define DASH_LED 8            // Number of Dash LED
#define DHT_TYPE DHT22        // DHT Sensor Type
#define DHT_PIN 4             // DHT Sensor pin
#define TIME_FORMAT 12        // 12 = 12 hours format || 24 = 24 hours format
#define MDASH_APP_NAME "ESP32_Clock"
#define DEVICE_PASSWORD "DwBi0IRZUHeHOoztzOxfmw" //Mdash, shyam.phirke@gmail.com 

DHT dht(DHT_PIN, DHT_TYPE);
RTC_DS3231 rtc;
WiFiMulti wifiMulti;
WiFiUDP ntpUDP;

Adafruit_NeoPixel strip = Adafruit_NeoPixel((PIXEL_PER_SEGMENT * 7 * PIXEL_DIGITS) + (PIXEL_DASH * DASH_LED), PIXEL_PIN, NEO_RGB + NEO_KHZ800);
NTPClient timeClient(ntpUDP, "time.nist.gov", 19800, 60000);  //GMT+5:30 : 5*3600+30*60=19800

int Brightness = 150;
int NightBrightness = 75;

float Temperature;
int Humidity;
int Second, Minute, Hour, Year, Month, Day, Hours;

// Digits array
byte digits[14] = {
  //gfedcba
  0b0111111,  // 0
  0b0000110,  // 1
  0b1011011,  // 2
  0b1001111,  // 3
  0b1100110,  // 4
  0b1101101,  // 5
  0b1111101,  // 6
  0b0000111,  // 7
  0b1111111,  // 8
  0b1101111,  // 9
  0b0111001,  // C
  0b1100011,  // º
  0b1110110,  // H
  0b0000000,  // Empty
};

void setup() {
  Serial.begin(9600);
  mDashBegin(DEVICE_PASSWORD);
  dht.begin();
  strip.begin();
  strip.show();

  wifiMulti.addAP("Galaxy A32950C", "vskq8024");
  wifiMulti.addAP("iPhone", "Simple@18");
  wifiMulti.addAP("sandip", "nilkanth");
  wifiMulti.addAP("Airtel_touch_1", "cgb@411027");
  wifiMulti.addAP("guest-home", "C@mplex#123");

  Serial.print("Connecting to Wifi...");
  if (wifiMulti.run() != WL_CONNECTED) {
    delay(5000);
    Serial.print("Waited for 5 seconds to check wifi connection.");
  }
  if (wifiMulti.run() == WL_CONNECTED) {
    Serial.println("Connected to Wifi...");
    Serial.println(WiFi.SSID());
    Serial.println(WiFi.localIP());
  }
  timeClient.begin();
  delay(10);

  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }
  if (rtc.lostPower()) {
    Serial.println("RTC lost power, lets set the time!");
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
}

void loop() {
  Serial.println("***************************************");
  Serial.println(WiFi.status());
  Serial.println("***************************************");
  if (WiFi.status() == WL_CONNECTED) {
    timeClient.update();
    unsigned long unix_epoch = timeClient.getEpochTime();  // get UNIX Epoch time
    Second = second(unix_epoch);  // get seconds from System
    Minute = minute(unix_epoch);  // get minutes from System
    Hours = hour(unix_epoch);     // get hours from System
    Year = year(unix_epoch);      // get year from System
    Month = month(unix_epoch);    // get month from System
    Day = day(unix_epoch);        // get day from System

    // Update RTC with current time from WiFi.
    rtc.adjust(DateTime(Year, Month, Day, Hours, Minute, Second));
  } else if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Wifi not found. Reading time from RTC...");
    DateTime now = rtc.now();
    Second = now.second();  // get seconds from RTC
    Minute = now.minute();  // get minutes from RTC
    Hours = now.hour();     // get hours from RTC
    Year = now.year();      // get year from RTC
    Month = now.month();    // get month from RTC
    Day = now.day();        // get day from RTC
  }

  if (TIME_FORMAT == 12) {
    if (Hours > 12) {
      Hour = Hours - 12;
    } else if (Hours < 1) {
      Hour = 12;
    } else {
      Hour = Hours;
    }
  } else {
    Hour = Hours;
  }
  
  disp_Time();
  disp_dayMonth();
  disp_Temp();
  disp_Humi();
}

void disp_Time() {
  clearDisplay();
  if (Hour > 9) {
    writeDigit_c(0, Hour / 10);
  }
  writeDigit_c(1, Hour % 10);
  writeDigit_c(2, Minute / 10);
  writeDigit_c(3, Minute % 10);
  // writeDigit_c(4, Second / 10);
  // writeDigit_c(5, Second % 10);
  disp_Dash();  // delay added for 20 seconds
  strip.show();
}

void disp_dayMonth() {
  clearDisplay();
  if (Day > 9) {
    writeDigit_d(0, Day / 10);
  }
  // writeDigit_d(0, Day / 10);
  writeDigit_d(1, Day % 10);
  if (Month > 9) {
    writeDigit_d(2, Month / 10);
  }
  // writeDigit_d(2, Month / 10);
  writeDigit_d(3, Month % 10);

  if (Hours >= 22 || Hours < 6) {
    strip.setPixelColor(224, strip.Color(0, NightBrightness, 0));
    strip.setPixelColor(225, strip.Color(0, NightBrightness, 0));
    strip.setPixelColor(226, strip.Color(0, NightBrightness, 0));
    strip.setPixelColor(227, strip.Color(0, NightBrightness, 0));
  } else {
    strip.setPixelColor(224, strip.Color(0, Brightness, 0));
    strip.setPixelColor(225, strip.Color(0, Brightness, 0));
    strip.setPixelColor(226, strip.Color(0, Brightness, 0));
    strip.setPixelColor(227, strip.Color(0, Brightness, 0));
  }
  strip.show();
  delay(3000);
}

void disp_Temp() {
  clearDisplay();
  Temperature = dht.readTemperature();
  float T1 = Temperature * 10;
  float T2 = fmod(T1, 10);
  float T3 = fmod(Temperature, 10);
  writeDigit_t(0, Temperature / 10);
  writeDigit_t(1, T3);
  writeDigit_t(2, T2);
  writeDigit_t(3, 11);
  if (Hours >= 22 || Hours < 6) {
    strip.setPixelColor(224, strip.Color(NightBrightness, 0, 0));
    strip.setPixelColor(225, strip.Color(NightBrightness, 0, 0));
    strip.setPixelColor(226, strip.Color(NightBrightness, 0, 0));
    strip.setPixelColor(227, strip.Color(NightBrightness, 0, 0));
  } else {
    strip.setPixelColor(224, strip.Color(Brightness, 0, 0));
    strip.setPixelColor(225, strip.Color(Brightness, 0, 0));
    strip.setPixelColor(226, strip.Color(Brightness, 0, 0));
    strip.setPixelColor(227, strip.Color(Brightness, 0, 0));
  }
  strip.show();
  delay(3000);
}

void disp_Humi() {
  clearDisplay();
  Humidity = dht.readHumidity();
  writeDigit_h(0, Humidity / 10);
  writeDigit_h(1, Humidity % 10);
  writeDigit_h(3, 12);
  // writeDigit_h(3, 12);
  // strip.setPixelColor(57, strip.Color(Brightness, Brightness, Brightness));
  strip.show();
  delay(3000);
}

void disp_Dash() {
  if (Hours >= 22 || Hours < 6) {
    for (int i = 0; i < 10; i++) {
      strip.setPixelColor(224, strip.Color(NightBrightness, NightBrightness, NightBrightness));
      strip.setPixelColor(225, strip.Color(NightBrightness, NightBrightness, NightBrightness));
      strip.setPixelColor(226, strip.Color(NightBrightness, NightBrightness, NightBrightness));
      strip.setPixelColor(227, strip.Color(NightBrightness, NightBrightness, NightBrightness));
      strip.setPixelColor(228, strip.Color(NightBrightness, NightBrightness, NightBrightness));
      strip.setPixelColor(229, strip.Color(NightBrightness, NightBrightness, NightBrightness));
      strip.setPixelColor(230, strip.Color(NightBrightness, NightBrightness, NightBrightness));
      strip.setPixelColor(231, strip.Color(NightBrightness, NightBrightness, NightBrightness));
      strip.show();
      delay(1000);

      strip.setPixelColor(224, strip.Color(0, 0, 0));
      strip.setPixelColor(225, strip.Color(0, 0, 0));
      strip.setPixelColor(226, strip.Color(0, 0, 0));
      strip.setPixelColor(227, strip.Color(0, 0, 0));
      strip.setPixelColor(228, strip.Color(0, 0, 0));
      strip.setPixelColor(229, strip.Color(0, 0, 0));
      strip.setPixelColor(230, strip.Color(0, 0, 0));
      strip.setPixelColor(231, strip.Color(0, 0, 0));
      strip.show();
      delay(1000);
    }
  } else {
    for (int i = 0; i < 10; i++) {
      strip.setPixelColor(224, strip.Color(0, Brightness, 0));
      strip.setPixelColor(225, strip.Color(0, Brightness, 0));
      strip.setPixelColor(226, strip.Color(0, Brightness, 0));
      strip.setPixelColor(227, strip.Color(0, Brightness, 0));
      strip.setPixelColor(228, strip.Color(0, Brightness, 0));
      strip.setPixelColor(229, strip.Color(0, Brightness, 0));
      strip.setPixelColor(230, strip.Color(0, Brightness, 0));
      strip.setPixelColor(231, strip.Color(0, Brightness, 0));
      strip.show();
      delay(1000);

      strip.setPixelColor(224, strip.Color(0, 0, 0));
      strip.setPixelColor(225, strip.Color(0, 0, 0));
      strip.setPixelColor(226, strip.Color(0, 0, 0));
      strip.setPixelColor(227, strip.Color(0, 0, 0));
      strip.setPixelColor(228, strip.Color(0, 0, 0));
      strip.setPixelColor(229, strip.Color(0, 0, 0));
      strip.setPixelColor(230, strip.Color(0, 0, 0));
      strip.setPixelColor(231, strip.Color(0, 0, 0));
      strip.show();
      delay(1000);
    }
  }
}

void writeDigit_c(int index, int val) {
  byte digit = digits[val];
  int margin;
  if (index == 0 || index == 1) margin = 0;
  if (index == 2 || index == 3) margin = 1;
  // if (index == 4 || index == 5 ) margin = 2; // to display seconds

  if (Hours >= 22 || Hours < 6) {
    for (int i = 0; i <= 6; i++) {
      int offset = index * (PIXEL_PER_SEGMENT * 7) + i * PIXEL_PER_SEGMENT + margin * DASH_LED;
      uint32_t color;
      if (digit & 0x01 != 0) {
        if (index == 0 || index == 1) color = strip.Color(NightBrightness, NightBrightness, NightBrightness);
        if (index == 2 || index == 3) color = strip.Color(NightBrightness, NightBrightness, NightBrightness);
      } else
        color = strip.Color(0, 0, 0);

      for (int j = offset; j < offset + PIXEL_PER_SEGMENT; j++) {
        strip.setPixelColor(j, color);
      }
      digit = digit >> 1;
    }
  } else {
    for (int i = 0; i <= 6; i++) {
      int offset = index * (PIXEL_PER_SEGMENT * 7) + i * PIXEL_PER_SEGMENT + margin * DASH_LED;
      uint32_t color;
      if (digit & 0x01 != 0) {
        if (index == 0 || index == 1) color = strip.Color(0, Brightness, 0);
        if (index == 2 || index == 3) color = strip.Color(0, Brightness, 0);
        // if (index == 4 || index == 5 ) color = strip.Color(0,  0, Brightness); // to display seconds color
      } else
        color = strip.Color(0, 0, 0);

      for (int j = offset; j < offset + PIXEL_PER_SEGMENT; j++) {
        strip.setPixelColor(j, color);
      }
      digit = digit >> 1;
    }
  }
}

void writeDigit_t(int index, int val) {
  byte digit = digits[val];
  int margin;
  if (index == 0 || index == 1) margin = 0;
  if (index == 2 || index == 3) margin = 1;
  // if (index == 4 || index == 5 ) margin = 2;
  if (Hours >= 22 || Hours < 6) {
    for (int i = 0; i <= 6; i++) {
      int offset = index * (PIXEL_PER_SEGMENT * 7) + i * PIXEL_PER_SEGMENT + margin * DASH_LED;
      uint32_t color;
      if (digit & 0x01 != 0) {
        if (index == 0 || index == 1) color = strip.Color(NightBrightness, 0, 0);
        if (index == 2 || index == 3) color = strip.Color(NightBrightness, 0, 0);
        // if (index == 4 || index == 5 ) color = strip.Color(0,  0, Brightness);
      } else
        color = strip.Color(0, 0, 0);

      for (int j = offset; j < offset + PIXEL_PER_SEGMENT; j++) {
        strip.setPixelColor(j, color);
      }
      digit = digit >> 1;
    }
  } else {
    for (int i = 0; i <= 6; i++) {
      int offset = index * (PIXEL_PER_SEGMENT * 7) + i * PIXEL_PER_SEGMENT + margin * DASH_LED;
      uint32_t color;
      if (digit & 0x01 != 0) {
        if (index == 0 || index == 1) color = strip.Color(Brightness, 0, 0);
        if (index == 2 || index == 3) color = strip.Color(Brightness, 0, 0);
        // if (index == 4 || index == 5 ) color = strip.Color(0,  0, Brightness);
      } else
        color = strip.Color(0, 0, 0);

      for (int j = offset; j < offset + PIXEL_PER_SEGMENT; j++) {
        strip.setPixelColor(j, color);
      }
      digit = digit >> 1;
    }
  }
}

void writeDigit_h(int index, int val) {
  byte digit = digits[val];
  int margin;
  if (index == 0 || index == 1) margin = 0;
  if (index == 2 || index == 3) margin = 1;
  // if (index == 4 || index == 5 ) margin = 2;
  if (Hours >= 22 || Hours < 6) {
    for (int i = 0; i <= 6; i++) {
      int offset = index * (PIXEL_PER_SEGMENT * 7) + i * PIXEL_PER_SEGMENT + margin * DASH_LED;
      uint32_t color;
      if (digit & 0x01 != 0) {
        if (index == 0 || index == 1) color = strip.Color(NightBrightness, 0, NightBrightness);
        if (index == 2 || index == 3) color = strip.Color(NightBrightness, 0, NightBrightness);
        // if (index == 4 || index == 5 ) color = strip.Color(0, 0, Brightness);
      } else
        color = strip.Color(0, 0, 0);

      for (int j = offset; j < offset + PIXEL_PER_SEGMENT; j++) {
        strip.setPixelColor(j, color);
      }
      digit = digit >> 1;
    }
  } else {
    for (int i = 0; i <= 6; i++) {
      int offset = index * (PIXEL_PER_SEGMENT * 7) + i * PIXEL_PER_SEGMENT + margin * DASH_LED;
      uint32_t color;
      if (digit & 0x01 != 0) {
        if (index == 0 || index == 1) color = strip.Color(204, 0, 102);
        if (index == 2 || index == 3) color = strip.Color(204, 0, 102);
        // if (index == 4 || index == 5 ) color = strip.Color(0, 0, Brightness);
      } else
        color = strip.Color(0, 0, 0);

      for (int j = offset; j < offset + PIXEL_PER_SEGMENT; j++) {
        strip.setPixelColor(j, color);
      }
      digit = digit >> 1;
    }
  }
}

void writeDigit_d(int index, int val) {
  byte digit = digits[val];
  int margin;
  if (index == 0 || index == 1) margin = 0;
  if (index == 2 || index == 3) margin = 1;
  if (index == 4 || index == 5) margin = 2;
  if (Hours >= 22 || Hours < 6) {
    for (int i = 0; i <= 6; i++) {
      int offset = index * (PIXEL_PER_SEGMENT * 7) + i * PIXEL_PER_SEGMENT + margin * DASH_LED;
      uint32_t color;
      if (digit & 0x01 != 0) {
        if (index == 0 || index == 1) color = strip.Color(0, NightBrightness, NightBrightness);
        if (index == 2 || index == 3) color = strip.Color(NightBrightness, NightBrightness, 0);
      } else
        color = strip.Color(0, 0, 0);

      for (int j = offset; j < offset + PIXEL_PER_SEGMENT; j++) {
        strip.setPixelColor(j, color);
      }
      digit = digit >> 1;
    }
  } else {
    for (int i = 0; i <= 6; i++) {
      int offset = index * (PIXEL_PER_SEGMENT * 7) + i * PIXEL_PER_SEGMENT + margin * DASH_LED;
      uint32_t color;
      if (digit & 0x01 != 0) {
        if (index == 0 || index == 1) color = strip.Color(0, Brightness, Brightness);
        if (index == 2 || index == 3) color = strip.Color(Brightness, Brightness, 0);
      } else
        color = strip.Color(0, 0, 0);

      for (int j = offset; j < offset + PIXEL_PER_SEGMENT; j++) {
        strip.setPixelColor(j, color);
      }
      digit = digit >> 1;
    }
  }
}

void clearDisplay() {
  for (int i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, strip.Color(0, 0, 0));
  }
  strip.show();
}
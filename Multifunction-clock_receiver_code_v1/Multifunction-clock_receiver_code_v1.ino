#include <WiFi.h>
#include <esp_now.h> //https://dronebotworkshop.com/esp-now/ 
#include <Adafruit_NeoPixel.h>
#include <cmath>


#define PIXEL_PER_SEGMENT 4  // Number of LEDs in each Segment
#define PIXEL_DIGITS 4        // Number of connected Digits
#define PIXEL_PIN 5           // GPIO Pin
#define PIXEL_DASH 1          // Binary segment
#define DASH_LED 1            // Number of Dash LED
#define TIME_FORMAT 12        // 12 = 12 hours format || 24 = 24 hours format




Adafruit_NeoPixel strip = Adafruit_NeoPixel((PIXEL_PER_SEGMENT * 7 * PIXEL_DIGITS) + (PIXEL_DASH * DASH_LED), PIXEL_PIN, NEO_RGB + NEO_KHZ800);


int Brightness = 125;
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


// Define a data structure
typedef struct struct_message {
  int var_Day;
  int var_Month;
  int var_Year;
  int var_Hour;
  int var_Hours;
  int var_Minute;
  int var_Second;
  int var_Humidity;
  float var_Temperature;
} struct_message;
 
// Create a structured object
struct_message myData;
 
 
// Callback function executed when data is received
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&myData, incomingData, sizeof(myData));
  Serial.print("Data received: ");
  Serial.println(len);
  Serial.print("Day - ");
  Serial.println(myData.var_Day);
  Serial.print("Month - ");
  Serial.println(myData.var_Month);
  Serial.print("Year - ");
  Serial.println(myData.var_Year);
  Serial.print("Hour - ");
  Serial.println(myData.var_Hour);
  Serial.print("Hours - ");
  Serial.println(myData.var_Hours);
  Serial.print("Minute - ");
  Serial.println(myData.var_Minute);
  Serial.print("Second - ");
  Serial.println(myData.var_Second);
  Serial.print("Humidity - ");
  Serial.println(myData.var_Humidity);
  Serial.print("Temperature - ");
  Serial.println(myData.var_Temperature);
  Serial.println();
}




void setup() {
  Serial.begin(9600);
  strip.begin();
  strip.show();

   // Set ESP32 as a Wi-Fi Station
  WiFi.mode(WIFI_STA);
 
  // Initilize ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  
  // Register callback function
  esp_now_register_recv_cb(OnDataRecv);
 

}

void loop() {
  Day = myData.var_Day;
  Month = myData.var_Month;
  Year = myData.var_Year;
  Hour = myData.var_Hour;
  Hours = myData.var_Hours;
  Minute = myData.var_Minute;
  Second = myData.var_Second;
  Humidity = myData.var_Humidity;
  Temperature =  myData.var_Temperature;


    
 
  Serial.print("Hour - ");
  Serial.println("modified");
  Serial.print("Hours - ");
  Serial.println("modified");
  Serial.print("Minute - ");
  Serial.println("modified");
  Serial.print("Second - ");

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

  // if (Hours >= 22 || Hours < 6) {
  //   strip.setPixelColor(224, strip.Color(0, NightBrightness, 0));
  //   strip.setPixelColor(225, strip.Color(0, NightBrightness, 0));
  //   strip.setPixelColor(226, strip.Color(0, NightBrightness, 0));
  //   strip.setPixelColor(227, strip.Color(0, NightBrightness, 0));
  // } else {
  //   strip.setPixelColor(224, strip.Color(0, Brightness, 0));
  //   strip.setPixelColor(225, strip.Color(0, Brightness, 0));
  //   strip.setPixelColor(226, strip.Color(0, Brightness, 0));
  //   strip.setPixelColor(227, strip.Color(0, Brightness, 0));
  // }
  strip.show();
  delay(3000);
}

void disp_Temp() {
  clearDisplay();
 
  float T1 = Temperature * 10;
  float T2 = fmod(T1, 10);
  float T3 = fmod(Temperature, 10);
  writeDigit_t(0, Temperature / 10);
  writeDigit_t(1, T3);
  writeDigit_t(2, T2);
  writeDigit_t(3, 11);
  // if (Hours >= 22 || Hours < 6) {
  //   strip.setPixelColor(224, strip.Color(NightBrightness, 0, 0));
  //   strip.setPixelColor(225, strip.Color(NightBrightness, 0, 0));
  //   strip.setPixelColor(226, strip.Color(NightBrightness, 0, 0));
  //   strip.setPixelColor(227, strip.Color(NightBrightness, 0, 0));
  // } else {
  //   strip.setPixelColor(224, strip.Color(Brightness, 0, 0));
  //   strip.setPixelColor(225, strip.Color(Brightness, 0, 0));
  //   strip.setPixelColor(226, strip.Color(Brightness, 0, 0));
  //   strip.setPixelColor(227, strip.Color(Brightness, 0, 0));
  // }
  strip.show();
  delay(3000);
}

void disp_Humi() {
  clearDisplay();

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
      // strip.setPixelColor(224, strip.Color(NightBrightness, NightBrightness, NightBrightness));
      // strip.setPixelColor(225, strip.Color(NightBrightness, NightBrightness, NightBrightness));
      // strip.setPixelColor(226, strip.Color(NightBrightness, NightBrightness, NightBrightness));
      // strip.setPixelColor(227, strip.Color(NightBrightness, NightBrightness, NightBrightness));
      // strip.setPixelColor(228, strip.Color(NightBrightness, NightBrightness, NightBrightness));
      // strip.setPixelColor(229, strip.Color(NightBrightness, NightBrightness, NightBrightness));
      // strip.setPixelColor(230, strip.Color(NightBrightness, NightBrightness, NightBrightness));
      // strip.setPixelColor(231, strip.Color(NightBrightness, NightBrightness, NightBrightness));
      strip.show();
      delay(1000);

      // strip.setPixelColor(224, strip.Color(0, 0, 0));
      // strip.setPixelColor(225, strip.Color(0, 0, 0));
      // strip.setPixelColor(226, strip.Color(0, 0, 0));
      // strip.setPixelColor(227, strip.Color(0, 0, 0));
      // strip.setPixelColor(228, strip.Color(0, 0, 0));
      // strip.setPixelColor(229, strip.Color(0, 0, 0));
      // strip.setPixelColor(230, strip.Color(0, 0, 0));
      // strip.setPixelColor(231, strip.Color(0, 0, 0));
      strip.show();
      delay(1000);
    }
  } else {
    for (int i = 0; i < 10; i++) {
      // strip.setPixelColor(224, strip.Color(0, Brightness, 0));
      // strip.setPixelColor(225, strip.Color(0, Brightness, 0));
      // strip.setPixelColor(226, strip.Color(0, Brightness, 0));
      // strip.setPixelColor(227, strip.Color(0, Brightness, 0));
      // strip.setPixelColor(228, strip.Color(0, Brightness, 0));
      // strip.setPixelColor(229, strip.Color(0, Brightness, 0));
      // strip.setPixelColor(230, strip.Color(0, Brightness, 0));
      // strip.setPixelColor(231, strip.Color(0, Brightness, 0));
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

void writeDigit_c(int index, int val) { //Clock
  byte digit = digits[val];
  int margin;
  if (index == 0 || index == 1) margin = 0;
  if (index == 2 || index == 3) margin = 1;
  // if (index == 4 || index == 5 ) margin = 2; // to display seconds

  if (Hours >= 20 || Hours < 6) {
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

void writeDigit_t(int index, int val) { //Temperature
  byte digit = digits[val];
  int margin;
  if (index == 0 || index == 1) margin = 0;
  if (index == 2 || index == 3) margin = 1;
  // if (index == 4 || index == 5 ) margin = 2;
  if (Hours >= 20 || Hours < 6) {
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

void writeDigit_h(int index, int val) { //Humidity
  byte digit = digits[val];
  int margin;
  if (index == 0 || index == 1) margin = 0;
  if (index == 2 || index == 3) margin = 1;
  // if (index == 4 || index == 5 ) margin = 2;
  if (Hours >= 20 || Hours < 6) {
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

void writeDigit_d(int index, int val) { //date
  byte digit = digits[val];
  int margin;
  if (index == 0 || index == 1) margin = 0;
  if (index == 2 || index == 3) margin = 1;
  if (index == 4 || index == 5) margin = 2;
  if (Hours >= 20 || Hours < 6) {
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
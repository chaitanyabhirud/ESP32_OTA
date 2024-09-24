#include <WiFi.h>
// #include <WifiMulti.h>
// #include <WiFiUdp.h>
// #include <NTPClient.h>
// #include <TimeLib.h>
// #include "RTClib.h"
#include <DHT.h>
// #include <Adafruit_Sensor.h>
// #include <cmath>
#include <esp_now.h> //https://dronebotworkshop.com/esp-now/
#include <TinyGPSPlus.h>
#include <SoftwareSerial.h>

static const int RXPin = 16, TXPin = 17;
static const uint32_t GPSBaud = 9600;

TinyGPSPlus gps; // The TinyGPSPlus object
SoftwareSerial ss(RXPin, TXPin); // The serial connection to the GPS device




#define DHT_TYPE DHT11        // DHT Sensor Type
#define DHT_PIN 4             // DHT Sensor pin
#define TIME_FORMAT 12        // 12 = 12 hours format || 24 = 24 hours format


DHT dht(DHT_PIN, DHT_TYPE);
// RTC_DS3231 rtc;


// WiFiMulti wifiMulti;
// WiFiUDP ntpUDP;

// NTPClient timeClient(ntpUDP, "time.nist.gov", 19800, 60000);  //GMT+5:30 : 5*3600+30*60=19800

float Temperature;
int Humidity, Second, Minute, Hour, Year, Month, Day, Hours;

// MAC Address of responder - edit as required
uint8_t broadcastAddress[] = {0x08, 0xD1, 0xF9, 0xD9, 0x4E, 0x38};

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
 
// Peer info
esp_now_peer_info_t peerInfo;
 
// Callback function called when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

void setup() {
  // Set up Serial Monitor
  Serial.begin(115200);
  dht.begin();
  ss.begin(GPSBaud);

  // Set ESP32 as a Wi-Fi Station
  WiFi.mode(WIFI_STA);
 
  // Initilize ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
 
  // Register the send callback
  esp_now_register_send_cb(OnDataSent);
  
  // Register peer
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
  // Add peer        
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
 


  // wifiMulti.addAP("Galaxy A32950C", "vskq8024");
  // wifiMulti.addAP("iPhone", "Simple@18");
  // wifiMulti.addAP("sandip", "nilkanth");
  // wifiMulti.addAP("Sweet home", "Sweethome@123");
  // wifiMulti.addAP("guest-home", "C@mplex#123");
  // wifiMulti.addAP("realmeXT", "1234567890");

  // Serial.print("Connecting to Wifi...");
  // if (wifiMulti.run() != WL_CONNECTED) {
  //   delay(5000);
  //   Serial.print("Waited for 5 seconds to check wifi connection.");
  // }
  // if (wifiMulti.run() == WL_CONNECTED) {
  //   Serial.println("Connected to Wifi...");
  //   Serial.println(WiFi.SSID());
  //   Serial.println(WiFi.localIP());
  // }
  // timeClient.begin();
  // delay(10);

  // if (!rtc.begin()) {
  //   Serial.println("Couldn't find RTC");
  //   while (1);
  // }
  // if (rtc.lostPower()) {
  //   Serial.println("RTC lost power, lets set the time!");
  //   // following line sets the RTC to the date & time this sketch was compiled
  //   rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  // }
}

void loop() {
  // Serial.println("***************************************");
  // Serial.println(WiFi.status());
  // Serial.println("***************************************");
  // if (WiFi.status() == WL_CONNECTED) {
  //   timeClient.update();
  //   unsigned long unix_epoch = timeClient.getEpochTime();  // get UNIX Epoch time
  //   Second = second(unix_epoch);  // get seconds from System
  //   Minute = minute(unix_epoch);  // get minutes from System
  //   Hours = hour(unix_epoch);     // get hours from System
  //   Year = year(unix_epoch);      // get year from System
  //   Month = month(unix_epoch);    // get month from System
  //   Day = day(unix_epoch);        // get day from System

  while (ss.available() > 0)
  if (gps.encode(ss.read()))
  {

    displayInfo();
    // rtc.adjust(DateTime(Year, Month, Day, Hours, Minute, Second));
  }
    if (millis() > 5000 && gps.charsProcessed() < 10)
  {
    Serial.println(F("No GPS detected: check wiring."));
    while(true);
  }
  //  else{
  //   Serial.println("Wifi not found. Reading time from RTC...");
  //   DateTime now = rtc.now();
  //   Second = now.second();  // get seconds from RTC
  //   Minute = now.minute();  // get minutes from RTC
  //   Hours = now.hour();     // get hours from RTC
  //   Year = now.year();      // get year from RTC
  //   Month = now.month();    // get month from RTC
  //   Day = now.day();        // get day from RTC
  // }
   
    Second = gps.time.second();  // get seconds from GPS
    Minute = gps.time.minute();  // get minutes from GPS
    Hours = gps.time.hour();     // get hours from GPS
    Year = gps.date.year();      // get year from GPS
    Month = gps.date.month();    // get month from GPS
    Day = gps.date.day();        // get day from GPS 




  //   // Update RTC with current time from WiFi.
  //   rtc.adjust(DateTime(Year, Month, Day, Hours, Minute, Second));
  // } else if (WiFi.status() != WL_CONNECTED) {
  //   Serial.println("Wifi not found. Reading time from RTC...");
  //   DateTime now = rtc.now();
  //   Second = now.second();  // get seconds from RTC
  //   Minute = now.minute();  // get minutes from RTC
  //   Hours = now.hour();     // get hours from RTC
  //   Year = now.year();      // get year from RTC
  //   Month = now.month();    // get month from RTC
  //   Day = now.day();        // get day from RTC
  // }


  Temperature = dht.readTemperature();
  Humidity = dht.readHumidity();


 

   Minute = Minute + 30;
    if (Minute > 60) {
      Hours = Hours + 6;
      Minute = Minute - 60;
      if (Hours > 24) {
      Hours = Hours - 24;
    }      
    } else {Hours = Hours + 5;
    if (Hours > 24) {
      Hours = Hours - 24;
    }      
    
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

 
  // Send message via ESP-NOW
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));
   
  if (result == ESP_OK) {
    Serial.println("Sending confirmed");
  }
  else {
    Serial.println("Sending error");
  }







  
  // Format structured data
 myData.var_Day = Day;
 myData.var_Month = Month;
 myData.var_Year = Year;
 myData.var_Hour = Hour;
 myData.var_Hours = Hours;
 myData.var_Minute = Minute;
 myData.var_Second = Second;
 myData.var_Humidity = Humidity;
 myData.var_Temperature = Temperature;

 
  Serial.println("*************************************** Time ");
  Serial.println("*************************************** Hours ");
  Serial.println(Hours);
  Serial.println("*************************************** Hour ");
  Serial.println(Hour);
  Serial.println("*************************************** Minute ");
  Serial.println(Minute);
  Serial.println("*************************************** Second ");
  Serial.println(Second);



  Serial.println("*************************************** Day ");
  Serial.println(Day);
  Serial.println("*************************************** Month ");
  Serial.println(Month);
  Serial.println("*************************************** Year ");
  Serial.println(Year);


  Serial.println("*************************************** Temperature ");
  Serial.println(Temperature);
  Serial.println("*************************************** Humidity ");
  Serial.println(Humidity);
  
  // delay(1000);

}

void displayInfo()
{
  Serial.print(F("Location: ")); 
  if (gps.location.isValid())
  {
    Serial.print(gps.location.lat(), 6);
    Serial.print(F(","));
    Serial.print(gps.location.lng(), 6);
  }
  else
  {
    Serial.print(F("INVALID"));
  }

  Serial.print(F("  Date/Time: "));
  if (gps.date.isValid())
  {
    Serial.print(gps.date.month());
    Serial.print(F("/"));
    Serial.print(gps.date.day());
    Serial.print(F("/"));
    Serial.print(gps.date.year());
  }
  else
  {
    Serial.print(F("INVALID"));
  }

  Serial.print(F(" "));
  if (gps.time.isValid())
  {
    if (gps.time.hour() < 10) Serial.print(F("0"));
    Serial.print(gps.time.hour());
    Serial.print(F(":"));
    if (gps.time.minute() < 10) Serial.print(F("0"));
    Serial.print(gps.time.minute());
    Serial.print(F(":"));
    if (gps.time.second() < 10) Serial.print(F("0"));
    Serial.print(gps.time.second());
    Serial.print(F("."));
    if (gps.time.centisecond() < 10) Serial.print(F("0"));
    Serial.print(gps.time.centisecond());
  }
  else
  {
    Serial.print(F("INVALID"));
  }

  Serial.println();
}



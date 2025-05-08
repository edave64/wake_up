#include "./rtc_internal.h"
#include <FastLED.h>

#if defined(ARDUINO_PORTENTA_C33)
#include <WiFiC3.h>
#elif defined(ARDUINO_UNOWIFIR4)
#include <WiFiS3.h>
#endif

//Include the NTP library
#include <NTPClient.h>

#include <WiFiUdp.h>
#include "arduino_secrets.h"

#define NUM_LEDS 150
#define DATA_PIN 6
CRGB leds[NUM_LEDS];

RTCTime initTime;
bool fastMode = false;

char ssid[] = SECRET_SSID; // your network SSID (name)
char pass[] = SECRET_PASS; // your network password (use for WPA, or use as key for WEP)

int wifiStatus = WL_IDLE_STATUS;
WiFiUDP Udp; // A UDP instance to let us send and receive packets over UDP
NTPClient timeClient(Udp);

void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}

void connectToWiFi(){
  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true);
  }

  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.println("Please upgrade the firmware");
  }

  // attempt to connect to WiFi network:
  while (wifiStatus != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    wifiStatus = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(10000);
  }

  Serial.println("Connected to WiFi");
  printWifiStatus();
}

void printNumPad2(uint8_t num) {
  if (num < 10) {
    Serial.print('0');
  }
  Serial.print(num, DEC);
}

void printDateTime(RTCTime dt) {
    Serial.print(dt.getYear(), DEC);
    Serial.print('-');
    printNumPad2((int)dt.getMonth());
    Serial.print('-');
    printNumPad2(dt.getDayOfMonth());
    Serial.print(' ');
    printNumPad2(dt.getHour());
    Serial.print(':');
    printNumPad2(dt.getMinutes());
    Serial.print(':');
    printNumPad2(dt.getSeconds());
    Serial.println();
}

void ntp() {
  connectToWiFi();
  Serial.println("\nStarting connection to server...");
  timeClient.begin();
  timeClient.update();

  // Get the current date and time from an NTP server and convert
  // it to UTC +2 by passing the time zone offset in hours.
  // You may change the time zone offset to your local one.
  auto timeZoneOffsetHours = 2;
  auto unixTime = timeClient.getEpochTime() + (timeZoneOffsetHours * 3600);
  Serial.print("Unix time = ");
  Serial.println(unixTime);
  RTCTime timeToSet = RTCTime(unixTime);
  RTC.setTime(timeToSet);

  // Retrieve the date and time from the RTC and print them
  RTCTime currentTime;
  RTC.getTime(currentTime); 
  Serial.println("The RTC was just set to: " + String(currentTime));
}

void processInput(String input) {
  if (input.startsWith("g")) { // get time
    RTCTime now;
    RTC.getTime(now);
    //DateTime now = rtc.now();
    printDateTime(now);
  } else if (input.startsWith("s")) {
    String hour = input.substring(1, 3);
    String min = input.substring(4, 6);
    String sec = input.substring(7, 9);

    RTCTime now;
    RTC.getTime(now);
    now.setHour(hour.toInt());
    now.setMinute(min.toInt());
    now.setSecond(sec.toInt());
    RTC.setTime(now);
  } else if (input.startsWith("f")) {
    fastMode = !fastMode;
  } else if (input.startsWith("n")) {
    ntp();
  }
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
  
  if (!RTC.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }
  for (int i = 0; i < NUM_LEDS; ++i) {
    leds[i] = 0;
  }
  FastLED.show();
  Serial.println("Start");
  ntp();
  // rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
}

void loop() {
  if (Serial.available()) {
    String line = Serial.readStringUntil('\n');
    processInput(line);
  }

  RTCTime now;
  RTC.getTime(now);

  bool wakingMode = false;

  if ((now.getDayOfWeek() == DayOfWeek::SUNDAY || now.getDayOfWeek() == DayOfWeek::SATURDAY)) {
    wakingMode = now.getHour() == 8;
  } else {
    wakingMode = now.getHour() == 6;
  }

  printDateTime(now);

  if (wakingMode && now.getMinutes() >= 45) {
    int32_t secs = (now.getMinutes() - 45) * 60 + now.getSeconds();

    float fraction = secs / (15.0 * 60.0);
    
    int8_t color = 255 * fraction;

    if (color == 0) {
      color = 1;
    }

    for (int i = 0; i < NUM_LEDS; ++i) {
      leds[i] = (uint32_t)color;
    }

    FastLED.show();
    delay(200);
  } else {
    for (int i = 0; i < NUM_LEDS; ++i) {
      if (leds[i] > 0) {
        leds[i] = (leds[i].as_uint32_t()) - 1;
      }
    }
    FastLED.show();
    if (!fastMode) {
      delay(30000);
    }
  }
}

#if defined(ARDUINO_PORTENTA_C33)
#include <WiFiC3.h>
#elif defined(ARDUINO_UNOWIFIR4)
#include <WiFiS3.h>
#endif

//Include the NTP library
#include <NTPClient.h>

#include <WiFiUdp.h>
#include "arduino_secrets.h"

char ssid[] = SECRET_SSID; // your network SSID (name)
char pass[] = SECRET_PASS; // your network password (use for WPA, or use as key for WEP)

int wifiStatus = WL_IDLE_STATUS;

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

void ntp() {
  connectToWiFi();
  Serial.println("\nStarting connection to server...");

  {
    WiFiUDP Udp; // A UDP instance to let us send and receive packets over UDP
    NTPClient timeClient(Udp);
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

  WiFi.end();
  wifiStatus = WiFi.status();
}

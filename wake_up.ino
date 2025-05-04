#include <RTClib.h>
#include <FastLED.h>

#define NUM_LEDS 150
#define DATA_PIN 6
CRGB leds[NUM_LEDS];

RTC_DS3231 rtc;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
  
  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }

  leds[0] = 0xFF0000;
  leds[1] = 0xFFFF00;
  leds[2] = 0x00FF00;
  leds[3] = 0x00FFFF;
  leds[4] = 0x0000FF;
  leds[5] = 0xFF00FF;
  // rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
}

void printNumPad2(uint8_t num) {
  if (num < 10) {
    Serial.print('0');
  }
  Serial.print(num, DEC);
}

void printDateTime(DateTime dt) {
    Serial.print(dt.year(), DEC);
    Serial.print('-');
    printNumPad2(dt.month());
    Serial.print('-');
    printNumPad2(dt.day());
    Serial.print(' ');
    printNumPad2(dt.hour());
    Serial.print(':');
    printNumPad2(dt.minute());
    Serial.print(':');
    printNumPad2(dt.second());
    Serial.println();
}

void processInput(String input) {
  if (input.startsWith("g")) { // get time
    DateTime now = rtc.now();
    printDateTime(now);
  } else if (input.startsWith("s")) {
    String hour = input.substring(1, 3);
    String min = input.substring(4, 6);
    String sec = input.substring(7, 9);

    DateTime now = rtc.now();
    DateTime newTime(now.year(), now.month(), now.day(), hour.toInt(), min.toInt(), sec.toInt());
    rtc.adjust(newTime);
  }
}

void loop() {
  if (Serial.available()) {
    String line = Serial.readStringUntil("\n");
    processInput(line);
  }
  CRGB tmp = leds[0];
  for (int i = 1; i < NUM_LEDS; ++i) {
    leds[i - 1] = leds[i];
  }
  leds[NUM_LEDS-1] = tmp;
  FastLED.show();
  delay(30);
}

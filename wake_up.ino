#include <RTClib.h>
#include <FastLED.h>

#define NUM_LEDS 150
#define DATA_PIN 6
CRGB leds[NUM_LEDS];

RTC_DS3231 rtc;

DateTime initTime;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
  
  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }
  for (int i = 0; i < NUM_LEDS; ++i) {
    leds[i] = 0;
  }
  FastLED.show();
  Serial.println("Start");
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

  DateTime now = rtc.now();

  bool wakingMode = false;

  if ((now.dayOfTheWeek() == 0 || now.dayOfTheWeek() == 6)) {
    wakingMode = now.hour() == 8;
  } else {
    wakingMode = now.hour() == 6;
  }

  if (wakingMode && now.minute() >= 45) {
    int32_t secs = (now.minute() - 45) * 60 + now.second();

    float fraction = secs / (15.0 * 60.0);
    
    int8_t color = 255 * fraction;

    if (color == 0) {
      color = 1;
    }

    for (int i = 0; i < NUM_LEDS; ++i) {
      leds[i] = color;
    }

    FastLED.show();
    delay(200);
  } else {
    for (int i = 0; i < NUM_LEDS; ++i) {
      if (leds[i] > 0) {
        --leds[i];
      }
    }
    FastLED.show();
    delay(30000);
  }

}

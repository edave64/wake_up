#include "./rtc_internal.h"
#include <FastLED.h>
#include "./wifi.h"

#define NUM_LEDS 150
#define DATA_PIN 6
CRGB leds[NUM_LEDS];

int WakeUpSeconds = 15 * 60;
int FadeOutTimeout = 3000;

RTCTime initTime;
bool fastMode = false;

enum State {
  Waiting,
  FadeIn,
  FadeOut,
  FastSerial
}

currentState = State::Waiting;

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

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.println("setup");
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

RTCTime startTime;

void serialEvent() {
  Serial.println("serialEvent");
  State newState = currentState;
  switch (Serial.read()) {
    case 'g': {
      RTCTime now;
      RTC.getTime(now);
      printDateTime(now);
      return;
    }
    case 's': {
      String input = Serial.readStringUntil('\n');
      String hour = input.substring(1, 3);
      String min = input.substring(4, 6);
      String sec = input.substring(7, 9);

      RTCTime now;
      RTC.getTime(now);
      now.setHour(hour.toInt());
      now.setMinute(min.toInt());
      now.setSecond(sec.toInt());
      RTC.setTime(now);
      return;
    }
    case 'n':
      ntp();
      return;
    case 'f':
      fastMode = !fastMode;
      break;
    case 'w':
      newState = State::FastSerial;
      break;
    case 'i':
      newState = State::FadeIn;
      break;
    case 'o':
      currentState = State::FadeOut;
      // Don't reset LEDs
      return;
  }
  if (newState != currentState) {
    currentState = newState;
    RTC.getTime(startTime);
    for (int i = 0; i < NUM_LEDS; ++i) {
      leds[i] = 0;
    }
  }
}

void loop() {
  if (Serial.available()) {
    serialEvent();
    return;
  }

  RTCTime now;
  RTC.getTime(now);

  switch (currentState) {
    case State::FastSerial:
      return;
    case State::Waiting: {
      bool wakingMode = false;

      RTCTime futureTime(now.getUnixTime() + WakeUpSeconds);

      if ((futureTime.getDayOfWeek() == DayOfWeek::SUNDAY || futureTime.getDayOfWeek() == DayOfWeek::SATURDAY)) {
        wakingMode = futureTime.getHour() == 9 && futureTime.getMinutes() < 10;
      } else {
        wakingMode = futureTime.getHour() == 7 && futureTime.getMinutes() < 10;
      }

      if (wakingMode) {
        currentState = State::FadeIn;
        startTime = now;
        return;
      }

      if (!fastMode) {
        delay(30000);
      }
      return;
    }
    case State::FadeIn: {
      time_t secs = now.getUnixTime() - startTime.getUnixTime();

      if (secs >= WakeUpSeconds) {
        currentState = State::FadeOut;
        startTime = now;
        return;
      }

      double fraction = secs / (double)(WakeUpSeconds);
      fraction = fraction * fraction;
      
      uint8_t shade = 255 * fraction;

      if (shade == 0) {
        shade = 1;
      }

      CRGB color (0,0,shade);
      for (int i = 0; i < NUM_LEDS; ++i) {
        leds[i] = color;
      }

      FastLED.show();
      delay(500);
      return;
    }
    case State::FadeOut: {
      CRGB color = leds[0];
      if (color.red > 0) { color.red--; }
      if (color.blue > 0) { color.blue--; }
      if (color.green > 0) { color.green--; }

      for (int i = 0; i < NUM_LEDS; ++i) {
        leds[i] = color;
      }
      FastLED.show();

      if (color.red == 0 && color.blue == 0 && color.blue == 0) {
        currentState = State::Waiting;
      } else {
        delay(FadeOutTimeout);
      }
    }
  }
}

uint8_t min(uint8_t a, uint8_t b) {
  if (a > b) return b;
  return a;
}

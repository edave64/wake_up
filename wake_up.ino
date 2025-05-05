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
  Serial.println("Start");

  initTime = rtc.now();
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

int i = 0;
int8_t cycle = 0;

void loop() {
  if (Serial.available()) {
    String line = Serial.readStringUntil("\n");
    processInput(line);
  }

  int32_t secs = ((rtc.now() - initTime).totalseconds());

  int32_t color = (secs > 100 ? 100 : secs);
  int8_t offset = (color / 4);
  if (offset <= 0) offset = 1;
  for (i = 0; i < NUM_LEDS; i += 1) {
    int32_t col = (color + (((cycle + i) % 3) - 1) * offset);
    leds[i] = col < 0 ? 0 : col << 16;
  }
  cycle++;
  Serial.println(secs);
  FastLED.show();
  delay(200);
}

#include <RTClib.h>

RTC_DS3231 rtc;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }

  // rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
}

void printNumPad2(uint8_t num) {
  if (num < 10) {
    Serial.print('0');
  }
  Serial.print(num, DEC);
}

void processInput(String input) {
  if (input.startsWith('g')) { // get time
    DateTime now = rtc.now();
    Serial.print(now.year(), DEC);
    Serial.print('-');
    printNumPad2(now.month());
    Serial.print('-');
    printNumPad2(now.day());
    Serial.print(' ');
    printNumPad2(now.hour());
    Serial.print(':');
    printNumPad2(now.minute());
    Serial.print(':');
    printNumPad2(now.second());
    Serial.println();
  } else if (input.startsWith('s')) {
    String hour = input.substr(1, 2);
    String min = input.substr(4, 2);
    String sec = input.substr(7, 2);

    DateTime now = rtc.now();
    hour.toInt();
  }
}

void loop() {
  if (Serial.available()) {
    String line = Serial.readStringUntil("\n");
    processInput(line);
  }
}

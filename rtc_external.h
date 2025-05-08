#include <RTClib.h>

enum class Month : uint8_t {
    JANUARY = 0,
    FEBRUARY,
    MARCH,
    APRIL,
    MAY,
    JUNE,
    JULY,
    AUGUST,
    SEPTEMBER,
    OCTOBER,
    NOVEMBER,
    DECEMBER
};

enum class DayOfWeek : uint8_t {
    MONDAY = 1,
    TUESDAY = 2,
    WEDNESDAY = 3,
    THURSDAY = 4,
    FRIDAY = 5,
    SATURDAY = 6,
    SUNDAY = 0
};

enum class SaveLight : uint8_t {
    SAVING_TIME_INACTIVE = 0,
    SAVING_TIME_ACTIVE
};

class RTCTime;

class RTClock {
  public:
  bool begin () {
    return this->rtc.begin();
  }

  bool getTime(RTCTime &t);
  bool setTime(RTCTime &t);

  private:
  RTC_DS3231 rtc;
};

class RTCTime {
  public:
    RTCTime() {}
    RTCTime(time_t t) {
      DateTime newVal(t);
      this->val = newVal;
    }
    RTCTime(int day, Month m, int year, int hours, int minutes, int seconds, DayOfWeek _dof, SaveLight _sl) {
      DateTime newVal(year, ((int)m) + 1, day, hours, minutes, seconds);
      this->val = newVal;
    }

    /* day from 1 to 31 */
    int getDayOfMonth() const {
      return this->val.month();
    }
    bool setDayOfMonth(int day) {
      DateTime newVal (this->val.year(), this->val.month(), day, this->val.hour(), this->val.minute(), this->val.second());
      this->val = newVal;
    }

    /* the year 1989 or 2022 */
    int getYear() const {
      return this->val.year();
    }
    bool setYear(int year) {
      DateTime newVal (year, this->val.month(), this->val.day(), this->val.hour(), this->val.minute(), this->val.second());
      this->val = newVal;
    }

    /* month from 0 (January) to 11 (December) */
    Month getMonth() const {
      return (Month)(this->val.month() - 1);
    }
    bool setMonthOfYear(Month m) {
      DateTime newVal (this->val.year(), ((int)m) + 1, this->val.day(), this->val.hour(), this->val.minute(), this->val.second());
      this->val = newVal;
    }

    /* from 0 (midnight) to 23 */
    int getHour() const {
      return this->val.hour();
    }
    bool setHour(int hour) {
      DateTime newVal (this->val.year(), this->val.month(), this->val.day(), hour, this->val.minute(), this->val.second());
      this->val = newVal;
    }
    int getMinutes() const {
      return this->val.minute();
    }

    bool setMinute(int minute) {
      DateTime newVal (this->val.year(), this->val.month(), this->val.day(), this->val.hour(), minute, this->val.second());
      this->val = newVal;
    }

    int getSeconds() const {
      return this->val.second();
    }
    bool setSecond(int second) {
      DateTime newVal (this->val.year(), this->val.month(), this->val.day(), this->val.hour(), this->val.minute(), second);
      this->val = newVal;
    }

    time_t getUnixTime() const {
      return this->val.unixtime();
    }

    bool setUnixTime(time_t time) {
      DateTime newVal(time);
      this->val = newVal;
    }

    DayOfWeek getDayOfWeek() const {
      return (DayOfWeek)(this->val.dayOfTheWeek() + 1);
    }

  private:
  DateTime val;

  friend RTClock;
};

bool RTClock::getTime(RTCTime &t) {
    t.val = this->rtc.now();
    return true;
}

bool RTClock::setTime(RTCTime &t) {
  this->rtc.adjust(t.val);
  return true;
}

extern RTClock RTC;
RTClock RTC;

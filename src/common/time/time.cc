#include "common/time/time.h"

#include <DS3232RTC.h>

#include <array>
#include <memory>
#include <time.h>
#include <RTClib.h>

namespace common {

Time Time::last_sync_;

//constexpr uint32_t kSyncTimeThreshold = 10000;

constexpr uint32_t kSecondsFrom1970To2000 = 946684800;

enum TimingSourceEnumerate : size_t {
  TimingSourceEnumerate_ARDUINO_TIME = 0,
  TimingSourceEnumerate_INTERNET_TIME = 1,
  TimingSourceEnumerate_DS3232RTC = 2,
  SIZE = 3,
};

struct TimeSource {
  virtual Time Now() = 0;
  virtual Time Sync(const Time&) = 0;
};

Time& GetArduinoTime() {
  static Time t = Time::FromSec(0, 0);
  return t;
}

void SyncAll() {

}

DS3232RTC& GetDS3232RTC() {
  static DS3232RTC rtc;
  return rtc;
}

bool Time::Init() {
  GetDS3232RTC().begin();
  setSyncProvider(GetDS3232RTC().get);
  return timeStatus() == timeSet;
}

void Time::SyncSysTime(uint32_t time_sec, uint32_t time_nsec) {
  GetDS3232RTC().set(time_sec);
}

Time Time::Now() {
  return Time::FromSec(GetDS3232RTC().get());
}

std::string Time::ToString(Time::TimeOption opt) const {
  char buf[sizeof "YYYY-MM-DDThh:mm:ss"];
  uint32_t time_from_2000 = sec_ - kSecondsFrom1970To2000;
  strftime(buf, sizeof buf,
           "%FT%T", gmtime(&time_from_2000));

  switch (opt) {
    case Time::TimeOption::TIMESTAMP_ISO: {
      return std::string(buf);
    }
    case Time::TimeOption::TIMESTAMP_BASIC: {
      return std::string(buf + sizeof "YYYY" - 1);
    }
  }
  return std::string();
}

}  // namespace common

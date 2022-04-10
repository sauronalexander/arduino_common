#include "common/time/time.h"

#include <DS3232RTC.h>

#include <array>
#include <memory>
#include <time.h>

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
std::string Time::ToString() const {
  char buf[sizeof "0000-00-00T00:00:00"];
  uint32_t time_from_2000 = sec_ - kSecondsFrom1970To2000;
  strftime(buf, sizeof buf,
           "%FT%T", gmtime(&time_from_2000));
  return std::string(buf);
}

}  // namespace common

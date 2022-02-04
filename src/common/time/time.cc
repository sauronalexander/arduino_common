#include "common/time/time.h"

#include <array>
#include <memory>
#include <utility>

namespace common {

Time Time::last_sync_;

//constexpr uint32_t kSyncTimeThreshold = 10000;

enum TimingSourceEnumerate : size_t {
  ARDUINO_TIME = 0,
  INTERNET_TIME = 1,
  DS1307 = 2,
  TimingSourceEnumerateSize = 3,
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

RTC_DS1307& Get() {
  static RTC_DS1307 rtc;
  return rtc;
}

bool Time::Init() {
  if (!Get().begin()) {
    return false;
  }
  return true;
}

void Time::SyncSysTime(uint32_t time_sec, uint32_t time_nsec) {
//  auto abs_diff = [](uint32_t a, uint32_t b) {
//    return a > b ? a - b : b - a;
//  };
  //  && abs_diff(Now().Sec(), time) < kSyncTimeThreshold
  Get().adjust(DateTime(time_sec));
  last_sync_ = FromSec(time_sec, time_nsec);
}

Time Time::Now() {
  return Time::FromSec(Get().now().unixtime());
}

}  // namespace common

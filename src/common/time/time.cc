#include "time.h"

#include <array>
#include <memory>
#include <utility>

namespace common {

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

//auto TimingSourceStore() ->
//std::array<std::auto_ptr<TimeSource>, TimingSourceEnumerate::TimingSourceEnumerateSize>& {
//  static std::array<std::auto_ptr<TimeSource>,
//                    TimingSourceEnumerate::TimingSourceEnumerateSize> sources;
//  return sources;
//}

struct DS1307 final : public TimeSource {
  DS1307() = default;
  Time Now() override {
    return Time::FromSec(rtc.now().unixtime());
  }

  Time Sync(const Time& other) override {
    rtc.adjust(DateTime(other.Sec()));
    return other;
  }
  RTC_DS1307 rtc;
};

struct InternetTime final : public TimeSource {
  Time Now() override {
    return Time::FromSec(0);
  }
  Time Sync(const Time& other) override {
    return Now();
  }
};

//struct ArduinoTime final : public TimeSource {
//  Time Now() override {
//    return base_time +
//  }
//
//  Time base_time;
//}

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

bool Time::Init(uint32_t adjust_sec) {
  if (!Get().begin()) {
    return false;
  }
  if (!Get().isrunning() || adjust_sec) {
    Get().adjust(DateTime(adjust_sec));
  }
  return true;
}

void Time::SyncSysTime(uint32_t adjust_sec) {

//  GetArduinoTime() += Time::FromSec();
}

Time Time::Now() {
  return Time::FromSec(Get().now().unixtime());
}

}  // namespace common

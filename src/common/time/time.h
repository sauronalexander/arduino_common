#pragma once

#include <stdint.h>
#include <time.h>

#include <avr/pgmspace.h>

#include "common/stl/string.h"

namespace common {

class Time {
public:
  Time() = default;
  Time(const Time&) = default;
  Time(Time&&) = default;
  Time& operator=(const Time&) = default;
  Time& operator=(Time&&) = default;

  static PROGMEM constexpr uint32_t kNsToMs = 1E6;
  static PROGMEM constexpr uint64_t kNsToS = 1E9;
  static PROGMEM constexpr uint32_t kMsToS = 1E3;

  static Time Now();
  static std::auto_ptr<int16_t> GetRTCTemperature();

  static bool Init();
  static void SyncSysTime(uint32_t time_sec, uint32_t time_nsec = 0);

  static Time FromNSec(uint64_t nsec) {
    return Time(nsec / kNsToS, nsec % kNsToS);
  }

  static Time FromSec(uint32_t sec, uint32_t nsec = 0) {
    return Time(sec, nsec);
  }

  inline uint32_t Sec() const {
    return sec_;
  }

  inline uint64_t MSec() const {
    uint32_t fra_part = nsec_ / kMsToS;
    return sec_ * kMsToS + fra_part;
  }

  inline uint64_t NSec() const {
    return static_cast<uint64_t>(sec_) * kNsToS + static_cast<uint64_t>(nsec_);
  }

  enum TimeOption {
    TIMESTAMP_BASIC, //!< `-03-23 01:03:52`
    TIMESTAMP_ISO, //!< `YYYY-MM-DDThh:mm:ss`
    TIMESTAMP_MS, //!< `123456789000`
  };

  std::string ToString(TimeOption opt = TimeOption::TIMESTAMP_BASIC) const;

  Time& operator+=(const Time& other) {
    this->sec_ += other.sec_;
    this->nsec_ += other.nsec_;
    if (this->nsec_ >= kNsToS) {
      this->nsec_ -= kNsToS;
      this->sec_ += 1;
    }
    return *this;
  }

  Time operator+(const Time& other) const {
    Time t(*this);
    t += other;
    return t;
  }

  Time& operator-=(const Time& other) {
    this->sec_ -= other.sec_;
    if (this->nsec_ < other.nsec_) {
      this->sec_ -= 1;
      this->nsec_ += kNsToS;
    }
    this->nsec_ -= other.nsec_;
    return *this;
  }

  Time operator-(const Time& other) const {
    Time t(*this);
    t -= other;
    return t;
  }

  bool operator<(const Time& other) const {
    return this->sec_ == other.sec_ ? this->nsec_ < other.nsec_ :
                                      this->sec_ < other.sec_;
  }

  bool operator<=(const Time& other) const {
    return (*this < other) || (*this == other);
  }

  bool operator>(const Time& other) const {
    return !(*this <= other);
  }

  bool operator>=(const Time& other) const {
    return !(*this < other);
  }

  bool operator==(const Time& other) const {
    return this->sec_ == other.sec_ && this->nsec_ == other.nsec_;
  }

  bool operator!=(const Time& other) const {
    return !(*this == other);
  }

private:
  Time(uint32_t sec, uint32_t nsec) : sec_{sec}, nsec_{nsec} {}
  uint32_t sec_{0};
  uint32_t nsec_{0};

  void SyncAll();
  static Time last_sync_;
};

}  // namespace common

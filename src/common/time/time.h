#pragma once

#include <stdint.h>
#include <time.h>
#include <RTClib.h>

#include "common/stl/string.h"

namespace common {

class Time {
public:
  Time() = default;
  Time(const Time&) = default;
  Time(Time&&) = default;
  Time& operator=(const Time&) = default;
  Time& operator=(Time&&) = default;

  static constexpr uint32_t kNsToMs = 1E6;
  static constexpr uint64_t kNsToS = 1E9;
  static constexpr uint32_t kMsToS = 1E3;

  static Time Now();

  static bool Init(uint32_t adjust_sec = 0);
  static void SyncSysTime(uint32_t adjust_sec);

  static Time FromNSec(uint64_t nsec) {
    return Time(nsec / kNsToS, nsec % kNsToS);
  }

  static Time FromSec(uint32_t sec, uint32_t nsec = 0) {
    return Time(sec, nsec);
  }

  inline uint32_t Sec() const {
    return sec_;
  }

  inline int64_t MSec() const {
    uint32_t fra_part = nsec_ / kMsToS;
    return sec_ * kMsToS + fra_part;
  }

  inline uint64_t NSec() const {
    return static_cast<uint64_t>(sec_) * kNsToS + static_cast<uint64_t>(nsec_);
  }

  inline std::string ToString(
      DateTime::timestampOpt opt
          = DateTime::timestampOpt::TIMESTAMP_FULL) const {
    return std::string(ToDateTime().timestamp(opt).c_str());
  }

  inline DateTime ToDateTime() const {
    return DateTime(sec_ / kNsToS);
  }
  
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

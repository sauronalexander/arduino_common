#pragma once

#include <LiquidCrystal.h>

#include "common/stream_handler/stream_handler.h"
#include "common/type_traits/type_traits.h"

namespace common {

template <uint8_t Col_, uint8_t Row_, uint8_t DotSize_ = LCD_5x8DOTS>
class LCDHandler final : public StreamHandler {
public:
  static constexpr uint8_t Row = Row_;
  static constexpr uint8_t Col = Col_;
  static constexpr uint8_t DotSize = DotSize_;

  LCDHandler() = delete;

  LCDHandler(uint8_t rs, uint8_t enable, uint8_t d0, uint8_t d1, uint8_t d2,
             uint8_t d3)
      : lcd_{rs, enable, d0, d1, d2, d3} {
    lcd_.begin(Col, Row, DotSize);
  }

  template <typename T, typename = decltype(&T::ToString)>
  inline void Log(const T& msg) {
    Log(msg.ToString());
  }

  template <typename T,
            typename = std::enable_if_t<!std::is_same<T, std::string>::value>,
            typename = decltype(std::to_string(std::declval<T>()))>
  inline void Log(const T &msg) {
    Log(std::to_string(msg));
  }

  inline void Close() {
    lcd_.clear();
    cursor_ = 0;
    lcd_.noDisplay();
  }

  inline void Clear() {
    lcd_.clear();
    cursor_ = 0;
  }

  void Log(const std::string &msg) override {
    // TODO: Check length of msg and scroll
    if (requires_clear_) {
      Clear();
    }
    lcd_.print(msg.c_str());
    requires_clear_ = true;
  }

  void Log(const char* const &msg) {
    if (requires_clear_) {
      Clear();
    }
    lcd_.print(msg);
    requires_clear_ = true;
  }

  template <typename T, typename... Args,
            typename = arx::stdx::enable_if_t<
                common::type_traits::VariadicTypeTraits<Args...>::N < Row>>
  void Log(const T &msg1, const Args &...msgs) {
    Clear();
    requires_clear_ = false;
    Log(msg1);
    lcd_.setCursor(0, ++cursor_);
    requires_clear_ = false;
    Log(msgs...);
    requires_clear_ = true;
  }

  void Log(const Event &msg) override {
    std::string metadata = ToString(msg.level);
    Log(ToString(msg.level).substr(0, 1) + " " +
        std::to_string(msg.error_code) + " " +
        msg.source_name,
        msg.event_msg);
  }

  void LogStructured(const SensorReading &msg) override {
    Log(msg.sensor_id, std::to_string(msg.reading) + msg.unit);
  }

private:
  LiquidCrystal lcd_;
  uint8_t cursor_{0};
  bool requires_clear_;
};

} // namespace common

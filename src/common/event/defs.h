#pragma once

#include <inttypes.h>
#include <ArduinoJson.h>

#include "common/stl/string.h"
#include "common/time/time.h"

namespace common {

enum LogLevel : int8_t {
  LOGLEVEL_DEBUG = -1,
  LOGLEVEL_INFO = 0,
  LOGLEVEL_WARN = 1,
  LOGLEVEL_ERROR = 2,
  LOGLEVEL_FATAL = 3,
  LOGLEVEL_SIZE = 5,
};

enum Error : uint8_t {
  ERROR_NORMAL = 0,
  ERROR_WARN = 1,
  ERROR_ERROR = 2,
  ERROR_SIZE = 3,
};

struct Event {
  Time time{Time::FromSec(0)};

  LogLevel level{LOGLEVEL_INFO};
  uint8_t error_code{0};
  std::string source_name{""};

  std::string event_msg{""};

  void Encode(std::string& msg) const;
  void Decode(const std::string& msg);
  StaticJsonDocument<256> ToJson() const;
};

struct SensorReading {
  Time time{Time::FromSec(0)};

  std::string sensor_id{""};
  std::string sensor_type{""};

  double reading{0.0};

  void Encode(std::string &msg) const;
  void Decode(const std::string& msg);
  StaticJsonDocument<128> ToJson() const;
};

}  // namespace common

#pragma once

#include <inttypes.h>
#include <string>

#include "common/time/time.h"

namespace common {

enum LogLevel : uint8_t {
  LOGLEVEL_DEBUG = 0,
  LOGLEVEL_INFO = 1,
  LOGLEVEL_WARN = 2,
  LOGLEVEL_ERROR = 3,
  LOGLEVEL_FATAL = 4,
  LOGLEVEL_SIZE = 5,
};

enum Error : uint8_t {
  ERROR_NORMAL = 0,
  ERROR_WARN = 1,
  ERROR_ERROR = 2,
  ERROR_SIZE = 3,
};

struct Event {
  Error error{Error::ERROR_NORMAL};
  LogLevel level{LOGLEVEL_INFO};
  uint16_t error_code;
  std::string event_msg;
  Time time{Time::FromSec(0)};

  void Encode(std::string& msg) const;
  void Decode(const std::string& msg);
  std::string ToJson(uint8_t indent = 0);
};

}  // namespace common

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
  uint16_t error_code;
  std::string msg;
  Time t{Time::FromSec(0)};
};

}  // namespace common

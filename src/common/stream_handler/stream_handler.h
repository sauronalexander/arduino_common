#pragma once

#include <Arduino.h>
#include "common/stl/string.h"
#include "common/event/defs.h"

namespace common {

class StreamHandler {
public:
  virtual void Log(const std::string&) {};
  virtual void Log(const Event&) {};
  virtual void LogStructured(const SensorReading&) {};
  virtual ~StreamHandler() = default;

protected:
  std::string GenerateTextLogFromEvent(const Event &msg) {
    return msg.time.ToString(Time::TimeOption::TIMESTAMP_ISO) + "    "
               + ToString(msg.level) + "    "
               + std::to_string(msg.error_code) + "    "
               + msg.event_msg;
  }
};

}  // namespace common

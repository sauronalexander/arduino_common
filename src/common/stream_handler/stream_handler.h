#pragma once

#include <Arduino.h>
#include "common/stl/string.h"

namespace common {

class StreamHandler {
public:
  virtual void Log(const std::string&) = 0;
};

}  // namespace common

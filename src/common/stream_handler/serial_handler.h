#pragma once

#include "common/stream_handler/stream_handler.h"

namespace common {

class SerialHandler : public StreamHandler {
public:
  SerialHandler() = default;
  ~SerialHandler() = default;

  void Log(const std::string&) override;
  void Log(const Event&) override;
  void LogStructured(const SensorReading&) override;

};

}  // namespace common

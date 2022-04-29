#pragma once

#include "common/device/device.h"

namespace common::device {

class CoolingFan final : public Executor  {
public:
  CoolingFan(const std::string &id, uint8_t pin);

  std::string GetExecutorType() const override;
  Event GenerateExecutorEvent() const override;
  void SendCommand(const DeviceDataType &cmd) override;
  bool IsActive() const override;
private:
  uint8_t pin_;
  static PROGMEM const char *const kExecutorType;
  double cmd_{0.0};
};

}  // namespace common::device

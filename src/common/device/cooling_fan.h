#pragma once

#include "common/device/device.h"

namespace common::device {

class CoolingFan final : public Executor {
public:
  CoolingFan(const std::string &id, uint8_t pin, const float *const data,
             size_t N);

  std::string GetExecutorType() const override;

  Event GenerateExecutorEvent() const override;

  void SendCommand(const DeviceDataType &cmd) override;

  bool IsActive() const override;

private:
  uint8_t pin_;
  static PROGMEM const char *const kExecutorType;
  static PROGMEM constexpr double kMaxVoltage{12};
  static PROGMEM constexpr double kMaxDigitalOutput{255};
  double cmd_{0.0};
  const float *const sys_id_data_;
  size_t sys_id_data_len_;
};

} // namespace common::device

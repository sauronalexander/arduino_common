#pragma once

#include "common/device/device.h"

class BH1750;

namespace common::device {

class GY302 final : public Sensor {
public:
  explicit GY302(const std::string &id);
  ~GY302();

  bool UpdateReading() override;
  std::string GetDataType(uint8_t datatype_idx = 0) const override;
  bool IsValid(uint8_t datatype_idx = 0) const override;
  std::string GetUnit(uint8_t datatype_idx = 0) const override;
  DeviceDataType GetReading(uint8_t datatype_idx = 0) const override;
  std::string GetSensorType() const override;

private:
  static PROGMEM const char *const kSensorType;
  static PROGMEM const char *const kUnitName;
  static PROGMEM const char *const kDataType;
  static PROGMEM const uint32_t kMeasureTimeLimit;

  BH1750 *device_;
  double data_{NAN};
};

} // namespace common::device

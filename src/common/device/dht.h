#pragma once

#include "common/device/device.h"

class DHT_nonblocking;

namespace common::device {

class DHT22 final : public Sensor {
public:
  DHT22(const std::string &id, uint8_t pin);
  ~DHT22();
  enum DHT22SensorType : uint8_t {
    TEMPERATURE = 0,
    HUMIDITY = 1,
    SIZE = 2,
  };

  bool UpdateReading() override;
  std::string GetDataType(uint8_t datatype_idx = 0) const override;
  bool IsValid(uint8_t datatype_idx = 0) const override;
  SensorReading GenerateSensorReading(uint8_t datatype_idx = 0) const override;
  DataType GetReading(uint8_t datatype_idx = 0) const override;
  std::string GetSensorType() const override;

private:
  static PROGMEM const char *const kSensorType;
  static PROGMEM const char *const kUnitName[];
  static PROGMEM const char *const kDataType[];
  static PROGMEM const uint32_t kMeasureTimeLimit;

  std::vector<float> data_{NAN, NAN};
  DHT_nonblocking* device_{nullptr};
};

}  // namespace common::device

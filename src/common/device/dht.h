#pragma once

#include "common/device/device.h"

class DHT_nonblocking;

namespace common::device {

class DHT22 final : public Sensor {
public:
  DHT22(const std::string &id, uint8_t pin);
  ~DHT22();
  enum DataType : uint8_t {
    TEMPERATURE = 0,
    HUMIDITY = 1,
    SIZE = 2,
  };

  bool UpdateReading() override;
  const std::string &GetDataType(uint8_t datatype_idx = 0) const override;
  bool IsValid(uint8_t datatype_idx = 0) const override;
  SensorReading GenerateSensorReading(uint8_t datatype_idx = 0) const override;
  double GetReading(uint8_t datatype_idx = 0) const override;

private:
  static const std::string kSensorType;
  static const std::vector<std::string> kUnitName;
  static const std::vector<std::string> kDataType;
  static const uint32_t kMeasureTimeLimit;

  std::vector<float> data_{NAN, NAN};
  DHT_nonblocking* device_{nullptr};
};

}  // namespace common::device

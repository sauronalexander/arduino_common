#pragma once

#include <cmath>
#include <type_traits>

#include "common/event/defs.h"
#include "common/stl/string.h"

namespace common::device {

class Sensor {
public:
  Sensor() = delete;
  Sensor( const std::string &id) : id_{id} {}
  Sensor(const Sensor &) = delete;
  Sensor &operator=(const Sensor &) = delete;
  Sensor(Sensor &&) = default;
  Sensor &operator=(Sensor &&) = default;
  virtual ~Sensor() = default;

  virtual bool UpdateReading() = 0;
  virtual std::string GetDataType(uint8_t datatype_idx = 0) const = 0;
  virtual bool IsValid(uint8_t datatype_idx = 0) const = 0;
  virtual SensorReading GenerateSensorReading(
      uint8_t datatype_idx = 0) const = 0;
  virtual double GetReading(uint8_t datatype_idx = 0) const = 0;
  virtual std::string GetSensorType() const = 0;

  inline const std::string &GetId() const {
    return id_;
  }

  inline common::Time GetTime() const {
    return t_;
  }

protected:
  static constexpr uint8_t kWaitTime{5};

  common::Time t_{common::Time::FromSec(0)};
  std::string id_{""};
};

}  // namespace common::device

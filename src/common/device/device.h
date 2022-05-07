#pragma once

#include <cmath>
#include <type_traits>

#include "common/event/defs.h"
#include "common/stl/string.h"
#include "common/utility/Variant.h"

namespace common::device {

// For input
class Sensor {
public:
  Sensor() = delete;
  explicit Sensor(const std::string &id) : id_{id} {}
  Sensor(const Sensor &) = delete;
  Sensor &operator=(const Sensor &) = delete;
  Sensor(Sensor &&) = default;
  Sensor &operator=(Sensor &&) = default;
  virtual ~Sensor() = default;

  virtual bool UpdateReading() = 0;
  virtual std::string GetDataType(uint8_t datatype_idx = 0) const = 0;
  virtual bool IsValid(uint8_t datatype_idx = 0) const = 0;
  virtual std::string GetUnit(uint8_t datatype_idx = 0) const = 0;
  virtual DeviceDataType GetReading(uint8_t datatype_idx = 0) const = 0;
  virtual std::string GetSensorType() const = 0;

  SensorReading GenerateSensorReading(uint8_t datatype_idx = 0) const {
    SensorReading reading;
    reading.time = t_;
    reading.sensor_id = id_;
    reading.sensor_type = GetSensorType();
    reading.reading = GetReading(datatype_idx);
    reading.data_type = GetDataType();
    reading.unit = GetUnit(datatype_idx);
    return reading;
  }

  inline const std::string &GetId() const { return id_; }

  inline common::Time GetTime() const { return t_; }

  inline virtual void Clear() { t_ = common::Time::FromSec(0); }

protected:
  static constexpr uint8_t kWaitTime{5};
  common::Time t_{common::Time::FromSec(0)};
  std::string id_{""};
};

class Executor {
public:
  Executor() = delete;
  Executor(const std::string &id) : id_{id} {}
  Executor(const Executor &) = delete;
  Executor &operator=(const Executor &) = delete;
  Executor(Executor &&) = default;
  Executor &operator=(Executor &&) = default;
  virtual ~Executor() = default;

  inline const std::string &GetId() const { return id_; }

  inline common::Time GetTime() const { return t_; }

  inline virtual void Clear() { t_ = common::Time::FromSec(0); }

  virtual std::string GetExecutorType() const = 0;
  virtual Event GenerateExecutorEvent() const = 0;
  virtual void SendCommand(const DeviceDataType &cmd) = 0;
  virtual bool IsActive() const = 0;

protected:
  common::Time t_{common::Time::FromSec(0)};
  std::string id_{""};
};

} // namespace common::device

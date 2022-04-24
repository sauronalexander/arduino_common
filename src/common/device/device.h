#pragma once

#include <cmath>
#include <type_traits>

#include "common/event/defs.h"
#include "common/stl/string.h"
#include "common/utility/Variant.h"

namespace common::device {

using DataType = common::Variant<double, int>;

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
  virtual SensorReading
  GenerateSensorReading(uint8_t datatype_idx = 0) const = 0;

  virtual DataType GetReading(uint8_t datatype_idx = 0) const = 0;
  virtual std::string GetSensorType() const = 0;

  inline const std::string &GetId() const { return id_; }

  inline common::Time GetTime() const { return t_; }

  inline virtual void Clear() { t_ = common::Time::FromSec(0); }

protected:
  static constexpr uint8_t kWaitTime{5};
  common::Time t_{common::Time::FromSec(0)};
  std::string id_{""};
};

class Actor {
public:
  Actor() = delete;
  Actor(const std::string &id) : id_{id} {}
  Actor(const Actor &) = delete;
  Actor &operator=(const Actor &) = delete;
  Actor(Actor &&) = default;
  Actor &operator=(Actor &&) = default;
  virtual ~Actor() = default;

  inline const std::string &GetId() const { return id_; }

  inline common::Time GetTime() const { return t_; }

  inline virtual void Clear() { t_ = common::Time::FromSec(0); }

  virtual std::string GetActorType() const = 0;
  virtual Event GenerateActorEvent() const = 0;
  virtual void SendCommand(const DataType &cmd) = 0;
  virtual bool IsActive() const = 0;

protected:
  common::Time t_{common::Time::FromSec(0)};
  std::string id_{""};
};

} // namespace common::device

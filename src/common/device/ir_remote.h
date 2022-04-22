#pragma once

#include "common/device/device.h"

#include "IRL_NecAPI.h"

namespace common::device {

template <uint8_t _PIN>
class IRNecRemote final : public Sensor {
public:
  static PROGMEM constexpr uint8_t PIN = _PIN;
  IRNecRemote(const std::string &id);
  ~IRNecRemote() = default;

  bool UpdateReading() override;
  std::string GetDataType(uint8_t datatype_idx = 0) const override;
  bool IsValid(uint8_t datatype_idx = 0) const override;
  std::string GetSensorType() const override;
  SensorReading GenerateSensorReading(uint8_t datatype_idx = 0) const override;
  ReadingType GetReading(uint8_t datatype_idx = 0) const override;
  void Clear() override;

private:
  static PROGMEM const char *const kSensorType;
  static PROGMEM const char *const kDataType;
  static void Callback();

  static CNecAPI<Callback> device_;
  static int command_;
  static bool ready_;
  bool initialized_{false};
};

template <uint8_t _PIN>
PROGMEM const char *const IRNecRemote<_PIN>::kSensorType = "remote_control";
template <uint8_t _PIN>
PROGMEM const char *const IRNecRemote<_PIN>::kDataType = "remote_control_cmd";

template <uint8_t _PIN>
int IRNecRemote<_PIN>::command_ = -1;
template <uint8_t _PIN>
bool IRNecRemote<_PIN>::ready_ = false;
template <uint8_t _PIN>
CNecAPI<IRNecRemote<_PIN>::Callback> IRNecRemote<_PIN>::device_;

template <uint8_t _PIN>
void IRNecRemote<_PIN>::Callback() {
  if (device_.released()) {
    return;
  }
  ready_ = true;
  command_ = device_.command();
}

template <uint8_t _PIN>
IRNecRemote<_PIN>::IRNecRemote(const std::string &id) : Sensor{id} {
  initialized_ = device_.begin(_PIN);
}

template <uint8_t _PIN>
bool IRNecRemote<_PIN>::UpdateReading() {
  if (!initialized_) {
    return false;
  }
  while (device_.available()) {
    device_.read();
  }
  if (ready_) {
    t_ = common::Time::Now();
  }
  return true;
}

template <uint8_t _PIN>
std::string IRNecRemote<_PIN>::GetSensorType() const {
  return kSensorType;
}

template <uint8_t _PIN>
std::string IRNecRemote<_PIN>::GetDataType(uint8_t datatype_idx) const {
  return std::string(kDataType);
}

template <uint8_t _PIN>
bool IRNecRemote<_PIN>::IsValid(uint8_t datatype_idx) const {
  return ready_;
}

template <uint8_t _PIN>
Sensor::ReadingType IRNecRemote<_PIN>::GetReading(uint8_t datatype_idx) const {
  return ReadingType(command_);
}

template <uint8_t _PIN>
SensorReading IRNecRemote<_PIN>::GenerateSensorReading(
    uint8_t datatype_idx) const {
  SensorReading reading;
  reading.time = t_;
  reading.sensor_id = id_;
  reading.sensor_type = kSensorType;
  reading.reading = command_;
  reading.unit = "";
  return reading;
}

template <uint8_t _PIN>
void IRNecRemote<_PIN>::Clear() {
  Sensor::Clear();
  ready_ = false;
  command_ = -1;
}

}  // namespace common::device

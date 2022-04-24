#include "common/device/dht.h"

#include <DHT_nonblocking.h>

namespace common::device {

PROGMEM const char *const DHT22::kSensorType = "DHT22";
PROGMEM const char *const DHT22::kUnitName[] = {"°C", "%"};
PROGMEM const char *const DHT22::kDataType[] = {
    "temperature", "humidity"};
PROGMEM const uint32_t DHT22::kMeasureTimeLimit = 2;

DHT22::DHT22(const std::string &id, uint8_t pin) : Sensor{id} {
  device_ = new DHT_nonblocking(pin, DHT_TYPE_22);
}

bool DHT22::UpdateReading() {
  if ((common::Time::Now() - t_).Sec() < kMeasureTimeLimit) {
    return false;
  }
  while (!device_->measure(&data_[0], &data_[1])) {
    t_ = common::Time::Now();
    delay(kWaitTime);
  }
  return true;
}

std::string DHT22::GetDataType(uint8_t datatype) const {
  const char* p = reinterpret_cast<const char*>(
      pgm_read_ptr(kDataType + datatype));
  return p;
}

bool DHT22::IsValid(uint8_t datatype_idx) const {
  return !isnan(data_[datatype_idx]) && t_.Sec() > 0;
}

DataType DHT22::GetReading(uint8_t datatype_idx) const {
  return DataType(double(data_[datatype_idx]));
}

std::string DHT22::GetSensorType() const {
  return kSensorType;
}

SensorReading DHT22::GenerateSensorReading(uint8_t datatype_idx) const {
  SensorReading reading;
  reading.time = t_;
  reading.sensor_id = id_;
  reading.sensor_type = GetSensorType();
  reading.reading = data_[datatype_idx];
  reading.data_type = GetDataType(datatype_idx);
  const char* p = reinterpret_cast<const char*>(
      pgm_read_ptr(kUnitName + datatype_idx));
  reading.unit = p + (!datatype_idx);  // A hack for wide char °
  return reading;
}

DHT22::~DHT22() {
  if (device_) {
    delete device_;
    device_ = nullptr;
  }
}

}  // namespace common::device

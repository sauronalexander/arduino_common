#include "common/device/dht.h"

#include <DHT_nonblocking.h>

namespace common::device {

const std::string DHT22::kSensorType = "DHT22";
const std::vector<std::string> DHT22::kUnitName = {"°C", "%"};
const std::vector<std::string> DHT22::kDataType = {"temperature", "humidity"};
const uint32_t kMeasureTimeLimit = 2;

DHT22::DHT22(const std::string &id, uint8_t pin)
    : Sensor{kSensorType, id} {
  device_ = new DHT_nonblocking(pin, DHT_TYPE_22);
}

bool DHT22::UpdateReading() {
  if ((common::Time::Now() - t_).Sec() < 2) {
    return false;
  }
  while (!device_->measure(&data_[0], &data_[1])) {
    t_ = common::Time::Now();
    delay(kWaitTime);
  }
  return true;
}

const std::string &DHT22::GetDataType(uint8_t datatype) const {
  return kDataType.at(datatype);
}

bool DHT22::IsValid(uint8_t datatype_idx) const {
  return !isnan(data_[datatype_idx]) && t_.Sec() > 0;
}

double DHT22::GetReading(uint8_t datatype_idx) const {
  return data_[datatype_idx];
}

SensorReading DHT22::GenerateSensorReading(uint8_t datatype_idx) const {
  SensorReading reading;
  reading.time = t_;
  reading.sensor_id = id_;
  reading.sensor_type = kSensorType;
  reading.reading = data_[datatype_idx];
  reading.unit = kUnitName[datatype_idx];
  return reading;
}

DHT22::~DHT22() {
  delete device_;
}

}  // namespace common::device

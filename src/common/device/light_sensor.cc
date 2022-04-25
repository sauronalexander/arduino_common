#include "common/device/light_sensor.h"

#include <BH1750.h>

#include "common/com/com.h"

namespace common::device {

PROGMEM const char *const GY302::kSensorType = "GY302";
PROGMEM const char *const GY302::kUnitName = "lx";
PROGMEM const char *const GY302::kDataType = "light intensity";
PROGMEM const uint32_t GY302::kMeasureTimeLimit = 1;

GY302::GY302(const std::string &id) : Sensor{id} {
  device_ = new BH1750;
  ::common::com::Com::MasterInit();
  device_->begin();
}

GY302::~GY302() {
  if (device_) {
    delete device_;
    device_ = nullptr;
  }
}

bool GY302::UpdateReading() {
  if ((common::Time::Now() - t_).Sec() < kMeasureTimeLimit) {
    return false;
  }
  data_ = device_->readLightLevel();
  t_ = common::Time::Now();
  return data_ >= 0.0;
}

std::string GY302::GetDataType(uint8_t) const {
  return kDataType;
}

bool GY302::IsValid(uint8_t) const {
  return !isnan(data_) && t_.Sec() > 0 && data_ >= 0.0;
}

std::string GY302::GetUnit(uint8_t) const {
  return kUnitName;
}

DeviceDataType GY302::GetReading(uint8_t) const {
  return DeviceDataType{double(data_)};
}

std::string GY302::GetSensorType() const {
  return kSensorType;
}


} // namespace common::device

#include "common/device/cooling_fan.h"

namespace common::device {

PROGMEM const char *const CoolingFan::kExecutorType = "cooling_fan";

CoolingFan::CoolingFan(const std::string &id, uint8_t pin)
    : Executor{id}, pin_{pin} {
  pinMode(pin_, OUTPUT);
}

bool CoolingFan::IsActive() const {
  return t_.Sec() > 0 && cmd_ > 0.0;
}

Event CoolingFan::GenerateExecutorEvent() const {
  Event ret;
  ret.time = t_;
  ret.error_code = 0;
  ret.event_msg = std::string("Cooling Fan: ") + std::to_string(cmd_) + "%";
  ret.level = LogLevel::LOGLEVEL_INFO;
  ret.source_name = id_;
  return ret;
}

std::string CoolingFan::GetExecutorType() const { return kExecutorType; }

void CoolingFan::SendCommand(const DeviceDataType &cmd) {
  if (auto data = cmd.GetIf<double>(); data) {
    cmd_ = std::min(*data, 100.0);
    cmd_ = std::max(cmd_, 0.0);
    analogWrite(pin_, cmd_);
    t_ = common::Time::Now();
    return;
  }
  Clear();
}

}  // namespace common::device

#include "common/device/cooling_fan.h"

#include "common/math/math.h"

namespace common::device {

PROGMEM const char *const CoolingFan::kExecutorType = "cooling_fan";

CoolingFan::CoolingFan(const std::string &id, uint8_t pin,
                       const float *const data, size_t N)
    : Executor{id}, pin_{pin}, sys_id_data_{data}, sys_id_data_len_{N} {
  pinMode(pin_, OUTPUT);
}

bool CoolingFan::IsActive() const { return t_.Sec() > 0 && cmd_ > 0.0; }

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
    cmd_ = *data;
    auto voltage = ::common::math::PROGMEMSysIdInterpolate(
        sys_id_data_, sys_id_data_len_, *data);
    analogWrite(pin_, voltage / kMaxVoltage * kMaxDigitalOutput);
    t_ = common::Time::Now();
    return;
  }
  Clear();
}

} // namespace common::device

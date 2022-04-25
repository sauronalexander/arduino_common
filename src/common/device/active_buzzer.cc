#include "common/device/active_buzzer.h"

namespace common::device {

PROGMEM const char *const ActiveBuzzer::kActorType = "active_buzzer";

ActiveBuzzer::ActiveBuzzer(const std::string &id, uint8_t pin,
                           uint8_t threshold)
    : Actor{id}, pin_{pin}, threshold_{threshold} {
  pinMode(pin_, OUTPUT);
}

void ActiveBuzzer::Clear() {
  Actor::Clear();
  error_code_ = 0;
  digitalWrite(pin_, LOW);
}

bool ActiveBuzzer::IsActive() const {
  return t_.Sec() > 0 && error_code_ >= threshold_;
}

Event ActiveBuzzer::GenerateActorEvent() const {
  Event ret;
  ret.time = t_;
  ret.error_code = error_code_;
  ret.event_msg = std::string("Active Buzzer: ") + (IsActive() ? "ON" : "OFF");
  ret.level = LogLevel::LOGLEVEL_INFO;
  ret.source_name = id_;
  return ret;
}

std::string ActiveBuzzer::GetActorType() const { return kActorType; }

void ActiveBuzzer::SendCommand(const DeviceDataType &cmd) {
  if (auto data = cmd.GetIf<int>(); data) {
    if (*data >= 0 && *data <= std::numeric_limits<uint8_t>::max()) {
      error_code_ = static_cast<uint8_t>(*data);
      if (error_code_ >= threshold_) {
        t_ = common::Time::Now();
        digitalWrite(pin_, HIGH);
        return;
      }
    }
  }
  Clear();
}

} // namespace common::device

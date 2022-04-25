#include "common/device/device.h"

namespace common::device {

class ActiveBuzzer final : public Actor {
public:
  ActiveBuzzer(const std::string &id, uint8_t pin, uint8_t threshold = 100);

  void Clear() override;

  Event GenerateActorEvent() const override;
  std::string GetActorType() const override;
  void SendCommand(const DeviceDataType &cmd) override;  // Accepts error_code
  bool IsActive() const override;

private:
  static PROGMEM const char *const kActorType;

  const uint8_t pin_;
  uint8_t error_code_{0};
  const uint8_t threshold_;
};

} // namespace common::device

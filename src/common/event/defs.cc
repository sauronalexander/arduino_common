#include "common/event/defs.h"

#include "common/com/specialized_encoding.h"

namespace common {

void Event::Encode(std::string& msg) const {
  msg.clear();
  msg.resize(event_msg.size() + 8);

  common::com::Encode(static_cast<uint8_t>(error), &msg[0]);
  common::com::Encode(static_cast<uint8_t>(level), &msg[1]);
  common::com::Encode(error_code, &msg[2]);
  common::com::Encode(time.Sec(), &msg[4]);
  msg.replace(8, event_msg.size(), event_msg.data());
}

void Event::Decode(const std::string& msg) {
  {
    uint8_t tmp{0};
    common::com::Decode(&msg[0], tmp);
    error = static_cast<Error>(tmp);
    common::com::Decode(&msg[1], tmp);
    level = static_cast<LogLevel>(tmp);
  }
  {
    uint16_t tmp{0};
    common::com::Decode(&msg[2], tmp);
    error_code = tmp;
  }
  {
    uint32_t tmp{0};
    common::com::Decode(&msg[4], tmp);
    time = Time::FromSec(tmp);
  }

  event_msg = msg.substr(8, msg.size() - 8);
}

std::string Event::ToJson(uint8_t indent) const {
  std::string ret;
  return ret;
}

}  // namespace common
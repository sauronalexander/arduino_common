#include "common/stream_handler/serial_handler.h"

namespace common {

void SerialHandler::Log(const std::string &msg) {
  Serial.println(msg.c_str());
}

void SerialHandler::Log(const Event& msg) {
  Serial.println(GenerateTextLogFromEvent(msg).c_str());
}

void SerialHandler::LogStructured(const SensorReading& msg) {
  std::string json_str;
  serializeJson(msg.ToJson(), json_str);
  Serial.println(json_str.c_str());
}

}  // namespace common

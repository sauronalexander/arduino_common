#include "common/event/defs.h"

#include <ArduinoJson.h>

#include "common/com/specialized_encoding.h"

namespace common {

void Event::Encode(std::string& msg) const {
  size_t fixed_size = sizeof(time.Sec()) + sizeof(level) + sizeof(error_code);
  msg.resize(event_msg.size() + source_name.size()
             + fixed_size + 2 * sizeof(uint32_t));

  std::string time_encoded, level_encoded, error_code_encoded;
  common::com::Encode(time.Sec(), time_encoded);
  common::com::Encode(static_cast<uint8_t>(level), level_encoded);
  common::com::Encode(error_code, error_code_encoded);
  msg.replace(0, time_encoded.size(), time_encoded);
  msg.replace(sizeof(time.Sec()),
              level_encoded.size(), level_encoded);
  msg.replace(sizeof(time.Sec()) + sizeof(level),
              error_code_encoded.size(), error_code_encoded);
  std::string size_encoded;
  common::com::Encode(static_cast<uint32_t>(source_name.size()), size_encoded);
  msg.replace(fixed_size, size_encoded.size(), size_encoded);
  msg.replace(fixed_size + sizeof(uint32_t), source_name.size(), source_name);

  fixed_size += (sizeof(uint32_t) + source_name.size());
  common::com::Encode(static_cast<uint32_t>(source_name.size()), size_encoded);
  msg.replace(fixed_size, size_encoded.size(), size_encoded);
  msg.replace(fixed_size + sizeof(size_t), event_msg.size(), event_msg);
}

void Event::Decode(const std::string& msg) {
  // TODO: Use better serialzation/deserialization methods.
  uint32_t idx{0};
  {
    uint32_t tmp{0};
    common::com::Decode(msg.substr(idx, sizeof(tmp)), tmp);
    idx += sizeof(tmp);
    time = Time::FromSec(tmp);
  }

  {
    uint8_t tmp{0};
    common::com::Decode(msg.substr(idx, sizeof(tmp)), tmp);
    idx += sizeof(tmp);
    level = static_cast<LogLevel>(tmp);
  }

  common::com::Decode(msg.substr(idx, sizeof(error_code)), error_code);
  idx += (sizeof(error_code) + 1);

  uint32_t str_size;
  common::com::Decode(msg.substr(idx, sizeof(uint32_t)), str_size);
  idx += sizeof(uint32_t);
  source_name = msg.substr(idx, str_size);
  idx += source_name.size();

  common::com::Decode(msg.substr(idx, sizeof(uint32_t)), str_size);
  idx += sizeof(uint32_t);
  event_msg = msg.substr(idx, str_size);
}

DynamicJsonDocument Event::ToJson() const {
  DynamicJsonDocument result(256);
  result["metadata"]["error"] = error_code;
  result["metadata"]["level"] = static_cast<uint8_t>(level);
  result["metadata"]["name"] = source_name;
  result["msg"] = event_msg;
  result["timestamp"]["$date"]["$numberLong"] = time.ToString(
      common::Time::TimeOption::TIMESTAMP_MS);
  result.shrinkToFit();
  return result;
}

void SensorReading::Encode(std::string& msg) const {
  size_t fixed_size = sizeof(time.Sec()) + sizeof(double);
  msg.resize(fixed_size + sensor_id.size() + unit.size() + data_type.size()
             + sensor_type.size() + 4 * sizeof(uint32_t));

  std::string time_encoded, reading_encoded;
  common::com::Encode(time.Sec(), time_encoded);
  common::com::Encode(reading, reading_encoded);
  msg.replace(0, time_encoded.size(), time_encoded);
  msg.replace(sizeof(time.Sec()),
              reading_encoded.size(), reading_encoded);

  std::string size_encoded;
  common::com::Encode(static_cast<uint32_t>(sensor_id.size()), size_encoded);
  msg.replace(fixed_size, size_encoded.size(), size_encoded);
  msg.replace(fixed_size + sizeof(uint32_t), sensor_id.size(), sensor_id);

  fixed_size += (sizeof(uint32_t) + sensor_id.size());
  common::com::Encode(static_cast<uint32_t>(sensor_type.size()), size_encoded);
  msg.replace(fixed_size, size_encoded.size(), size_encoded);
  msg.replace(fixed_size + sizeof(uint32_t), sensor_type.size(), sensor_type);

  fixed_size += (sizeof(uint32_t) + sensor_type.size());
  common::com::Encode(static_cast<uint32_t>(data_type.size()), size_encoded);
  msg.replace(fixed_size, size_encoded.size(), size_encoded);
  msg.replace(fixed_size + sizeof(uint32_t), data_type.size(), data_type);

  fixed_size += (sizeof(uint32_t)) + data_type.size();
  common::com::Encode(static_cast<uint32_t>(unit.size()), size_encoded);
  msg.replace(fixed_size, size_encoded.size(), size_encoded);
  msg.replace(fixed_size + sizeof(uint32_t), unit.size(), unit);
}

void SensorReading::Decode(const std::string& msg) {
  // TODO: Use better serialzation/deserialization methods.
  uint32_t idx{0};
  {
    uint32_t tmp{0};
    common::com::Decode(msg.substr(idx, sizeof(tmp)), tmp);
    idx += sizeof(tmp);
    time = Time::FromSec(tmp);
  }

  {
    common::com::Decode(msg.substr(idx, sizeof(reading)), reading);
    idx += sizeof(reading);
  }

  uint32_t str_size;
  common::com::Decode(msg.substr(idx, sizeof(uint32_t)), str_size);
  idx += sizeof(uint32_t);
  sensor_id = msg.substr(idx, str_size);
  idx += sensor_id.size();

  common::com::Decode(msg.substr(idx, sizeof(uint32_t)), str_size);
  idx += sizeof(uint32_t);
  sensor_type = msg.substr(idx, str_size);
  idx += sensor_type.size();

  common::com::Decode(msg.substr(idx, sizeof(uint32_t)), str_size);
  idx += sizeof(uint32_t);
  data_type = msg.substr(idx, str_size);
  idx += data_type.size();

  common::com::Decode(msg.substr(idx, sizeof(uint32_t)), str_size);
  idx += sizeof(uint32_t);
  unit = msg.substr(idx, str_size);
}

DynamicJsonDocument SensorReading::ToJson() const {
  DynamicJsonDocument result(256);
  result["metadata"]["sensor_id"] = sensor_id;
  result["metadata"]["sensor_type"] = sensor_type;
  result["data_type"] = data_type;
  result["data"]["reading"] = reading;
  result["data"]["unit"] = unit;
  result["timestamp"]["$date"]["$numberLong"] = time.ToString(
      common::Time::TimeOption::TIMESTAMP_MS);
  result.shrinkToFit();
  return result;
}

}  // namespace common
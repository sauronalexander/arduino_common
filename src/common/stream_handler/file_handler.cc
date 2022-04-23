#include "common/stream_handler/file_handler.h"

#include "common/utility/utility.h"

namespace common {

FileHandler::FileHandler(const std::string &base_path, const std::string &name)
    : base_path_{base_path}, name_{name}{}

FileHandler::FileHandler(const char *base_path, const char *name)
    : base_path_{base_path}, name_{name} {}

void FileHandler::CreateDefaultFileHanderData() {
  file_path_ = filesystem::Path{base_path_};
  if (!name_.empty()) {
    file_path_ /= name_;
  }
}

void FileHandler::LogImpl(const std::string &msg) {
  if (!filesystem::Exists(file_path_.directory())) {
    filesystem::mkdir(file_path_.directory());
  }
  auto file = SD.open(file_path_.c_str(), O_RDWR | O_CREAT | O_APPEND);
  file.flush();
  file.close();
}


void FileHandler::Log(const std::string &msg) {
  CreateDefaultFileHanderData();
  static bool lock{false};
  while (lock) {
    Serial.println("Waiting for release");
  }
  lock = true;
  LogImpl(msg);
  lock = false;
}

void FileHandler::CheckAndRotate(const Time &t, bool structured) {
  auto t_str = t.ToString();
  if (auto current_hour = t_str.substr(sizeof "-MM-DDT" - 1, 2);
      current_hour_ != current_hour) {
    // Rotate
    current_hour_ = common::move(current_hour);
    file_path_ = filesystem::Path(base_path_);
    file_path_ /= t_str.substr(1, 5);
    file_path_ /= current_hour_;
    if (!filesystem::Exists(file_path_.c_str())) {
      filesystem::mkdir(file_path_.c_str());
    }
    file_path_ /= structured ? (name_ + ".s" + current_hour_) :
                               (name_ + "." + current_hour_);
  }
}

void FileHandler::Log(const Event &msg) {
  CheckAndRotate(msg.time, false);
  LogImpl(GenerateTextLogFromEvent(msg));
}

void FileHandler::LogStructured(const SensorReading &msg) {
  CheckAndRotate(msg.time, true);
  std::string json_str;
  serializeJson(msg.ToJson(), json_str);
  LogImpl(json_str);
}

}  // namespace common

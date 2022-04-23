#pragma once

#include <memory>

#include "common/filesystem/filesystem.h"
#include "common/stream_handler/stream_handler.h"
#include "common/type_traits/type_traits.h"


namespace common {

class FileHandler final : public StreamHandler {
public:
  FileHandler() = delete;
  FileHandler(const std::string &base_path, const std::string &name = "");
  FileHandler(const char *base_path, const char *name = "");

  template <typename T,
            typename = std::enable_if_t<!std::is_same<T, std::string>::value>>
  inline void Log(const T &msg) {
    Log(std::to_string(msg));
  }

  void Log(const std::string &msg) override;
  void Log(const Event &msg) override;
  void LogStructured(const SensorReading &msg) override;

private:
  void CheckAndRotate(const Time &t, bool structured);
  void CreateDefaultFileHanderData();

  void LogImpl(const std::string &msg);

  std::string current_hour_{""};
  filesystem::Path base_path_{""};
  std::string name_{""};
  filesystem::Path file_path_{""};
};

} // namespace common

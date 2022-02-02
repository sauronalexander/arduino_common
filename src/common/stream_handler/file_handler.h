#pragma once

#include <memory>

#include "common/filesystem/filesystem.h"
#include "common/stream_handler/stream_handler.h"
#include "common/type_traits/type_traits.h"


namespace common {

class FileHandler final : private StreamHandler {
public:
  FileHandler() = delete;
  FileHandler(const std::string &path);
  FileHandler(const char *path);

  template <typename T,
            typename = std::enable_if_t<!std::is_same<T, std::string>::value>>
  inline void Log(const T &msg) {
    Log(std::to_string(msg));
  }

  inline void Close() {
    handler_.reset();
  }

  void Log(const std::string &msg) override;

private:
  struct FileHandlerData {
    FileHandlerData(const char *filename);
    ~FileHandlerData();
    ::File file;
  };

  std::auto_ptr<FileHandlerData> handler_{nullptr};
};

} // namespace common

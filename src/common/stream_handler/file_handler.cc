#include "common/stream_handler/file_handler.h"

namespace common {

FileHandler::FileHandlerData::FileHandlerData(const char *filename) {
  ::common::filesystem::Path path(filename);
  if (!::common::filesystem::Exists(path.directory())) {
    ::common::filesystem::mkdir(path.directory());
  }
  file = SD.open(filename, FILE_WRITE);
}

FileHandler::FileHandlerData::~FileHandlerData() {
  file.close();
}

FileHandler::FileHandler(const std::string &path) {
  handler_.reset(new ::common::FileHandler::FileHandlerData(path.c_str()));
  // TODO: add error handling
}

FileHandler::FileHandler(const char *path) {
  handler_.reset(new ::common::FileHandler::FileHandlerData(path));
  // TODO: add error handling
}

void FileHandler::Log(const std::string &msg) {
  static bool lock{false};
  while (lock) {
    Serial.println("Waiting for release");
  }
  lock = true;
  handler_->file.println(msg.c_str());
  handler_->file.flush();
  lock = false;
}

}  // namespace common

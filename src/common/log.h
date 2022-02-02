#pragma once

#include <utility>

#include "common/event/defs.h"
#include "common/stl/string.h"
#include "common/stream_handler/stream_handler.h"

namespace common {

void FormatImpl(std::string& msg, const std::string& item) {
  auto pos = msg.find("%%");
  if (pos != std::string::npos) {
    msg.replace(pos, 2, item);
  }
}

void FormatImpl(std::string& msg, const char* const& item) {
  auto pos = msg.find("%%");
  if (pos != std::string::npos) {
    msg.replace(pos, 2, item, strlen(item));
  }
}

template <typename T>
void FormatImpl(std::string& msg, const T& item) {
  auto pos = msg.find("%%");
  if (pos != std::string::npos) {
    msg.replace(pos, 2, std::to_string(item));
  }
}

template <typename T, typename ...Args>
void FormatImpl(std::string& msg, const T& item, const Args&... args) {
  auto pos = msg.find("%%");
  if (pos != std::string::npos) {
    msg.replace(pos, 2, std::to_string(item));
    FormatImpl(msg, args...);
  }
}

template <typename ...Args>
std::string Format(std::string msg, const Args& ...args) {
  FormatImpl(msg, args...);
  return msg;
}

template <typename ...Args>
std::string Format(const char* msg, const Args& ...args) {
  std::string msg_str(msg);
  FormatImpl(msg_str, args...);
  return msg_str;
}

class Log {
public:
  Log() = delete;
  Log(const char* name) : name_{name} {}
  Log(const std::string& name) : name_{name} {}

  template <typename ...Args>
  void Debug(const std::string& msg, const Args&... args) {
    LogImpl(msg, LogLevel::LOGLEVEL_DEBUG, args...);
  }

  template <typename ...Args>
  void Info(const std::string& msg, const Args&... args) {
    LogImpl(msg, LogLevel::LOGLEVEL_INFO, args...);
  }

  template <typename ...Args>
  void Warn(const std::string& msg, const Args&... args) {
    LogImpl(msg, LogLevel::LOGLEVEL_WARN, args...);
  }

  template <typename ...Args>
  void Error(const std::string& msg, const Args&... args) {
    LogImpl(msg, LogLevel::LOGLEVEL_ERROR, args...);
  }

  template <typename ...Args>
  void Fatal(const std::string& msg, const Args&... args) {
    LogImpl(msg, LogLevel::LOGLEVEL_FATAL, args...);
  }

private:
  template <typename ...Args, uint8_t N = sizeof...(Args)>
  void LogImpl(std::string msg, LogLevel level, const Args&... args) {
    if (sizeof...(Args) || msg.find("%%") != std::string::npos) {
      FormatImpl(msg, args...);
    }
    for (auto handler : handlers_) {
      if (handler.second <= level) {
        handler.first->Log(msg);
      }
    }
  }

  void AddStreamHandler(StreamHandler* handler,
                        LogLevel min_level = LogLevel::LOGLEVEL_INFO) {
    if (handler) {
      handlers_.push_back({handler, min_level});
    }
  }

  std::vector<std::pair<StreamHandler*, LogLevel>> handlers_{};
  std::string name_;
};

}  // namespace common

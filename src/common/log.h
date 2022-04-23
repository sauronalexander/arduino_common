#pragma once

#include "common/event/defs.h"
#include "common/stream_handler/lcd_handler.h"
#include "common/stream_handler/file_handler.h"
#include "common/stream_handler/serial_handler.h"
#include "common/stl/string.h"
#include "common/utility/utility.h"

namespace common {

namespace detail {
void FormatImpl(std::string &msg, const std::string &item) {
  auto pos = msg.find("%%");
  if (pos != std::string::npos) {
    msg.replace(pos, 2, item);
  }
}

void FormatImpl(std::string &msg, const char *const &item) {
  auto pos = msg.find("%%");
  if (pos != std::string::npos) {
    msg.replace(pos, 2, item, strlen(item));
  }
}

template<typename T>
void FormatImpl(std::string &msg, const T &item) {
  auto pos = msg.find("%%");
  if (pos != std::string::npos) {
    msg.replace(pos, 2, std::to_string(item));
  }
}

template<typename T, typename ...Args>
void FormatImpl(std::string &msg, const T &item, const Args &... args) {
  auto pos = msg.find("%%");
  if (pos != std::string::npos) {
    msg.replace(pos, 2, std::to_string(item));
    FormatImpl(msg, args...);
  }
}

template<typename ...Args>
std::string Format(std::string msg, const Args &...args) {
  FormatImpl(msg, args...);
  return msg;
}

template<typename ...Args>
std::string Format(const char *msg, const Args &...args) {
  std::string msg_str(msg);
  FormatImpl(msg_str, args...);
  return msg_str;
}

} // namespace detail

class Log {
public:
  Log() = delete;
  explicit Log(const char* name) : name_{name} {
    Init();
  }

  explicit Log(const std::string& name) : name_{name} {
    Init();
  }

  ~Log() = default;

  void AddStreamHandler(StreamHandler *handler, LogLevel level) {
    handlers_.push_back(std::make_pair(handler, level));
  }

  void AddStructuredStreamHandler(StreamHandler *handler) {
    structured_handlers_.push_back(handler);
  }

  template <typename ...Args>
  void Debug(const std::string& msg, const Args&... args) {
    LogImpl(msg, 0, LogLevel::LOGLEVEL_DEBUG, args...);
  }

  template <typename ...Args>
  void Info(const std::string& msg, const Args&... args) {
    LogImpl(msg, 0, LogLevel::LOGLEVEL_INFO, args...);
  }

  template <typename ...Args>
  void Warn(uint8_t error_code, const std::string& msg, const Args&... args) {
    LogImpl(msg, error_code, LogLevel::LOGLEVEL_WARN, args...);
  }

  template <typename ...Args>
  void Error(uint8_t error_code, const std::string& msg, const Args&... args) {
    LogImpl(msg, error_code, LogLevel::LOGLEVEL_ERROR, args...);
  }

  template <typename ...Args>
  void Fatal(uint8_t error_code, const std::string& msg, const Args&... args) {
    LogImpl(msg, error_code, LogLevel::LOGLEVEL_FATAL, args...);
  }

  void LogStructured(const SensorReading &msg) {
    for (auto &handler : structured_handlers_) {
      handler->LogStructured(msg);
    }
  }

private:
  void Init() {
    txt_logger_.reset(new FileHandler("log", name_));
    serial_logger_.reset(new SerialHandler());
    handlers_.push_back(
        std::make_pair(txt_logger_.get(), LogLevel::LOGLEVEL_INFO));
    structured_handlers_.push_back(txt_logger_.get());
    handlers_.push_back(
        std::make_pair(serial_logger_.get(), LogLevel::LOGLEVEL_INFO));
    structured_handlers_.push_back(serial_logger_.get());
  }

  template <typename ...Args, uint8_t N = sizeof...(Args)>
  void LogImpl(std::string msg, uint8_t error_code,
               LogLevel level, const Args&... args) {
    if (sizeof...(Args) || msg.find("%%") != std::string::npos) {
      detail::FormatImpl(msg, args...);
    }
    Event event{Time::Now(), level, error_code, name_, common::move(msg)};
    for (auto &handler : handlers_) {
      if (handler.second <= level) {
        handler.first->Log(event);
      }
    }
  }

  std::vector<std::pair<StreamHandler*, LogLevel>> handlers_{};
  std::vector<StreamHandler*> structured_handlers_{};
  std::auto_ptr<FileHandler> txt_logger_{nullptr};
  std::auto_ptr<SerialHandler> serial_logger_{nullptr};
  std::string name_;
};

}  // namespace common

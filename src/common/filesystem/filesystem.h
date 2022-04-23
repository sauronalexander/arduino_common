#pragma once

#include <SD.h>
#include <Arduino.h>
#include <vector>

#include "common/stl/string.h"

namespace common {

class filesystem {
public:
  class Path {
  public:
    Path(const std::string& path) {
      path_ = path;
    }

    Path(const char* path) {
      path_ = std::string(path);
    }

    Path& operator/=(const char* other) {
      path_ += "/";
      path_ += other;
      return *this;
    }
    Path& operator/=(const std::string& other) {
      path_ += "/";
      path_ += other;
      return *this;
    }

    Path& operator/=(const Path& other) {
      path_ += "/";
      path_ += other.path_;
      return *this;
    }

    Path& operator+=(const char* other) {
      path_ += other;
      return *this;
    }
    Path& operator+=(const std::string& other) {
      path_ += other;
      return *this;
    }

    Path& operator+=(const Path& other) {
      path_ += other.path_;
      return *this;
    }

    const std::string& string() const {
      return path_;
    }

    const std::string directory() const {
      auto pos = path_.find_last_of('/');
      if (pos != std::string::npos) {
        return path_.substr(0, pos);
      }
      return std::string();
    }

    const char* c_str() const {
      return path_.c_str();
    }

    /*
    const std::string& extension() const {}

    const std::string& stem() const {}

    const std::string& filename() const {}

    const std::string& parent_path() const {}
*/
  private:
    std::string path_{};
  };

  static PROGMEM constexpr unsigned int kMaxBufferSize = 1024;


  static bool Init(int chip_select);
  static bool Exists(const std::string& path);
  static bool mkdir(const std::string& path);
  static bool rm(const std::string& path);
  static std::vector<std::string> ls(const std::string& path, bool recursive);
private:
  static void ls(File dir,
                 const Path& cur_dir,
                 bool recursive,
                 std::vector<std::string>& dirs);
};

}  // namespace common

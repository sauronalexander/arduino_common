#include "common/filesystem/filesystem.h"


namespace common {

bool& Initialized() {
  static bool initialized{false};
  return initialized;
}

bool filesystem::Init() {
  Initialized() = SD.begin(kChipSelect);
  return Initialized();
}

bool filesystem::Exists(const std::string& path) {
  if (!Initialized()) {
    return false;
  }
  return SD.exists(path.c_str());
}

bool filesystem::mkdir(const std::string& path) {
  if (!Initialized()) {
    return false;
  }
  return SD.mkdir(path.c_str());
}

bool filesystem::rm(const std::string& path) {
  if (!Initialized()) {
    return false;
  }
  return SD.remove(path.c_str());
}

std::vector<std::string> filesystem::ls(const std::string& path, bool resursive) {
  std::vector<std::string> dirs;
  if (!Initialized()) {
    return dirs;
  }
  ls(SD.open(path.c_str(), O_RDONLY), std::string(), resursive, dirs);
  return dirs;
}

void filesystem::ls(File dir,
                    const Path& cur_dir,
                    bool recursive,
                    std::vector<std::string>& dirs) {
  while (true) {
    File next = dir.openNextFile();
    if (!next) {
      return;
    }
    Path tmp_path = cur_dir;
    tmp_path /= next.name();
    dirs.push_back(tmp_path.string());
    if (next.isDirectory()) {
      if (recursive) {
        ls(next, tmp_path, recursive, dirs);
      }
    }
  }
}

}  // namespace common

#include "file_utils.h"

#include <fstream>
#include <spdlog/spdlog.h>
#include <stdexcept>

namespace fs = std::filesystem;

namespace utils {
bool FileUtils::createDirectory(const std::string &path) {
  try {
    return fs::create_directories(path);
  } catch (const std::exception &e) {
    spdlog::error("Error creating directory: {}", e.what());
    return false;
  }
}

bool FileUtils::writeToFile(const std::string &path,
                            const std::string &content) {
  try {
    std::ofstream file(path);
    if (!file.is_open()) {
      spdlog::error("Could not open file: {}", path);
      return false;
    }
    file << content;
    return true;
  } catch (const std::exception &e) {
    spdlog::error("Error writing to file: {}", e.what());
    return false;
  }
}

std::string FileUtils::readFromFile(const std::string &path) {
  try {
    std::ifstream file(path);
    if (!file.is_open()) {
      throw std::runtime_error("Could not open file: " + path);
    }
    return std::string(std::istreambuf_iterator<char>(file),
                       std::istreambuf_iterator<char>());
  } catch (const std::exception &e) {
    spdlog::error("Error reading from file: {}", e.what());
    return "";
  }
}

bool FileUtils::fileExists(const std::string &path) {
  return fs::exists(path) && fs::is_regular_file(path);
}

bool FileUtils::directoryExists(const std::string &path) {
  return fs::exists(path) && fs::is_directory(path);
}

std::vector<std::string> FileUtils::listFiles(const std::string &directory) {
  std::vector<std::string> files;
  try {
    for (const auto &entry : fs::directory_iterator(directory)) {
      if (entry.is_regular_file()) {
        files.push_back(entry.path().filename().string());
      }
    }
  } catch (const std::exception &e) {
    spdlog::error("Error listing files: {}", e.what());
  }
  return files;
}

bool FileUtils::copyFile(const std::string &source,
                         const std::string &destination) {
  try {
    fs::copy_file(source, destination, fs::copy_options::overwrite_existing);
    return true;
  } catch (const std::exception &e) {
    spdlog::error("Error copying file: {}", e.what());
    return false;
  }
}

bool FileUtils::copyDirectory(const std::string &source,
                              const std::string &destination) {
  try {
    if (!directoryExists(destination)) {
      createDirectory(destination);
    }

    for (const auto &entry : fs::directory_iterator(source)) {
      const auto &path = entry.path();
      const auto &targetPath = fs::path(destination) / path.filename();

      if (fs::is_directory(path)) {
        if (!copyDirectory(path.string(), targetPath.string())) {
          return false;
        }
      } else {
        if (!copyFile(path.string(), targetPath.string())) {
          return false;
        }
      }
    }
    return true;
  } catch (const std::exception &e) {
    spdlog::error("Error copying directory: {}", e.what());
    return false;
  }
}

std::string FileUtils::getFileName(const std::string &path) {
  return fs::path(path).filename().string();
}

std::string FileUtils::getDirectoryName(const std::string &path) {
  auto parent = fs::path(path).parent_path();
  if (parent.empty()) {
    return ".";
  }
  return parent.string();
}

std::string FileUtils::getFileExtension(const std::string &path) {
  return fs::path(path).extension().string();
}

std::string FileUtils::combinePath(const std::string &path1,
                                   const std::string &path2) {
  return (fs::path(path1) / fs::path(path2)).string();
}

bool FileUtils::setExecutable(const std::string& filePath) {
  #ifdef _WIN32
    // Windows不需要设置执行权限
    (void)filePath; // Suppress unused parameter warning
    return true;
  #else
    // Unix系统使用chmod
    std::string command = "chmod +x \"" + filePath + "\"";
    return system(command.c_str()) == 0;
  #endif
}

bool FileUtils::commandExists(const std::string& command) {
  #ifdef _WIN32
    std::string cmd = "where " + command + " >nul 2>&1";
  #else
    std::string cmd = "command -v " + command + " >/dev/null 2>&1";
  #endif
  return system(cmd.c_str()) == 0;
}
} // namespace utils

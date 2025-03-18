#pragma once
#include <filesystem>
#include <string>
#include <vector>

namespace utils {
class FileUtils {
public:
  // 创建目录(递归)
  static bool createDirectory(const std::string &path);

  // 写入文件内容
  static bool writeToFile(const std::string &path, const std::string &content);

  // 读取文件内容
  static std::string readFromFile(const std::string &path);

  // 检查文件是否存在
  static bool fileExists(const std::string &path);

  // 检查目录是否存在
  static bool directoryExists(const std::string &path);

  // 列出目录中的文件
  static std::vector<std::string> listFiles(const std::string &directory);

  // 复制文件
  static bool copyFile(const std::string &source,
                       const std::string &destination);

  // 复制目录(递归)
  static bool copyDirectory(const std::string &source,
                            const std::string &destination);

  // 获取路径的文件名部分
  static std::string getFileName(const std::string &path);

  // 获取路径的目录部分
  static std::string getDirectoryName(const std::string &path);

  // 获取文件扩展名
  static std::string getFileExtension(const std::string &path);

  // 组合路径
  static std::string combinePath(const std::string &path1,
                                 const std::string &path2);

  // 设置文件为可执行文件
  static bool setExecutable(const std::string &filePath);

  // 检查命令是否存在
  static bool commandExists(const std::string &command);
};
} // namespace utils
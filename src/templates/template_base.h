#pragma once
#include "../cli/cli_parser.h"
#include "../utils/file_utils.h"
#include <string>

class TemplateBase {
public:
  TemplateBase(const CliOptions &options) : options_(options) {}

  virtual ~TemplateBase() = default;

  // 创建项目的主入口
  virtual bool create() = 0;

protected:
  CliOptions options_;

  // 创建基本项目结构
  virtual bool createProjectStructure() = 0;

  // 创建构建系统文件
  virtual bool createBuildSystem() = 0;

  // 设置包管理器
  virtual bool setupPackageManager() = 0;

  // 设置测试框架
  virtual bool setupTestFramework() = 0;

  // 初始化git
  bool initializeGit(const std::string &projectPath) {
    if (!options_.initGit) {
      return true;
    }

    std::string gitignoreContent = getGitignoreContent();
    utils::FileUtils::writeToFile(
        utils::FileUtils::combinePath(projectPath, ".gitignore"),
        gitignoreContent);

    std::string command =
        "cd " + projectPath +
        " && git init && git add . && git commit -m \"Initial commit\"";
    return system(command.c_str()) == 0;
  }

  // 获取.gitignore内容
  std::string getGitignoreContent() {
    return R"(# Build directories
build/
bin/
lib/
out/
cmake-build-*/

# Dependency directories
vcpkg_installed/
conan/

# IDE files
.vs/
.vscode/
.idea/
*.swp
*~

# Compiled files
*.o
*.obj
*.exe
*.dll
*.so
*.dylib
*.a
*.lib

# CMake files
CMakeCache.txt
CMakeFiles/
cmake_install.cmake
install_manifest.txt

# Bazel files
bazel-*

# Generated files
compile_commands.json

#Clangd
.cache/
)";
  }
};
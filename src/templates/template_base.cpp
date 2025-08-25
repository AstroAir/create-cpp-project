#include "template_base.h"
#include "../utils/file_utils.h"
#include "../utils/git_utils.h"
#include "../utils/progress_indicator.h"
#include "../config/ci_config.h"
#include "../config/editor_config.h"
#include "../config/code_style_config.h"
#include "../config/doc_config.h"
// #include "../utils/post_creation_actions.h"
#include <iostream>

using namespace cli_enums;

bool TemplateBase::initializeGit(const std::string &projectPath) {
  if (!options_.initGit) {
    return true;
  }

  std::cout << "📦 初始化Git仓库...\n";

  // Use enhanced Git utilities
  std::filesystem::path projectDir(projectPath);

  // Initialize Git repository
  if (!utils::GitUtils::initializeRepository(projectDir)) {
    std::cerr << "❌ Git仓库初始化失败\n";
    return false;
  }

  // Create comprehensive .gitignore
  std::string templateType = std::string(to_string(options_.templateType));
  std::string buildSystem = std::string(to_string(options_.buildSystem));
  std::string packageManager = std::string(to_string(options_.packageManager));

  if (!utils::GitUtils::createGitignore(projectDir, templateType, buildSystem, packageManager)) {
    std::cerr << "❌ 创建.gitignore文件失败\n";
    return false;
  }

  // Create .gitattributes
  if (!utils::GitUtils::createGitAttributes(projectDir)) {
    std::cerr << "❌ 创建.gitattributes文件失败\n";
    return false;
  }

  // Enhanced repository configuration
  std::string gitWorkflow = std::string(to_string(options_.gitWorkflow));
  std::string licenseType = std::string(to_string(options_.licenseType));

  if (!utils::GitUtils::configureRepositoryAdvanced(projectDir,
                                                   options_.gitUserName,
                                                   options_.gitUserEmail,
                                                   options_.gitRemoteUrl,
                                                   options_.setupGitHooks)) {
    std::cerr << "❌ Git仓库高级配置失败\n";
  }

  // Setup Git workflow
  if (gitWorkflow != "none") {
    if (!utils::GitUtils::setupGitWorkflow(projectDir, gitWorkflow)) {
      std::cerr << "❌ Git工作流配置失败\n";
    }
  }

  // Create branches based on strategy
  std::string branchStrategy = std::string(to_string(options_.gitBranchStrategy));
  if (!utils::GitUtils::createBranchesFromStrategy(projectDir, branchStrategy, options_.gitBranches)) {
    std::cerr << "❌ Git分支创建失败\n";
  }

  // Create license file
  if (licenseType != "none") {
    if (!utils::GitUtils::createLicenseFile(projectDir, licenseType, options_.projectName,
                                           options_.gitUserName.empty() ? "Project Author" : options_.gitUserName)) {
      std::cerr << "❌ 许可证文件创建失败\n";
    }
  }

  // Create code quality configurations if enabled
  if (options_.includeCodeStyleTools) {
    utils::CodeQualityTools::createClangFormatConfig(projectDir);
    utils::CodeQualityTools::createClangTidyConfig(projectDir);
    utils::CodeQualityTools::createEditorConfig(projectDir);
  }

  // Create initial README if it doesn't exist
  std::string readmePath = utils::FileUtils::combinePath(projectPath, "README.md");
  if (!utils::FileUtils::fileExists(readmePath)) {
    if (!utils::FileUtils::writeToFile(
            readmePath, "# " + options_.projectName +
                            "\n\n初始项目结构由CPP-Scaffold创建。\n")) {
      std::cerr << "❌ 创建README.md文件失败\n";
    }
  }

  // Create initial commit if requested
  if (options_.createInitialCommit) {
    if (!utils::GitUtils::createInitialCommit(projectDir, "Initial commit")) {
      std::cerr << "❌ 创建初始提交失败\n";
      return false;
    }
  }

  std::cout << "✅ Git仓库初始化完成\n";
  return true;
}

std::string TemplateBase::getGitignoreContent() {
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

# 包管理器相关
.vcpkg/
conanbuildinfo.*
conaninfo.*
graph_info.json

# 测试相关
Testing/
CTestTestfile.cmake
)";
}

// 添加获取项目许可证内容的辅助方法
std::string TemplateBase::getLicenseContent(const std::string &projectName) {
  // 返回一个简单的MIT许可证模板
  return R"(MIT License

Copyright (c) )" +
         std::to_string(2025) + R"( )" + projectName + R"(

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
)";
}

// 创建许可证文件
bool TemplateBase::createLicense(const std::string &projectPath) {
  return utils::FileUtils::writeToFile(
      utils::FileUtils::combinePath(projectPath, "LICENSE"),
      getLicenseContent(options_.projectName));
}

// 创建空的.clang-format文件
bool TemplateBase::createClangFormat(const std::string &projectPath) {
  return utils::FileUtils::writeToFile(
      utils::FileUtils::combinePath(projectPath, ".clang-format"),
      R"(BasedOnStyle: LLVM
IndentWidth: 4
TabWidth: 4
UseTab: Never
ColumnLimit: 100
AccessModifierOffset: -4
NamespaceIndentation: All
BreakBeforeBraces: Allman
AllowShortFunctionsOnASingleLine: None
AllowShortIfStatementsOnASingleLine: false
AllowShortLoopsOnASingleLine: false
AlwaysBreakTemplateDeclarations: Yes
)");
}

// 创建.gitattributes文件
bool TemplateBase::createGitAttributes(const std::string &projectPath) {
  return utils::FileUtils::writeToFile(
      utils::FileUtils::combinePath(projectPath, ".gitattributes"),
      R"(# 设置默认行尾风格
* text=auto

# C/C++ 源文件
*.c text
*.cc text
*.cxx text
*.cpp text
*.c++ text
*.hpp text
*.h text
*.h++ text
*.hh text

# 声明为二进制文件不做行尾转换
*.png binary
*.jpg binary
*.jpeg binary
*.gif binary
*.ico binary
*.mov binary
*.mp4 binary
*.mp3 binary
*.flv binary
*.fla binary
*.swf binary
*.gz binary
*.zip binary
*.7z binary
*.ttf binary
*.eot binary
*.woff binary
*.pyc binary
*.pdf binary
*.dll binary
*.so binary
*.dylib binary
*.exe binary
*.lib binary
*.a binary
)");
}

// 设置CI/CD配置
bool TemplateBase::setupCICD(const std::string &projectPath) {
  if (options_.ciOptions.empty()) {
    return true; // 没有选择CI/CD系统，直接成功返回
  }

  std::cout << "📦 设置CI/CD配置...\n";

  bool result = CIConfig::createCIConfigs(projectPath, options_.ciOptions, options_);

  if (result) {
    std::cout << "✅ CI/CD配置创建成功\n";
  } else {
    std::cerr << "❌ CI/CD配置创建失败\n";
  }

  return result;
}

// 设置编辑器配置
bool TemplateBase::setupEditorConfig(const std::string &projectPath) {
  if (options_.editorOptions.empty()) {
    return true; // 没有选择编辑器配置，直接成功返回
  }

  std::cout << "📦 设置编辑器配置...\n";

  bool result = EditorConfigManager::createEditorConfigs(projectPath, options_.editorOptions, options_);

  if (result) {
    std::cout << "✅ 编辑器配置创建成功\n";
  } else {
    std::cerr << "❌ 编辑器配置创建失败\n";
  }

  return result;
}

// 设置代码风格配置
bool TemplateBase::setupCodeStyleConfig(const std::string &projectPath) {
  if (!options_.includeCodeStyleTools) {
    return true; // 没有选择包含代码风格工具，直接成功返回
  }

  std::cout << "📦 设置代码风格配置...\n";

  bool result = CodeStyleConfig::createCodeStyleConfig(projectPath, options_);

  if (result) {
    std::cout << "✅ 代码风格配置创建成功\n";
  } else {
    std::cerr << "❌ 代码风格配置创建失败\n";
  }

  return result;
}

// 设置文档配置
bool TemplateBase::setupDocConfig(const std::string &projectPath) {
  if (!options_.includeDocumentation) {
    return true; // 没有选择包含文档，直接成功返回
  }

  std::cout << "📦 设置文档配置...\n";

  bool result = DocConfig::createDocConfig(projectPath, options_);

  if (result) {
    std::cout << "✅ 文档配置创建成功\n";
  } else {
    std::cerr << "❌ 文档配置创建失败\n";
  }

  return result;
}

// 打印项目创建后的使用指南
void TemplateBase::printUsageGuide() {
  std::cout << "\n📋 项目使用指南:\n\n";

  std::cout << "1. 进入项目目录:\n";
  std::cout << "   cd " << options_.projectName << "\n\n";

  std::cout << "2. 构建项目:\n";
  if (to_string(options_.buildSystem) == "cmake") {
    std::cout << "   mkdir build && cd build\n";
    std::cout << "   cmake ..\n";
    std::cout << "   cmake --build .\n";
  } else if (to_string(options_.buildSystem) == "meson") {
    std::cout << "   meson setup build\n";
    std::cout << "   cd build\n";
    std::cout << "   meson compile\n";
  } else if (to_string(options_.buildSystem) == "bazel") {
    std::cout << "   bazel build //...\n";
  } else if (to_string(options_.buildSystem) == "xmake") {
    std::cout << "   xmake\n";
  } else if (to_string(options_.buildSystem) == "premake") {
    std::cout << "   premake5 gmake\n";
    std::cout << "   make\n";
  }
  std::cout << "\n";

  if (options_.includeTests) {
    std::cout << "3. 运行测试:\n";
    if (to_string(options_.buildSystem) == "cmake") {
      std::cout << "   cd build\n";
      std::cout << "   ctest\n";
    } else if (to_string(options_.buildSystem) == "meson") {
      std::cout << "   cd build\n";
      std::cout << "   meson test\n";
    } else if (to_string(options_.buildSystem) == "bazel") {
      std::cout << "   bazel test //...\n";
    } else if (to_string(options_.buildSystem) == "xmake") {
      std::cout << "   xmake test\n";
    } else if (to_string(options_.buildSystem) == "premake") {
      std::cout << "   bin/Debug/" << options_.projectName << "_tests\n";
    }
    std::cout << "\n";
  }

  if (to_string(options_.packageManager) != "none") {
    std::cout << "4. 包管理: \n";
    if (to_string(options_.packageManager) == "vcpkg") {
      std::cout << "   vcpkg安装依赖已在vcpkg.json中配置\n";
    } else if (to_string(options_.packageManager) == "conan") {
      std::cout << "   在构建项目前运行:\n";
      std::cout << "   conan install . --build=missing\n";
    }
    std::cout << "\n";
  }

  if (!options_.ciOptions.empty()) {
    std::cout << "5. CI/CD配置: \n";
    std::cout << "   已为以下CI/CD系统创建配置:\n";
    for (const auto& ci : options_.ciOptions) {
      std::cout << "   - " << to_string(ci) << "\n";
    }
    std::cout << "\n";
  }

  if (options_.includeCodeStyleTools) {
    std::cout << "6. 代码风格工具: \n";
    std::cout << "   项目已配置clang-format、clang-tidy和EditorConfig\n";
    std::cout << "\n";
  }

  if (options_.includeDocumentation) {
    std::cout << "7. 文档生成: \n";
    std::cout << "   使用以下命令生成文档:\n";
    std::cout << "   cd docs\n";
    #ifdef _WIN32
    std::cout << "   .\\generate_docs.bat\n";
    #else
    std::cout << "   ./generate_docs.sh\n";
    #endif
    std::cout << "\n";
  }

  std::cout << "祝编码愉快! 🎉\n";
}

// 执行创建后的操作
bool TemplateBase::executePostCreationActions() {
  // Temporarily disabled due to namespace conflicts
  // utils::PostCreationActions& actions = utils::PostCreationActions::getInstance();
  // return actions.executeAll(options_);
  return true;
}

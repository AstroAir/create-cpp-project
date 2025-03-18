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

  // 设置CI/CD配置
  bool setupCICD(const std::string &projectPath);
  
  // 设置编辑器配置
  bool setupEditorConfig(const std::string &projectPath);
  
  // 设置代码风格配置
  bool setupCodeStyleConfig(const std::string &projectPath);
  
  // 设置文档配置
  bool setupDocConfig(const std::string &projectPath);

  // 初始化git
  bool initializeGit(const std::string &projectPath);

  // 获取.gitignore内容
  std::string getGitignoreContent();
  
  // 获取许可证内容
  std::string getLicenseContent(const std::string &projectName);
  
  // 创建许可证文件
  bool createLicense(const std::string &projectPath);
  
  // 创建空的.clang-format文件
  bool createClangFormat(const std::string &projectPath);
  
  // 创建.gitattributes文件
  bool createGitAttributes(const std::string &projectPath);
  
  // 打印项目创建后的使用指南
  void printUsageGuide();
};
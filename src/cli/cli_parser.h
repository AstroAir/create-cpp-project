#pragma once
#include <map>
#include <optional>
#include <string>
#include <vector>

struct CliOptions {
  std::string projectName;
  std::string templateType;   // console, lib, gui, network, embedded
  std::string buildSystem;    // cmake, meson, bazel, xmake, premake
  std::string packageManager; // vcpkg, conan, none
  std::string networkLibrary; // asio, boost, poco (for network projects)

  bool includeTests = false;
  std::string testFramework; // gtest, catch2, doctest

  bool includeDocumentation = false; // 是否包含文档配置
  
  bool includeCodeStyleTools = false; // 是否包含代码风格工具配置

  std::vector<std::string> editorOptions; // vscode, clion, vs
  std::vector<std::string> ciOptions; // github, gitlab, travis, appveyor

  bool initGit = true;
  bool showHelp = false;
  bool version = false;

  std::string language = "en"; // 界面语言 (en, zh)
};

class CliParser {
public:
  // 解析命令行参数
  static CliOptions parse(int argc, char *argv[]);

  // 显示帮助信息
  static void showHelp(const std::string &language = "en");

  // 显示版本信息
  static void showVersion();

  // 交互式提示获取选项
  static CliOptions promptUserForOptions(const CliOptions &defaultOptions);

  // 保存选项作为默认配置
  static bool saveOptionsAsDefaults(const CliOptions &options);

  // 从配置文件加载默认选项
  static CliOptions loadDefaultOptions();

private:
  // 验证选项是否有效
  static bool validateOptions(CliOptions &options);

  // 读取用户输入
  static std::string readUserInput(const std::string &prompt,
                                   const std::string &defaultValue = "");

  // 读取用户选择
  static std::string readUserChoice(const std::string &prompt,
                                    const std::vector<std::string> &choices,
                                    const std::string &defaultValue = "");

  // 读取用户多选
  static std::vector<std::string>
  readUserMultiChoice(const std::string &prompt,
                      const std::vector<std::string> &choices,
                      const std::vector<std::string> &defaultValues = {});

  // 读取用户确认(是/否)
  static bool readUserConfirmation(const std::string &prompt,
                                   bool defaultValue = true);

  // 打印选项到控制台
  static void printOptions(const std::vector<std::string> &options,
                           const std::string &defaultOption = "");

  // 打印多选选项到控制台
  static void
  printMultiOptions(const std::vector<std::string> &options,
                    const std::vector<std::string> &selectedOptions);

  // 获取配置文件路径
  static std::string getConfigFilePath();

  // 本地化字符串映射
  static std::map<std::string, std::map<std::string, std::string>>
      localizationStrings;

  // 获取本地化字符串
  static std::string getLocalizedString(const std::string &key,
                                        const std::string &language);
};
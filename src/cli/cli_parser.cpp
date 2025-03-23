#include "cli_parser.h"

#include <algorithm>
#include <cctype>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <sstream>

#include "../utils/string_utils.h"
#include "../utils/terminal_utils.h"
#include <fmt/color.h>
#include <fmt/core.h>
#include <spdlog/spdlog.h>

using namespace utils;
using json = nlohmann::json;

// 初始化静态成员
Language Localization::s_currentLanguage = Language::English;
std::unordered_map<std::string, std::unordered_map<Language, std::string>>
    Localization::s_strings;

// 枚举转换实现
namespace enums {
std::string_view to_string(TemplateType type) {
  static const std::unordered_map<TemplateType, std::string_view> map = {
      {TemplateType::Console, "console"},
      {TemplateType::Lib, "lib"},
      {TemplateType::Gui, "gui"},
      {TemplateType::Network, "network"},
      {TemplateType::Embedded, "embedded"},
      {TemplateType::WebService, "webservice"},
      {TemplateType::GameEngine, "gameengine"}};
  return map.at(type);
}

std::string_view to_string(BuildSystem system) {
  static const std::unordered_map<BuildSystem, std::string_view> map = {
      {BuildSystem::CMake, "cmake"},     {BuildSystem::Meson, "meson"},
      {BuildSystem::Bazel, "bazel"},     {BuildSystem::XMake, "xmake"},
      {BuildSystem::Premake, "premake"}, {BuildSystem::Make, "make"},
      {BuildSystem::Ninja, "ninja"}};
  return map.at(system);
}

std::string_view to_string(PackageManager manager) {
  static const std::unordered_map<PackageManager, std::string_view> map = {
      {PackageManager::Vcpkg, "vcpkg"},
      {PackageManager::Conan, "conan"},
      {PackageManager::None, "none"},
      {PackageManager::Spack, "spack"},
      {PackageManager::Hunter, "hunter"}};
  return map.at(manager);
}

std::string_view to_string(TestFramework framework) {
  static const std::unordered_map<TestFramework, std::string_view> map = {
      {TestFramework::GTest, "gtest"},
      {TestFramework::Catch2, "catch2"},
      {TestFramework::Doctest, "doctest"},
      {TestFramework::Boost, "boost"},
      {TestFramework::None, "none"}};
  return map.at(framework);
}

std::string_view to_string(EditorConfig editor) {
  static const std::unordered_map<EditorConfig, std::string_view> map = {
      {EditorConfig::VSCode, "vscode"}, {EditorConfig::CLion, "clion"},
      {EditorConfig::VS, "vs"},         {EditorConfig::Vim, "vim"},
      {EditorConfig::Emacs, "emacs"},   {EditorConfig::Sublime, "sublime"}};
  return map.at(editor);
}

std::string_view to_string(CiSystem ci) {
  static const std::unordered_map<CiSystem, std::string_view> map = {
      {CiSystem::GitHub, "github"},     {CiSystem::GitLab, "gitlab"},
      {CiSystem::Travis, "travis"},     {CiSystem::AppVeyor, "appveyor"},
      {CiSystem::AzureDevOps, "azure"}, {CiSystem::CircleCI, "circleci"}};
  return map.at(ci);
}

std::string_view to_string(Language lang) {
  static const std::unordered_map<Language, std::string_view> map = {
      {Language::English, "en"}, {Language::Chinese, "zh"},
      {Language::Spanish, "es"}, {Language::Japanese, "jp"},
      {Language::German, "de"},  {Language::French, "fr"}};
  return map.at(lang);
}

// 字符串转枚举（带验证）
std::optional<TemplateType> to_template_type(std::string_view str) {
  static const std::unordered_map<std::string_view, TemplateType> map = {
      {"console", TemplateType::Console},
      {"lib", TemplateType::Lib},
      {"gui", TemplateType::Gui},
      {"network", TemplateType::Network},
      {"embedded", TemplateType::Embedded},
      {"webservice", TemplateType::WebService},
      {"gameengine", TemplateType::GameEngine}};

  auto it = map.find(str);
  if (it != map.end()) {
    return it->second;
  }
  return std::nullopt;
}

std::optional<BuildSystem> to_build_system(std::string_view str) {
  static const std::unordered_map<std::string_view, BuildSystem> map = {
      {"cmake", BuildSystem::CMake},     {"meson", BuildSystem::Meson},
      {"bazel", BuildSystem::Bazel},     {"xmake", BuildSystem::XMake},
      {"premake", BuildSystem::Premake}, {"make", BuildSystem::Make},
      {"ninja", BuildSystem::Ninja}};

  auto it = map.find(str);
  if (it != map.end()) {
    return it->second;
  }
  return std::nullopt;
}

// 其他枚举转换实现...

// 获取所有枚举值的字符串表示
std::vector<std::string_view> all_template_types() {
  return {"console",  "lib",        "gui",       "network",
          "embedded", "webservice", "gameengine"};
}

std::vector<std::string_view> all_build_systems() {
  return {"cmake", "meson", "bazel", "xmake", "premake", "make", "ninja"};
}

std::vector<std::string_view> all_package_managers() {
  return {"vcpkg", "conan", "none", "spack", "hunter"};
}

// 其他枚举列表实现...
} // namespace enums

// 本地化实现
void Localization::initialize() { loadLanguageStrings(); }

std::string_view Localization::get(std::string_view key, Language lang) {
  auto langStr = std::string(key);
  auto langIt = s_strings.find(langStr);

  if (langIt != s_strings.end()) {
    auto strIt = langIt->second.find(lang);
    if (strIt != langIt->second.end()) {
      return strIt->second;
    }

    // 尝试使用英语作为后备
    if (lang != Language::English) {
      strIt = langIt->second.find(Language::English);
      if (strIt != langIt->second.end()) {
        return strIt->second;
      }
    }
  }

  // 返回键作为后备
  return key;
}

void Localization::loadLanguageStrings() {
  // 英语字符串（默认）
  s_strings["welcomeMessage"][Language::English] =
      "Welcome to CPP-Scaffold! Let's create your C++ project.";
  s_strings["projectName"][Language::English] = "Project name";
  s_strings["projectNameEmpty"][Language::English] =
      "Project name cannot be empty.";
  s_strings["selectProjectType"][Language::English] = "Select project type";
  s_strings["selectBuildSystem"][Language::English] = "Select build system";
  s_strings["selectPackageManager"][Language::English] =
      "Select package manager";
  s_strings["includeTests"][Language::English] = "Include unit test framework?";
  s_strings["selectTestFramework"][Language::English] = "Select test framework";
  s_strings["includeDocumentation"][Language::English] =
      "Include documentation configuration?";
  s_strings["includeCodeStyleTools"][Language::English] =
      "Include code style and static analysis tools?";
  s_strings["selectEditors"][Language::English] = "Select editors to configure";
  s_strings["selectCISystems"][Language::English] =
      "Select CI/CD systems to configure";
  s_strings["initGit"][Language::English] = "Initialize Git repository?";
  s_strings["invalidSelection"][Language::English] =
      "Invalid selection, please try again.";
  s_strings["pleaseSelect"][Language::English] = "Please select";
  s_strings["saveAsDefault"][Language::English] =
      "Save these options as default?";
  s_strings["continueOrDone"][Language::English] =
      "Continue selecting or type 'done' to finish";

  // 中文字符串
  s_strings["welcomeMessage"][Language::Chinese] =
      "欢迎使用 CPP-Scaffold! 让我们创建您的 C++ 项目。";
  s_strings["projectName"][Language::Chinese] = "项目名称";
  s_strings["projectNameEmpty"][Language::Chinese] = "项目名称不能为空。";
  s_strings["selectProjectType"][Language::Chinese] = "选择项目类型";
  s_strings["selectBuildSystem"][Language::Chinese] = "选择构建系统";
  s_strings["selectPackageManager"][Language::Chinese] = "选择包管理器";
  s_strings["includeTests"][Language::Chinese] = "包含单元测试框架？";
  s_strings["selectTestFramework"][Language::Chinese] = "选择测试框架";
  s_strings["includeDocumentation"][Language::Chinese] =
      "是否包含项目文档配置?";
  s_strings["includeCodeStyleTools"][Language::Chinese] =
      "是否包含代码风格和静态分析工具?";
  s_strings["selectEditors"][Language::Chinese] = "选择要配置的编辑器支持";
  s_strings["selectCISystems"][Language::Chinese] = "选择要配置的CI/CD系统";
  s_strings["initGit"][Language::Chinese] = "初始化 Git 仓库？";
  s_strings["invalidSelection"][Language::Chinese] = "选择无效，请重试。";
  s_strings["pleaseSelect"][Language::Chinese] = "请选择";
  s_strings["saveAsDefault"][Language::Chinese] =
      "是否保存这些选项作为默认配置?";
  s_strings["continueOrDone"][Language::Chinese] = "继续选择或输入 'done' 完成";

  // 可以添加更多语言
}

// 用户输入实现
std::string UserInput::read(std::string_view prompt,
                            std::optional<std::string_view> defaultValue,
                            std::optional<ValidatorFunction> validator) {
  std::string input;

  while (true) {
    // 显示提示和默认值
    if (defaultValue.has_value()) {
      fmt::print("{} [{}]: ", prompt, *defaultValue);
    } else {
      fmt::print("{}: ", prompt);
    }

    // 获取用户输入
    std::getline(std::cin, input);

    // 如果输入为空且有默认值，使用默认值
    if (input.empty() && defaultValue.has_value()) {
      return std::string(*defaultValue);
    }

    // 验证输入
    if (validator.has_value()) {
      auto result = (*validator)(input);

      if (std::holds_alternative<bool>(result)) {
        if (std::get<bool>(result)) {
          break; // 验证通过
        }
      } else {
        // 显示验证错误信息
        fmt::print(fmt::fg(fmt::color::red), "{}\n",
                   std::get<std::string>(result));
      }
    } else {
      break; // 无需验证
    }
  }

  return input;
}

UserInput::ValidatorFunction UserInput::notEmptyValidator() {
  return [](const std::string &input) -> ValidatorResult {
    return !input.empty() || "Input cannot be empty";
  };
}

// 读取用户确认
bool UserInput::readConfirmation(std::string_view prompt, bool defaultValue) {
  std::string defaultStr = defaultValue ? "Y/n" : "y/N";
  std::string input = read(prompt, defaultStr);

  if (input.empty()) {
    return defaultValue;
  }

  std::string lowerInput = StringUtils::toLower(input);
  return lowerInput == "y" || lowerInput == "yes";
}

// 读取用户选择
std::string
UserInput::readChoice(std::string_view prompt,
                      const std::vector<std::string_view> &choices,
                      std::optional<std::string_view> defaultValue) {
  fmt::print("{}\n", prompt);
  printOptions(choices, defaultValue);

  while (true) {
    std::string input = read(Localization::get("pleaseSelect"), defaultValue);

    // 检查是否是数字
    if (std::all_of(input.begin(), input.end(),
                    [](char c) { return std::isdigit(c); })) {
      int index = std::stoi(input) - 1;
      if (index >= 0 && index < static_cast<int>(choices.size())) {
        return std::string(choices[index]);
      }
    }
    // 检查是否是选项之一
    else {
      for (const auto &choice : choices) {
        if (StringUtils::toLower(input) ==
            StringUtils::toLower(std::string(choice))) {
          return std::string(choice);
        }
      }
    }

    fmt::print(fmt::fg(fmt::color::red), "{}\n",
               Localization::get("invalidSelection"));
  }
}

// 多选实现
std::vector<std::string>
UserInput::readMultiChoice(std::string_view prompt,
                           const std::vector<std::string_view> &choices,
                           const std::vector<std::string_view> &defaultValues) {
  std::vector<std::string> selectedOptions;

  // 转换默认值为字符串
  for (const auto &val : defaultValues) {
    selectedOptions.push_back(std::string(val));
  }

  fmt::print("{}\n", prompt);
  printMultiOptions(choices, selectedOptions);

  fmt::print("Enter numbers to toggle selection (comma or space separated),\n");
  fmt::print("or 'all' to select all, 'none' to clear, 'done' when finished: ");

  std::string input;
  while (true) {
    std::getline(std::cin, input);
    input = StringUtils::toLower(input);

    if (input == "done") {
      break;
    } else if (input == "all") {
      selectedOptions.clear();
      for (const auto &choice : choices) {
        selectedOptions.push_back(std::string(choice));
      }
      printMultiOptions(choices, selectedOptions);
    } else if (input == "none") {
      selectedOptions.clear();
      printMultiOptions(choices, selectedOptions);
    } else {
      // 解析输入，支持逗号和空格分隔
      std::vector<std::string> selections;
      std::string token;
      std::istringstream tokenStream(input);

      while (std::getline(tokenStream, token, ',')) {
        token = StringUtils::trim(token);
        if (!token.empty()) {
          selections.push_back(token);
        }
      }

      // 处理每个选择
      for (const auto &selection : selections) {
        // 先尝试作为数字
        if (std::all_of(selection.begin(), selection.end(),
                        [](char c) { return std::isdigit(c); })) {
          int index = std::stoi(selection) - 1;
          if (index >= 0 && index < static_cast<int>(choices.size())) {
            std::string choice = std::string(choices[index]);
            // 切换选择状态
            auto it = std::find(selectedOptions.begin(), selectedOptions.end(),
                                choice);
            if (it != selectedOptions.end()) {
              selectedOptions.erase(it);
            } else {
              selectedOptions.push_back(choice);
            }
          }
        }
        // 尝试作为选项名称
        else {
          for (const auto &choice : choices) {
            if (StringUtils::toLower(selection) ==
                StringUtils::toLower(std::string(choice))) {
              std::string choiceStr = std::string(choice);
              // 切换选择状态
              auto it = std::find(selectedOptions.begin(),
                                  selectedOptions.end(), choiceStr);
              if (it != selectedOptions.end()) {
                selectedOptions.erase(it);
              } else {
                selectedOptions.push_back(choiceStr);
              }
              break;
            }
          }
        }
      }

      printMultiOptions(choices, selectedOptions);
    }

    fmt::print("{}: ", Localization::get("continueOrDone"));
  }

  return selectedOptions;
}

// 打印选项
void UserInput::printOptions(const std::vector<std::string_view> &options,
                             std::optional<std::string_view> defaultOption) {
  for (size_t i = 0; i < options.size(); i++) {
    std::string marker;
    if (defaultOption.has_value() && options[i] == *defaultOption) {
      marker = " (default)";
      fmt::print("  {}. **{}**{}\n", i + 1, options[i], marker);
    } else {
      fmt::print("  {}. {}\n", i + 1, options[i]);
    }
  }
}

// 打印多选选项
void UserInput::printMultiOptions(
    const std::vector<std::string_view> &options,
    const std::vector<std::string> &selectedOptions) {
  for (size_t i = 0; i < options.size(); i++) {
    bool isSelected =
        std::find(selectedOptions.begin(), selectedOptions.end(),
                  std::string(options[i])) != selectedOptions.end();

    std::string marker = isSelected ? " [x]" : " [ ]";

    if (isSelected) {
      fmt::print("  {}. **{}**{}\n", i + 1, options[i], marker);
    } else {
      fmt::print("  {}. {}{}\n", i + 1, options[i], marker);
    }
  }
}

// 配置管理器实现
CliOptions ConfigManager::loadDefaultOptions() {
  CliOptions options;

  try {
    std::ifstream configFile(getConfigFilePath());
    if (!configFile.is_open()) {
      spdlog::info("未找到默认配置，使用内置默认值");
      return options;
    }

    json config = json::parse(configFile);

    // 读取基本设置
    if (config.contains("templateType")) {
      auto templateType =
          enums::to_template_type(config["templateType"].get<std::string>());
      if (templateType) {
        options.templateType = *templateType;
      }
    }

    if (config.contains("buildSystem")) {
      auto buildSystem =
          enums::to_build_system(config["buildSystem"].get<std::string>());
      if (buildSystem) {
        options.buildSystem = *buildSystem;
      }
    }

    // 读取其他基本设置...

    // 读取数组设置
    if (config.contains("editorOptions") &&
        config["editorOptions"].is_array()) {
      options.editorOptions.clear();
      for (const auto &editor : config["editorOptions"]) {
        auto editorConfig = enums::to_editor_config(editor.get<std::string>());
        if (editorConfig) {
          options.editorOptions.push_back(*editorConfig);
        }
      }
    }

    // 读取CI选项...

    return options;
  } catch (const std::exception &e) {
    spdlog::error("加载默认选项时出错: {}", e.what());
    return options;
  }
}

// 保存选项作为默认配置
bool ConfigManager::saveOptionsAsDefaults(const CliOptions &options) {
  try {
    json config;

    // 保存基本设置
    config["templateType"] =
        std::string(enums::to_string(options.templateType));
    config["buildSystem"] = std::string(enums::to_string(options.buildSystem));
    config["packageManager"] =
        std::string(enums::to_string(options.packageManager));

    if (options.networkLibrary) {
      config["networkLibrary"] = *options.networkLibrary;
    }

    config["includeTests"] = options.includeTests;
    config["testFramework"] =
        std::string(enums::to_string(options.testFramework));
    config["includeDocumentation"] = options.includeDocumentation;
    config["includeCodeStyleTools"] = options.includeCodeStyleTools;
    config["initGit"] = options.initGit;
    config["language"] = std::string(enums::to_string(options.language));

    // 保存数组设置
    json editorArray = json::array();
    for (const auto &editor : options.editorOptions) {
      editorArray.push_back(std::string(enums::to_string(editor)));
    }
    config["editorOptions"] = editorArray;

    json ciArray = json::array();
    for (const auto &ci : options.ciOptions) {
      ciArray.push_back(std::string(enums::to_string(ci)));
    }
    config["ciOptions"] = ciArray;

    // 创建配置目录（如果不存在）
    std::filesystem::path configPath = getConfigFilePath();
    std::filesystem::create_directories(configPath.parent_path());

    // 保存到文件
    std::ofstream configFile(configPath);
    if (!configFile.is_open()) {
      spdlog::error("无法打开配置文件进行写入");
      return false;
    }

    configFile << config.dump(2);
    return true;
  } catch (const std::exception &e) {
    spdlog::error("保存默认选项时出错: {}", e.what());
    return false;
  }
}

// 获取配置文件路径
std::filesystem::path ConfigManager::getConfigFilePath() {
  // 使用标准配置文件位置
  const char *homeDir = nullptr;

#ifdef _WIN32
  homeDir = std::getenv("USERPROFILE");
#else
  homeDir = std::getenv("HOME");
#endif

  if (!homeDir) {
    return ".cpp-scaffold.json"; // 如果无法获取家目录，使用当前目录
  }

  std::filesystem::path configDir =
      std::filesystem::path(homeDir) / ".config" / "cpp-scaffold";
  return configDir / "config.json";
}

// 主 CLI 解析器实现
CliOptions CliParser::parse(int argc, char *argv[]) {
  CliOptions options;

  // 初始化本地化
  Localization::initialize();

  if (argc <= 1) {
    // 无参数时直接启动交互式模式
    return runInteractiveMode();
  }

  std::string_view command = argv[1];
  if (command == "help" || command == "--help" || command == "-h") {
    options.showHelp = true;
    return options;
  }

  if (command == "version" || command == "--version" || command == "-v") {
    options.version = true;
    return options;
  }

  if (command == "interactive" || command == "-i") {
    return runInteractiveMode();
  }

  if (command != "create" && command != "new") {
    spdlog::error("未知命令: {}", command);
    options.showHelp = true;
    return options;
  }

  // 如果有项目名称，设置它
  if (argc > 2) {
    options.projectName = argv[2];
  }

  // 解析其他参数
  for (int i = 3; i < argc; i++) {
    std::string_view arg = argv[i];

    if (arg == "--template" || arg == "-t") {
      if (i + 1 < argc) {
        auto templateType = enums::to_template_type(argv[++i]);
        if (templateType) {
          options.templateType = *templateType;
        }
      }
    } else if (arg == "--build" || arg == "-b") {
      if (i + 1 < argc) {
        auto buildSystem = enums::to_build_system(argv[++i]);
        if (buildSystem) {
          options.buildSystem = *buildSystem;
        }
      }
    } else if (arg == "--package" || arg == "-p") {
      if (i + 1 < argc) {
        auto packageManager = enums::to_package_manager(argv[++i]);
        if (packageManager) {
          options.packageManager = *packageManager;
        }
      }
    } else if (arg == "--network-lib") {
      if (i + 1 < argc) {
        options.networkLibrary = argv[++i];
      }
    } else if (arg == "--tests") {
      options.includeTests = true;
      if (i + 1 < argc && argv[i + 1][0] != '-') {
        auto testFramework = enums::to_test_framework(argv[++i]);
        if (testFramework) {
          options.testFramework = *testFramework;
        }
      }
    } else if (arg == "--docs" || arg == "--documentation") {
      options.includeDocumentation = true;
    } else if (arg == "--code-style") {
      options.includeCodeStyleTools = true;
    } else if (arg == "--editor") {
      if (i + 1 < argc) {
        auto editor = enums::to_editor_config(argv[++i]);
        if (editor) {
          options.editorOptions.push_back(*editor);
        }
      }
    } else if (arg == "--ci" || arg == "--cicd") {
      if (i + 1 < argc) {
        auto ci = enums::to_ci_system(argv[++i]);
        if (ci) {
          options.ciOptions.push_back(*ci);
        }
      }
    } else if (arg == "--no-git") {
      options.initGit = false;
    } else if (arg == "--language" || arg == "-l") {
      if (i + 1 < argc) {
        auto language = enums::to_language(argv[++i]);
        if (language) {
          options.language = *language;
          Localization::setCurrentLanguage(options.language);
        }
      }
    } else if (arg == "--verbose") {
      options.verbose = true;
    } else if (arg == "--template-path") {
      if (i + 1 < argc) {
        options.customTemplatePath = argv[++i];
      }
    }
    // 处理其他命令行选项...
  }

  // 如果没有足够信息，通过交互式提示获取
  if (options.projectName.empty()) {
    options = promptUserForOptions(options);
  }

  return options;
}

// 显示帮助信息
void CliParser::showHelp([[maybe_unused]] Language lang) {
  std::cout << TerminalUtils::colorize("**CPP-Scaffold - C++ 项目脚手架工具**",
                                       utils::Color::BrightCyan)
            << "\n\n";

  std::cout << TerminalUtils::colorize("用法:", utils::Color::BrightYellow)
            << "\n";
  std::cout << "  cpp-scaffold create <项目名称> [选项]\n";
  std::cout << "  cpp-scaffold new <项目名称> [选项]\n";
  std::cout << "  cpp-scaffold interactive\n\n";

  std::cout << TerminalUtils::colorize("**选项:**", utils::Color::BrightYellow)
            << "\n";
  fmt::print("  -t, --template <类型>        项目模板类型: console, lib, gui, "
             "network, embedded, webservice, gameengine\n");
  fmt::print("  -b, --build <系统>           构建系统: cmake, meson, bazel, "
             "xmake, premake, make, ninja\n");
  fmt::print("  -p, --package <管理器>       包管理器: vcpkg, conan, none, "
             "spack, hunter\n");
  fmt::print(
      "  --network-lib <库>           网络项目的网络库: asio, boost, poco\n");
  fmt::print("  --tests [框架]               包含测试框架: gtest, catch2, "
             "doctest, boost, none\n");
  fmt::print("  --docs, --documentation      包含文档配置\n");
  fmt::print("  --code-style                 包含代码风格和静态分析工具\n");
  fmt::print("  --editor <编辑器>            配置编辑器支持: vscode, clion, "
             "vs, vim, emacs, sublime\n");
  fmt::print("                               (可多次使用)\n");
  fmt::print("  --ci, --cicd <系统>          配置CI/CD: github, gitlab, "
             "travis, appveyor, azure, circleci\n");
  fmt::print("                               (可多次使用)\n");
  fmt::print("  --no-git                     不初始化Git仓库\n");
  fmt::print("  --profile <名称>             使用保存的配置文件\n");
  fmt::print("  --template-path <路径>       使用自定义项目模板\n");
  fmt::print(
      "  -l, --language <语言>        界面语言: en, zh, es, jp, de, fr\n");
  fmt::print("  --verbose                    显示详细输出\n");
  fmt::print("  -h, --help                   显示此帮助信息\n");
  fmt::print("  -v, --version                显示版本信息\n\n");

  fmt::print("**示例:**\n");
  fmt::print("  cpp-scaffold create my-app --template console --build cmake "
             "--package vcpkg --tests\n");
  fmt::print("  cpp-scaffold new my-lib -t lib -b cmake -p none --docs "
             "--code-style\n");
  fmt::print(
      "  cpp-scaffold create my-app --ci github --ci gitlab --editor vscode\n");
  fmt::print("  cpp-scaffold create my-app --profile webservice\n");
}

// 显示版本信息
void CliParser::showVersion() { fmt::print("CPP-Scaffold Version 1.1.0\n"); }

// 交互式提示获取选项
CliOptions CliParser::promptUserForOptions(const CliOptions &defaultOptions) {
  CliOptions options = defaultOptions;
  Language lang = Localization::getCurrentLanguage();

  // 使用终端工具显示欢迎信息
  TerminalUtils::showInfo(
      std::string(Localization::get("welcomeMessage", lang)));
  std::cout << "\n";

  // 加载默认配置
  CliOptions defaults = ConfigManager::loadDefaultOptions();

  // 基于操作系统等的系统建议默认值
  CliOptions systemDefaults = getSystemSuggestedDefaults();

  // 如果没有项目名称，询问
  if (options.projectName.empty()) {
    options.projectName =
        UserInput::readWithHighlight(Localization::get("projectName", lang),
                                     std::nullopt, utils::Color::BrightCyan);

    if (options.projectName.empty()) {
      TerminalUtils::showError(
          std::string(Localization::get("projectNameEmpty", lang)));
      options.projectName =
          UserInput::readWithHighlight(Localization::get("projectName", lang),
                                       std::nullopt, utils::Color::BrightCyan);
    }
  }

  // 模板类型
  if (options.templateType == defaults.templateType) {
    std::string templateTypeStr = UserInput::readChoiceWithStyle(
        Localization::get("selectProjectType", lang),
        enums::all_template_types(), enums::to_string(defaults.templateType),
        utils::Color::BrightGreen);

    auto templateType = enums::to_template_type(templateTypeStr);
    if (templateType) {
      options.templateType = *templateType;
    }
  }

  // 构建系统
  if (options.buildSystem == defaults.buildSystem) {
    std::string buildSystemStr = UserInput::readChoiceWithStyle(
        Localization::get("selectBuildSystem", lang),
        enums::all_build_systems(), enums::to_string(defaults.buildSystem),
        utils::Color::BrightGreen);

    auto buildSystem = enums::to_build_system(buildSystemStr);
    if (buildSystem) {
      options.buildSystem = *buildSystem;
    }
  }

  // 询问是否包含测试框架
  options.includeTests = UserInput::readConfirmation(
      Localization::get("includeTests", lang), defaults.includeTests);

  if (options.includeTests) {
    std::string testFrameworkStr = UserInput::readChoiceWithStyle(
        Localization::get("selectTestFramework", lang),
        enums::all_test_frameworks(), enums::to_string(defaults.testFramework),
        utils::Color::BrightGreen);

    auto testFramework = enums::to_test_framework(testFrameworkStr);
    if (testFramework) {
      options.testFramework = *testFramework;
    }
  }

  // 询问其他选项...

  // 询问是否保存为默认值
  if (UserInput::readConfirmation(Localization::get("saveAsDefault", lang),
                                  false)) {
    ConfigManager::saveOptionsAsDefaults(options);
  }

  return options;
}

// 验证选项的一致性和完整性
bool CliParser::validateOptions(CliOptions &options) {
  bool isValid = true;

  // 项目名称验证
  if (options.projectName.empty()) {
    spdlog::error("项目名称不能为空");
    isValid = false;
  }

  // 检查网络项目的网络库
  if (options.templateType == TemplateType::Network &&
      !options.networkLibrary.has_value()) {
    spdlog::warn("网络项目未指定网络库，默认使用'asio'");
    options.networkLibrary = "asio";
  }

  // 检查测试框架
  if (options.includeTests && options.testFramework == TestFramework::None) {
    spdlog::warn("包含测试但未指定测试框架，默认使用GTest");
    options.testFramework = TestFramework::GTest;
  }

  return isValid;
}

// 检测操作系统以提供默认建议
std::string CliParser::detectOS() {
#ifdef _WIN32
  return "windows";
#elif defined(__APPLE__)
  return "macos";
#elif defined(__linux__)
  return "linux";
#else
  return "unknown";
#endif
}

// 获取基于系统环境的建议默认值
CliOptions CliParser::getSystemSuggestedDefaults() {
  CliOptions options;
  std::string os = detectOS();

  // 基于操作系统设置默认值
  if (os == "windows") {
    options.buildSystem = BuildSystem::CMake;
    options.packageManager = PackageManager::Vcpkg;
    options.editorOptions = {EditorConfig::VS, EditorConfig::VSCode};
  } else if (os == "macos") {
    options.buildSystem = BuildSystem::CMake;
    options.packageManager = PackageManager::Conan;
    options.editorOptions = {EditorConfig::VSCode, EditorConfig::CLion};
  } else if (os == "linux") {
    options.buildSystem = BuildSystem::CMake;
    options.packageManager = PackageManager::Conan;
    options.editorOptions = {EditorConfig::VSCode, EditorConfig::Vim};
  }

  return options;
}

// 修改 UserInput 实现，使用终端工具增强视觉效果
std::string
UserInput::readWithHighlight(std::string_view prompt,
                             std::optional<std::string_view> defaultValue,
                             utils::Color promptColor) {
  std::string promptStr = std::string(prompt);

  // 为默认值添加说明
  if (defaultValue.has_value()) {
    promptStr += " [" + std::string(*defaultValue) + "]";
  }

  // 使用终端工具着色
  std::cout << TerminalUtils::colorize(promptStr, promptColor) << ": ";

  std::string input;
  std::getline(std::cin, input);

  // 如果输入为空且有默认值，返回默认值
  if (input.empty() && defaultValue.has_value()) {
    return std::string(*defaultValue);
  }

  return input;
}

std::string UserInput::readChoiceWithStyle(
    std::string_view prompt, const std::vector<std::string_view> &choices,
    std::optional<std::string_view> defaultValue, utils::Color highlightColor) {
  // 显示带颜色的提示
  std::cout << TerminalUtils::colorize(std::string(prompt),
                                       utils::Color::BrightCyan)
            << "\n";

  // 显示带样式的选项
  printStyledOptions(choices, defaultValue, highlightColor);

  // 获取用户输入
  while (true) {
    std::string pleaseSelect = std::string(Localization::get("pleaseSelect"));

    if (defaultValue.has_value()) {
      pleaseSelect += " [" + std::string(*defaultValue) + "]";
    }

    std::cout << TerminalUtils::colorize(pleaseSelect, utils::Color::White)
              << ": ";

    std::string input;
    std::getline(std::cin, input);

    // 处理空输入
    if (input.empty() && defaultValue.has_value()) {
      return std::string(*defaultValue);
    }

    // 检查数字选择
    if (std::all_of(input.begin(), input.end(),
                    [](char c) { return std::isdigit(c); })) {
      int index = std::stoi(input) - 1;
      if (index >= 0 && index < static_cast<int>(choices.size())) {
        // 突出显示所选项
        TerminalUtils::clearLine();
        std::cout << TerminalUtils::colorize("✓ 已选择: " +
                                                 std::string(choices[index]),
                                             utils::Color::BrightGreen)
                  << "\n";
        return std::string(choices[index]);
      }
    }

    // 检查名称选择
    for (const auto &choice : choices) {
      if (StringUtils::toLower(input) ==
          StringUtils::toLower(std::string(choice))) {
        // 突出显示所选项
        TerminalUtils::clearLine();
        std::cout << TerminalUtils::colorize("✓ 已选择: " + std::string(choice),
                                             utils::Color::BrightGreen)
                  << "\n";
        return std::string(choice);
      }
    }

    // 无效输入
    TerminalUtils::showError("无效选择，请重试。");
  }
}

void UserInput::printStyledOptions(
    const std::vector<std::string_view> &options,
    std::optional<std::string_view> defaultOption,
    utils::Color highlightColor) {
  int maxNumWidth = static_cast<int>(std::to_string(options.size()).length());

  for (size_t i = 0; i < options.size(); i++) {
    std::string optionLine = "  " + std::to_string(i + 1) + ".";
    // 填充空格以对齐
    optionLine += std::string(maxNumWidth + 2 - optionLine.length(), ' ');

    // 默认选项使用特殊颜色
    if (defaultOption.has_value() && options[i] == *defaultOption) {
      optionLine += std::string(options[i]) + " (默认)";
      std::cout << TerminalUtils::colorize(optionLine, highlightColor) << "\n";
    } else {
      optionLine += std::string(options[i]);
      std::cout << optionLine << "\n";
    }
  }
  std::cout << "\n";
}

// 添加交互模式入口
CliOptions CliParser::runInteractiveMode() {
  showCliHeader();
  return ProjectWizard::runWizard();
}

// 显示漂亮的CLI标题
void CliParser::showCliHeader() {
  TerminalUtils::clearLine();

  std::vector<std::string> headerLines = {"C++ Project Scaffold",
                                          "交互式项目创建向导", "版本 1.1.0"};

  // 使用框显示标题
  TerminalUtils::showBox(headerLines, BorderStyle::Single,
                         utils::Color::BrightCyan, utils::Color::Reset, "");
  std::cout << "\n";
}
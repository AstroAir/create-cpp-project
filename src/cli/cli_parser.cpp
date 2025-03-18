#include "cli_parser.h"

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <string>

#include "../utils/string_utils.h"
#include <spdlog/spdlog.h>

using namespace utils;
using json = nlohmann::json;

// 初始化本地化字符串映射
std::map<std::string, std::map<std::string, std::string>>
    CliParser::localizationStrings = {
        {"en",
         {{"welcomeMessage",
           "Welcome to CPP-Scaffold! Let's create your C++ project."},
          {"projectName", "Project name"},
          {"projectNameEmpty", "Project name cannot be empty."},
          {"selectProjectType", "Select project type"},
          {"selectBuildSystem", "Select build system"},
          {"selectPackageManager", "Select package manager"},
          {"includeTests", "Include unit test framework?"},
          {"selectTestFramework", "Select test framework"},
          {"initGit", "Initialize Git repository?"},
          {"invalidSelection", "Invalid selection, please try again."},
          {"pleaseSelect", "Please select"}}},
        {"zh",
         {{"welcomeMessage",
           "欢迎使用 CPP-Scaffold! 让我们创建您的 C++ 项目。"},
          {"projectName", "项目名称"},
          {"projectNameEmpty", "项目名称不能为空。"},
          {"selectProjectType", "选择项目类型"},
          {"selectBuildSystem", "选择构建系统"},
          {"selectPackageManager", "选择包管理器"},
          {"includeTests", "包含单元测试框架？"},
          {"selectTestFramework", "选择测试框架"},
          {"initGit", "初始化 Git 仓库？"},
          {"invalidSelection", "选择无效，请重试。"},
          {"pleaseSelect", "请选择"}}}};

// 获取本地化字符串
std::string CliParser::getLocalizedString(const std::string &key,
                                          const std::string &language) {
  auto langIt = localizationStrings.find(language);
  if (langIt != localizationStrings.end()) {
    auto strIt = langIt->second.find(key);
    if (strIt != langIt->second.end()) {
      return strIt->second;
    }
  }

  // 如果找不到指定语言或键，尝试使用英语
  if (language != "en") {
    return getLocalizedString(key, "en");
  }

  // 如果英语也找不到，返回键名作为默认值
  return key;
}

// Parses command line arguments
CliOptions CliParser::parse(int argc, char *argv[]) {
  CliOptions options;

  if (argc <= 1) {
    options.showHelp = true;
    return options;
  }

  std::string command = argv[1];
  if (command == "help" || command == "--help" || command == "-h") {
    options.showHelp = true;
    return options;
  }

  if (command == "version" || command == "--version" || command == "-v") {
    options.version = true;
    return options;
  }

  if (command != "create" && command != "new") {
    spdlog::error("Unknown command: {}", command);
    options.showHelp = true;
    return options;
  }

  // If there is a project name, set it
  if (argc > 2) {
    options.projectName = argv[2];
  }

  // Parse other arguments
  for (int i = 3; i < argc; i++) {
    std::string arg = argv[i];

    if (arg == "--template" || arg == "-t") {
      if (i + 1 < argc) {
        options.templateType = argv[++i];
      }
    } else if (arg == "--build" || arg == "-b") {
      if (i + 1 < argc) {
        options.buildSystem = argv[++i];
      }
    } else if (arg == "--package" || arg == "-p") {
      if (i + 1 < argc) {
        options.packageManager = argv[++i];
      }
    } else if (arg == "--tests") {
      options.includeTests = true;
      if (i + 1 < argc && argv[i + 1][0] != '-') {
        options.testFramework = argv[++i];
      } else {
        options.testFramework = "gtest"; // Default testing framework
      }
    } else if (arg == "--docs" || arg == "--documentation") {
      options.includeDocumentation = true;
    } else if (arg == "--code-style") {
      options.includeCodeStyleTools = true;
    } else if (arg == "--editor") {
      if (i + 1 < argc) {
        options.editorOptions.push_back(argv[++i]);
      }
    } else if (arg == "--ci" || arg == "--cicd") {
      if (i + 1 < argc) {
        options.ciOptions.push_back(argv[++i]);
      }
    } else if (arg == "--no-git") {
      options.initGit = false;
    } else if (arg == "--language" || arg == "-l") {
      if (i + 1 < argc) {
        options.language = argv[++i];
      }
    }
  }

  // If there is not enough information, get it through interactive prompts
  if (options.projectName.empty() || options.templateType.empty() ||
      options.buildSystem.empty() || options.packageManager.empty()) {
    options = promptUserForOptions(options);
  }

  return options;
}

// Show help information
void CliParser::showHelp(const std::string &language) {
  std::cout << "CPP-Scaffold - C++ project scaffolding tool\n\n";
  std::cout << "Usage:\n";
  std::cout << "  cpp-scaffold create <project name> [options]\n";
  std::cout << "  cpp-scaffold new <project name> [options]\n\n";
  std::cout << "Options:\n";
  std::cout << "  -t, --template <type>      Project template type: console, "
               "lib, gui, network, embedded\n";
  std::cout << "  -b, --build <build system>     Build system: cmake, meson, "
               "bazel, xmake, premake\n";
  std::cout << "  -p, --package <package manager>   Package manager: vcpkg, "
               "conan, none\n";
  std::cout << "  --tests [test framework]         Include test framework: "
               "gtest, catch2, doctest\n";
  std::cout << "  --docs, --documentation          Include documentation "
               "configuration\n";
  std::cout << "  --code-style                     Include code style and "
               "static analysis tools\n";
  std::cout << "  --editor <editor>                Configure editor support: "
               "vscode, clion, vs\n";
  std::cout
      << "                                  (can be used multiple times)\n";
  std::cout << "  --ci, --cicd <ci system>         Configure CI/CD: github, "
               "gitlab, travis, appveyor\n";
  std::cout
      << "                                  (can be used multiple times)\n";
  std::cout << "  --no-git                         Do not initialize Git "
               "repository\n";
  std::cout
      << "  -l, --language <lang>            Interface language: en, zh\n";
  std::cout
      << "  -h, --help                       Show this help information\n";
  std::cout
      << "  -v, --version                    Show version information\n\n";
  std::cout << "Examples:\n";
  std::cout << "  cpp-scaffold create my-app --template console --build cmake "
               "--package vcpkg --tests\n";
  std::cout << "  cpp-scaffold new my-lib -t lib -b cmake -p none --docs "
               "--code-style\n";
  std::cout << "  cpp-scaffold create my-app --ci github --ci gitlab --editor "
               "vscode\n";
}

// Show version information
void CliParser::showVersion() { std::cout << "CPP-Scaffold Version 1.0.0\n"; }

// Interactive prompt to get options
CliOptions CliParser::promptUserForOptions(const CliOptions &defaultOptions) {
  CliOptions options = defaultOptions;
  std::string lang = options.language;

  std::cout << getLocalizedString("welcomeMessage", lang) << "\n\n";

  // 加载默认配置
  CliOptions defaults = loadDefaultOptions();

  // If there is no project name, ask
  if (options.projectName.empty()) {
    options.projectName =
        readUserInput(getLocalizedString("projectName", lang));
    while (options.projectName.empty()) {
      std::cout << getLocalizedString("projectNameEmpty", lang) << "\n";
      options.projectName =
          readUserInput(getLocalizedString("projectName", lang));
    }
  }

  // If there is no template type, ask
  if (options.templateType.empty()) {
    std::vector<std::string> templateChoices = {"console", "lib", "gui",
                                                "network", "embedded"};
    options.templateType = readUserChoice(
        getLocalizedString("selectProjectType", lang), templateChoices,
        defaults.templateType.empty() ? "console" : defaults.templateType);
  }

  // If there is no build system, ask
  if (options.buildSystem.empty()) {
    std::vector<std::string> buildChoices = {"cmake", "meson", "bazel", "xmake",
                                             "premake"};
    options.buildSystem = readUserChoice(
        getLocalizedString("selectBuildSystem", lang), buildChoices,
        defaults.buildSystem.empty() ? "cmake" : defaults.buildSystem);
  }

  // If there is no package manager, ask
  if (options.packageManager.empty()) {
    std::vector<std::string> packageChoices = {"vcpkg", "conan", "none"};
    options.packageManager = readUserChoice(
        getLocalizedString("selectPackageManager", lang), packageChoices,
        defaults.packageManager.empty() ? "vcpkg" : defaults.packageManager);
  }

  // Ask whether to include the test framework
  options.includeTests = readUserConfirmation(
      getLocalizedString("includeTests", lang), defaults.includeTests);

  if (options.includeTests) {
    std::vector<std::string> testChoices = {"gtest", "catch2", "doctest"};
    options.testFramework = readUserChoice(
        getLocalizedString("selectTestFramework", lang), testChoices,
        defaults.testFramework.empty() ? "gtest" : defaults.testFramework);
  }

  // 询问是否包含文档配置
  options.includeDocumentation = readUserConfirmation(
      "是否包含项目文档配置?", defaults.includeDocumentation);

  // 询问是否包含代码风格工具
  options.includeCodeStyleTools = readUserConfirmation(
      "是否包含代码风格和静态分析工具?", defaults.includeCodeStyleTools);

  // 询问编辑器配置
  std::vector<std::string> editorChoices = {"vscode", "clion", "vs"};
  options.editorOptions = readUserMultiChoice(
      "选择要配置的编辑器支持", editorChoices, defaults.editorOptions);

  // 询问CI/CD配置
  std::vector<std::string> ciChoices = {"github", "gitlab", "travis",
                                        "appveyor"};
  options.ciOptions = readUserMultiChoice("选择要配置的CI/CD系统", ciChoices,
                                          defaults.ciOptions);

  // Ask whether to initialize git
  options.initGit = readUserConfirmation(getLocalizedString("initGit", lang),
                                         defaults.initGit);

  // 询问是否保存这些选项作为默认配置
  if (readUserConfirmation("是否保存这些选项作为默认配置?", false)) {
    saveOptionsAsDefaults(options);
  }

  return options;
}

// 保存选项作为默认配置
bool CliParser::saveOptionsAsDefaults(const CliOptions &options) {
  try {
    json config;

    // 保存基本选项
    config["templateType"] = options.templateType;
    config["buildSystem"] = options.buildSystem;
    config["packageManager"] = options.packageManager;
    config["includeTests"] = options.includeTests;
    config["testFramework"] = options.testFramework;
    config["includeDocumentation"] = options.includeDocumentation;
    config["includeCodeStyleTools"] = options.includeCodeStyleTools;
    config["initGit"] = options.initGit;
    config["language"] = options.language;

    // 保存数组选项
    config["editorOptions"] = options.editorOptions;
    config["ciOptions"] = options.ciOptions;

    // 将配置写入文件
    std::ofstream configFile(getConfigFilePath());
    if (!configFile.is_open()) {
      spdlog::error("Failed to open config file for writing");
      return false;
    }

    configFile << config.dump(2); // 保存JSON (缩进2个空格)
    return true;
  } catch (const std::exception &e) {
    spdlog::error("Error saving default options: {}", e.what());
    return false;
  }
}

// 从配置文件加载默认选项
CliOptions CliParser::loadDefaultOptions() {
  CliOptions options;

  try {
    std::ifstream configFile(getConfigFilePath());
    if (!configFile.is_open()) {
      spdlog::info("No default config found, using built-in defaults");
      return options; // 返回默认构造的选项
    }

    json config = json::parse(configFile);

    // 读取基本选项
    if (config.contains("templateType"))
      options.templateType = config["templateType"];
    if (config.contains("buildSystem"))
      options.buildSystem = config["buildSystem"];
    if (config.contains("packageManager"))
      options.packageManager = config["packageManager"];
    if (config.contains("includeTests"))
      options.includeTests = config["includeTests"];
    if (config.contains("testFramework"))
      options.testFramework = config["testFramework"];
    if (config.contains("includeDocumentation"))
      options.includeDocumentation = config["includeDocumentation"];
    if (config.contains("includeCodeStyleTools"))
      options.includeCodeStyleTools = config["includeCodeStyleTools"];
    if (config.contains("initGit"))
      options.initGit = config["initGit"];
    if (config.contains("language"))
      options.language = config["language"];

    // 读取数组选项
    if (config.contains("editorOptions")) {
      options.editorOptions =
          config["editorOptions"].get<std::vector<std::string>>();
    }

    if (config.contains("ciOptions")) {
      options.ciOptions = config["ciOptions"].get<std::vector<std::string>>();
    }

    return options;
  } catch (const std::exception &e) {
    spdlog::error("Error loading default options: {}", e.what());
    return options; // 返回默认构造的选项
  }
}

// Verify that the options are valid
bool CliParser::validateOptions(CliOptions &options) {
  // Verify template type
  std::vector<std::string> validTemplates = {"console", "lib", "gui", "network",
                                             "embedded"};
  if (!options.templateType.empty() &&
      std::find(validTemplates.begin(), validTemplates.end(),
                StringUtils::toLower(options.templateType)) ==
          validTemplates.end()) {
    spdlog::error("Invalid template type: {}", options.templateType);
    return false;
  }

  // Verify build system
  std::vector<std::string> validBuildSystems = {"cmake", "meson", "bazel",
                                                "xmake", "premake"};
  if (!options.buildSystem.empty() &&
      std::find(validBuildSystems.begin(), validBuildSystems.end(),
                StringUtils::toLower(options.buildSystem)) ==
          validBuildSystems.end()) {
    spdlog::error("Invalid build system: {}", options.buildSystem);
    return false;
  }

  // Verify package manager
  std::vector<std::string> validPackageManagers = {"vcpkg", "conan", "none"};
  if (!options.packageManager.empty() &&
      std::find(validPackageManagers.begin(), validPackageManagers.end(),
                StringUtils::toLower(options.packageManager)) ==
          validPackageManagers.end()) {
    spdlog::error("Invalid package manager: {}", options.packageManager);
    return false;
  }

  // Verify test framework
  std::vector<std::string> validTestFrameworks = {"gtest", "catch2", "doctest"};
  if (options.includeTests && !options.testFramework.empty() &&
      std::find(validTestFrameworks.begin(), validTestFrameworks.end(),
                StringUtils::toLower(options.testFramework)) ==
          validTestFrameworks.end()) {
    spdlog::error("Invalid test framework: {}", options.testFramework);
    return false;
  }

  // 验证编辑器选择
  std::vector<std::string> validEditors = {"vscode", "clion", "vs"};
  for (const auto &editor : options.editorOptions) {
    if (std::find(validEditors.begin(), validEditors.end(),
                  StringUtils::toLower(editor)) == validEditors.end()) {
      spdlog::error("Invalid editor: {}", editor);
      return false;
    }
  }

  // 验证CI/CD选择
  std::vector<std::string> validCiSystems = {"github", "gitlab", "travis",
                                             "appveyor"};
  for (const auto &ci : options.ciOptions) {
    if (std::find(validCiSystems.begin(), validCiSystems.end(),
                  StringUtils::toLower(ci)) == validCiSystems.end()) {
      spdlog::error("Invalid CI/CD system: {}", ci);
      return false;
    }
  }

  return true;
}

// Read user input
std::string CliParser::readUserInput(const std::string &prompt,
                                     const std::string &defaultValue) {
  std::string input;
  if (defaultValue.empty()) {
    std::cout << prompt << ": ";
  } else {
    std::cout << prompt << " [" << defaultValue << "]: ";
  }

  std::getline(std::cin, input);
  if (input.empty()) {
    return defaultValue;
  }
  return input;
}

// Read user choice
std::string CliParser::readUserChoice(const std::string &prompt,
                                      const std::vector<std::string> &choices,
                                      const std::string &defaultValue) {
  std::cout << prompt << ":\n";
  printOptions(choices, defaultValue);

  while (true) {
    std::string input = readUserInput("Please select", defaultValue);

    // Check if it is a serial number
    if (std::all_of(input.begin(), input.end(), ::isdigit)) {
      int index = std::stoi(input) - 1;
      if (index >= 0 && index < choices.size()) {
        return choices[index];
      }
    }
    // Check if it is an option name
    else if (std::find(choices.begin(), choices.end(),
                       StringUtils::toLower(input)) != choices.end()) {
      return StringUtils::toLower(input);
    }

    std::cout << "Invalid selection, please try again.\n";
  }
}

// Read user multi-choice selection
std::vector<std::string>
CliParser::readUserMultiChoice(const std::string &prompt,
                               const std::vector<std::string> &choices,
                               const std::vector<std::string> &defaultValues) {

  std::vector<std::string> selectedOptions = defaultValues;

  std::cout << prompt << ":\n";
  printMultiOptions(choices, selectedOptions);

  std::cout
      << "Enter numbers to toggle selection (comma or space separated),\n";
  std::cout
      << "or 'all' to select all, 'none' to clear, 'done' when finished: ";

  std::string input;
  while (true) {
    std::getline(std::cin, input);
    input = StringUtils::toLower(input);

    if (input == "done") {
      break;
    } else if (input == "all") {
      selectedOptions = choices;
      printMultiOptions(choices, selectedOptions);
    } else if (input == "none") {
      selectedOptions.clear();
      printMultiOptions(choices, selectedOptions);
    } else {
      // 分割输入，允许逗号和空格作为分隔符
      std::vector<std::string> selections;
      std::string token;
      std::istringstream tokenStream(input);

      while (std::getline(tokenStream, token, ',')) {
        token = StringUtils::trim(token);
        if (!token.empty()) {
          selections.push_back(token);
        }
      }

      for (const auto &selection : selections) {
        // 检查是否为数字
        if (std::all_of(selection.begin(), selection.end(), ::isdigit)) {
          int index = std::stoi(selection) - 1;
          if (index >= 0 && index < choices.size()) {
            // 切换选择状态
            auto it = std::find(selectedOptions.begin(), selectedOptions.end(),
                                choices[index]);
            if (it != selectedOptions.end()) {
              selectedOptions.erase(it);
            } else {
              selectedOptions.push_back(choices[index]);
            }
          }
        }
        // 检查是否为选项名称
        else {
          auto it = std::find(choices.begin(), choices.end(), selection);
          if (it != choices.end()) {
            // 切换选择状态
            auto selected = std::find(selectedOptions.begin(),
                                      selectedOptions.end(), selection);
            if (selected != selectedOptions.end()) {
              selectedOptions.erase(selected);
            } else {
              selectedOptions.push_back(selection);
            }
          }
        }
      }

      printMultiOptions(choices, selectedOptions);
    }

    std::cout << "Continue selecting or type 'done' to finish: ";
  }

  return selectedOptions;
}

// Read user confirmation
bool CliParser::readUserConfirmation(const std::string &prompt,
                                     bool defaultValue) {
  std::string defaultStr = defaultValue ? "Y/n" : "y/N";
  std::string input;

  std::cout << prompt << " [" << defaultStr << "]: ";
  std::getline(std::cin, input);

  if (input.empty()) {
    return defaultValue;
  }

  std::string lowerInput = StringUtils::toLower(input);
  return lowerInput == "y" || lowerInput == "yes";
}

// Print options to the console
void CliParser::printOptions(const std::vector<std::string> &options,
                             const std::string &defaultOption) {
  for (size_t i = 0; i < options.size(); i++) {
    std::string option = options[i];
    std::string marker = (option == defaultOption) ? " (default)" : "";
    std::cout << "  " << (i + 1) << ". " << option << marker << "\n";
  }
}

// Print multi-options to the console
void CliParser::printMultiOptions(
    const std::vector<std::string> &options,
    const std::vector<std::string> &selectedOptions) {
  for (size_t i = 0; i < options.size(); i++) {
    std::string option = options[i];
    std::string marker =
        std::find(selectedOptions.begin(), selectedOptions.end(), option) !=
                selectedOptions.end()
            ? " [x]"
            : " [ ]";
    std::cout << "  " << (i + 1) << ". " << option << marker << "\n";
  }
}

// 获取配置文件路径
std::string CliParser::getConfigFilePath() {
  // 使用标准的配置文件位置
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

  // 确保配置目录存在
  try {
    std::filesystem::create_directories(configDir);
  } catch (const std::exception &e) {
    spdlog::warn("Failed to create config directory: {}", e.what());
    return ".cpp-scaffold.json"; // 如果创建失败，使用当前目录
  }

  return (configDir / "config.json").string();
}
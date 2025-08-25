#include "cli_parser.h"

#include <algorithm>
#include <cctype>
#include <iostream>
#include <nlohmann/json.hpp>
#include <sstream>

#include "../utils/string_utils.h"
#include "../utils/terminal_utils.h"
#include "../utils/file_utils.h"
#include "../config/config_manager.h"
#include "../config/user_preferences.h"
#include "../config/interactive_config.h"
#include "../config/project_profiles.h"
#include "../config/config_validator.h"
#include <fmt/color.h>
#include <fmt/core.h>
#include <fmt/format.h>
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
      {TemplateType::HeaderOnlyLib, "header-only-lib"},
      {TemplateType::MultiExecutable, "multi-executable"},
      {TemplateType::Gui, "gui"},
      {TemplateType::Network, "network"},
      {TemplateType::Embedded, "embedded"},
      {TemplateType::WebService, "webservice"},
      {TemplateType::GameEngine, "gameengine"},
      {TemplateType::QtApp, "qt-app"},
      {TemplateType::SfmlApp, "sfml-app"},
      {TemplateType::BoostApp, "boost-app"},
      {TemplateType::TestProject, "test-project"}};
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
      {PackageManager::Hunter, "hunter"},
      {PackageManager::CPM, "cpm"},
      {PackageManager::FetchContent, "fetchcontent"}};
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

std::string_view to_string(GitWorkflow workflow) {
  static const std::unordered_map<GitWorkflow, std::string_view> map = {
      {GitWorkflow::None, "none"}, {GitWorkflow::GitFlow, "gitflow"},
      {GitWorkflow::GitHubFlow, "github-flow"}, {GitWorkflow::GitLabFlow, "gitlab-flow"},
      {GitWorkflow::Custom, "custom"}};
  return map.at(workflow);
}

std::string_view to_string(GitBranchStrategy strategy) {
  static const std::unordered_map<GitBranchStrategy, std::string_view> map = {
      {GitBranchStrategy::SingleBranch, "single"}, {GitBranchStrategy::FeatureBranches, "feature"},
      {GitBranchStrategy::GitFlow, "gitflow"}, {GitBranchStrategy::Custom, "custom"}};
  return map.at(strategy);
}

std::string_view to_string(LicenseType license) {
  static const std::unordered_map<LicenseType, std::string_view> map = {
      {LicenseType::MIT, "mit"}, {LicenseType::Apache2, "apache2"},
      {LicenseType::GPL3, "gpl3"}, {LicenseType::BSD3, "bsd3"},
      {LicenseType::BSD2, "bsd2"}, {LicenseType::Unlicense, "unlicense"},
      {LicenseType::Custom, "custom"}, {LicenseType::None, "none"}};
  return map.at(license);
}

// 字符串转枚举（带验证）
std::optional<TemplateType> to_template_type(std::string_view str) {
  static const std::unordered_map<std::string_view, TemplateType> map = {
      {"console", TemplateType::Console},
      {"lib", TemplateType::Lib},
      {"header-only-lib", TemplateType::HeaderOnlyLib},
      {"multi-executable", TemplateType::MultiExecutable},
      {"gui", TemplateType::Gui},
      {"network", TemplateType::Network},
      {"embedded", TemplateType::Embedded},
      {"webservice", TemplateType::WebService},
      {"gameengine", TemplateType::GameEngine},
      {"qt-app", TemplateType::QtApp},
      {"sfml-app", TemplateType::SfmlApp},
      {"boost-app", TemplateType::BoostApp},
      {"test-project", TemplateType::TestProject}};

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

std::optional<PackageManager> to_package_manager(std::string_view str) {
  static const std::unordered_map<std::string_view, PackageManager> map = {
      {"vcpkg", PackageManager::Vcpkg}, {"conan", PackageManager::Conan},
      {"none", PackageManager::None}, {"spack", PackageManager::Spack},
      {"hunter", PackageManager::Hunter}, {"cpm", PackageManager::CPM},
      {"fetchcontent", PackageManager::FetchContent}};

  auto it = map.find(str);
  if (it != map.end()) {
    return it->second;
  }
  return std::nullopt;
}

std::optional<TestFramework> to_test_framework(std::string_view str) {
  static const std::unordered_map<std::string_view, TestFramework> map = {
      {"gtest", TestFramework::GTest}, {"catch2", TestFramework::Catch2},
      {"doctest", TestFramework::Doctest}, {"boost", TestFramework::Boost},
      {"none", TestFramework::None}};

  auto it = map.find(str);
  if (it != map.end()) {
    return it->second;
  }
  return std::nullopt;
}

std::optional<EditorConfig> to_editor_config(std::string_view str) {
  static const std::unordered_map<std::string_view, EditorConfig> map = {
      {"vscode", EditorConfig::VSCode}, {"clion", EditorConfig::CLion},
      {"vs", EditorConfig::VS}, {"vim", EditorConfig::Vim},
      {"emacs", EditorConfig::Emacs}, {"sublime", EditorConfig::Sublime}};

  auto it = map.find(str);
  if (it != map.end()) {
    return it->second;
  }
  return std::nullopt;
}

std::optional<CiSystem> to_ci_system(std::string_view str) {
  static const std::unordered_map<std::string_view, CiSystem> map = {
      {"github", CiSystem::GitHub}, {"gitlab", CiSystem::GitLab},
      {"travis", CiSystem::Travis}, {"appveyor", CiSystem::AppVeyor},
      {"azure", CiSystem::AzureDevOps}, {"circleci", CiSystem::CircleCI}};

  auto it = map.find(str);
  if (it != map.end()) {
    return it->second;
  }
  return std::nullopt;
}

std::optional<Language> to_language(std::string_view str) {
  static const std::unordered_map<std::string_view, Language> map = {
      {"en", Language::English}, {"zh", Language::Chinese},
      {"es", Language::Spanish}, {"jp", Language::Japanese},
      {"de", Language::German}, {"fr", Language::French}};

  auto it = map.find(str);
  if (it != map.end()) {
    return it->second;
  }
  return std::nullopt;
}

std::optional<GitWorkflow> to_git_workflow(std::string_view str) {
  static const std::unordered_map<std::string_view, GitWorkflow> map = {
      {"none", GitWorkflow::None}, {"gitflow", GitWorkflow::GitFlow},
      {"github-flow", GitWorkflow::GitHubFlow}, {"gitlab-flow", GitWorkflow::GitLabFlow},
      {"custom", GitWorkflow::Custom}};

  auto it = map.find(str);
  if (it != map.end()) {
    return it->second;
  }
  return std::nullopt;
}

std::optional<GitBranchStrategy> to_git_branch_strategy(std::string_view str) {
  static const std::unordered_map<std::string_view, GitBranchStrategy> map = {
      {"single", GitBranchStrategy::SingleBranch}, {"feature", GitBranchStrategy::FeatureBranches},
      {"gitflow", GitBranchStrategy::GitFlow}, {"custom", GitBranchStrategy::Custom}};

  auto it = map.find(str);
  if (it != map.end()) {
    return it->second;
  }
  return std::nullopt;
}

std::optional<LicenseType> to_license_type(std::string_view str) {
  static const std::unordered_map<std::string_view, LicenseType> map = {
      {"mit", LicenseType::MIT}, {"apache2", LicenseType::Apache2},
      {"gpl3", LicenseType::GPL3}, {"bsd3", LicenseType::BSD3},
      {"bsd2", LicenseType::BSD2}, {"unlicense", LicenseType::Unlicense},
      {"custom", LicenseType::Custom}, {"none", LicenseType::None}};

  auto it = map.find(str);
  if (it != map.end()) {
    return it->second;
  }
  return std::nullopt;
}

// 获取所有枚举值的字符串表示
std::vector<std::string_view> all_template_types() {
  return {"console", "lib", "header-only-lib", "multi-executable", "gui",
          "network", "embedded", "webservice", "gameengine", "qt-app",
          "sfml-app", "boost-app", "test-project"};
}

std::vector<std::string_view> all_build_systems() {
  return {"cmake", "meson", "bazel", "xmake", "premake", "make", "ninja"};
}

std::vector<std::string_view> all_package_managers() {
  return {"vcpkg", "conan", "none", "spack", "hunter"};
}

std::vector<std::string_view> all_test_frameworks() {
  return {"gtest", "catch2", "doctest", "boost", "none"};
}

std::vector<std::string_view> all_editor_configs() {
  return {"vscode", "clion", "vs", "vim", "emacs", "sublime"};
}

std::vector<std::string_view> all_ci_systems() {
  return {"github", "gitlab", "travis", "appveyor", "azure", "circleci"};
}

std::vector<std::string_view> all_languages() {
  return {"en", "zh", "es", "jp", "de", "fr"};
}

std::vector<std::string_view> all_git_workflows() {
  return {"none", "gitflow", "github-flow", "gitlab-flow", "custom"};
}

std::vector<std::string_view> all_git_branch_strategies() {
  return {"single", "feature", "gitflow", "custom"};
}

std::vector<std::string_view> all_license_types() {
  return {"mit", "apache2", "gpl3", "bsd3", "bsd2", "unlicense", "custom", "none"};
}

// New enum conversions for enhanced configuration
std::string_view to_string(CppStandard standard) {
  static const std::unordered_map<CppStandard, std::string_view> map = {
      {CppStandard::Cpp11, "c++11"},
      {CppStandard::Cpp14, "c++14"},
      {CppStandard::Cpp17, "c++17"},
      {CppStandard::Cpp20, "c++20"},
      {CppStandard::Cpp23, "c++23"},
      {CppStandard::Latest, "latest"}};
  return map.at(standard);
}

std::optional<CppStandard> to_cpp_standard(std::string_view str) {
  static const std::unordered_map<std::string_view, CppStandard> map = {
      {"c++11", CppStandard::Cpp11}, {"11", CppStandard::Cpp11},
      {"c++14", CppStandard::Cpp14}, {"14", CppStandard::Cpp14},
      {"c++17", CppStandard::Cpp17}, {"17", CppStandard::Cpp17},
      {"c++20", CppStandard::Cpp20}, {"20", CppStandard::Cpp20},
      {"c++23", CppStandard::Cpp23}, {"23", CppStandard::Cpp23},
      {"latest", CppStandard::Latest}};
  auto it = map.find(str);
  return it != map.end() ? std::optional<CppStandard>{it->second} : std::nullopt;
}

std::string_view to_string(ProjectStructure structure) {
  static const std::unordered_map<ProjectStructure, std::string_view> map = {
      {ProjectStructure::Minimal, "minimal"},
      {ProjectStructure::Standard, "standard"},
      {ProjectStructure::Advanced, "advanced"},
      {ProjectStructure::Custom, "custom"}};
  return map.at(structure);
}

std::optional<ProjectStructure> to_project_structure(std::string_view str) {
  static const std::unordered_map<std::string_view, ProjectStructure> map = {
      {"minimal", ProjectStructure::Minimal},
      {"standard", ProjectStructure::Standard},
      {"advanced", ProjectStructure::Advanced},
      {"custom", ProjectStructure::Custom}};
  auto it = map.find(str);
  return it != map.end() ? std::optional<ProjectStructure>{it->second} : std::nullopt;
}

std::string_view to_string(CompilerFlags flags) {
  static const std::unordered_map<CompilerFlags, std::string_view> map = {
      {CompilerFlags::Debug, "debug"},
      {CompilerFlags::Release, "release"},
      {CompilerFlags::RelWithDebInfo, "relwithdebinfo"},
      {CompilerFlags::MinSizeRel, "minsizerel"},
      {CompilerFlags::Custom, "custom"}};
  return map.at(flags);
}

std::optional<CompilerFlags> to_compiler_flags(std::string_view str) {
  static const std::unordered_map<std::string_view, CompilerFlags> map = {
      {"debug", CompilerFlags::Debug},
      {"release", CompilerFlags::Release},
      {"relwithdebinfo", CompilerFlags::RelWithDebInfo},
      {"minsizerel", CompilerFlags::MinSizeRel},
      {"custom", CompilerFlags::Custom}};
  auto it = map.find(str);
  return it != map.end() ? std::optional<CompilerFlags>{it->second} : std::nullopt;
}

std::vector<std::string_view> all_cpp_standards() {
  return {"c++11", "c++14", "c++17", "c++20", "c++23", "latest"};
}

std::vector<std::string_view> all_project_structures() {
  return {"minimal", "standard", "advanced", "custom"};
}

std::vector<std::string_view> all_compiler_flags() {
  return {"debug", "release", "relwithdebinfo", "minsizerel", "custom"};
}

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

Language Localization::getCurrentLanguage() {
  return s_currentLanguage;
}

void Localization::setCurrentLanguage(Language lang) {
  s_currentLanguage = lang;
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
  try {
    // Use the new enhanced configuration manager
    auto& configManager = config::ConfigManager::getInstance();
    auto& userPrefs = config::UserPreferences::getInstance();

    // Load configuration if not already loaded
    if (!configManager.loadConfiguration()) {
      spdlog::warn("Failed to load configuration, using system defaults");
    }

    if (!userPrefs.loadPreferences()) {
      spdlog::warn("Failed to load user preferences, using system defaults");
    }

    // Get default options from the enhanced system
    CliOptions options = configManager.loadDefaultOptions();

    // Apply user preferences
    options = userPrefs.applyPreferencesToOptions(options);

    return options;

  } catch (const std::exception &e) {
    spdlog::error("加载默认选项时出错: {}", e.what());
    return CliOptions{};
  }
}

// 保存选项作为默认配置
bool ConfigManager::saveOptionsAsDefaults(const CliOptions &options) {
  try {
    // Use the new enhanced configuration manager
    auto& configManager = config::ConfigManager::getInstance();
    auto& userPrefs = config::UserPreferences::getInstance();

    // Save options as defaults in the enhanced system
    bool success = configManager.saveOptionsAsDefaults(options);

    // Also update user preferences
    userPrefs.updatePreferencesFromOptions(options);

    if (success) {
      spdlog::info("Default options saved successfully");
    } else {
      spdlog::error("Failed to save default options");
    }

    return success;

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

  if (command == "config" || command == "configure") {
    // Run the interactive configuration wizard
    config::InteractiveConfigWizard::runConfigurationWizard();
    options.showHelp = true; // Exit after configuration
    return options;
  }

  if (command == "list-templates" || command == "list") {
    showAvailableTemplates();
    options.showHelp = true; // Exit after showing templates
    return options;
  }

  if (command == "validate") {
    if (argc > 2) {
      validateProject(argv[2]);
    } else {
      validateProject(".");
    }
    options.showHelp = true; // Exit after validation
    return options;
  }

  if (command == "list-profiles" || command == "profiles") {
    showAvailableProfiles();
    options.showHelp = true; // Exit after showing profiles
    return options;
  }

  if (command == "show-profile" || command == "profile-info") {
    if (argc > 2) {
      showProfileInfo(argv[2]);
    } else {
      std::cout << TerminalUtils::colorize("❌ Please specify a profile name", utils::Color::BrightRed) << "\n";
      std::cout << "Usage: cpp-scaffold show-profile <profile-name>\n";
    }
    options.showHelp = true; // Exit after showing profile info
    return options;
  }

  if (command == "validate-config") {
    // Parse remaining arguments to get configuration
    CliOptions configOptions = parse(argc, argv);
    validateConfiguration(configOptions);
    options.showHelp = true; // Exit after validation
    return options;
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
    } else if (arg == "--profile") {
      if (i + 1 < argc) {
        options.profileName = argv[++i];
      }
    } else if (arg == "--validate-config") {
      options.validateConfig = true;
    } else if (arg == "--strict-validation") {
      options.strictValidation = true;
    } else if (arg == "--git-workflow") {
      if (i + 1 < argc) {
        auto workflow = enums::to_git_workflow(argv[++i]);
        if (workflow) {
          options.gitWorkflow = *workflow;
        }
      }
    } else if (arg == "--git-branch-strategy") {
      if (i + 1 < argc) {
        auto strategy = enums::to_git_branch_strategy(argv[++i]);
        if (strategy) {
          options.gitBranchStrategy = *strategy;
        }
      }
    } else if (arg == "--license") {
      if (i + 1 < argc) {
        auto license = enums::to_license_type(argv[++i]);
        if (license) {
          options.licenseType = *license;
        }
      }
    } else if (arg == "--git-remote") {
      if (i + 1 < argc) {
        options.gitRemoteUrl = argv[++i];
      }
    } else if (arg == "--git-user") {
      if (i + 1 < argc) {
        options.gitUserName = argv[++i];
      }
    } else if (arg == "--git-email") {
      if (i + 1 < argc) {
        options.gitUserEmail = argv[++i];
      }
    } else if (arg == "--git-hooks") {
      options.setupGitHooks = true;
    } else if (arg == "--no-initial-commit") {
      options.createInitialCommit = false;
    } else if (arg == "--doc-formats") {
      if (i + 1 < argc) {
        std::string formats = argv[++i];
        options.docFormats.clear();
        // Parse comma-separated formats
        std::stringstream ss(formats);
        std::string format;
        while (std::getline(ss, format, ',')) {
          options.docFormats.push_back(format);
        }
      }
    } else if (arg == "--doc-types") {
      if (i + 1 < argc) {
        std::string types = argv[++i];
        options.docTypes.clear();
        // Parse comma-separated types
        std::stringstream ss(types);
        std::string type;
        while (std::getline(ss, type, ',')) {
          options.docTypes.push_back(type);
        }
      }
    } else if (arg == "--doxygen") {
      options.generateDoxygen = true;
    } else if (arg == "--doxygen-theme") {
      if (i + 1 < argc) {
        options.doxygenTheme = argv[++i];
      }
    } else if (arg == "--no-code-examples") {
      options.includeCodeExamples = false;
    } else if (arg == "--changelog") {
      options.generateChangelog = true;
    } else if (arg == "--gui-frameworks") {
      if (i + 1 < argc) {
        std::string frameworks = argv[++i];
        options.guiFrameworks.clear();
        // Parse comma-separated frameworks
        std::stringstream ss(frameworks);
        std::string framework;
        while (std::getline(ss, framework, ',')) {
          options.guiFrameworks.push_back(framework);
        }
      }
    } else if (arg == "--game-frameworks") {
      if (i + 1 < argc) {
        std::string frameworks = argv[++i];
        options.gameFrameworks.clear();
        // Parse comma-separated frameworks
        std::stringstream ss(frameworks);
        std::string framework;
        while (std::getline(ss, framework, ',')) {
          options.gameFrameworks.push_back(framework);
        }
      }
    } else if (arg == "--graphics-libs") {
      if (i + 1 < argc) {
        std::string libs = argv[++i];
        options.graphicsLibraries.clear();
        // Parse comma-separated libraries
        std::stringstream ss(libs);
        std::string lib;
        while (std::getline(ss, lib, ',')) {
          options.graphicsLibraries.push_back(lib);
        }
      }
    } else if (arg == "--include-shaders") {
      options.includeShaders = true;
    }
    // 处理其他命令行选项...
  }

  // Apply profile if specified
  if (!options.profileName.empty()) {
    auto& profileManager = config::ProjectProfileManager::getInstance();
    profileManager.loadBuiltInProfiles();

    CliOptions profileOptions = profileManager.applyProfile(options.profileName, options);
    if (profileOptions.projectName != options.projectName) {
      // Profile was found and applied
      options = profileOptions;
      // Restore the project name from command line
      if (!options.projectName.empty()) {
        options.projectName = profileOptions.projectName;
      }
    }
  }

  // Validate configuration if requested
  if (options.validateConfig) {
    auto& validator = config::ConfigValidator::getInstance();
    if (options.strictValidation) {
      validator.setStrictMode(true);
    }

    auto result = validator.validateConfiguration(options);
    config::validation_utils::printValidationResult(result);

    if (!result.isValid && options.strictValidation) {
      std::cout << TerminalUtils::colorize("❌ Configuration validation failed in strict mode. Aborting.", utils::Color::BrightRed) << "\n";
      exit(1);
    }
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
  std::cout << "  cpp-scaffold interactive\n";
  std::cout << "  cpp-scaffold config\n";
  std::cout << "  cpp-scaffold list-templates\n";
  std::cout << "  cpp-scaffold validate [项目路径]\n\n";

  std::cout << TerminalUtils::colorize("**选项:**", utils::Color::BrightYellow)
            << "\n";
  fmt::print("  -t, --template <类型>        项目模板类型: console, lib, header-only-lib, "
             "multi-executable, gui, network, embedded, webservice, gameengine, "
             "qt-app, sfml-app, boost-app, test-project\n");
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
  fmt::print("  --git-workflow <类型>        Git工作流: none, gitflow, github-flow, gitlab-flow, custom\n");
  fmt::print("  --git-branch-strategy <策略> Git分支策略: single, feature, gitflow, custom\n");
  fmt::print("  --license <类型>             许可证类型: mit, apache2, gpl3, bsd3, bsd2, unlicense, custom, none\n");
  fmt::print("  --git-remote <URL>           Git远程仓库URL\n");
  fmt::print("  --git-user <用户名>          Git用户名\n");
  fmt::print("  --git-email <邮箱>           Git用户邮箱\n");
  fmt::print("  --git-hooks                  设置Git钩子\n");
  fmt::print("  --no-initial-commit          不创建初始提交\n");
  fmt::print("  --doc-formats <格式>         文档输出格式: markdown,html,pdf (逗号分隔)\n");
  fmt::print("  --doc-types <类型>           文档类型: readme,api,user,developer (逗号分隔)\n");
  fmt::print("  --doxygen                    生成Doxygen配置\n");
  fmt::print("  --doxygen-theme <主题>       Doxygen主题\n");
  fmt::print("  --no-code-examples           不包含代码示例\n");
  fmt::print("  --changelog                  生成变更日志\n");
  fmt::print("  --gui-frameworks <框架>      GUI框架: qt,gtk,fltk,imgui,wxwidgets (逗号分隔)\n");
  fmt::print("  --game-frameworks <框架>     游戏框架: sdl2,sfml,allegro (逗号分隔)\n");
  fmt::print("  --graphics-libs <库>         图形库: opengl,vulkan,directx (逗号分隔)\n");
  fmt::print("  --include-shaders            包含着色器模板\n");
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
  fmt::print("  cpp-scaffold create my-game --template gui --gui-frameworks sdl2,opengl --include-shaders\n");
  fmt::print("  cpp-scaffold create my-project --git-workflow gitflow --license mit --doxygen\n");
  fmt::print("  cpp-scaffold create my-lib --doc-formats markdown,html --doc-types readme,api,user\n");
}

// 显示版本信息
void CliParser::showVersion() { fmt::print("CPP-Scaffold Version 1.1.0\n"); }

// 显示可用模板
void CliParser::showAvailableTemplates() {
  std::cout << TerminalUtils::colorize("**Available Project Templates**", utils::Color::BrightCyan) << "\n\n";

  std::vector<std::pair<std::string, std::string>> templates = {
    {"console", "Console application with basic I/O"},
    {"lib", "Static/shared library with headers"},
    {"gui", "GUI application using Qt or similar"},
    {"network", "Network application with socket programming"},
    {"webservice", "Web service with REST API (disabled)"},
    {"embedded", "Embedded systems project (disabled)"},
    {"gameengine", "Game engine project (planned)"}
  };

  for (const auto& [name, description] : templates) {
    if (name == "webservice" || name == "embedded") {
      std::cout << TerminalUtils::colorize("  " + name, utils::Color::BrightBlack)
                << " - " << TerminalUtils::colorize(description, utils::Color::BrightBlack) << "\n";
    } else if (name == "gameengine") {
      std::cout << TerminalUtils::colorize("  " + name, utils::Color::Yellow)
                << " - " << TerminalUtils::colorize(description, utils::Color::Yellow) << "\n";
    } else {
      std::cout << TerminalUtils::colorize("  " + name, utils::Color::BrightGreen)
                << " - " << description << "\n";
    }
  }

  std::cout << "\n" << TerminalUtils::colorize("Usage:", utils::Color::BrightYellow) << "\n";
  std::cout << "  cpp-scaffold create <project-name> --template <template-name>\n\n";
}

// 验证项目结构
void CliParser::validateProject(const std::string& projectPath) {
  std::cout << TerminalUtils::colorize("**Validating Project: " + projectPath + "**", utils::Color::BrightCyan) << "\n\n";

  bool isValid = true;
  std::vector<std::string> issues;

  // Check if directory exists
  if (!utils::FileUtils::directoryExists(projectPath)) {
    issues.push_back("Project directory does not exist");
    isValid = false;
  } else {
    // Check for essential files
    std::vector<std::string> requiredFiles = {"CMakeLists.txt", "README.md"};
    std::vector<std::string> requiredDirs = {"src", "include"};

    for (const auto& file : requiredFiles) {
      std::string filePath = utils::FileUtils::combinePath(projectPath, file);
      if (!utils::FileUtils::fileExists(filePath)) {
        issues.push_back("Missing required file: " + file);
        isValid = false;
      } else {
        std::cout << TerminalUtils::colorize("✓", utils::Color::BrightGreen) << " Found: " << file << "\n";
      }
    }

    for (const auto& dir : requiredDirs) {
      std::string dirPath = utils::FileUtils::combinePath(projectPath, dir);
      if (!utils::FileUtils::directoryExists(dirPath)) {
        issues.push_back("Missing required directory: " + dir);
        isValid = false;
      } else {
        std::cout << TerminalUtils::colorize("✓", utils::Color::BrightGreen) << " Found: " << dir << "/\n";
      }
    }
  }

  std::cout << "\n";

  if (isValid) {
    std::cout << TerminalUtils::colorize("✅ Project structure is valid!", utils::Color::BrightGreen) << "\n";
  } else {
    std::cout << TerminalUtils::colorize("❌ Project validation failed:", utils::Color::BrightRed) << "\n";
    for (const auto& issue : issues) {
      std::cout << TerminalUtils::colorize("  • " + issue, utils::Color::Red) << "\n";
    }
  }
  std::cout << "\n";
}

// 显示可用项目配置文件
void CliParser::showAvailableProfiles() {
  std::cout << TerminalUtils::colorize("**Available Project Profiles**", utils::Color::BrightCyan) << "\n\n";

  auto& profileManager = config::ProjectProfileManager::getInstance();
  profileManager.loadBuiltInProfiles();

  auto profileInfos = profileManager.getProfileInfos();

  // Group profiles by category
  std::map<std::string, std::vector<config::ProfileInfo>> categorizedProfiles;
  for (const auto& info : profileInfos) {
    categorizedProfiles[info.category].push_back(info);
  }

  for (const auto& [category, profiles] : categorizedProfiles) {
    std::cout << TerminalUtils::colorize("📁 " + category, utils::Color::BrightYellow) << "\n";

    for (const auto& profile : profiles) {
      std::cout << TerminalUtils::colorize("  " + profile.name, utils::Color::BrightGreen)
                << " - " << profile.description << "\n";

      if (!profile.tags.empty()) {
        std::cout << "    Tags: " << TerminalUtils::colorize(
          utils::StringUtils::join(profile.tags, ", "), utils::Color::Cyan) << "\n";
      }
    }
    std::cout << "\n";
  }

  std::cout << TerminalUtils::colorize("Usage:", utils::Color::BrightYellow) << "\n";
  std::cout << "  cpp-scaffold create <project-name> --profile <profile-name>\n";
  std::cout << "  cpp-scaffold show-profile <profile-name>  # Show detailed profile info\n\n";
}

// 显示特定配置文件信息
void CliParser::showProfileInfo(const std::string& profileName) {
  auto& profileManager = config::ProjectProfileManager::getInstance();
  profileManager.loadBuiltInProfiles();

  auto profile = profileManager.getProfile(profileName);
  if (!profile) {
    std::cout << TerminalUtils::colorize("❌ Profile '" + profileName + "' not found", utils::Color::BrightRed) << "\n";
    std::cout << "Use 'cpp-scaffold list-profiles' to see available profiles.\n";
    return;
  }

  std::cout << TerminalUtils::colorize("📋 Profile: " + profile->info.name, utils::Color::BrightCyan) << "\n\n";

  std::cout << TerminalUtils::colorize("Description:", utils::Color::BrightYellow) << " " << profile->info.description << "\n";
  std::cout << TerminalUtils::colorize("Category:", utils::Color::BrightYellow) << " " << profile->info.category << "\n";
  std::cout << TerminalUtils::colorize("Version:", utils::Color::BrightYellow) << " " << profile->info.version << "\n";
  std::cout << TerminalUtils::colorize("Author:", utils::Color::BrightYellow) << " " << profile->info.author << "\n";

  if (!profile->info.tags.empty()) {
    std::cout << TerminalUtils::colorize("Tags:", utils::Color::BrightYellow) << " "
              << utils::StringUtils::join(profile->info.tags, ", ") << "\n";
  }

  std::cout << "\n" << TerminalUtils::colorize("Configuration:", utils::Color::BrightYellow) << "\n";
  std::cout << "  Template: " << enums::to_string(profile->options.templateType) << "\n";
  std::cout << "  Build System: " << enums::to_string(profile->options.buildSystem) << "\n";
  std::cout << "  Package Manager: " << enums::to_string(profile->options.packageManager) << "\n";
  std::cout << "  Tests: " << (profile->options.includeTests ? "Yes" : "No") << "\n";
  std::cout << "  Documentation: " << (profile->options.includeDocumentation ? "Yes" : "No") << "\n";

  if (!profile->recommendedDependencies.empty()) {
    std::cout << "\n" << TerminalUtils::colorize("Recommended Dependencies:", utils::Color::BrightYellow) << "\n";
    for (const auto& dep : profile->recommendedDependencies) {
      std::cout << "  • " << dep << "\n";
    }
  }

  if (!profile->setupInstructions.empty()) {
    std::cout << "\n" << TerminalUtils::colorize("Setup Instructions:", utils::Color::BrightYellow) << "\n";
    std::cout << profile->setupInstructions << "\n";
  }

  std::cout << "\n";
}

// 验证配置
void CliParser::validateConfiguration(const CliOptions& options) {
  std::cout << TerminalUtils::colorize("🔍 Validating Configuration...", utils::Color::BrightCyan) << "\n\n";

  auto& validator = config::ConfigValidator::getInstance();
  auto result = validator.validateConfiguration(options);

  config::validation_utils::printValidationResult(result);

  if (result.isValid) {
    std::cout << "\n" << TerminalUtils::colorize("✅ Configuration is valid!", utils::Color::BrightGreen) << "\n";
  } else {
    std::cout << "\n" << TerminalUtils::colorize("❌ Configuration has errors that need to be fixed.", utils::Color::BrightRed) << "\n";
  }
}

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

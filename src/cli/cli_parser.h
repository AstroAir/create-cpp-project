#pragma once
#include "../utils/terminal_utils.h" // 引入终端工具
#include <filesystem>
#include <functional>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <variant>
#include <vector>
#include <fmt/format.h>

// 使用枚举类以提高类型安全性
enum class TemplateType {
  Console,
  Lib,
  HeaderOnlyLib,
  MultiExecutable,
  Gui,
  Network,
  Embedded,
  WebService,
  GameEngine,
  QtApp,
  SfmlApp,
  BoostApp,
  TestProject
};
enum class BuildSystem { CMake, Meson, Bazel, XMake, Premake, Make, Ninja };
enum class PackageManager { Vcpkg, Conan, None, Spack, Hunter, CPM, FetchContent };
enum class TestFramework { GTest, Catch2, Doctest, Boost, None };
enum class EditorConfig { VSCode, CLion, VS, Vim, Emacs, Sublime };
enum class CiSystem { GitHub, GitLab, Travis, AppVeyor, AzureDevOps, CircleCI };
enum class Language { English, Chinese, Spanish, Japanese, German, French };

// New enums for advanced configuration
enum class CppStandard { Cpp11, Cpp14, Cpp17, Cpp20, Cpp23, Latest };
enum class ProjectStructure { Minimal, Standard, Advanced, Custom };
enum class CompilerFlags { Debug, Release, RelWithDebInfo, MinSizeRel, Custom };

// Git workflow and configuration enums
enum class GitWorkflow { None, GitFlow, GitHubFlow, GitLabFlow, Custom };
enum class GitBranchStrategy { SingleBranch, FeatureBranches, GitFlow, Custom };
enum class LicenseType { MIT, Apache2, GPL3, BSD3, BSD2, Unlicense, Custom, None };

// 枚举与字符串互相转换的命名空间
namespace enums {
// 将枚举转换为字符串
std::string_view to_string(TemplateType type);
std::string_view to_string(BuildSystem system);
std::string_view to_string(PackageManager manager);
std::string_view to_string(TestFramework framework);
std::string_view to_string(EditorConfig editor);
std::string_view to_string(CiSystem ci);
std::string_view to_string(Language lang);
std::string_view to_string(GitWorkflow workflow);
std::string_view to_string(GitBranchStrategy strategy);
std::string_view to_string(LicenseType license);

// 从字符串转换为枚举
std::optional<TemplateType> to_template_type(std::string_view str);
std::optional<BuildSystem> to_build_system(std::string_view str);
std::optional<PackageManager> to_package_manager(std::string_view str);
std::optional<TestFramework> to_test_framework(std::string_view str);
std::optional<EditorConfig> to_editor_config(std::string_view str);
std::optional<CiSystem> to_ci_system(std::string_view str);
std::optional<Language> to_language(std::string_view str);
std::optional<GitWorkflow> to_git_workflow(std::string_view str);
std::optional<GitBranchStrategy> to_git_branch_strategy(std::string_view str);
std::optional<LicenseType> to_license_type(std::string_view str);

// 获取所有值的字符串表示
std::vector<std::string_view> all_template_types();
std::vector<std::string_view> all_build_systems();
std::vector<std::string_view> all_package_managers();
std::vector<std::string_view> all_test_frameworks();
std::vector<std::string_view> all_editor_configs();
std::vector<std::string_view> all_ci_systems();
std::vector<std::string_view> all_languages();
std::vector<std::string_view> all_git_workflows();
std::vector<std::string_view> all_git_branch_strategies();
std::vector<std::string_view> all_license_types();

// New enum conversions for advanced configuration
std::string_view to_string(CppStandard standard);
std::optional<CppStandard> to_cpp_standard(std::string_view str);
std::string_view to_string(ProjectStructure structure);
std::optional<ProjectStructure> to_project_structure(std::string_view str);
std::string_view to_string(CompilerFlags flags);
std::optional<CompilerFlags> to_compiler_flags(std::string_view str);
std::vector<std::string_view> all_cpp_standards();
std::vector<std::string_view> all_project_structures();
std::vector<std::string_view> all_compiler_flags();

} // namespace enums

// 增强的命令行选项结构
struct CliOptions {
  std::string projectName;
  std::string projectDescription; // New: Project description
  TemplateType templateType = TemplateType::Console;
  BuildSystem buildSystem = BuildSystem::CMake;
  PackageManager packageManager = PackageManager::Vcpkg;
  std::optional<std::string> networkLibrary; // 用于网络项目: asio, boost, poco

  // Enhanced C++ configuration
  CppStandard cppStandard = CppStandard::Cpp17;
  ProjectStructure projectStructure = ProjectStructure::Standard;
  CompilerFlags compilerFlags = CompilerFlags::Release;
  std::vector<std::string> customCompilerFlags;

  bool includeTests = false;
  TestFramework testFramework = TestFramework::GTest;

  bool includeDocumentation = false;
  bool includeCodeStyleTools = false;

  std::vector<EditorConfig> editorOptions;
  std::vector<CiSystem> ciOptions;

  bool initGit = true;
  bool showHelp = false;
  bool version = false;
  bool verbose = false;

  Language language = Language::English;
  std::filesystem::path customTemplatePath; // 自定义项目模板路径

  // Profile and validation options
  std::string profileName; // Project profile to use
  bool validateConfig = true; // Enable configuration validation
  bool strictValidation = false; // Enable strict validation mode

  // Git-related options
  GitWorkflow gitWorkflow = GitWorkflow::None;
  GitBranchStrategy gitBranchStrategy = GitBranchStrategy::SingleBranch;
  LicenseType licenseType = LicenseType::MIT;
  std::string gitRemoteUrl; // Optional remote repository URL
  std::string gitUserName; // Git user name for configuration
  std::string gitUserEmail; // Git user email for configuration
  bool createInitialCommit = true;
  bool setupGitHooks = false;
  std::vector<std::string> gitBranches; // Additional branches to create

  // Documentation-related options
  std::vector<std::string> docFormats{"markdown"}; // Output formats: markdown, html, pdf
  std::vector<std::string> docTypes{"readme", "api"}; // Types: readme, api, user, developer
  bool generateDoxygen = false;
  std::string doxygenTheme = "default";
  bool includeCodeExamples = true;
  bool generateChangelog = false;

  // Extended graphical framework options
  std::vector<std::string> guiFrameworks; // Multiple GUI frameworks
  std::vector<std::string> gameFrameworks; // Game development frameworks
  std::vector<std::string> graphicsLibraries; // Graphics rendering libraries
  bool includeShaders = false; // Include shader templates for graphics projects

  // 用于高级用例的额外选项
  std::unordered_map<std::string, std::string> additionalOptions;
};

// 用户输入处理类
class UserInput {
public:
  // 类型安全的输入验证变体类型
  using ValidatorResult = std::variant<bool, std::string>;
  using ValidatorFunction = std::function<ValidatorResult(const std::string &)>;

  // 带验证功能的用户输入读取
  static std::string
  read(std::string_view prompt,
       std::optional<std::string_view> defaultValue = std::nullopt,
       std::optional<ValidatorFunction> validator = std::nullopt);

  // 创建常用的验证器
  static ValidatorFunction notEmptyValidator();
  static ValidatorFunction numericValidator();
  static ValidatorFunction rangeValidator(int min, int max);
  static ValidatorFunction
  oneOfValidator(const std::vector<std::string_view> &validValues);

  // 特殊输入方法
  static bool readConfirmation(std::string_view prompt,
                               bool defaultValue = true);
  static int readNumber(std::string_view prompt, int min, int max,
                        std::optional<int> defaultValue = std::nullopt);
  static std::string
  readChoice(std::string_view prompt,
             const std::vector<std::string_view> &choices,
             std::optional<std::string_view> defaultValue = std::nullopt);
  static std::vector<std::string>
  readMultiChoice(std::string_view prompt,
                  const std::vector<std::string_view> &choices,
                  const std::vector<std::string_view> &defaultValues = {});

  // 增强型交互式输入函数
  static std::string
  readWithHighlight(std::string_view prompt,
                    std::optional<std::string_view> defaultValue = std::nullopt,
                    utils::Color promptColor = utils::Color::BrightCyan);

  static std::string readChoiceWithStyle(
      std::string_view prompt, const std::vector<std::string_view> &choices,
      std::optional<std::string_view> defaultValue = std::nullopt,
      utils::Color highlightColor = utils::Color::BrightGreen);

private:
  // 输出格式化辅助方法
  static void
  printOptions(const std::vector<std::string_view> &options,
               std::optional<std::string_view> defaultOption = std::nullopt);
  static void
  printMultiOptions(const std::vector<std::string_view> &options,
                    const std::vector<std::string> &selectedOptions);

  // 使用终端工具的美化选项显示
  static void printStyledOptions(
      const std::vector<std::string_view> &options,
      std::optional<std::string_view> defaultOption = std::nullopt,
      utils::Color highlightColor = utils::Color::BrightGreen);
};

// 本地化辅助类
class Localization {
public:
  static void initialize();
  static std::string_view get(std::string_view key,
                              Language lang = Language::English);
  static Language getCurrentLanguage();
  static void setCurrentLanguage(Language lang);

private:
  static Language s_currentLanguage;
  static std::unordered_map<std::string,
                            std::unordered_map<Language, std::string>>
      s_strings;
  static void loadLanguageStrings();
};

// 配置管理器用于持久化设置
class ConfigManager {
public:
  [[nodiscard]] static CliOptions loadDefaultOptions();
  static bool saveOptionsAsDefaults(const CliOptions &options);
  static std::filesystem::path getConfigFilePath();
  static bool saveProfile(std::string_view profileName,
                          const CliOptions &options);
  [[nodiscard]] static std::optional<CliOptions>
  loadProfile(std::string_view profileName);
  [[nodiscard]] static std::vector<std::string> listProfiles();

private:
  static constexpr std::string_view DEFAULT_PROFILE = "default";
};

// 添加交互式项目向导类
class ProjectWizard {
public:
  // 运行完整的项目创建向导
  static CliOptions runWizard(const CliOptions &initialOptions = {});

  // 专项向导
  static CliOptions runQuickStartWizard();
  static CliOptions runAdvancedWizard(const CliOptions &baseOptions);
  static CliOptions runTemplateSelectionWizard();

  // 项目名称和路径向导
  static bool configureProjectDetails(CliOptions &options);

  // 构建系统和包管理器向导
  static bool configureBuildSystem(CliOptions &options);

  // 测试框架向导
  static bool configureTestFramework(CliOptions &options);

  // 编辑器和开发工具向导
  static bool configureDevTools(CliOptions &options);

  // CI/CD系统向导
  static bool configureCiCd(CliOptions &options);

  // 项目配置总结和确认
  static bool showSummaryAndConfirm(const CliOptions &options);

private:
  // 显示向导欢迎界面
  static void showWelcomeScreen();
  static void showEnhancedWelcomeScreen();
  static void showNpmStyleWelcomeScreen();

  // 显示步骤进度
  static void showWizardProgress(int currentStep, int totalSteps,
                                 const std::string &stepName);

  // 依据模板类型提供推荐配置
  static CliOptions getRecommendedOptionsForTemplate(TemplateType type);

  // 保存用户配置为模板
  static bool saveAsTemplate(const CliOptions &options,
                             const std::string &templateName);
};

// 主 CLI 解析器类
class CliParser {
public:
  // 解析命令行参数
  [[nodiscard]] static CliOptions parse(int argc, char *argv[]);

  // 显示信息
  static void showHelp(Language lang = Language::English);
  static void showVersion();
  static void showAvailableTemplates();
  static void showAvailableProfiles();
  static void showProfileInfo(const std::string& profileName);
  static void validateProject(const std::string& projectPath);
  static void validateConfiguration(const CliOptions& options);

  // 交互式选项配置
  [[nodiscard]] static CliOptions
  promptUserForOptions(const CliOptions &defaultOptions = {});

  // 验证选项
  [[nodiscard]] static bool validateOptions(CliOptions &options);

  // 增加交互式向导接口
  [[nodiscard]] static CliOptions runInteractiveMode();

private:
  // 检测操作系统的辅助方法
  static std::string detectOS();

  // 获取基于系统的建议默认值
  static CliOptions getSystemSuggestedDefaults();

  // 显示漂亮的CLI标题
  static void showCliHeader();
};

// fmt formatters for custom enums
template <>
struct fmt::formatter<TemplateType> : fmt::formatter<std::string_view> {
  auto format(TemplateType type, fmt::format_context& ctx) const {
    return fmt::formatter<std::string_view>::format(::enums::to_string(type), ctx);
  }
};

template <>
struct fmt::formatter<BuildSystem> : fmt::formatter<std::string_view> {
  auto format(BuildSystem system, fmt::format_context& ctx) const {
    return fmt::formatter<std::string_view>::format(::enums::to_string(system), ctx);
  }
};

template <>
struct fmt::formatter<PackageManager> : fmt::formatter<std::string_view> {
  auto format(PackageManager manager, fmt::format_context& ctx) const {
    return fmt::formatter<std::string_view>::format(::enums::to_string(manager), ctx);
  }
};

template <>
struct fmt::formatter<CiSystem> : fmt::formatter<std::string_view> {
  auto format(CiSystem ci, fmt::format_context& ctx) const {
    return fmt::formatter<std::string_view>::format(::enums::to_string(ci), ctx);
  }
};

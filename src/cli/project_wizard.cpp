#include "../utils/terminal_utils.h"
#include "../templates/template_discovery.h"
#include "../utils/project_scaffolding.h"
#include "../utils/build_system_config.h"
#include "cli_parser.h"
#include <chrono>
#include <filesystem>
#include <iostream>
#include <spdlog/spdlog.h>
#include <thread>

using namespace utils;
using namespace cli_enums;

// 实现 ProjectWizard 类的静态方法

// 运行完整的项目创建向导
CliOptions ProjectWizard::runWizard(const CliOptions &initialOptions) {
  CliOptions options = initialOptions;

  // 显示NPM风格的欢迎屏幕
  showNpmStyleWelcomeScreen();

  // 设置总步骤数
  const int totalSteps = 6;

  // 第1步：配置项目名称和路径
  TerminalUtils::showWizardHeader("C++ Project Scaffold", 1, totalSteps);
  TerminalUtils::showWizardProgress(1, totalSteps, "Project Details");
  if (!configureProjectDetails(options)) {
    TerminalUtils::showNpmStyleError("Project creation cancelled", "User cancelled during project details configuration");
    return initialOptions;
  }

  // 第2步：构建系统和包管理器
  TerminalUtils::showWizardHeader("C++ Project Scaffold", 2, totalSteps);
  TerminalUtils::showWizardProgress(2, totalSteps, "Build System & Package Manager");
  if (!configureBuildSystem(options)) {
    TerminalUtils::showNpmStyleError("Project creation cancelled", "User cancelled during build system configuration");
    return initialOptions;
  }

  // 第3步：测试框架
  TerminalUtils::showWizardHeader("C++ Project Scaffold", 3, totalSteps);
  TerminalUtils::showWizardProgress(3, totalSteps, "Testing Framework");
  if (!configureTestFramework(options)) {
    TerminalUtils::showNpmStyleError("Project creation cancelled", "User cancelled during testing framework configuration");
    return initialOptions;
  }

  // 第4步：编辑器和开发工具
  TerminalUtils::showWizardHeader("C++ Project Scaffold", 4, totalSteps);
  TerminalUtils::showWizardProgress(4, totalSteps, "Development Tools");
  if (!configureDevTools(options)) {
    TerminalUtils::showNpmStyleError("Project creation cancelled", "User cancelled during development tools configuration");
    return initialOptions;
  }

  // 第5步：CI/CD系统
  TerminalUtils::showWizardHeader("C++ Project Scaffold", 5, totalSteps);
  TerminalUtils::showWizardProgress(5, totalSteps, "CI/CD Configuration");
  if (!configureCiCd(options)) {
    TerminalUtils::showNpmStyleError("Project creation cancelled", "User cancelled during CI/CD configuration");
    return initialOptions;
  }

  // 第6步：最终确认
  TerminalUtils::showWizardHeader("C++ Project Scaffold", 6, totalSteps);
  TerminalUtils::showWizardProgress(6, totalSteps, "Final Review");

  // 显示摘要并确认
  if (!showSummaryAndConfirm(options)) {
    TerminalUtils::showNpmStyleError("Project creation cancelled", "User cancelled during final review");
    return initialOptions;
  }

  // 询问是否保存作为默认值
  bool saveAsDefault = TerminalUtils::showConfirmDialog("Save these settings as default configuration?", false);

  if (saveAsDefault) {
    // 显示保存中动画
    TerminalUtils::showLoadingDots("Saving default configuration", 1000);

    if (ConfigManager::saveOptionsAsDefaults(options)) {
      TerminalUtils::showNpmStyleSuccess("Default configuration saved");
    } else {
      TerminalUtils::showNpmStyleError("Failed to save default configuration", "Check file permissions and try again");
    }
  }

  // 询问是否保存为模板
  bool saveAsTemplateOption = TerminalUtils::showConfirmDialog("Save these settings as a project template?", false);

  if (saveAsTemplateOption) {
    std::string templateName = TerminalUtils::showValidatedInput(
        "Template name",
        [](const std::string& name) { return !name.empty() && name.length() <= 50; },
        "Template name must be non-empty and less than 50 characters",
        "my-template");

    // 显示保存中动画
    TerminalUtils::showLoadingDots("Saving project template", 1500);

    if (saveAsTemplate(options, templateName)) {
      TerminalUtils::showNpmStyleSuccess("Project template saved", templateName);
    } else {
      TerminalUtils::showNpmStyleError("Failed to save project template", "Check file permissions and try again");
    }
  }

  return options;
}

// 快速启动向导
CliOptions ProjectWizard::runQuickStartWizard() {
  CliOptions options;

  // 显示简化的欢迎信息
  std::vector<std::string> quickStartHeader = {"快速项目创建向导",
                                               "简化配置，快速创建项目"};

  TerminalUtils::showBox(quickStartHeader, BorderStyle::Rounded,
                         Color::BrightCyan, Color::White);
  std::cout << "\n";

  // 询问项目名称
  options.projectName = UserInput::readWithHighlight("项目名称", std::nullopt,
                                                     utils::Color::BrightCyan);

  if (options.projectName.empty()) {
    TerminalUtils::showError("项目名称不能为空。");
    return options;
  }

  // 创建模拟用的项目类型选项表格
  std::vector<std::vector<TableCell>> projectTypeTable;

  // 表头
  std::vector<TableCell> header;
  TableCell typeHeader("类型", Color::BrightWhite, {TextStyle::Bold});
  typeHeader.centered = true;
  TableCell descHeader("描述", Color::BrightWhite, {TextStyle::Bold});
  descHeader.centered = true;
  header.push_back(typeHeader);
  header.push_back(descHeader);
  projectTypeTable.push_back(header);

  // 数据行
  std::vector<TableCell> consoleRow;
  TableCell consoleCell("console", Color::BrightGreen);
  consoleCell.centered = true;
  consoleRow.push_back(consoleCell);
  consoleRow.push_back(TableCell("控制台应用程序"));
  projectTypeTable.push_back(consoleRow);

  std::vector<TableCell> libRow;
  TableCell libCell("lib", Color::BrightYellow);
  libCell.centered = true;
  libRow.push_back(libCell);
  libRow.push_back(TableCell("库项目"));
  projectTypeTable.push_back(libRow);

  std::vector<TableCell> networkRow;
  TableCell networkCell("network", Color::BrightBlue);
  networkCell.centered = true;
  networkRow.push_back(networkCell);
  networkRow.push_back(TableCell("网络应用"));
  projectTypeTable.push_back(networkRow);

  // 显示表格指南
  std::cout << "项目类型参考：\n";
  TerminalUtils::showTable(projectTypeTable, true, BorderStyle::Rounded,
                           Color::BrightCyan);
  std::cout << "\n";

  // 询问项目类型
  std::string templateTypeStr = UserInput::readChoiceWithStyle(
      "选择项目类型", all_template_types(), "console",
      utils::Color::BrightGreen);

  auto templateType = to_template_type(templateTypeStr);
  if (templateType) {
    options.templateType = *templateType;
  }

  // 询问构建系统
  std::string buildSystemStr =
      UserInput::readChoiceWithStyle("选择构建系统", all_build_systems(),
                                     "cmake", utils::Color::BrightGreen);

  auto buildSystem = to_build_system(buildSystemStr);
  if (buildSystem) {
    options.buildSystem = *buildSystem;
  }

  // 询问包管理器
  std::string packageManagerStr = UserInput::readChoiceWithStyle(
      "选择包管理器", all_package_managers(), "vcpkg",
      utils::Color::BrightGreen);

  auto packageManager = to_package_manager(packageManagerStr);
  if (packageManager) {
    options.packageManager = *packageManager;
  }

  // 询问是否包含测试
  options.includeTests =
      UserInput::readConfirmation("是否包含测试框架？", true);

  // 询问是否初始化Git
  options.initGit = UserInput::readConfirmation("是否初始化Git仓库？", true);

  // 显示完成动画
  TerminalUtils::runAnimatedProgress(
      2000, [](int elapsed) { return elapsed / 20; }, // 简单的线性进度函数
      50, Color::BrightGreen, Color::BrightBlack);

  TerminalUtils::showSuccess("快速配置已完成!");
  return options;
}

// 实现项目详情配置
bool ProjectWizard::configureProjectDetails(CliOptions &options) {
  std::cout << "\n";

  // Project name with validation
  std::string defaultName = options.projectName.empty() ? "my-awesome-project" : options.projectName;

  auto projectNameValidator = [](const std::string& name) {
    if (name.empty()) return false;
    if (name.length() > 100) return false;
    // Check for valid characters (alphanumeric, hyphens, underscores)
    for (char c : name) {
      if (!std::isalnum(c) && c != '-' && c != '_') {
        return false;
      }
    }
    return true;
  };

  std::string projectName = TerminalUtils::showValidatedInput(
      "Project name",
      projectNameValidator,
      "Project name must be non-empty, under 100 characters, and contain only letters, numbers, hyphens, and underscores",
      defaultName);

  options.projectName = projectName;
  TerminalUtils::showNpmStyleSuccess("Project name set", projectName);

  // Project path with validation
  std::string defaultPath = std::filesystem::current_path().string();

  auto pathValidator = [](const std::string& path) {
    try {
      std::filesystem::path p(path);
      return std::filesystem::exists(p.parent_path()) || std::filesystem::exists(p);
    } catch (...) {
      return false;
    }
  };

  std::string projectPath = TerminalUtils::showValidatedInput(
      "Project directory",
      pathValidator,
      "Path must be valid and accessible",
      defaultPath);

  TerminalUtils::showNpmStyleSuccess("Project path set", projectPath);

  // Enhanced template selection
  std::cout << "\n";
  TerminalUtils::showNpmStyleCommand("cpp-scaffold create", "Choose your project template");
  std::cout << "\n";

  // Modern template selection
  std::cout << TerminalUtils::colorize("  📋 Available project templates:", Color::BrightCyan) << "\n\n";

  // Show template options with descriptions
  std::vector<std::pair<std::string, std::string>> templateOptions = {
    {"console", "Console application - Perfect for command-line tools"},
    {"lib", "Library project - Create reusable components"},
    {"header-only-lib", "Header-only library - Lightweight template library"},
    {"gui", "GUI application - Modern graphical interface"},
    {"network", "Network application - Client/server communication"},
    {"webservice", "Web service - REST API and web backend"},
    {"embedded", "Embedded project - Microcontroller and IoT"},
    {"gameengine", "Game engine - Game development framework"}
  };

  for (size_t i = 0; i < templateOptions.size(); ++i) {
    const auto& [type, desc] = templateOptions[i];
    std::cout << "  " << TerminalUtils::colorize(std::to_string(i + 1) + ".", Color::BrightBlack)
              << " " << TerminalUtils::colorize(type, Color::BrightGreen)
              << " - " << TerminalUtils::colorize(desc, Color::BrightWhite) << "\n";
  }

  std::cout << "\n";

  // Get user choice
  int choice = TerminalUtils::showInteractiveMenu(
    {"console", "lib", "header-only-lib", "gui", "network", "webservice", "embedded", "gameengine"},
    "Select project template",
    0
  );

  if (choice >= 0 && choice < static_cast<int>(templateOptions.size())) {
    std::string selectedTemplate = templateOptions[choice].first;
    auto templateType = to_template_type(selectedTemplate);
    if (templateType) {
      options.templateType = *templateType;
      TerminalUtils::showNpmStyleSuccess("Template selected", selectedTemplate);
    }
  } else {
    TerminalUtils::showNpmStyleError("Invalid template selection");
    return false;
  }

  // 如果是网络项目，询问网络库
  if (options.templateType == TemplateType::Network) {
    std::vector<std::string_view> networkLibs = {"asio", "boost", "poco"};
    std::string networkLib = UserInput::readChoiceWithStyle(
        "选择网络库", networkLibs, "asio", utils::Color::BrightGreen);

    options.networkLibrary = networkLib;
  }

  return true;
}

// 实现构建系统配置
bool ProjectWizard::configureBuildSystem(CliOptions &options) {
  // 标题
  std::vector<std::string> headerLines = {"构建系统配置",
                                          "选择构建工具和依赖管理方式"};
  TerminalUtils::showBox(headerLines, BorderStyle::Rounded,
                         Color::BrightMagenta, Color::White, "第2步");
  std::cout << "\n";

  // 构建系统信息表格
  std::vector<std::vector<TableCell>> buildSystemTable;

  // 表头
  std::vector<TableCell> header;
  TableCell systemCell("构建系统", Color::BrightWhite, {TextStyle::Bold});
  systemCell.centered = true;

  TableCell featureCell("特点", Color::BrightWhite, {TextStyle::Bold});
  featureCell.centered = false;

  TableCell useCaseCell("适用场景", Color::BrightWhite, {TextStyle::Bold});
  useCaseCell.centered = false;

  header.push_back(systemCell);
  header.push_back(featureCell);
  header.push_back(useCaseCell);
  buildSystemTable.push_back(header);

  // 添加构建系统信息
  std::vector<TableCell> cmakeRow;
  TableCell cmakeCell("cmake", Color::BrightGreen);
  cmakeCell.centered = true;
  cmakeRow.push_back(cmakeCell);
  cmakeRow.push_back(TableCell("跨平台，广泛支持"));
  cmakeRow.push_back(TableCell("通用项目，需要良好跨平台支持"));
  buildSystemTable.push_back(cmakeRow);

  std::vector<TableCell> mesonRow;
  TableCell mesonCell("meson", Color::BrightYellow);
  mesonCell.centered = true;
  mesonRow.push_back(mesonCell);
  mesonRow.push_back(TableCell("现代，快速"));
  mesonRow.push_back(TableCell("需要快速构建的大型项目"));
  buildSystemTable.push_back(mesonRow);

  std::vector<TableCell> makeRow;
  TableCell makeCell("make", Color::BrightRed);
  makeCell.centered = true;
  makeRow.push_back(makeCell);
  makeRow.push_back(TableCell("简单，传统"));
  makeRow.push_back(TableCell("嵌入式或简单项目"));
  buildSystemTable.push_back(makeRow);

  // 显示选项表格
  std::cout << "构建系统对比：\n";
  TerminalUtils::showTable(buildSystemTable, true, BorderStyle::Rounded,
                           Color::BrightMagenta);
  std::cout << "\n";

  // 构建系统
  std::string buildSystemStr = UserInput::readChoiceWithStyle(
      "选择构建系统", all_build_systems(),
      to_string(options.buildSystem), utils::Color::BrightGreen);

  auto buildSystem = to_build_system(buildSystemStr);
  if (buildSystem) {
    options.buildSystem = *buildSystem;
  }

  // 包管理器信息
  std::cout << "\n"
            << TerminalUtils::colorize("包管理器说明：", Color::BrightCyan,
                                       Color::Reset)
            << "\n";
  std::cout << " - vcpkg: 微软维护，与Visual Studio集成良好\n";
  std::cout << " - conan: C++包管理器，中心化存储库\n";
  std::cout << " - none: 不使用包管理器，手动管理依赖\n\n";

  // 包管理器
  std::string packageManagerStr = UserInput::readChoiceWithStyle(
      "选择包管理器", all_package_managers(),
      to_string(options.packageManager), utils::Color::BrightGreen);

  auto packageManager = to_package_manager(packageManagerStr);
  if (packageManager) {
    options.packageManager = *packageManager;
  }

  // 显示完成动画
  TerminalUtils::runSpinner(500, "应用配置中...", 10);
  TerminalUtils::showSuccess("构建系统配置已完成");

  return true;
}

// 实现测试框架配置
bool ProjectWizard::configureTestFramework(CliOptions &options) {
  // 标题
  std::vector<std::string> headerLines = {"测试框架配置", "配置测试和文档支持"};
  TerminalUtils::showBox(headerLines, BorderStyle::Rounded,
                         Color::BrightMagenta, Color::White, "第3步");
  std::cout << "\n";

  // 测试框架简介
  std::cout << TerminalUtils::colorize("为什么需要测试？", Color::BrightYellow,
                                       Color::Reset)
            << "\n";
  std::vector<std::string> testDesc = {"单元测试帮助验证代码正确性",
                                       "自动化测试提高开发效率",
                                       "测试驱动开发(TDD)可以改进代码设计"};
  TerminalUtils::showBox(testDesc, BorderStyle::Single, Color::BrightBlue,
                         Color::White);
  std::cout << "\n";

  // 是否包含测试
  options.includeTests =
      UserInput::readConfirmation("是否包含测试框架？", options.includeTests);

  if (options.includeTests) {
    // 测试框架比较
    std::vector<std::vector<TableCell>> testFrameworkTable;

    // 表头
    std::vector<TableCell> headerRow;
    TableCell frameworkCell("框架", Color::BrightWhite, {TextStyle::Bold});
    frameworkCell.centered = true;
    headerRow.push_back(frameworkCell);

    TableCell featureCell("特点", Color::BrightWhite, {TextStyle::Bold});
    featureCell.centered = false;
    headerRow.push_back(featureCell);
    testFrameworkTable.push_back(headerRow);

    // 各测试框架
    std::vector<TableCell> gtestRow;
    TableCell gtestCell("gtest", Color::BrightGreen);
    gtestCell.centered = true;
    gtestRow.push_back(gtestCell);
    gtestRow.push_back(TableCell("Google开发，功能丰富，广泛使用"));
    testFrameworkTable.push_back(gtestRow);

    std::vector<TableCell> catch2Row;
    TableCell catch2Cell("catch2", Color::BrightYellow);
    catch2Cell.centered = true;
    catch2Row.push_back(catch2Cell);
    catch2Row.push_back(TableCell("现代C++设计，头文件即可使用"));
    testFrameworkTable.push_back(catch2Row);

    std::vector<TableCell> doctestRow;
    TableCell doctestCell("doctest", Color::BrightBlue);
    doctestCell.centered = true;
    doctestRow.push_back(doctestCell);
    doctestRow.push_back(TableCell("轻量级，快速编译"));
    testFrameworkTable.push_back(doctestRow);

    // 显示表格
    std::cout << "测试框架比较：\n";
    TerminalUtils::showTable(testFrameworkTable, true, BorderStyle::Rounded,
                             Color::BrightMagenta);
    std::cout << "\n";

    // 测试框架
    std::string testFrameworkStr = UserInput::readChoiceWithStyle(
        "选择测试框架", all_test_frameworks(),
        to_string(options.testFramework), utils::Color::BrightGreen);

    auto testFramework = to_test_framework(testFrameworkStr);
    if (testFramework) {
      options.testFramework = *testFramework;
    }
  }

  // 是否包含文档
  options.includeDocumentation = UserInput::readConfirmation(
      "是否包含文档配置？", options.includeDocumentation);

  if (options.includeDocumentation) {
    // 显示文档配置动画
    TerminalUtils::runSpinner(800, "准备文档工具配置...", 10);
    TerminalUtils::showInfo("将添加Doxygen文档配置");
  }

  return true;
}

// 实现开发工具配置
bool ProjectWizard::configureDevTools(CliOptions &options) {
  // 标题
  std::vector<std::string> headerLines = {"开发工具配置",
                                          "配置编辑器和开发辅助工具"};
  TerminalUtils::showBox(headerLines, BorderStyle::Rounded,
                         Color::BrightMagenta, Color::White, "第4步");
  std::cout << "\n";

  // 代码风格工具说明
  std::vector<std::string> codeStyleDesc = {
      "代码风格工具可以：", " - 保持代码一致性", " - 自动检测潜在问题",
      " - 强制执行团队代码规范"};
  TerminalUtils::showBox(codeStyleDesc, BorderStyle::Single, Color::BrightBlue,
                         Color::White);
  std::cout << "\n";

  // 是否包含代码风格工具
  options.includeCodeStyleTools = UserInput::readConfirmation(
      "是否包含代码风格和静态分析工具？", options.includeCodeStyleTools);

  if (options.includeCodeStyleTools) {
    TerminalUtils::runSpinner(600, "配置代码风格工具...", 10);
    TerminalUtils::showInfo("将添加clang-format和clang-tidy配置");
  }

  // 编辑器配置说明
  std::cout << "\n"
            << TerminalUtils::colorize("编辑器配置将添加对应IDE的支持文件：",
                                       Color::BrightCyan, Color::Reset)
            << "\n";
  std::cout << " - VSCode: tasks.json, launch.json配置\n";
  std::cout << " - CLion: 额外的CMake配置\n";
  std::cout << " - VS: 解决方案和项目文件\n\n";

  // 编辑器配置
  std::vector<std::string_view> editorOptions = all_editor_configs();

  // 转换现有的编辑器选项为字符串
  std::vector<std::string_view> defaultEditors;
  for (const auto &editor : options.editorOptions) {
    defaultEditors.push_back(to_string(editor));
  }

  std::vector<std::string> selectedEditors = UserInput::readMultiChoice(
      "选择要配置的编辑器", editorOptions, defaultEditors);

  // 清空现有选项并添加新选择
  options.editorOptions.clear();
  for (const auto &editor : selectedEditors) {
    auto editorConfig = to_editor_config(editor);
    if (editorConfig) {
      options.editorOptions.push_back(*editorConfig);
    }
  }

  // 处理每个选择的编辑器
  if (!options.editorOptions.empty()) {
    TerminalUtils::runSpinner(1000, "创建编辑器配置文件...", 10);
    TerminalUtils::showSuccess("编辑器配置已添加");
  }

  return true;
}

// 实现CI/CD系统配置
bool ProjectWizard::configureCiCd(CliOptions &options) {
  // 标题
  std::vector<std::string> headerLines = {"CI/CD系统配置",
                                          "配置持续集成和部署"};
  TerminalUtils::showBox(headerLines, BorderStyle::Rounded,
                         Color::BrightMagenta, Color::White, "第5步");
  std::cout << "\n";

  // CI/CD系统说明
  std::vector<std::string> cicdDesc = {
      "持续集成/持续部署可以：", " - 自动构建和测试代码",
      " - 在提交代码时自动执行检查", " - 简化发布流程"};
  TerminalUtils::showBox(cicdDesc, BorderStyle::Single, Color::BrightBlue,
                         Color::White);
  std::cout << "\n";

  // 是否初始化Git
  options.initGit =
      UserInput::readConfirmation("是否初始化Git仓库？", options.initGit);

  if (options.initGit) {
    // 显示Git信息
    std::cout << "\n"
              << TerminalUtils::colorize("Git配置将包含：", Color::BrightCyan,
                                         Color::Reset)
              << "\n";
    std::cout << " - 初始化本地仓库\n";
    std::cout << " - 添加适当的.gitignore文件\n";
    std::cout << " - 创建初始提交\n\n";

    // CI/CD系统
    std::vector<std::string_view> ciOptions = all_ci_systems();

    // 转换现有的CI选项为字符串
    std::vector<std::string_view> defaultCi;
    for (const auto &ci : options.ciOptions) {
      defaultCi.push_back(to_string(ci));
    }

    // CI/CD系统比较表格
    std::vector<std::vector<TableCell>> ciSystemTable;

    // 表头
    std::vector<TableCell> headerRow;
    TableCell systemCell("CI系统", Color::BrightWhite, {TextStyle::Bold});
    systemCell.centered = true;
    headerRow.push_back(systemCell);

    TableCell featureCell("特点", Color::BrightWhite, {TextStyle::Bold});
    featureCell.centered = false;
    headerRow.push_back(featureCell);
    ciSystemTable.push_back(headerRow);

    // 各CI系统
    std::vector<TableCell> githubRow;
    TableCell githubCell("github", Color::BrightGreen);
    githubCell.centered = true;
    githubRow.push_back(githubCell);
    githubRow.push_back(TableCell("GitHub Actions，与GitHub完美集成"));
    ciSystemTable.push_back(githubRow);

    std::vector<TableCell> gitlabRow;
    TableCell gitlabCell("gitlab", Color::BrightYellow);
    gitlabCell.centered = true;
    gitlabRow.push_back(gitlabCell);
    gitlabRow.push_back(TableCell("GitLab CI，适合自托管GitLab"));
    ciSystemTable.push_back(gitlabRow);

    std::vector<TableCell> travisRow;
    TableCell travisCell("travis", Color::BrightBlue);
    travisCell.centered = true;
    travisRow.push_back(travisCell);
    travisRow.push_back(TableCell("Travis CI，老牌CI服务"));
    ciSystemTable.push_back(travisRow);

    // 显示表格
    std::cout << "CI/CD系统比较：\n";
    TerminalUtils::showTable(ciSystemTable, true, BorderStyle::Rounded,
                             Color::BrightMagenta);
    std::cout << "\n";

    std::vector<std::string> selectedCi = UserInput::readMultiChoice(
        "选择要配置的CI/CD系统", ciOptions, defaultCi);

    // 清空现有选项并添加新选择
    options.ciOptions.clear();
    for (const auto &ci : selectedCi) {
      auto ciSystem = to_ci_system(ci);
      if (ciSystem) {
        options.ciOptions.push_back(*ciSystem);
      }
    }
  }

  return true;
}

// 实现项目配置总结和确认
bool ProjectWizard::showSummaryAndConfirm(const CliOptions &options) {
  // 标题
  std::vector<std::string> headerLines = {"项目配置摘要", "确认您的项目配置"};
  TerminalUtils::showBox(headerLines, BorderStyle::Rounded,
                         Color::BrightMagenta, Color::White);
  std::cout << "\n";

  // 构建摘要信息
  std::vector<std::vector<TableCell>> summaryTable;

  // 表头
  // 表头
  std::vector<TableCell> headerRow;
  TableCell configCell("配置项", Color::BrightWhite, {TextStyle::Bold});
  configCell.centered = true;
  headerRow.push_back(configCell);

  TableCell valueCell("已选值", Color::BrightWhite, {TextStyle::Bold});
  valueCell.centered = false;
  headerRow.push_back(valueCell);
  summaryTable.push_back(headerRow);

  // 基本项目信息
  std::vector<TableCell> projectNameRow;
  TableCell projectNameTitle("项目名称", Color::BrightYellow);
  projectNameTitle.centered = true;
  projectNameRow.push_back(projectNameTitle);

  TableCell projectNameValue(options.projectName, Color::BrightGreen);
  projectNameRow.push_back(projectNameValue);
  summaryTable.push_back(projectNameRow);

  std::vector<TableCell> templateTypeRow;
  TableCell templateTypeTitle("项目类型", Color::BrightYellow);
  templateTypeTitle.centered = true;
  templateTypeRow.push_back(templateTypeTitle);

  TableCell templateTypeValue(
      std::string(to_string(options.templateType)), Color::BrightGreen);
  templateTypeRow.push_back(templateTypeValue);
  summaryTable.push_back(templateTypeRow);

  std::vector<TableCell> buildSystemRow;
  TableCell buildSystemTitle("构建系统", Color::BrightYellow);
  buildSystemTitle.centered = true;
  buildSystemRow.push_back(buildSystemTitle);

  TableCell buildSystemValue(std::string(to_string(options.buildSystem)),
                             Color::BrightGreen);
  buildSystemRow.push_back(buildSystemValue);
  summaryTable.push_back(buildSystemRow);

  std::vector<TableCell> packageManagerRow;
  TableCell packageManagerTitle("包管理器", Color::BrightYellow);
  packageManagerTitle.centered = true;
  packageManagerRow.push_back(packageManagerTitle);

  TableCell packageManagerValue(
      std::string(to_string(options.packageManager)),
      Color::BrightGreen);
  packageManagerRow.push_back(packageManagerValue);
  summaryTable.push_back(packageManagerRow);

  if (options.networkLibrary) {
    std::vector<TableCell> networkLibRow;
    TableCell networkLibTitle("网络库", Color::BrightYellow);
    networkLibTitle.centered = true;
    networkLibRow.push_back(networkLibTitle);
  }

  if (options.includeTests) {
    std::vector<TableCell> testFrameworkRow;
    TableCell testFrameworkTitle("测试框架", Color::BrightYellow);
    testFrameworkTitle.centered = true;
    testFrameworkRow.push_back(testFrameworkTitle);

    TableCell testFrameworkValue(
        std::string(to_string(options.testFramework)),
        Color::BrightGreen);
    testFrameworkRow.push_back(testFrameworkValue);
    summaryTable.push_back(testFrameworkRow);
  }

  std::vector<TableCell> docRow;
  TableCell docTitle("包含文档", Color::BrightYellow);
  docTitle.centered = true;
  docRow.push_back(docTitle);

  TableCell docValue(options.includeDocumentation ? "是" : "否",
                     options.includeDocumentation ? Color::BrightGreen
                                                  : Color::BrightRed);
  docRow.push_back(docValue);
  summaryTable.push_back(docRow);

  std::vector<TableCell> codeStyleRow;
  TableCell codeStyleTitle("代码风格工具", Color::BrightYellow);
  codeStyleTitle.centered = true;
  codeStyleRow.push_back(codeStyleTitle);

  TableCell codeStyleValue(options.includeCodeStyleTools ? "是" : "否",
                           options.includeCodeStyleTools ? Color::BrightGreen
                                                         : Color::BrightRed);
  codeStyleRow.push_back(codeStyleValue);
  summaryTable.push_back(codeStyleRow);

  if (!options.editorOptions.empty()) {
    std::string editors;
    for (size_t i = 0; i < options.editorOptions.size(); ++i) {
      if (i > 0)
        editors += ", ";
      editors += std::string(to_string(options.editorOptions[i]));
    }

    std::vector<TableCell> editorRow;
    TableCell editorTitle("编辑器配置", Color::BrightYellow);
    editorTitle.centered = true;
    editorRow.push_back(editorTitle);

    TableCell editorValue(editors, Color::BrightGreen);
    editorRow.push_back(editorValue);
    summaryTable.push_back(editorRow);
  }

  std::vector<TableCell> gitRow;
  TableCell gitTitle("初始化Git", Color::BrightYellow);
  gitTitle.centered = true;
  gitRow.push_back(gitTitle);

  TableCell gitValue(options.initGit ? "是" : "否",
                     options.initGit ? Color::BrightGreen : Color::BrightRed);
  gitRow.push_back(gitValue);
  summaryTable.push_back(gitRow);

  if (options.initGit && !options.ciOptions.empty()) {
    std::string ciSystems;
    for (size_t i = 0; i < options.ciOptions.size(); ++i) {
      if (i > 0)
        ciSystems += ", ";
      ciSystems += std::string(to_string(options.ciOptions[i]));
    }

    std::vector<TableCell> ciRow;
    TableCell ciTitle("CI/CD系统", Color::BrightYellow);
    ciTitle.centered = true;
    ciRow.push_back(ciTitle);

    TableCell ciValue(ciSystems, Color::BrightGreen);
    ciRow.push_back(ciValue);
    summaryTable.push_back(ciRow);
  }

  if (options.initGit && !options.ciOptions.empty()) {
    std::string ciSystems;
    for (size_t i = 0; i < options.ciOptions.size(); ++i) {
      if (i > 0)
        ciSystems += ", ";
      ciSystems += std::string(to_string(options.ciOptions[i]));
    }

    std::vector<TableCell> ciRow;
    TableCell ciTitle("CI/CD系统", Color::BrightYellow);
    ciTitle.centered = true;
    ciRow.push_back(ciTitle);

    TableCell ciValue(ciSystems, Color::BrightGreen);
    ciRow.push_back(ciValue);
    summaryTable.push_back(ciRow);
  }

  // 显示摘要表格
  TerminalUtils::showTable(summaryTable, true, BorderStyle::Rounded,
                           Color::BrightBlue);
  std::cout << "\n";

  // 生成项目预览
  std::vector<std::string> projectPreview;
  projectPreview.push_back("项目" + options.projectName + "将包含：");
  projectPreview.push_back(
      " - " + std::string(to_string(options.buildSystem)) + "构建配置");
  projectPreview.push_back(" - src/目录包含源代码架构");
  projectPreview.push_back(" - include/目录包含头文件");

  if (options.includeTests) {
    projectPreview.push_back(" - tests/目录包含测试框架");
  }

  if (options.includeDocumentation) {
    projectPreview.push_back(" - docs/目录包含文档配置");
  }

  if (options.initGit) {
    projectPreview.push_back(" - .git目录和基本Git配置");
  }

  // 显示项目预览
  TerminalUtils::showBox(projectPreview, BorderStyle::Single,
                         Color::BrightGreen, Color::White, "项目预览");
  std::cout << "\n";

  // 确认是否继续，使用动画按钮样式
  std::cout << TerminalUtils::colorize("请确认配置无误后继续",
                                       Color::BrightYellow)
            << "\n\n";

  return UserInput::readConfirmation("确认以上配置并创建项目？", true);
}

// 实现欢迎界面
void ProjectWizard::showWelcomeScreen() {
  // 清屏
  TerminalUtils::clearScreen();

  // 显示标题
  std::vector<std::string> welcomeLines = {
      "欢迎使用 C++ 项目脚手架工具", "互动项目创建向导",
      "本向导将引导您完成项目创建的各个步骤"};

  // 使用更漂亮的边框样式并添加颜色
  TerminalUtils::showBox(welcomeLines, BorderStyle::Double, Color::BrightCyan,
                         Color::White, "C++ Project Scaffold");

  // 添加版本信息和提示
  std::cout << "\n"
            << TerminalUtils::colorize("版本: 1.1.0", Color::BrightBlue)
            << "\n";
  std::cout << TerminalUtils::colorize("使用方向键和Enter进行选择",
                                       Color::BrightYellow)
            << "\n\n";

  // 显示加载动画
  TerminalUtils::runSpinner(1500, "正在加载项目模板...", 10);

  // 提示用户按Enter继续
  std::cout << TerminalUtils::colorize("按Enter键继续...",
                                       utils::Color::BrightYellow)
            << "\n";
  std::cin.get();
}

// Enhanced Next.js-style welcome screen
void ProjectWizard::showEnhancedWelcomeScreen() {
  // Show animated logo
  TerminalUtils::showAnimatedLogo();

  // Show branded header with typing animation
  TerminalUtils::showBrandedHeader("C++ Project Scaffold", "Create modern C++ projects with ease");

  // Show typing animation for welcome message
  TerminalUtils::showTypingAnimation("Welcome to the interactive project creation wizard!", 30);
  std::cout << "\n";

  // Show feature highlights
  std::vector<std::pair<std::string, std::string>> features = {
    {"🚀", "Modern C++ project templates"},
    {"⚡", "Multiple build systems (CMake, Meson, Bazel)"},
    {"📦", "Package manager integration (vcpkg, Conan)"},
    {"🧪", "Testing framework setup (GTest, Catch2)"},
    {"🔧", "IDE configuration (VS Code, CLion)"},
    {"🔄", "CI/CD pipeline templates"}
  };

  std::cout << TerminalUtils::colorAndStyle("✨ Features:", Color::BrightYellow, {TextStyle::Bold}) << "\n\n";

  for (const auto& feature : features) {
    std::cout << "  " << TerminalUtils::colorize(feature.first, Color::BrightGreen)
              << " " << TerminalUtils::colorize(feature.second, Color::White) << "\n";
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
  }

  std::cout << "\n";

  // Show pulsing "Let's get started" message
  TerminalUtils::showPulsingText("Let's create something amazing!", Color::BrightCyan, 2);

  // Wait for user to continue
  std::cout << "\n" << TerminalUtils::colorize("Press Enter to continue...", Color::BrightYellow);
  std::cin.get();
}

// NPM-style welcome screen
void ProjectWizard::showNpmStyleWelcomeScreen() {
  // Show NPM-style header
  TerminalUtils::showNpmStyleHeader("cpp-scaffold", "1.2.0");

  // Show welcome message
  std::cout << TerminalUtils::colorize("  Welcome to C++ Project Scaffold!", Color::BrightWhite) << "\n";
  std::cout << TerminalUtils::colorize("  The modern way to create C++ projects", Color::BrightBlack) << "\n\n";

  // Show what we'll create
  std::cout << TerminalUtils::colorize("  This wizard will help you:", Color::BrightCyan) << "\n";
  std::cout << TerminalUtils::colorize("  ✓", Color::BrightGreen) << " Choose a project template\n";
  std::cout << TerminalUtils::colorize("  ✓", Color::BrightGreen) << " Configure build system\n";
  std::cout << TerminalUtils::colorize("  ✓", Color::BrightGreen) << " Set up package management\n";
  std::cout << TerminalUtils::colorize("  ✓", Color::BrightGreen) << " Add testing framework\n";
  std::cout << TerminalUtils::colorize("  ✓", Color::BrightGreen) << " Configure development tools\n";
  std::cout << TerminalUtils::colorize("  ✓", Color::BrightGreen) << " Set up CI/CD pipeline\n\n";

  // Show loading animation
  TerminalUtils::showLoadingDots("Initializing project wizard", 2000);

  std::cout << "\n";
  TerminalUtils::showNpmStyleSuccess("Ready to create your project!");

  // Confirmation to continue
  if (!TerminalUtils::showConfirmDialog("Continue with project creation?", true)) {
    TerminalUtils::showNpmStyleWarning("Project creation cancelled");
    exit(0);
  }
}

// 显示向导进度
void ProjectWizard::showWizardProgress(int currentStep, int totalSteps,
                                       const std::string &stepName) {
  std::cout << "\n";

  // 计算进度百分比
  int percent = (currentStep * 100) / totalSteps;

  // 显示进度标题
  std::cout << TerminalUtils::colorize("步骤 " + std::to_string(currentStep) +
                                           " / " + std::to_string(totalSteps) +
                                           ": " + stepName,
                                       utils::Color::BrightBlue)
            << "\n";

  // 显示增强的彩色进度条
  TerminalUtils::showProgressBar(percent, 50, Color::BrightGreen,
                                 Color::BrightBlack);
  std::cout << "\n\n";

  // 短暂延迟以展示进度
  std::this_thread::sleep_for(std::chrono::milliseconds(500));
}

// 实现保存模板功能
bool ProjectWizard::saveAsTemplate(const CliOptions &options,
                                   const std::string &templateName) {
  try {
    // 创建模板目录
    std::filesystem::path templateDir =
        std::filesystem::path(std::getenv("HOME")) / ".config" /
        "cpp-scaffold" / "templates";

    std::filesystem::create_directories(templateDir);

    // 构建模板文件路径
    std::filesystem::path templatePath = templateDir / (templateName + ".json");

    // 使用 ConfigManager 的逻辑保存为JSON
    return ConfigManager::saveOptionsAsDefaults(options);
  } catch (const std::exception &e) {
    spdlog::error("保存模板失败: {}", e.what());
    return false;
  }
}

// 根据模板类型获取推荐配置
CliOptions ProjectWizard::getRecommendedOptionsForTemplate(TemplateType type) {
  CliOptions options;
  options.templateType = type;

  // 根据项目类型设置推荐配置
  switch (type) {
  case TemplateType::Console:
    options.buildSystem = BuildSystem::CMake;
    options.packageManager = PackageManager::Vcpkg;
    options.includeTests = true;
    options.testFramework = TestFramework::Catch2;
    options.editorOptions = {EditorConfig::VSCode};
    break;

  case TemplateType::Lib:
    options.buildSystem = BuildSystem::CMake;
    options.packageManager = PackageManager::Vcpkg;
    options.includeTests = true;
    options.testFramework = TestFramework::GTest;
    options.includeDocumentation = true;
    options.includeCodeStyleTools = true;
    options.editorOptions = {EditorConfig::VSCode, EditorConfig::CLion};
    break;

  case TemplateType::Network:
    options.buildSystem = BuildSystem::CMake;
    options.packageManager = PackageManager::Conan;
    options.networkLibrary = "asio";
    options.includeTests = true;
    options.testFramework = TestFramework::Catch2;
    options.editorOptions = {EditorConfig::VSCode};
    break;

  case TemplateType::Gui:
    options.buildSystem = BuildSystem::CMake;
    options.packageManager = PackageManager::Vcpkg;
    options.includeTests = false;
    options.editorOptions = {EditorConfig::VS, EditorConfig::VSCode};
    break;

  case TemplateType::Embedded:
    options.buildSystem = BuildSystem::Make;
    options.packageManager = PackageManager::None;
    options.includeTests = true;
    options.editorOptions = {EditorConfig::VSCode};
    break;

  case TemplateType::WebService:
    options.buildSystem = BuildSystem::CMake;
    options.packageManager = PackageManager::Conan;
    options.includeTests = true;
    options.testFramework = TestFramework::GTest;
    options.editorOptions = {EditorConfig::VSCode, EditorConfig::CLion};
    options.ciOptions = {CiSystem::GitHub};
    break;

  case TemplateType::GameEngine:
    options.buildSystem = BuildSystem::CMake;
    options.packageManager = PackageManager::Vcpkg;
    options.includeTests = true;
    options.testFramework = TestFramework::GTest;
    options.editorOptions = {EditorConfig::VS, EditorConfig::VSCode};
    break;

  default:
    // 默认配置
    options.buildSystem = BuildSystem::CMake;
    options.packageManager = PackageManager::Vcpkg;
    options.includeTests = true;
    options.testFramework = TestFramework::GTest;
    options.editorOptions = {EditorConfig::VSCode};
  }

  return options;
}

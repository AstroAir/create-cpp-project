#include "cli_parser.h"
#include "../config/config_manager.h"
#include "../utils/terminal_utils.h"
#include <iostream>
#include <spdlog/spdlog.h>

using namespace utils;

// Main parsing method
CliOptions CliParser::parse(int argc, char* argv[]) {
  try {
    // Initialize localization
    Localization::initialize();

    // Use the argument parser to handle all parsing logic
    return argument_parser::parseArguments(argc, argv);

  } catch (const std::exception& e) {
    handleParsingError(e.what());
    CliOptions errorOptions;
    errorOptions.showHelp = true;
    return errorOptions;
  }
}

// Interactive options configuration
CliOptions CliParser::promptUserForOptions(const CliOptions& defaultOptions) {
  CliOptions options = defaultOptions;
  Language lang = Localization::getCurrentLanguage();

  // Show welcome message
  TerminalUtils::showInfo(
      std::string(Localization::get("welcomeMessage", lang)));
  std::cout << "\n";

  // Load default configuration
  CliOptions defaults = loadDefaultConfiguration();

  // Get system-suggested defaults
  CliOptions systemDefaults = getSystemSuggestedDefaults();

  // If no project name, ask for it
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

  // Template type
  if (options.templateType == defaults.templateType) {
    std::string templateTypeStr = UserInput::readChoiceWithStyle(
        Localization::get("selectProjectType", lang),
        cli_enums::all_template_types(), cli_enums::to_string(defaults.templateType),
        utils::Color::BrightGreen);

    auto templateType = cli_enums::to_template_type(templateTypeStr);
    if (templateType) {
      options.templateType = *templateType;
    }
  }

  // Build system
  if (options.buildSystem == defaults.buildSystem) {
    std::string buildSystemStr = UserInput::readChoiceWithStyle(
        Localization::get("selectBuildSystem", lang),
        cli_enums::all_build_systems(), cli_enums::to_string(defaults.buildSystem),
        utils::Color::BrightGreen);

    auto buildSystem = cli_enums::to_build_system(buildSystemStr);
    if (buildSystem) {
      options.buildSystem = *buildSystem;
    }
  }

  // Package manager
  if (options.packageManager == defaults.packageManager) {
    std::string packageManagerStr = UserInput::readChoiceWithStyle(
        Localization::get("selectPackageManager", lang),
        cli_enums::all_package_managers(), cli_enums::to_string(defaults.packageManager),
        utils::Color::BrightGreen);

    auto packageManager = cli_enums::to_package_manager(packageManagerStr);
    if (packageManager) {
      options.packageManager = *packageManager;
    }
  }

  // Network library for network projects
  if (options.templateType == TemplateType::Network) {
    std::vector<std::string_view> networkLibs = {"asio", "boost", "poco"};
    std::string networkLib = UserInput::readChoiceWithStyle(
        "Select network library", networkLibs, "asio", utils::Color::BrightGreen);

    options.networkLibrary = networkLib;
  }

  // Testing
  options.includeTests =
      UserInput::readConfirmation(Localization::get("includeTests", lang),
                                  defaults.includeTests);

  if (options.includeTests) {
    std::string testFrameworkStr = UserInput::readChoiceWithStyle(
        Localization::get("selectTestFramework", lang),
        cli_enums::all_test_frameworks(), cli_enums::to_string(defaults.testFramework),
        utils::Color::BrightGreen);

    auto testFramework = cli_enums::to_test_framework(testFrameworkStr);
    if (testFramework) {
      options.testFramework = *testFramework;
    }
  }

  // Documentation
  options.includeDocumentation = UserInput::readConfirmation(
      Localization::get("includeDocumentation", lang),
      defaults.includeDocumentation);

  // Code style tools
  options.includeCodeStyleTools = UserInput::readConfirmation(
      Localization::get("includeCodeStyleTools", lang),
      defaults.includeCodeStyleTools);

  // Editor configuration
  std::vector<std::string_view> editorOptions = cli_enums::all_editor_configs();
  std::vector<std::string_view> defaultEditors;
  for (const auto& editor : defaults.editorOptions) {
    defaultEditors.push_back(cli_enums::to_string(editor));
  }

  std::vector<std::string> selectedEditors = UserInput::readMultiChoice(
      Localization::get("selectEditors", lang), editorOptions, defaultEditors);

  options.editorOptions.clear();
  for (const auto& editor : selectedEditors) {
    auto editorConfig = cli_enums::to_editor_config(editor);
    if (editorConfig) {
      options.editorOptions.push_back(*editorConfig);
    }
  }

  // Git initialization
  options.initGit =
      UserInput::readConfirmation(Localization::get("initGit", lang),
                                  defaults.initGit);

  if (options.initGit) {
    // CI/CD systems
    std::vector<std::string_view> ciOptions = cli_enums::all_ci_systems();
    std::vector<std::string_view> defaultCi;
    for (const auto& ci : defaults.ciOptions) {
      defaultCi.push_back(cli_enums::to_string(ci));
    }

    std::vector<std::string> selectedCi = UserInput::readMultiChoice(
        Localization::get("selectCISystems", lang), ciOptions, defaultCi);

    options.ciOptions.clear();
    for (const auto& ci : selectedCi) {
      auto ciSystem = cli_enums::to_ci_system(ci);
      if (ciSystem) {
        options.ciOptions.push_back(*ciSystem);
      }
    }
  }

  // Ask if user wants to save as defaults
  bool saveAsDefault = UserInput::readConfirmation(
      Localization::get("saveAsDefault", lang), false);

  if (saveAsDefault) {
    try {
      auto& configManager = config::ConfigManager::getInstance();
      configManager.saveOptionsAsDefaults(options);
      TerminalUtils::showSuccess("Default configuration saved successfully");
    } catch (const std::exception& e) {
      TerminalUtils::showError("Failed to save default configuration: " + std::string(e.what()));
    }
  }

  return options;
}

// Validation methods
bool CliParser::validateOptions(CliOptions& options) {
  bool isValid = true;

  // Validate project name
  if (!validateProjectName(options.projectName)) {
    spdlog::error("Invalid project name: {}", options.projectName);
    isValid = false;
  }

  // Validate template-specific requirements
  if (options.templateType == TemplateType::Network && !options.networkLibrary.has_value()) {
    spdlog::warn("Network template selected but no network library specified, using default 'asio'");
    options.networkLibrary = "asio";
  }

  // Validate test framework selection
  if (options.includeTests && options.testFramework == TestFramework::None) {
    spdlog::warn("Tests enabled but no test framework selected, using default GTest");
    options.testFramework = TestFramework::GTest;
  }

  return isValid;
}

// Interactive mode
CliOptions CliParser::runInteractiveMode() {
  cli_commands::showCliHeader();

  ProjectWizard wizard;
  return wizard.runWizard();
}

// System detection and defaults
CliOptions CliParser::getSystemSuggestedDefaults() {
  return cli_commands::getSystemSuggestedDefaults();
}

// Configuration loading
CliOptions CliParser::loadDefaultConfiguration() {
  try {
    auto& configManager = config::ConfigManager::getInstance();
    return configManager.loadDefaultOptions();
  } catch (const std::exception& e) {
    spdlog::warn("Failed to load default configuration: {}", e.what());
    return getSystemSuggestedDefaults();
  }
}

// Validation helpers
bool CliParser::validateProjectName(const std::string& name) {
  if (name.empty()) return false;
  if (name.length() > 100) return false;

  // Check for valid characters (alphanumeric, hyphens, underscores)
  for (char c : name) {
    if (!std::isalnum(c) && c != '-' && c != '_') {
      return false;
    }
  }

  return true;
}

bool CliParser::validateProjectPath(const std::string& path) {
  try {
    std::filesystem::path p(path);
    return std::filesystem::exists(p.parent_path()) || std::filesystem::exists(p);
  } catch (...) {
    return false;
  }
}

// Error handling
void CliParser::handleParsingError(const std::string& error) {
  spdlog::error("CLI parsing error: {}", error);
  TerminalUtils::showError("Failed to parse command line arguments: " + error);
}

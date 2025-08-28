#include "command_handlers.h"
#include "../localization/localization.h"
#include "../../utils/terminal_utils.h"
#include "../../utils/file_utils.h"
#include "../../utils/string_utils.h"
#include "../../utils/enhanced_terminal.h"
#include "../../utils/enhanced_wizard.h"
#include "../../config/config_manager.h"
#include "../../config/interactive_config.h"
#include "../../config/project_profiles.h"
#include "../../config/config_validator.h"
#include <iostream>
#include <fmt/format.h>
#include <fmt/color.h>

using namespace utils;

namespace cli_commands {

// Show help information
void showHelp(Language lang) {
  (void)lang; // TODO: Implement localization support
  std::cout << TerminalUtils::colorize("**CPP-Scaffold - C++ Project Scaffolding Tool**",
                                       utils::Color::BrightCyan)
            << "\n\n";

  std::cout << TerminalUtils::colorize("Usage:", utils::Color::BrightYellow)
            << "\n";
  std::cout << "  cpp-scaffold create <project-name> [options]\n";
  std::cout << "  cpp-scaffold new <project-name> [options]\n";
  std::cout << "  cpp-scaffold interactive\n";
  std::cout << "  cpp-scaffold config\n";
  std::cout << "  cpp-scaffold list-templates\n";
  std::cout << "  cpp-scaffold validate [project-path]\n\n";

  std::cout << TerminalUtils::colorize("**Options:**", utils::Color::BrightYellow)
            << "\n";
  fmt::print("  -t, --template <type>        Project template type: console, lib, header-only-lib, "
             "modules, multi-executable, gui, network, embedded, webservice, gameengine, "
             "qt-app, sfml-app, boost-app, test-project\n");
  fmt::print("  -b, --build <system>         Build system: cmake, meson, bazel, "
             "xmake, premake, make, ninja\n");
  fmt::print("  -p, --package <manager>      Package manager: vcpkg, conan, none, "
             "spack, hunter, cpm, fetchcontent, msys2\n");
  fmt::print("  --std <standard>             C++ standard: cpp11, cpp14, cpp17, cpp20, cpp23 "
             "(modules require cpp20+)\n");
  fmt::print(
      "  --network-lib <library>      Network library for network projects: asio, boost, poco\n");
  fmt::print("  --tests [framework]          Include test framework: gtest, catch2, "
             "doctest, boost, none\n");
  fmt::print("  --docs, --documentation      Include documentation configuration\n");
  fmt::print("  --code-style                 Include code style and static analysis tools\n");
  fmt::print("  --editor <editor>            Configure editor support: vscode, clion, "
             "vs, vim, emacs, sublime\n");
  fmt::print("                               (can be used multiple times)\n");
  fmt::print("  --ci, --cicd <system>        Configure CI/CD: github, gitlab, "
             "travis, appveyor, azure, circleci\n");
  fmt::print("                               (can be used multiple times)\n");
  fmt::print("  --no-git                     Don't initialize Git repository\n");
  fmt::print("  --git-workflow <type>        Git workflow: none, gitflow, github-flow, gitlab-flow, custom\n");
  fmt::print("  --git-branch-strategy <strategy> Git branch strategy: single, feature, gitflow, custom\n");
  fmt::print("  --license <type>             License type: mit, apache2, gpl3, bsd3, bsd2, unlicense, custom, none\n");
  fmt::print("  --git-remote <URL>           Git remote repository URL\n");
  fmt::print("  --git-user <username>        Git username\n");
  fmt::print("  --git-email <email>          Git user email\n");
  fmt::print("  --git-hooks                  Setup Git hooks\n");
  fmt::print("  --no-initial-commit          Don't create initial commit\n");
  fmt::print("  --doc-formats <formats>      Documentation output formats: markdown,html,pdf (comma-separated)\n");
  fmt::print("  --doc-types <types>          Documentation types: readme,api,user,developer (comma-separated)\n");
  fmt::print("  --doxygen                    Generate Doxygen configuration\n");
  fmt::print("  --doxygen-theme <theme>      Doxygen theme\n");
  fmt::print("  --no-code-examples           Don't include code examples\n");
  fmt::print("  --changelog                  Generate changelog\n");
  fmt::print("  --gui-frameworks <frameworks> GUI frameworks: qt,gtk,fltk,imgui,wxwidgets (comma-separated)\n");
  fmt::print("  --game-frameworks <frameworks> Game frameworks: sdl2,sfml,allegro (comma-separated)\n");
  fmt::print("  --graphics-libs <libraries>  Graphics libraries: opengl,vulkan,directx (comma-separated)\n");
  fmt::print("  --include-shaders            Include shader templates\n");
  fmt::print("  --profile <name>             Use saved configuration profile\n");
  fmt::print("  --template-path <path>       Use custom project template\n");
  fmt::print(
      "  -l, --language <language>    Interface language: en, zh, es, jp, de, fr\n");
  fmt::print("  --verbose                    Show verbose output\n");
  fmt::print("  -h, --help                   Show this help information\n");
  fmt::print("  -v, --version                Show version information\n\n");

  std::cout << TerminalUtils::colorize("**Remote Source Options:**", utils::Color::BrightYellow) << "\n";
  fmt::print("  --from-git <url>             Create project from Git repository\n");
  fmt::print("  --from-archive <path/url>    Create project from archive file or URL\n");
  fmt::print("  --branch <name>              Git branch to checkout (default: main/master)\n");
  fmt::print("  --tag <name>                 Git tag to checkout\n");
  fmt::print("  --commit <hash>              Git commit to checkout\n");
  fmt::print("  --shallow                    Use shallow clone (default, faster)\n");
  fmt::print("  --no-shallow                 Use full clone (slower, complete history)\n");
  fmt::print("  --preserve-git               Keep .git directory after cloning\n");
  fmt::print("  --git-username <name>        Git authentication username\n");
  fmt::print("  --git-password <token>       Git authentication password/token\n");
  fmt::print("  --ssh-key <path>             SSH key path for Git authentication\n\n");

  fmt::print("**Examples:**\n");
  fmt::print("  cpp-scaffold create my-app --template console --build cmake "
             "--package vcpkg --tests\n");
  fmt::print("  cpp-scaffold new my-lib -t lib -b cmake -p none --docs "
             "--code-style\n");
  fmt::print("  cpp-scaffold create my-modules-app --template modules --build cmake "
             "--package vcpkg --tests\n");
  fmt::print(
      "  cpp-scaffold create my-app --ci github --ci gitlab --editor vscode\n");
  fmt::print("  cpp-scaffold create my-app --profile webservice\n");
  fmt::print("  cpp-scaffold create my-game --template gui --gui-frameworks sdl2,opengl --include-shaders\n");
  fmt::print("  cpp-scaffold create my-project --git-workflow gitflow --license mit --doxygen\n");
  fmt::print("  cpp-scaffold create my-lib --doc-formats markdown,html --doc-types readme,api,user\n");
  fmt::print("  cpp-scaffold create my-project --from-git https://github.com/user/template.git --branch develop\n");
  fmt::print("  cpp-scaffold create my-app --from-archive https://example.com/template.zip\n");
  fmt::print("  cpp-scaffold create my-lib --from-git git@github.com:user/template.git --tag v1.0.0 --ssh-key ~/.ssh/id_rsa\n");
}

// Show version information
void showVersion() {
  fmt::print("CPP-Scaffold Version 1.2.0\n");
}

// Show available templates
void showAvailableTemplates() {
  std::cout << TerminalUtils::colorize("**Available Project Templates**", utils::Color::BrightCyan) << "\n\n";

  std::vector<std::pair<std::string, std::string>> templates = {
    {"console", "Console application with basic I/O"},
    {"lib", "Static/shared library with headers"},
    {"header-only-lib", "Header-only library template"},
    {"multi-executable", "Multiple executable project"},
    {"gui", "GUI application using Qt or similar"},
    {"network", "Network application with socket programming"},
    {"embedded", "Embedded systems project"},
    {"webservice", "Web service with REST API"},
    {"gameengine", "Game engine project"},
    {"qt-app", "Qt-based GUI application"},
    {"sfml-app", "SFML-based game/graphics application"},
    {"boost-app", "Boost-based application"},
    {"test-project", "Testing-focused project"}
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

// Show available profiles
void showAvailableProfiles() {
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

// Show specific profile information
void showProfileInfo(const std::string& profileName) {
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
  std::cout << "  Template: " << cli_enums::to_string(profile->options.templateType) << "\n";
  std::cout << "  Build System: " << cli_enums::to_string(profile->options.buildSystem) << "\n";
  std::cout << "  Package Manager: " << cli_enums::to_string(profile->options.packageManager) << "\n";
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

// Validate project structure
void validateProject(const std::string& projectPath) {
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

// Validate configuration
void validateConfiguration(const CliOptions& options) {
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

// Run configuration wizard
void runConfigurationWizard() {
  config::InteractiveConfigWizard::runConfigurationWizard();
}

// List profiles (alias for showAvailableProfiles)
void listProfiles() {
  showAvailableProfiles();
}

// Show CLI header
void showCliHeader() {
  TerminalUtils::clearLine();

  std::vector<std::string> headerLines = {"C++ Project Scaffold",
                                          "Interactive Project Creation Wizard", "Version 1.2.0"};

  // Use box to display header
  TerminalUtils::showBox(headerLines, BorderStyle::Single,
                         utils::Color::BrightCyan, utils::Color::Reset, "");
  std::cout << "\n";
}

// Detect operating system
std::string detectOS() {
#ifdef _WIN32
  return "Windows";
#elif __APPLE__
  return "macOS";
#elif __linux__
  return "Linux";
#elif __unix__
  return "Unix";
#else
  return "Unknown";
#endif
}

// Get system-suggested defaults based on OS and environment
CliOptions getSystemSuggestedDefaults() {
  CliOptions defaults;

  std::string os = detectOS();

  // Set OS-specific defaults
  if (os == "Windows") {
    defaults.buildSystem = BuildSystem::CMake;
    defaults.packageManager = PackageManager::Vcpkg;
    defaults.editorOptions = {EditorConfig::VSCode, EditorConfig::VS};
    defaults.ciOptions = {CiSystem::GitHub};
  } else if (os == "macOS") {
    defaults.buildSystem = BuildSystem::CMake;
    defaults.packageManager = PackageManager::Conan;
    defaults.editorOptions = {EditorConfig::VSCode, EditorConfig::CLion};
    defaults.ciOptions = {CiSystem::GitHub};
  } else { // Linux/Unix
    defaults.buildSystem = BuildSystem::CMake;
    defaults.packageManager = PackageManager::Conan;
    defaults.editorOptions = {EditorConfig::VSCode, EditorConfig::Vim};
    defaults.ciOptions = {CiSystem::GitLab};
  }

  // Common defaults
  defaults.templateType = TemplateType::Console;
  defaults.includeTests = true;
  defaults.testFramework = TestFramework::GTest;
  defaults.includeDocumentation = false;
  defaults.includeCodeStyleTools = false;
  defaults.initGit = true;
  defaults.language = Language::English;

  return defaults;
}

// Run interactive mode
CliOptions runInteractiveMode() {
  // Use the enhanced wizard for a better user experience
  auto& wizard = utils::EnhancedWizard::getInstance();
  return wizard.runInteractiveWizard();
}

} // namespace cli_commands

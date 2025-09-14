#include "command_handlers.h"

#include <spdlog/fmt/fmt.h>

#include <iostream>

#include "../../config/config_manager.h"
#include "../../config/config_validator.h"
#include "../../config/interactive_config.h"
#include "../../config/project_profiles.h"
#include "../../utils/core/file_utils.h"
#include "../../utils/core/string_utils.h"
#include "../../utils/ui/terminal_utils.h"
#include "../../utils/validation/context_sensitive_error_system.h"
#include "../localization/localization.h"

using namespace utils;

namespace cli_commands {

// Show help information
void showHelp(Language lang) {
    (void)lang;  // TODO: Implement localization support

    // Header with visual appeal
    TerminalUtils::showBrandedHeader("CPP-Scaffold", "Modern C++ Project Scaffolding Tool");

    std::cout << "\n";
    TerminalUtils::showCard("Quick Start",
                            {"🚀 Create a new project:     cpp-scaffold create my-project",
                             "�?Interactive mode:         cpp-scaffold",
                             "📋 List templates:           cpp-scaffold list-templates",
                             "�?Get help:                 cpp-scaffold --help"});

    std::cout << "\n";
    std::cout << TerminalUtils::colorAndStyle("Usage:", utils::Color::BrightYellow,
                                              {TextStyle::Bold})
              << "\n";
    std::cout << "  cpp-scaffold create <project-name> [options]\n";
    std::cout << "  cpp-scaffold new <project-name> [options]\n";
    std::cout << "  cpp-scaffold interactive\n";
    std::cout << "  cpp-scaffold config\n";
    std::cout << "  cpp-scaffold list-templates\n";
    std::cout << "  cpp-scaffold validate [project-path]\n\n";

    // Core Options Section
    TerminalUtils::showCard(
            "Core Options",
            {"-t, --template <type>        Project template (console, lib, header-only-lib, "
             "modules, etc.)",
             "-b, --build <system>         Build system (cmake, meson, bazel, xmake, etc.)",
             "-p, --package <manager>      Package manager (vcpkg, conan, none, etc.)",
             "--std <standard>             C++ standard (cpp11, cpp14, cpp17, cpp20, cpp23)",
             "--tests [framework]          Test framework (gtest, catch2, doctest, boost, none)"});

    std::cout << "\n";

    // Development Tools Section
    TerminalUtils::showCard(
            "Development Tools",
            {"--docs, --documentation      Include documentation configuration",
             "--code-style                 Include code style and static analysis tools",
             "--editor <editor>            Configure editor support (vscode, clion, vs, vim, etc.)",
             "--ci, --cicd <system>        Configure CI/CD (github, gitlab, travis, etc.)",
             "--doxygen                    Generate Doxygen configuration"});

    std::cout << "\n";

    // Git Integration Section
    TerminalUtils::showCard(
            "Git Integration",
            {"--no-git                     Don't initialize Git repository",
             "--git-workflow <type>        Git workflow (gitflow, github-flow, gitlab-flow)",
             "--git-branch-strategy <strategy> Git branch strategy (single, feature, gitflow)",
             "--license <type>             License type (mit, apache2, gpl3, bsd3, etc.)",
             "--git-remote <URL>           Git remote repository URL",
             "--git-hooks                  Setup Git hooks"});

    std::cout << "\n";

    // Specialized Options Section
    TerminalUtils::showCard(
            "Specialized Options",
            {"--network-lib <library>      Network library (asio, boost, poco)",
             "--gui-frameworks <frameworks> GUI frameworks (qt, gtk, fltk, imgui, wxwidgets)",
             "--game-frameworks <frameworks> Game frameworks (sdl2, sfml, allegro)",
             "--graphics-libs <libraries>  Graphics libraries (opengl, vulkan, directx)",
             "--doc-formats <formats>      Documentation formats (markdown, html, pdf)"});

    std::cout << "\n";

    // Additional Options Section
    TerminalUtils::showCard(
            "Additional Options",
            {"--include-shaders            Include shader templates",
             "--profile <name>             Use saved configuration profile",
             "--template-path <path>       Use custom project template",
             "-l, --language <language>    Interface language (en, zh, es, jp, de, fr)",
             "--verbose                    Show verbose output",
             "-h, --help                   Show this help information",
             "-v, --version                Show version information"});

    std::cout << "\n";

    // Remote Source Options Section
    TerminalUtils::showCard(
            "Remote Source Options",
            {"--from-git <url>             Create project from Git repository",
             "--from-archive <path/url>    Create project from archive file or URL",
             "--branch <name>              Git branch to checkout (default: main/master)",
             "--tag <name>                 Git tag to checkout",
             "--commit <hash>              Git commit to checkout",
             "--shallow                    Use shallow clone (default, faster)",
             "--preserve-git               Keep .git directory after cloning"});

    std::cout << "\n";

    // Examples Section
    TerminalUtils::showCard(
            "Common Examples",
            {"🏗�? Basic console app:         cpp-scaffold create my-app --template console "
             "--build "
             "cmake",
             "📚 Library with docs:         cpp-scaffold create my-lib -t lib --docs --code-style",
             "🎮 Game project:              cpp-scaffold create my-game --template gui "
             "--gui-frameworks sdl2",
             "🌐 Web service:               cpp-scaffold create my-service --profile webservice",
             "🧪 Test-driven project:       cpp-scaffold create my-project --tests gtest --ci "
             "github"});

    std::cout << "\n";

    // Advanced Examples Section
    TerminalUtils::showCard(
            "Advanced Examples",
            {"📦 From Git repository:       cpp-scaffold create my-project "
             "--from-git https://github.com/user/template.git",
             "🏷�? Specific Git tag:          cpp-scaffold create my-lib --from-git "
             "repo.git --tag v1.0.0",
             "🔧 Multiple CI systems:       cpp-scaffold create my-app --ci github "
             "--ci gitlab --editor vscode",
             "📖 Full documentation:        cpp-scaffold create my-lib "
             "--doc-formats markdown,html --doxygen"});

    std::cout << "\n";

    // Footer with helpful information
    TerminalUtils::showCard("💡 Pro Tips",
                            {"Use 'cpp-scaffold' without arguments for interactive mode",
                             "Run 'cpp-scaffold list-templates' to see all available templates",
                             "Use '--profile <name>' to save and reuse common configurations",
                             "Add '--verbose' to see detailed progress information"});

    std::cout
            << "\n"
            << TerminalUtils::colorAndStyle(
                       "For more information, visit: https://github.com/cpp-scaffold/cpp-scaffold",
                       utils::Color::BrightBlue, {TextStyle::Underline})
            << "\n\n";
}

// Show version information
void showVersion() {
    fmt::print("CPP-Scaffold Version 1.2.0\n");
}

// Show available templates
void showAvailableTemplates() {
    std::cout << TerminalUtils::colorize("**Available Project Templates**",
                                         utils::Color::BrightCyan)
              << "\n\n";

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
            {"test-project", "Testing-focused project"}};

    for (const auto& [name, description] : templates) {
        if (name == "webservice" || name == "embedded") {
            std::cout << TerminalUtils::colorize("  " + name, utils::Color::BrightBlack) << " - "
                      << TerminalUtils::colorize(description, utils::Color::BrightBlack) << "\n";
        } else if (name == "gameengine") {
            std::cout << TerminalUtils::colorize("  " + name, utils::Color::Yellow) << " - "
                      << TerminalUtils::colorize(description, utils::Color::Yellow) << "\n";
        } else {
            std::cout << TerminalUtils::colorize("  " + name, utils::Color::BrightGreen) << " - "
                      << description << "\n";
        }
    }

    std::cout << "\n" << TerminalUtils::colorize("Usage:", utils::Color::BrightYellow) << "\n";
    std::cout << "  cpp-scaffold create <project-name> --template <template-name>\n\n";
}

// Show available profiles
void showAvailableProfiles() {
    std::cout << TerminalUtils::colorize("**Available Project Profiles**", utils::Color::BrightCyan)
              << "\n\n";

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
                std::cout << "    Tags: "
                          << TerminalUtils::colorize(utils::StringUtils::join(profile.tags, ", "),
                                                     utils::Color::Cyan)
                          << "\n";
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
        std::cout << TerminalUtils::colorize("�?Profile '" + profileName + "' not found",
                                             utils::Color::BrightRed)
                  << "\n";
        std::cout << "Use 'cpp-scaffold list-profiles' to see available profiles.\n";
        return;
    }

    std::cout << TerminalUtils::colorize("📋 Profile: " + profile->info.name,
                                         utils::Color::BrightCyan)
              << "\n\n";

    std::cout << TerminalUtils::colorize("Description:", utils::Color::BrightYellow) << " "
              << profile->info.description << "\n";
    std::cout << TerminalUtils::colorize("Category:", utils::Color::BrightYellow) << " "
              << profile->info.category << "\n";
    std::cout << TerminalUtils::colorize("Version:", utils::Color::BrightYellow) << " "
              << profile->info.version << "\n";
    std::cout << TerminalUtils::colorize("Author:", utils::Color::BrightYellow) << " "
              << profile->info.author << "\n";

    if (!profile->info.tags.empty()) {
        std::cout << TerminalUtils::colorize("Tags:", utils::Color::BrightYellow) << " "
                  << utils::StringUtils::join(profile->info.tags, ", ") << "\n";
    }

    std::cout << "\n"
              << TerminalUtils::colorize("Configuration:", utils::Color::BrightYellow) << "\n";
    std::cout << "  Template: " << cli_enums::to_string(profile->options.templateType) << "\n";
    std::cout << "  Build System: " << cli_enums::to_string(profile->options.buildSystem) << "\n";
    std::cout << "  Package Manager: " << cli_enums::to_string(profile->options.packageManager)
              << "\n";
    std::cout << "  Tests: " << (profile->options.includeTests ? "Yes" : "No") << "\n";
    std::cout << "  Documentation: " << (profile->options.includeDocumentation ? "Yes" : "No")
              << "\n";

    if (!profile->recommendedDependencies.empty()) {
        std::cout << "\n"
                  << TerminalUtils::colorize("Recommended Dependencies:",
                                             utils::Color::BrightYellow)
                  << "\n";
        for (const auto& dep : profile->recommendedDependencies) {
            std::cout << "  �?" << dep << "\n";
        }
    }

    if (!profile->setupInstructions.empty()) {
        std::cout << "\n"
                  << TerminalUtils::colorize("Setup Instructions:", utils::Color::BrightYellow)
                  << "\n";
        std::cout << profile->setupInstructions << "\n";
    }

    std::cout << "\n";
}

// Validate project structure
void validateProject(const std::string& projectPath) {
    std::cout << TerminalUtils::colorize("**Validating Project: " + projectPath + "**",
                                         utils::Color::BrightCyan)
              << "\n\n";

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
                std::cout << TerminalUtils::colorize("[OK]", utils::Color::BrightGreen)
                          << " Found: " << file << "\n";
            }
        }

        for (const auto& dir : requiredDirs) {
            std::string dirPath = utils::FileUtils::combinePath(projectPath, dir);
            if (!utils::FileUtils::directoryExists(dirPath)) {
                issues.push_back("Missing required directory: " + dir);
                isValid = false;
            } else {
                std::cout << TerminalUtils::colorize("[OK]", utils::Color::BrightGreen)
                          << " Found: " << dir << "/\n";
            }
        }
    }

    std::cout << "\n";

    if (isValid) {
        std::cout << TerminalUtils::colorize("�?Project structure is valid!",
                                             utils::Color::BrightGreen)
                  << "\n";
    } else {
        std::cout << TerminalUtils::colorize("�?Project validation failed:",
                                             utils::Color::BrightRed)
                  << "\n";
        for (const auto& issue : issues) {
            std::cout << TerminalUtils::colorize("  �?" + issue, utils::Color::Red) << "\n";
        }
    }
    std::cout << "\n";
}

// Validate configuration
void validateConfiguration(const CliOptions& options) {
    std::cout << TerminalUtils::colorize("🔍 Validating Configuration...", utils::Color::BrightCyan)
              << "\n\n";

    auto& validator = config::ConfigValidator::getInstance();
    auto result = validator.validateConfiguration(options);

    config::validation_utils::printValidationResult(result);

    if (result.isValid) {
        std::cout << "\n"
                  << TerminalUtils::colorize("�?Configuration is valid!", utils::Color::BrightGreen)
                  << "\n";
    } else {
        std::cout << "\n"
                  << TerminalUtils::colorize("�?Configuration has errors that need to be fixed.",
                                             utils::Color::BrightRed)
                  << "\n";
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
    TerminalUtils::showBox(headerLines, BorderStyle::Single, utils::Color::BrightCyan,
                           utils::Color::Reset, "");
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
    } else {  // Linux/Unix
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
    // Use basic interactive functionality
    CliOptions options;
    // TODO: Implement basic interactive wizard
    std::cout << "Interactive mode not yet implemented. Please use command line options.\n";
    return options;
}

}  // namespace cli_commands

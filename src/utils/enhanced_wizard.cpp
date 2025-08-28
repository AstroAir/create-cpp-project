#include "enhanced_wizard.h"
#include "string_utils.h"
#include <iostream>
#include <algorithm>
#include <regex>

namespace utils {

EnhancedWizard& EnhancedWizard::getInstance() {
    static EnhancedWizard instance;
    return instance;
}

CliOptions EnhancedWizard::runInteractiveWizard() {
    auto& terminal = EnhancedTerminal::getInstance();

    terminal.showWelcomeScreen();
    showWelcomeMessage();

    // Ask user what type of setup they want
    std::vector<std::string> setupTypes = {
        "Quick Setup (Recommended defaults)",
        "Guided Setup (Step-by-step with explanations)",
        "Advanced Setup (Full customization)"
    };

    int choice = terminal.selectFromMenu(setupTypes,
        "🚀 Choose Your Setup Experience",
        "Select the setup mode that best fits your needs");

    switch (choice) {
        case 0: return runQuickSetup();
        case 1: return runGuidedSetup();
        case 2: return runAdvancedSetup();
        default: return runQuickSetup(); // fallback
    }
}

CliOptions EnhancedWizard::runQuickSetup() {
    auto& terminal = EnhancedTerminal::getInstance();

    terminal.showApplicationHeader();
    std::cout << TerminalUtils::colorAndStyle("⚡ Quick Setup", Color::BrightYellow, {TextStyle::Bold}) << "\n\n";

    CliOptions options;

    // Only ask for essential information
    options.projectName = promptProjectName();
    options.templateType = promptTemplateType();
    options.buildSystem = BuildSystem::CMake; // Default
    options.packageManager = PackageManager::None; // Default
    options.includeTests = true; // Default
    options.testFramework = TestFramework::GTest; // Default
    options.initGit = true; // Default

    // Show summary and confirm
    showConfigurationSummary(options);

    if (confirmConfiguration(options)) {
        return validateAndFixConfiguration(options);
    } else {
        return runGuidedSetup(); // Fall back to guided setup
    }
}

CliOptions EnhancedWizard::runGuidedSetup() {
    auto& terminal = EnhancedTerminal::getInstance();

    terminal.showApplicationHeader();
    std::cout << TerminalUtils::colorAndStyle("🎯 Guided Setup", Color::BrightCyan, {TextStyle::Bold}) << "\n\n";

    CliOptions options;
    const int totalSteps = 7;

    // Step 1: Project Name
    showStepHeader("Project Name", 1, totalSteps);
    options.projectName = promptProjectName();

    // Step 2: Template Type
    showStepHeader("Project Template", 2, totalSteps);
    showTemplateHelp();
    options.templateType = promptTemplateType();

    // Step 3: Build System
    showStepHeader("Build System", 3, totalSteps);
    showBuildSystemHelp();
    options.buildSystem = promptBuildSystem(options.templateType);

    // Step 4: Package Manager
    showStepHeader("Package Manager", 4, totalSteps);
    showPackageManagerHelp();
    options.packageManager = promptPackageManager();

    // Step 5: Testing
    showStepHeader("Testing Framework", 5, totalSteps);
    options.includeTests = promptIncludeTests();
    if (options.includeTests) {
        options.testFramework = promptTestFramework();
    }

    // Step 6: Documentation
    showStepHeader("Documentation", 6, totalSteps);
    options.includeDocumentation = promptIncludeDocumentation();

    // Step 7: Git
    showStepHeader("Version Control", 7, totalSteps);
    options.initGit = promptInitGit();

    // Show summary and confirm
    std::cout << "\n";
    showConfigurationSummary(options);
    showBestPracticesAdvice(options);

    if (confirmConfiguration(options)) {
        return validateAndFixConfiguration(options);
    } else {
        std::cout << "\n" << TerminalUtils::colorize("Let's try again...", Color::BrightYellow) << "\n\n";
        return runGuidedSetup(); // Restart guided setup
    }
}

CliOptions EnhancedWizard::runAdvancedSetup() {
    auto& terminal = EnhancedTerminal::getInstance();

    terminal.showApplicationHeader();
    std::cout << TerminalUtils::colorAndStyle("🔧 Advanced Setup", Color::BrightMagenta, {TextStyle::Bold}) << "\n\n";

    CliOptions options;

    // All the guided setup steps plus advanced options
    options = runGuidedSetup();

    // Advanced options
    std::cout << "\n" << TerminalUtils::colorAndStyle("Advanced Options:", Color::BrightMagenta, {TextStyle::Bold}) << "\n\n";

    if (options.templateType == TemplateType::Network) {
        std::cout << TerminalUtils::colorize("🌐 Network Library Configuration:", Color::BrightCyan) << "\n";
        options.networkLibrary = promptNetworkLibrary();
    }

    if (options.templateType == TemplateType::Gui) {
        std::cout << TerminalUtils::colorize("🖥️  GUI Framework Configuration:", Color::BrightCyan) << "\n";
        // For now, just show the prompt but don't store the result
        // since guiFramework field doesn't exist in CliOptions
        promptGuiFramework();
    }

    // Additional features
    std::cout << TerminalUtils::colorize("✨ Additional Features:", Color::BrightCyan) << "\n";
    auto additionalFeatures = promptAdditionalFeatures();

    // Apply additional features to options (only existing fields)
    for (const auto& feature : additionalFeatures) {
        if (feature == "Code Style Tools") {
            options.includeCodeStyleTools = true;
        }
        // Note: includeBenchmarks and setupCiCd fields don't exist in CliOptions
        // These would need to be added to the CliOptions struct
    }

    return validateAndFixConfiguration(options);
}

std::string EnhancedWizard::promptProjectName() {
    return promptString(
        "📝 Enter project name",
        "",
        [this](const std::string& name) { return isValidProjectName(name); }
    );
}

TemplateType EnhancedWizard::promptTemplateType() {
    std::vector<std::pair<TemplateType, std::string>> templates = {
        {TemplateType::Console, "Console Application - Command-line program"},
        {TemplateType::Lib, "Library - Reusable code library"},
        {TemplateType::HeaderOnlyLib, "Header-Only Library - Template/inline library"},
        {TemplateType::Gui, "GUI Application - Desktop application with UI"},
        {TemplateType::Network, "Network Application - Client/server networking"},
        {TemplateType::WebService, "Web Service - REST API or web server"},
        {TemplateType::GameEngine, "Game Engine - Game development framework"}
    };

    return promptFromEnum<TemplateType>(
        "🎯 Select project template",
        templates,
        "Choose the type of project you want to create"
    );
}

BuildSystem EnhancedWizard::promptBuildSystem(TemplateType /*templateType*/) {
    std::vector<std::pair<BuildSystem, std::string>> buildSystems = {
        {BuildSystem::CMake, "CMake - Cross-platform build system (Recommended)"},
        {BuildSystem::Meson, "Meson - Fast and user-friendly build system"},
        {BuildSystem::Bazel, "Bazel - Scalable build system by Google"},
        {BuildSystem::Make, "Make - Traditional Unix build system"}
    };

    return promptFromEnum<BuildSystem>(
        "🔧 Select build system",
        buildSystems,
        "Choose how your project will be built"
    );
}

bool EnhancedWizard::isValidProjectName(const std::string& name) {
    if (name.empty()) {
        std::cout << TerminalUtils::colorize("❌ Project name cannot be empty", Color::BrightRed) << "\n";
        return false;
    }

    if (name.length() > 50) {
        std::cout << TerminalUtils::colorize("❌ Project name too long (max 50 characters)", Color::BrightRed) << "\n";
        return false;
    }

    // Check for valid characters (alphanumeric, underscore, hyphen)
    std::regex validName("^[a-zA-Z][a-zA-Z0-9_-]*$");
    if (!std::regex_match(name, validName)) {
        std::cout << TerminalUtils::colorize("❌ Invalid project name. Use letters, numbers, underscore, or hyphen only", Color::BrightRed) << "\n";
        return false;
    }

    return true;
}

void EnhancedWizard::showWelcomeMessage() {
    auto& terminal = EnhancedTerminal::getInstance();

    std::cout << TerminalUtils::colorAndStyle("Welcome to the Interactive Project Wizard!", Color::BrightGreen, {TextStyle::Bold}) << "\n\n";
    std::cout << TerminalUtils::colorize("This wizard will guide you through creating a new C++ project.", Color::White) << "\n";
    std::cout << TerminalUtils::colorize("You can customize every aspect or use our recommended defaults.", Color::White) << "\n\n";

    terminal.showTip("Press Ctrl+C at any time to exit the wizard");
    std::cout << "\n";
}

void EnhancedWizard::showStepHeader(const std::string& stepName, int stepNumber, int totalSteps) {
    auto& terminal = EnhancedTerminal::getInstance();

    std::cout << "\n";
    terminal.showMultiStepProgress(stepNumber, totalSteps, stepName);
}

void EnhancedWizard::showConfigurationSummary(const CliOptions& options) {
    auto& terminal = EnhancedTerminal::getInstance();

    std::cout << "\n";
    std::cout << TerminalUtils::colorAndStyle("📋 Configuration Summary:", Color::BrightCyan, {TextStyle::Bold}) << "\n\n";

    terminal.showProjectSummary(options);
}

bool EnhancedWizard::confirmConfiguration(const CliOptions& /*options*/) {
    auto& terminal = EnhancedTerminal::getInstance();

    return terminal.confirmWithStyle(
        "✅ Create project with this configuration?",
        true
    );
}

CliOptions EnhancedWizard::validateAndFixConfiguration(const CliOptions& options) {
    // For now, just return the options as-is
    // In a full implementation, this would validate and fix any issues
    return options;
}

PackageManager EnhancedWizard::promptPackageManager() {
    std::vector<std::pair<PackageManager, std::string>> packageManagers = {
        {PackageManager::None, "None - No package manager"},
        {PackageManager::Vcpkg, "vcpkg - Microsoft's C++ package manager"},
        {PackageManager::Conan, "Conan - Cross-platform package manager"},
        {PackageManager::Spack, "Spack - Scientific computing package manager"},
        {PackageManager::MSYS2, "MSYS2 - Unix-like environment for Windows"}
    };

    return promptFromEnum<PackageManager>(
        "📦 Select package manager",
        packageManagers,
        "Choose how dependencies will be managed"
    );
}

TestFramework EnhancedWizard::promptTestFramework() {
    std::vector<std::pair<TestFramework, std::string>> testFrameworks = {
        {TestFramework::GTest, "Google Test - Google's testing framework"},
        {TestFramework::Catch2, "Catch2 - Modern C++ testing framework"},
        {TestFramework::Doctest, "Doctest - Lightweight testing framework"},
        {TestFramework::Boost, "Boost.Test - Part of Boost libraries"}
    };

    return promptFromEnum<TestFramework>(
        "🧪 Select test framework",
        testFrameworks,
        "Choose the testing framework for your project"
    );
}

bool EnhancedWizard::promptIncludeTests() {
    return promptYesNo("🧪 Include testing framework?", true);
}

bool EnhancedWizard::promptIncludeDocumentation() {
    return promptYesNo("📚 Include documentation generation?", false);
}

bool EnhancedWizard::promptInitGit() {
    return promptYesNo("📝 Initialize Git repository?", true);
}

std::string EnhancedWizard::promptNetworkLibrary() {
    std::vector<std::string> libraries = {"asio", "boost", "poco"};
    int choice = promptChoice("🌐 Select network library", libraries);
    return libraries[choice];
}

std::string EnhancedWizard::promptGuiFramework() {
    std::vector<std::string> frameworks = {"qt", "gtk", "wxwidgets"};
    int choice = promptChoice("🖥️ Select GUI framework", frameworks);
    return frameworks[choice];
}

std::vector<std::string> EnhancedWizard::promptAdditionalFeatures() {
    // For now, return empty vector
    // In full implementation, this would show a multi-select menu
    return {};
}

void EnhancedWizard::showTemplateHelp() {
    std::cout << TerminalUtils::colorize("📖 Template Types:", Color::BrightCyan) << "\n";
    std::cout << "  • Console: Command-line applications\n";
    std::cout << "  • Library: Reusable code libraries\n";
    std::cout << "  • GUI: Desktop applications with user interface\n";
    std::cout << "  • Network: Client/server networking applications\n\n";
}

void EnhancedWizard::showBuildSystemHelp() {
    std::cout << TerminalUtils::colorize("🔧 Build Systems:", Color::BrightCyan) << "\n";
    std::cout << "  • CMake: Cross-platform, widely supported\n";
    std::cout << "  • Meson: Fast and user-friendly\n";
    std::cout << "  • Bazel: Scalable, used by Google\n\n";
}

void EnhancedWizard::showPackageManagerHelp() {
    std::cout << TerminalUtils::colorize("📦 Package Managers:", Color::BrightCyan) << "\n";
    std::cout << "  • vcpkg: Microsoft's package manager\n";
    std::cout << "  • Conan: Cross-platform dependency manager\n";
    std::cout << "  • MSYS2: Unix-like environment for Windows\n";
    std::cout << "  • None: Manual dependency management\n\n";
}

void EnhancedWizard::showBestPracticesAdvice(const CliOptions& /*options*/) {
    auto& terminal = EnhancedTerminal::getInstance();

    std::cout << "\n";
    terminal.showTip("Consider enabling tests for better code quality");
    terminal.showTip("Use a package manager for easier dependency management");
}

// Helper method implementations
std::string EnhancedWizard::promptString(const std::string& prompt,
                                       const std::string& defaultValue,
                                       const std::function<bool(const std::string&)>& validator) {
    std::string input;
    bool valid = false;

    while (!valid) {
        std::cout << TerminalUtils::colorize("📝 ", Color::BrightBlue) << prompt;
        if (!defaultValue.empty()) {
            std::cout << " [" << defaultValue << "]";
        }
        std::cout << ": ";

        std::getline(std::cin, input);

        if (input.empty() && !defaultValue.empty()) {
            input = defaultValue;
        }

        if (!validator || validator(input)) {
            valid = true;
        }
    }

    return input;
}

bool EnhancedWizard::promptYesNo(const std::string& prompt, bool defaultValue) {
    auto& terminal = EnhancedTerminal::getInstance();
    return terminal.confirmWithStyle(prompt, defaultValue);
}

int EnhancedWizard::promptChoice(const std::string& prompt,
                               const std::vector<std::string>& choices,
                               const std::string& helpText) {
    auto& terminal = EnhancedTerminal::getInstance();
    return terminal.selectFromMenu(choices, prompt, helpText);
}

// Template specializations
template<>
TemplateType EnhancedWizard::promptFromEnum<TemplateType>(
    const std::string& prompt,
    const std::vector<std::pair<TemplateType, std::string>>& options,
    const std::string& helpText) {

    auto& terminal = EnhancedTerminal::getInstance();

    std::vector<std::string> choices;
    for (const auto& option : options) {
        choices.push_back(option.second);
    }

    int choice = terminal.selectFromMenu(choices, prompt, helpText);
    return options[choice].first;
}

template<>
BuildSystem EnhancedWizard::promptFromEnum<BuildSystem>(
    const std::string& prompt,
    const std::vector<std::pair<BuildSystem, std::string>>& options,
    const std::string& helpText) {

    auto& terminal = EnhancedTerminal::getInstance();

    std::vector<std::string> choices;
    for (const auto& option : options) {
        choices.push_back(option.second);
    }

    int choice = terminal.selectFromMenu(choices, prompt, helpText);
    return options[choice].first;
}

template<>
PackageManager EnhancedWizard::promptFromEnum<PackageManager>(
    const std::string& prompt,
    const std::vector<std::pair<PackageManager, std::string>>& options,
    const std::string& helpText) {

    auto& terminal = EnhancedTerminal::getInstance();

    std::vector<std::string> choices;
    for (const auto& option : options) {
        choices.push_back(option.second);
    }

    int choice = terminal.selectFromMenu(choices, prompt, helpText);
    return options[choice].first;
}

template<>
TestFramework EnhancedWizard::promptFromEnum<TestFramework>(
    const std::string& prompt,
    const std::vector<std::pair<TestFramework, std::string>>& options,
    const std::string& helpText) {

    auto& terminal = EnhancedTerminal::getInstance();

    std::vector<std::string> choices;
    for (const auto& option : options) {
        choices.push_back(option.second);
    }

    int choice = terminal.selectFromMenu(choices, prompt, helpText);
    return options[choice].first;
}

} // namespace utils

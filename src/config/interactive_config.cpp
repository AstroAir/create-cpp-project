#include "interactive_config.h"
#include "../utils/terminal_utils.h"
#include <spdlog/spdlog.h>
#include <iostream>
#include <limits>
#include <algorithm>

using namespace utils;
using namespace cli_enums;

namespace config {

// Static member initialization
bool InteractiveConfigWizard::s_wizardRunning = false;
std::string InteractiveConfigWizard::s_currentSection = "";

bool InteractiveConfigWizard::runConfigurationWizard() {
    s_wizardRunning = true;

    try {
        showWelcomeMessage();

        std::vector<std::string> mainMenuOptions = {
            "Configure General Settings",
            "Configure Default Options",
            "Manage Profiles",
            "Manage Custom Templates",
            "Configure Advanced Settings",
            "View Current Configuration",
            "Export/Import Configuration",
            "Exit"
        };

        while (s_wizardRunning) {
            int choice = showMenu("Configuration Wizard", mainMenuOptions);

            switch (choice) {
                case 1:
                    configureGeneralSettings();
                    break;
                case 2:
                    configureDefaultOptions();
                    break;
                case 3:
                    manageProfiles();
                    break;
                case 4:
                    manageCustomTemplates();
                    break;
                case 5:
                    configureAdvancedSettings();
                    break;
                case 6:
                    showConfigurationSummary();
                    waitForEnter();
                    break;
                case 7:
                    // Export/Import menu would go here
                    showInfo("Export/Import functionality coming soon!");
                    waitForEnter();
                    break;
                case 8:
                    s_wizardRunning = false;
                    break;
                default:
                    showError("Invalid choice. Please try again.");
                    waitForEnter();
                    break;
            }
        }

        showSuccess("Configuration wizard completed!");
        return true;

    } catch (const std::exception& e) {
        showError("Error in configuration wizard: " + std::string(e.what()));
        s_wizardRunning = false;
        return false;
    }
}

bool InteractiveConfigWizard::configureGeneralSettings() {
    showSectionHeader("General Settings");

    auto& prefs = UserPreferences::getInstance();

    try {
        // Auto save setting
        bool autoSave = prefs.getPreference<bool>("general.auto_save", true);
        autoSave = promptForBool("Enable auto-save for configuration changes?", autoSave);
        prefs.setPreference("general.auto_save", autoSave);

        // Verbose logging
        bool verboseLogging = prefs.getPreference<bool>("general.verbose_logging", false);
        verboseLogging = promptForBool("Enable verbose logging?", verboseLogging);
        prefs.setPreference("general.verbose_logging", verboseLogging);

        // Check for updates
        bool checkUpdates = prefs.getPreference<bool>("general.check_for_updates", true);
        checkUpdates = promptForBool("Automatically check for updates?", checkUpdates);
        prefs.setPreference("general.check_for_updates", checkUpdates);

        // Default language
        std::vector<std::string> languages = {"en", "zh", "es", "jp", "de", "fr"};
        std::string currentLang = prefs.getPreference<std::string>("default.language", "en");
        std::string newLang = promptForChoice("Select default language:", languages, currentLang);
        prefs.setPreference("default.language", newLang);

        // Git initialization default
        bool initGit = prefs.getPreference<bool>("default.init_git", true);
        initGit = promptForBool("Initialize Git repository by default?", initGit);
        prefs.setPreference("default.init_git", initGit);

        if (autoSave) {
            prefs.savePreferences();
        }

        showSuccess("General settings updated successfully!");
        showSectionFooter();
        return true;

    } catch (const std::exception& e) {
        showError("Error configuring general settings: " + std::string(e.what()));
        showSectionFooter();
        return false;
    }
}

bool InteractiveConfigWizard::configureDefaultOptions() {
    showSectionHeader("Default Project Options");

    auto& prefs = UserPreferences::getInstance();

    try {
        // Default template type
        std::vector<std::string> templates = {"console", "lib", "header-only-lib", "multi-executable", "gui", "network"};
        std::string currentTemplate = prefs.getPreference<std::string>("default.template_type", "console");
        std::string newTemplate = promptForChoice("Select default template type:", templates, currentTemplate);
        prefs.setPreference("default.template_type", newTemplate);

        // Default build system
        std::vector<std::string> buildSystems = {"cmake", "meson", "bazel", "xmake", "premake"};
        std::string currentBuild = prefs.getPreference<std::string>("default.build_system", "cmake");
        std::string newBuild = promptForChoice("Select default build system:", buildSystems, currentBuild);
        prefs.setPreference("default.build_system", newBuild);

        // Default package manager
        std::vector<std::string> packageManagers = {"vcpkg", "conan", "none"};
        std::string currentPackage = prefs.getPreference<std::string>("default.package_manager", "vcpkg");
        std::string newPackage = promptForChoice("Select default package manager:", packageManagers, currentPackage);
        prefs.setPreference("default.package_manager", newPackage);

        // Include tests by default
        bool includeTests = prefs.getPreference<bool>("default.include_tests", false);
        includeTests = promptForBool("Include test framework by default?", includeTests);
        prefs.setPreference("default.include_tests", includeTests);

        if (includeTests) {
            // Default test framework
            std::vector<std::string> testFrameworks = {"gtest", "catch2", "doctest", "boost"};
            std::string currentTest = prefs.getPreference<std::string>("default.test_framework", "gtest");
            std::string newTest = promptForChoice("Select default test framework:", testFrameworks, currentTest);
            prefs.setPreference("default.test_framework", newTest);
        }

        // Include documentation by default
        bool includeDocs = prefs.getPreference<bool>("default.include_documentation", false);
        includeDocs = promptForBool("Include documentation by default?", includeDocs);
        prefs.setPreference("default.include_documentation", includeDocs);

        // Include code style tools by default
        bool includeCodeStyle = prefs.getPreference<bool>("default.include_code_style_tools", false);
        includeCodeStyle = promptForBool("Include code style tools by default?", includeCodeStyle);
        prefs.setPreference("default.include_code_style_tools", includeCodeStyle);

        // Save preferences if auto-save is enabled
        if (prefs.getPreference<bool>("general.auto_save", true)) {
            prefs.savePreferences();
        }

        showSuccess("Default options updated successfully!");
        showSectionFooter();
        return true;

    } catch (const std::exception& e) {
        showError("Error configuring default options: " + std::string(e.what()));
        showSectionFooter();
        return false;
    }
}

bool InteractiveConfigWizard::manageProfiles() {
    showSectionHeader("Profile Management");

    std::vector<std::string> profileMenuOptions = {
        "Create New Profile",
        "Edit Existing Profile",
        "Delete Profile",
        "List All Profiles",
        "Export Profile",
        "Import Profile",
        "Back to Main Menu"
    };

    while (true) {
        int choice = showMenu("Profile Management", profileMenuOptions);

        switch (choice) {
            case 1:
                createNewProfile();
                break;
            case 2:
                editExistingProfile();
                break;
            case 3:
                // Delete profile functionality
                showInfo("Delete profile functionality coming soon!");
                waitForEnter();
                break;
            case 4:
                showProfilesSummary();
                waitForEnter();
                break;
            case 5:
                // Export profile functionality
                showInfo("Export profile functionality coming soon!");
                waitForEnter();
                break;
            case 6:
                // Import profile functionality
                showInfo("Import profile functionality coming soon!");
                waitForEnter();
                break;
            case 7:
                showSectionFooter();
                return true;
            default:
                showError("Invalid choice. Please try again.");
                waitForEnter();
                break;
        }
    }
}

bool InteractiveConfigWizard::createNewProfile() {
    showSectionHeader("Create New Profile");

    try {
        auto& configManager = ConfigManager::getInstance();

        // Get profile name
        std::string profileName;
        while (true) {
            profileName = promptForString("Enter profile name:");
            if (profileName.empty()) {
                showError("Profile name cannot be empty.");
                continue;
            }
            if (!utils::isValidProfileName(profileName)) {
                showError("Invalid profile name. Use only alphanumeric characters, underscores, and hyphens.");
                continue;
            }

            // Check if profile already exists
            auto existingProfiles = configManager.listProfiles();
            if (std::find(existingProfiles.begin(), existingProfiles.end(), profileName) != existingProfiles.end()) {
                showError("Profile '" + profileName + "' already exists.");
                continue;
            }

            break;
        }

        // Create CLI options based on current preferences
        auto& prefs = UserPreferences::getInstance();
        CliOptions options;
        options = prefs.applyPreferencesToOptions(options);

        // Allow user to customize the profile
        showInfo("Configuring profile '" + profileName + "'...");

        // Template type
        std::vector<std::string> templates = {"console", "lib", "header-only-lib", "multi-executable", "gui", "network"};
        std::string templateStr = std::string(to_string(options.templateType));
        templateStr = promptForChoice("Template type:", templates, templateStr);
        auto templateType = to_template_type(templateStr);
        if (templateType) {
            options.templateType = *templateType;
        }

        // Build system
        std::vector<std::string> buildSystems = {"cmake", "meson", "bazel", "xmake", "premake"};
        std::string buildStr = std::string(to_string(options.buildSystem));
        buildStr = promptForChoice("Build system:", buildSystems, buildStr);
        auto buildSystem = to_build_system(buildStr);
        if (buildSystem) {
            options.buildSystem = *buildSystem;
        }

        // Package manager
        std::vector<std::string> packageManagers = {"vcpkg", "conan", "none"};
        std::string packageStr = std::string(to_string(options.packageManager));
        packageStr = promptForChoice("Package manager:", packageManagers, packageStr);
        auto packageManager = to_package_manager(packageStr);
        if (packageManager) {
            options.packageManager = *packageManager;
        }

        // Tests
        options.includeTests = promptForBool("Include tests?", options.includeTests);
        if (options.includeTests) {
            std::vector<std::string> testFrameworks = {"gtest", "catch2", "doctest", "boost"};
            std::string testStr = std::string(to_string(options.testFramework));
            testStr = promptForChoice("Test framework:", testFrameworks, testStr);
            auto testFramework = to_test_framework(testStr);
            if (testFramework) {
                options.testFramework = *testFramework;
            }
        }

        // Documentation
        options.includeDocumentation = promptForBool("Include documentation?", options.includeDocumentation);

        // Code style tools
        options.includeCodeStyleTools = promptForBool("Include code style tools?", options.includeCodeStyleTools);

        // Git initialization
        options.initGit = promptForBool("Initialize Git repository?", options.initGit);

        // Save the profile
        if (configManager.saveProfile(profileName, options)) {
            showSuccess("Profile '" + profileName + "' created successfully!");
        } else {
            showError("Failed to create profile '" + profileName + "'.");
            return false;
        }

        showSectionFooter();
        return true;

    } catch (const std::exception& e) {
        showError("Error creating profile: " + std::string(e.what()));
        showSectionFooter();
        return false;
    }
}

bool InteractiveConfigWizard::editExistingProfile() {
    showSectionHeader("Edit Existing Profile");

    try {
        auto& configManager = ConfigManager::getInstance();
        auto profiles = configManager.listProfiles();

        if (profiles.empty()) {
            showInfo("No profiles found. Create a profile first.");
            showSectionFooter();
            return true;
        }

        // Select profile to edit
        std::string profileName = promptForChoice("Select profile to edit:", profiles);
        if (profileName.empty()) {
            showSectionFooter();
            return true;
        }

        // Load the profile
        auto optionsOpt = configManager.loadProfile(profileName);
        if (!optionsOpt) {
            showError("Failed to load profile '" + profileName + "'.");
            showSectionFooter();
            return false;
        }

        CliOptions options = *optionsOpt;

        showInfo("Editing profile '" + profileName + "'...");

        // Edit options (similar to create profile, but with existing values)
        // This would be the same configuration flow as in createNewProfile
        // but starting with the loaded options

        // For brevity, I'll just show a simple example
        options.includeTests = promptForBool("Include tests?", options.includeTests);
        options.includeDocumentation = promptForBool("Include documentation?", options.includeDocumentation);
        options.includeCodeStyleTools = promptForBool("Include code style tools?", options.includeCodeStyleTools);

        // Save the updated profile
        if (configManager.saveProfile(profileName, options)) {
            showSuccess("Profile '" + profileName + "' updated successfully!");
        } else {
            showError("Failed to update profile '" + profileName + "'.");
            return false;
        }

        showSectionFooter();
        return true;

    } catch (const std::exception& e) {
        showError("Error editing profile: " + std::string(e.what()));
        showSectionFooter();
        return false;
    }
}

// Helper method implementations
std::string InteractiveConfigWizard::promptForString(const std::string& prompt, const std::string& defaultValue) {
    std::cout << prompt;
    if (!defaultValue.empty()) {
        std::cout << " [" << defaultValue << "]";
    }
    std::cout << ": ";

    std::string input;
    std::getline(std::cin, input);

    if (input.empty() && !defaultValue.empty()) {
        return defaultValue;
    }

    return input;
}

bool InteractiveConfigWizard::promptForBool(const std::string& prompt, bool defaultValue) {
    std::string defaultStr = defaultValue ? "Y/n" : "y/N";
    std::cout << prompt << " [" << defaultStr << "]: ";

    std::string input;
    std::getline(std::cin, input);

    if (input.empty()) {
        return defaultValue;
    }

    std::transform(input.begin(), input.end(), input.begin(), ::tolower);
    return (input == "y" || input == "yes" || input == "true" || input == "1");
}

int InteractiveConfigWizard::promptForInt(const std::string& prompt, int defaultValue, int minValue, int maxValue) {
    while (true) {
        std::cout << prompt;
        if (defaultValue != 0) {
            std::cout << " [" << defaultValue << "]";
        }
        std::cout << ": ";

        std::string input;
        std::getline(std::cin, input);

        if (input.empty() && defaultValue != 0) {
            return defaultValue;
        }

        try {
            int value = std::stoi(input);
            if (value >= minValue && value <= maxValue) {
                return value;
            } else {
                showError("Value must be between " + std::to_string(minValue) + " and " + std::to_string(maxValue));
            }
        } catch (...) {
            showError("Please enter a valid integer.");
        }
    }
}

std::string InteractiveConfigWizard::promptForChoice(const std::string& prompt, const std::vector<std::string>& choices, const std::string& defaultChoice) {
    std::cout << prompt << std::endl;

    for (size_t i = 0; i < choices.size(); ++i) {
        std::cout << "  " << (i + 1) << ". " << choices[i];
        if (choices[i] == defaultChoice) {
            std::cout << " (default)";
        }
        std::cout << std::endl;
    }

    while (true) {
        std::cout << "Enter choice (1-" << choices.size() << ")";
        if (!defaultChoice.empty()) {
            std::cout << " [" << defaultChoice << "]";
        }
        std::cout << ": ";

        std::string input;
        std::getline(std::cin, input);

        if (input.empty() && !defaultChoice.empty()) {
            return defaultChoice;
        }

        try {
            int choice = std::stoi(input);
            if (choice >= 1 && choice <= static_cast<int>(choices.size())) {
                return choices[choice - 1];
            } else {
                showError("Please enter a number between 1 and " + std::to_string(choices.size()));
            }
        } catch (...) {
            showError("Please enter a valid number.");
        }
    }
}

std::vector<std::string> InteractiveConfigWizard::promptForMultiChoice(const std::string& prompt, const std::vector<std::string>& choices) {
    std::cout << prompt << std::endl;
    std::cout << "Enter multiple choices separated by commas (e.g., 1,3,5):" << std::endl;

    for (size_t i = 0; i < choices.size(); ++i) {
        std::cout << "  " << (i + 1) << ". " << choices[i] << std::endl;
    }

    while (true) {
        std::cout << "Enter choices: ";

        std::string input;
        std::getline(std::cin, input);

        if (input.empty()) {
            return {};
        }

        std::vector<std::string> result;
        std::stringstream ss(input);
        std::string item;

        while (std::getline(ss, item, ',')) {
            // Trim whitespace
            item.erase(0, item.find_first_not_of(" \t"));
            item.erase(item.find_last_not_of(" \t") + 1);

            try {
                int choice = std::stoi(item);
                if (choice >= 1 && choice <= static_cast<int>(choices.size())) {
                    result.push_back(choices[choice - 1]);
                } else {
                    showError("Invalid choice: " + item);
                    result.clear();
                    break;
                }
            } catch (...) {
                showError("Invalid number: " + item);
                result.clear();
                break;
            }
        }

        if (!result.empty() || input.empty()) {
            return result;
        }
    }
}

void InteractiveConfigWizard::showWelcomeMessage() {
    clearScreen();
    std::cout << TerminalUtils::colorize("╔══════════════════════════════════════════════════════════════╗", Color::BrightCyan) << std::endl;
    std::cout << TerminalUtils::colorize("║                                                              ║", Color::BrightCyan) << std::endl;
    std::cout << TerminalUtils::colorize("║              CPP-Scaffold Configuration Wizard              ║", Color::BrightCyan) << std::endl;
    std::cout << TerminalUtils::colorize("║                                                              ║", Color::BrightCyan) << std::endl;
    std::cout << TerminalUtils::colorize("║  Welcome! This wizard will help you configure CPP-Scaffold  ║", Color::BrightCyan) << std::endl;
    std::cout << TerminalUtils::colorize("║  to match your preferences and development workflow.        ║", Color::BrightCyan) << std::endl;
    std::cout << TerminalUtils::colorize("║                                                              ║", Color::BrightCyan) << std::endl;
    std::cout << TerminalUtils::colorize("╚══════════════════════════════════════════════════════════════╝", Color::BrightCyan) << std::endl;
    std::cout << std::endl;
}

void InteractiveConfigWizard::showConfigurationSummary() {
    showSectionHeader("Current Configuration Summary");

    auto& prefs = UserPreferences::getInstance();
    auto& configManager = ConfigManager::getInstance();

    std::cout << TerminalUtils::colorize("General Settings:", Color::BrightYellow) << std::endl;
    std::cout << "  Auto Save: " << (prefs.getPreference<bool>("general.auto_save", true) ? "Enabled" : "Disabled") << std::endl;
    std::cout << "  Verbose Logging: " << (prefs.getPreference<bool>("general.verbose_logging", false) ? "Enabled" : "Disabled") << std::endl;
    std::cout << "  Check Updates: " << (prefs.getPreference<bool>("general.check_for_updates", true) ? "Enabled" : "Disabled") << std::endl;
    std::cout << "  Default Language: " << prefs.getPreference<std::string>("default.language", "en") << std::endl;
    std::cout << std::endl;

    std::cout << TerminalUtils::colorize("Default Project Options:", Color::BrightYellow) << std::endl;
    std::cout << "  Template Type: " << prefs.getPreference<std::string>("default.template_type", "console") << std::endl;
    std::cout << "  Build System: " << prefs.getPreference<std::string>("default.build_system", "cmake") << std::endl;
    std::cout << "  Package Manager: " << prefs.getPreference<std::string>("default.package_manager", "vcpkg") << std::endl;
    std::cout << "  Include Tests: " << (prefs.getPreference<bool>("default.include_tests", false) ? "Yes" : "No") << std::endl;
    std::cout << "  Test Framework: " << prefs.getPreference<std::string>("default.test_framework", "gtest") << std::endl;
    std::cout << "  Include Documentation: " << (prefs.getPreference<bool>("default.include_documentation", false) ? "Yes" : "No") << std::endl;
    std::cout << "  Include Code Style Tools: " << (prefs.getPreference<bool>("default.include_code_style_tools", false) ? "Yes" : "No") << std::endl;
    std::cout << "  Initialize Git: " << (prefs.getPreference<bool>("default.init_git", true) ? "Yes" : "No") << std::endl;
    std::cout << std::endl;

    auto profiles = configManager.listProfiles();
    std::cout << TerminalUtils::colorize("Profiles (" + std::to_string(profiles.size()) + "):", Color::BrightYellow) << std::endl;
    for (const auto& profile : profiles) {
        std::cout << "  - " << profile << std::endl;
    }
    std::cout << std::endl;
}

void InteractiveConfigWizard::showProfilesSummary() {
    showSectionHeader("Profiles Summary");

    auto& configManager = ConfigManager::getInstance();
    auto profiles = configManager.listProfiles();

    if (profiles.empty()) {
        std::cout << "No profiles found." << std::endl;
        return;
    }

    std::cout << TerminalUtils::colorize("Available Profiles:", Color::BrightYellow) << std::endl;
    for (const auto& profileName : profiles) {
        auto optionsOpt = configManager.loadProfile(profileName);
        if (optionsOpt) {
            const auto& options = *optionsOpt;
            std::cout << std::endl;
            std::cout << TerminalUtils::colorize("Profile: " + profileName, Color::BrightCyan) << std::endl;
            std::cout << "  Template: " << std::string(to_string(options.templateType)) << std::endl;
            std::cout << "  Build System: " << std::string(to_string(options.buildSystem)) << std::endl;
            std::cout << "  Package Manager: " << std::string(to_string(options.packageManager)) << std::endl;
            std::cout << "  Tests: " << (options.includeTests ? "Yes" : "No") << std::endl;
            if (options.includeTests) {
                std::cout << "  Test Framework: " << std::string(to_string(options.testFramework)) << std::endl;
            }
            std::cout << "  Documentation: " << (options.includeDocumentation ? "Yes" : "No") << std::endl;
            std::cout << "  Code Style Tools: " << (options.includeCodeStyleTools ? "Yes" : "No") << std::endl;
            std::cout << "  Git Init: " << (options.initGit ? "Yes" : "No") << std::endl;
        }
    }
}

int InteractiveConfigWizard::showMenu(const std::string& title, const std::vector<std::string>& options) {
    std::cout << std::endl;
    std::cout << TerminalUtils::colorize("=== " + title + " ===", Color::BrightCyan) << std::endl;
    std::cout << std::endl;

    for (size_t i = 0; i < options.size(); ++i) {
        std::cout << "  " << (i + 1) << ". " << options[i] << std::endl;
    }

    std::cout << std::endl;

    while (true) {
        std::cout << "Enter your choice (1-" << options.size() << "): ";

        std::string input;
        std::getline(std::cin, input);

        try {
            int choice = std::stoi(input);
            if (choice >= 1 && choice <= static_cast<int>(options.size())) {
                return choice;
            } else {
                showError("Please enter a number between 1 and " + std::to_string(options.size()));
            }
        } catch (...) {
            showError("Please enter a valid number.");
        }
    }
}

void InteractiveConfigWizard::showSectionHeader(const std::string& title) {
    s_currentSection = title;
    std::cout << std::endl;
    std::cout << TerminalUtils::colorize("+- " + title + " " + std::string(50 - title.length(), '-') + "+", Color::BrightGreen) << std::endl;
    std::cout << std::endl;
}

void InteractiveConfigWizard::showSectionFooter() {
    std::cout << std::endl;
    std::cout << TerminalUtils::colorize("+" + std::string(52, '-') + "+", Color::BrightGreen) << std::endl;
    s_currentSection = "";
}

void InteractiveConfigWizard::showError(const std::string& message) {
    std::cout << TerminalUtils::colorize("❌ Error: " + message, Color::BrightRed) << std::endl;
}

void InteractiveConfigWizard::showSuccess(const std::string& message) {
    std::cout << TerminalUtils::colorize("✅ " + message, Color::BrightGreen) << std::endl;
}

void InteractiveConfigWizard::showInfo(const std::string& message) {
    std::cout << TerminalUtils::colorize("ℹ️  " + message, Color::BrightBlue) << std::endl;
}

void InteractiveConfigWizard::showWarning(const std::string& message) {
    std::cout << TerminalUtils::colorize("⚠️  " + message, Color::BrightYellow) << std::endl;
}

void InteractiveConfigWizard::waitForEnter() {
    std::cout << std::endl;
    std::cout << "Press Enter to continue...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

void InteractiveConfigWizard::clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

bool InteractiveConfigWizard::confirmAction(const std::string& action) {
    return promptForBool("Are you sure you want to " + action + "?", false);
}

bool InteractiveConfigWizard::isValidProjectName(const std::string& name) {
    if (name.empty() || name.length() > 64) {
        return false;
    }

    for (char c : name) {
        if (!std::isalnum(c) && c != '_' && c != '-') {
            return false;
        }
    }

    return true;
}

bool InteractiveConfigWizard::manageCustomTemplates() {
    showSectionHeader("Custom Template Management");

    std::vector<std::string> templateMenuOptions = {
        "Create New Custom Template",
        "Import Custom Template",
        "List Custom Templates",
        "Delete Custom Template",
        "Back to Main Menu"
    };

    while (true) {
        int choice = showMenu("Custom Template Management", templateMenuOptions);

        switch (choice) {
            case 1:
                showInfo("Create custom template functionality coming soon!");
                waitForEnter();
                break;
            case 2:
                showInfo("Import custom template functionality coming soon!");
                waitForEnter();
                break;
            case 3:
                showInfo("List custom templates functionality coming soon!");
                waitForEnter();
                break;
            case 4:
                showInfo("Delete custom template functionality coming soon!");
                waitForEnter();
                break;
            case 5:
                showSectionFooter();
                return true;
            default:
                showError("Invalid choice. Please try again.");
                waitForEnter();
                break;
        }
    }
}

bool InteractiveConfigWizard::configureAdvancedSettings() {
    showSectionHeader("Advanced Settings");

    showInfo("Advanced settings configuration coming soon!");
    showInfo("This will include:");
    std::cout << "  - Environment variable configuration" << std::endl;
    std::cout << "  - Custom template paths" << std::endl;
    std::cout << "  - Build system specific settings" << std::endl;
    std::cout << "  - Performance tuning options" << std::endl;

    waitForEnter();
    showSectionFooter();
    return true;
}

} // namespace config

#pragma once

#include "config_manager.h"
#include "user_preferences.h"
#include "../cli/cli_parser.h"
#include <string>
#include <vector>
#include <functional>
#include <climits>

namespace config {

// Interactive configuration wizard
class InteractiveConfigWizard {
public:
    // Main wizard entry points
    static bool runConfigurationWizard();
    static bool runPreferencesWizard();
    static bool runProfileWizard();
    static bool runCustomTemplateWizard();

    // Specific configuration wizards
    static bool configureGeneralSettings();
    static bool configureDefaultOptions();
    static bool configureAdvancedSettings();

    // Profile management wizards
    static bool createNewProfile();
    static bool editExistingProfile();
    static bool manageProfiles();

    // Custom template wizards
    static bool createCustomTemplate();
    static bool importCustomTemplate();
    static bool manageCustomTemplates();

    // Interactive helpers
    static std::string promptForString(const std::string& prompt, const std::string& defaultValue = "");
    static bool promptForBool(const std::string& prompt, bool defaultValue = false);
    static int promptForInt(const std::string& prompt, int defaultValue = 0, int minValue = INT_MIN, int maxValue = INT_MAX);
    static std::string promptForChoice(const std::string& prompt, const std::vector<std::string>& choices, const std::string& defaultChoice = "");
    static std::vector<std::string> promptForMultiChoice(const std::string& prompt, const std::vector<std::string>& choices);

    // Display helpers
    static void showWelcomeMessage();
    static void showConfigurationSummary();
    static void showPreferencesSummary();
    static void showProfilesSummary();
    static void showCustomTemplatesSummary();

    // Validation helpers
    static bool confirmAction(const std::string& action);
    static bool validateInput(const std::string& input, const std::function<bool(const std::string&)>& validator);

private:
    // Internal wizard state
    static bool s_wizardRunning;
    static std::string s_currentSection;

    // Internal helpers
    static void showSectionHeader(const std::string& title);
    static void showSectionFooter();
    static void showError(const std::string& message);
    static void showSuccess(const std::string& message);
    static void showInfo(const std::string& message);
    static void showWarning(const std::string& message);

    // Input validation
    static bool isValidProjectName(const std::string& name);
    static bool isValidProfileName(const std::string& name);
    static bool isValidTemplateName(const std::string& name);
    static bool isValidPath(const std::string& path);

    // Menu helpers
    static int showMenu(const std::string& title, const std::vector<std::string>& options);
    static void waitForEnter();
    static void clearScreen();
};

// Configuration validation and migration utilities
class ConfigurationValidator {
public:
    // Validation methods
    static bool validateConfiguration();
    static bool validateProfiles();
    static bool validateCustomTemplates();
    static bool validatePreferences();

    // Migration methods
    static bool migrateConfiguration(int fromVersion, int toVersion);
    static bool backupConfiguration();
    static bool restoreConfiguration(const std::string& backupName);

    // Repair methods
    static bool repairConfiguration();
    static bool resetCorruptedSettings();

    // Diagnostic methods
    static std::vector<std::string> getDiagnosticInfo();
    static void showDiagnosticReport();

private:
    static std::vector<std::string> s_validationErrors;
    static std::vector<std::string> s_validationWarnings;

    // Internal validation helpers
    static bool validateConfigFile(const std::filesystem::path& configPath);
    static bool validateProfileFile(const std::filesystem::path& profilePath);
    static bool validateTemplateFile(const std::filesystem::path& templatePath);

    // Migration helpers
    static bool migrateFromVersion0To1();
    static bool migrateFromVersion1To2();

    // Repair helpers
    static bool repairConfigFile();
    static bool repairProfileFiles();
    static bool repairTemplateFiles();
};

// Configuration export/import utilities
class ConfigurationPortability {
public:
    // Export methods
    static bool exportFullConfiguration(const std::filesystem::path& exportPath);
    static bool exportProfiles(const std::filesystem::path& exportPath);
    static bool exportCustomTemplates(const std::filesystem::path& exportPath);
    static bool exportPreferences(const std::filesystem::path& exportPath);

    // Import methods
    static bool importFullConfiguration(const std::filesystem::path& importPath);
    static bool importProfiles(const std::filesystem::path& importPath);
    static bool importCustomTemplates(const std::filesystem::path& importPath);
    static bool importPreferences(const std::filesystem::path& importPath);

    // Sharing utilities
    static std::string generateShareableConfig(const std::vector<std::string>& profileNames);
    static bool applyShareableConfig(const std::string& shareableConfig);

    // Backup utilities
    static bool createFullBackup();
    static bool restoreFromFullBackup(const std::string& backupName);
    static std::vector<std::string> listAvailableBackups();

private:
    // Archive helpers
    static bool createArchive(const std::filesystem::path& sourcePath, const std::filesystem::path& archivePath);
    static bool extractArchive(const std::filesystem::path& archivePath, const std::filesystem::path& targetPath);

    // Validation helpers
    static bool validateImportData(const nlohmann::json& importData);
    static bool validateShareableConfig(const std::string& config);
};

// Environment variable integration
class EnvironmentIntegration {
public:
    // Environment variable management
    static void scanEnvironmentVariables();
    static void applyEnvironmentOverrides();
    static std::map<std::string, std::string> getRelevantEnvironmentVariables();

    // Environment variable helpers
    static bool setEnvironmentVariable(const std::string& name, const std::string& value);
    static std::optional<std::string> getEnvironmentVariable(const std::string& name);
    static bool hasEnvironmentVariable(const std::string& name);

    // Shell integration
    static bool generateShellCompletions(const std::string& shell);
    static bool installShellIntegration();
    static std::string getShellConfigSnippet();

    // System integration
    static bool registerFileAssociations();
    static bool createDesktopEntry();
    static bool addToSystemPath();

private:
    static std::map<std::string, std::string> s_environmentCache;

    // Platform-specific helpers
    static bool setWindowsEnvironmentVariable(const std::string& name, const std::string& value);
    static bool setUnixEnvironmentVariable(const std::string& name, const std::string& value);

    // Shell-specific helpers
    static std::string getBashCompletions();
    static std::string getZshCompletions();
    static std::string getFishCompletions();
    static std::string getPowerShellCompletions();
};

} // namespace config

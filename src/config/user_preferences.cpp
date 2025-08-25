#include "user_preferences.h"
#include "../utils/file_utils.h"
#include "../utils/terminal_utils.h"
#include <spdlog/spdlog.h>
#include <iostream>

using namespace utils;
using json = nlohmann::json;

namespace config {

UserPreferences& UserPreferences::getInstance() {
    static UserPreferences instance;
    return instance;
}

bool UserPreferences::loadPreferences() {
    try {
        auto& configManager = ConfigManager::getInstance();
        auto configPath = configManager.getConfigDirectory() / "preferences.json";

        if (!FileUtils::fileExists(configPath.string())) {
            spdlog::info("Preferences file not found, using defaults");
            m_preferences = preferences::getDefaultPreferences();
            return savePreferences();
        }

        std::string content = FileUtils::readFromFile(configPath.string());
        if (content.empty()) {
            spdlog::error("Failed to read preferences file");
            return false;
        }

        m_preferences = json::parse(content);

        // Initialize preference definitions
        initializePreferenceDefinitions();

        // Apply environment variable overrides
        applyEnvironmentOverrides();

        spdlog::info("User preferences loaded successfully");
        return true;

    } catch (const std::exception& e) {
        spdlog::error("Error loading preferences: {}", e.what());
        return false;
    }
}

bool UserPreferences::savePreferences() {
    try {
        auto& configManager = ConfigManager::getInstance();
        auto configPath = configManager.getConfigDirectory() / "preferences.json";

        // Ensure config directory exists
        if (!FileUtils::createDirectory(configManager.getConfigDirectory().string())) {
            spdlog::error("Failed to create config directory");
            return false;
        }

        std::string content = m_preferences.dump(2);
        if (!FileUtils::writeToFile(configPath.string(), content)) {
            spdlog::error("Failed to write preferences file");
            return false;
        }

        spdlog::info("User preferences saved successfully");
        return true;

    } catch (const std::exception& e) {
        spdlog::error("Error saving preferences: {}", e.what());
        return false;
    }
}

bool UserPreferences::resetToDefaults() {
    try {
        spdlog::info("Resetting preferences to defaults");
        m_preferences = preferences::getDefaultPreferences();
        return savePreferences();

    } catch (const std::exception& e) {
        spdlog::error("Error resetting preferences: {}", e.what());
        return false;
    }
}

bool UserPreferences::hasPreference(const std::string& key) {
    return m_preferences.contains(key);
}

bool UserPreferences::removePreference(const std::string& key) {
    if (m_preferences.contains(key)) {
        m_preferences.erase(key);
        return true;
    }
    return false;
}

std::vector<PreferenceDefinition> UserPreferences::getPreferencesByCategory(PreferenceCategory category) {
    std::vector<PreferenceDefinition> result;

    for (const auto& pref : m_preferenceDefinitions) {
        if (pref.category == category) {
            result.push_back(pref);
        }
    }

    return result;
}

std::vector<PreferenceDefinition> UserPreferences::getAllPreferences() {
    return m_preferenceDefinitions;
}

std::optional<PreferenceDefinition> UserPreferences::getPreferenceDefinition(const std::string& key) {
    for (const auto& pref : m_preferenceDefinitions) {
        if (pref.key == key) {
            return pref;
        }
    }
    return std::nullopt;
}

void UserPreferences::applyEnvironmentOverrides() {
    try {
        for (const auto& pref : m_preferenceDefinitions) {
            if (!pref.environmentVariable.empty()) {
                const char* envValue = std::getenv(pref.environmentVariable.c_str());
                if (envValue) {
                    json value;
                    std::string envStr(envValue);

                    // Convert environment variable value based on preference type
                    switch (pref.type) {
                        case ConfigValueType::String:
                            value = envStr;
                            break;
                        case ConfigValueType::Boolean:
                            value = (envStr == "true" || envStr == "1" || envStr == "yes");
                            break;
                        case ConfigValueType::Integer:
                            try {
                                value = std::stoi(envStr);
                            } catch (...) {
                                continue; // Skip invalid integer values
                            }
                            break;
                        default:
                            continue; // Skip unsupported types
                    }

                    if (validatePreference(pref.key, value)) {
                        m_preferences[pref.key] = value;
                        spdlog::debug("Applied environment override for {}: {}", pref.key, envStr);
                    }
                }
            }
        }

    } catch (const std::exception& e) {
        spdlog::error("Error applying environment overrides: {}", e.what());
    }
}

std::map<std::string, std::string> UserPreferences::getEnvironmentOverrides() {
    std::map<std::string, std::string> overrides;

    for (const auto& pref : m_preferenceDefinitions) {
        if (!pref.environmentVariable.empty()) {
            const char* envValue = std::getenv(pref.environmentVariable.c_str());
            if (envValue) {
                overrides[pref.key] = envValue;
            }
        }
    }

    return overrides;
}

bool UserPreferences::validatePreference(const std::string& key, const nlohmann::json& value) {
    auto prefDef = getPreferenceDefinition(key);
    if (!prefDef) {
        m_validationErrors.push_back("Unknown preference key: " + key);
        return false;
    }

    // Type validation
    switch (prefDef->type) {
        case ConfigValueType::String:
            if (!value.is_string()) {
                m_validationErrors.push_back("Preference " + key + " must be a string");
                return false;
            }
            break;
        case ConfigValueType::Boolean:
            if (!value.is_boolean()) {
                m_validationErrors.push_back("Preference " + key + " must be a boolean");
                return false;
            }
            break;
        case ConfigValueType::Integer:
            if (!value.is_number_integer()) {
                m_validationErrors.push_back("Preference " + key + " must be an integer");
                return false;
            }
            break;
        case ConfigValueType::Array:
            if (!value.is_array()) {
                m_validationErrors.push_back("Preference " + key + " must be an array");
                return false;
            }
            break;
        case ConfigValueType::Object:
            if (!value.is_object()) {
                m_validationErrors.push_back("Preference " + key + " must be an object");
                return false;
            }
            break;
    }

    // Value validation (allowed values)
    if (!prefDef->allowedValues.empty() && value.is_string()) {
        std::string strValue = value.get<std::string>();
        bool found = false;
        for (const auto& allowedValue : prefDef->allowedValues) {
            if (strValue == allowedValue) {
                found = true;
                break;
            }
        }
        if (!found) {
            m_validationErrors.push_back("Invalid value for " + key + ": " + strValue);
            return false;
        }
    }

    return true;
}

std::vector<std::string> UserPreferences::getValidationErrors() {
    return m_validationErrors;
}

bool UserPreferences::exportPreferences(const std::filesystem::path& filePath) {
    try {
        std::string content = m_preferences.dump(2);
        return FileUtils::writeToFile(filePath.string(), content);

    } catch (const std::exception& e) {
        spdlog::error("Error exporting preferences: {}", e.what());
        return false;
    }
}

bool UserPreferences::importPreferences(const std::filesystem::path& filePath) {
    try {
        if (!FileUtils::fileExists(filePath.string())) {
            spdlog::error("Preferences file not found: {}", filePath.string());
            return false;
        }

        std::string content = FileUtils::readFromFile(filePath.string());
        if (content.empty()) {
            spdlog::error("Failed to read preferences file");
            return false;
        }

        json importedPrefs = json::parse(content);

        // Validate imported preferences
        m_validationErrors.clear();
        for (const auto& [key, value] : importedPrefs.items()) {
            if (!validatePreference(key, value)) {
                spdlog::warn("Skipping invalid preference: {}", key);
            } else {
                m_preferences[key] = value;
            }
        }

        return savePreferences();

    } catch (const std::exception& e) {
        spdlog::error("Error importing preferences: {}", e.what());
        return false;
    }
}

CliOptions UserPreferences::applyPreferencesToOptions(const CliOptions& baseOptions) {
    CliOptions options = baseOptions;

    try {
        // Apply preferences to CLI options
        if (hasPreference("default.template_type")) {
            std::string templateStr = getPreference<std::string>("default.template_type");
            auto templateType = enums::to_template_type(templateStr);
            if (templateType && options.templateType == TemplateType::Console) {
                options.templateType = *templateType;
            }
        }

        if (hasPreference("default.build_system")) {
            std::string buildStr = getPreference<std::string>("default.build_system");
            auto buildSystem = enums::to_build_system(buildStr);
            if (buildSystem && options.buildSystem == BuildSystem::CMake) {
                options.buildSystem = *buildSystem;
            }
        }

        if (hasPreference("default.package_manager")) {
            std::string packageStr = getPreference<std::string>("default.package_manager");
            auto packageManager = enums::to_package_manager(packageStr);
            if (packageManager && options.packageManager == PackageManager::Vcpkg) {
                options.packageManager = *packageManager;
            }
        }

        if (hasPreference("default.include_tests")) {
            options.includeTests = getPreference<bool>("default.include_tests", options.includeTests);
        }

        if (hasPreference("default.test_framework")) {
            std::string testStr = getPreference<std::string>("default.test_framework");
            auto testFramework = enums::to_test_framework(testStr);
            if (testFramework && options.testFramework == TestFramework::GTest) {
                options.testFramework = *testFramework;
            }
        }

        if (hasPreference("default.include_documentation")) {
            options.includeDocumentation = getPreference<bool>("default.include_documentation", options.includeDocumentation);
        }

        if (hasPreference("default.include_code_style_tools")) {
            options.includeCodeStyleTools = getPreference<bool>("default.include_code_style_tools", options.includeCodeStyleTools);
        }

        if (hasPreference("default.init_git")) {
            options.initGit = getPreference<bool>("default.init_git", options.initGit);
        }

        if (hasPreference("default.language")) {
            std::string langStr = getPreference<std::string>("default.language");
            auto language = enums::to_language(langStr);
            if (language && options.language == Language::English) {
                options.language = *language;
            }
        }

    } catch (const std::exception& e) {
        spdlog::error("Error applying preferences to options: {}", e.what());
    }

    return options;
}

void UserPreferences::updatePreferencesFromOptions(const CliOptions& options) {
    try {
        setPreference("default.template_type", std::string(enums::to_string(options.templateType)));
        setPreference("default.build_system", std::string(enums::to_string(options.buildSystem)));
        setPreference("default.package_manager", std::string(enums::to_string(options.packageManager)));
        setPreference("default.include_tests", options.includeTests);
        setPreference("default.test_framework", std::string(enums::to_string(options.testFramework)));
        setPreference("default.include_documentation", options.includeDocumentation);
        setPreference("default.include_code_style_tools", options.includeCodeStyleTools);
        setPreference("default.init_git", options.initGit);
        setPreference("default.language", std::string(enums::to_string(options.language)));

        savePreferences();

    } catch (const std::exception& e) {
        spdlog::error("Error updating preferences from options: {}", e.what());
    }
}

nlohmann::json UserPreferences::getPreferenceValue(const std::string& key) {
    if (m_preferences.contains(key)) {
        return m_preferences[key];
    }

    // Return default value if preference not found
    auto prefDef = getPreferenceDefinition(key);
    if (prefDef) {
        return prefDef->defaultValue;
    }

    return json{};
}

bool UserPreferences::setPreferenceValue(const std::string& key, const nlohmann::json& value) {
    if (validatePreference(key, value)) {
        m_preferences[key] = value;
        return true;
    }
    return false;
}

void UserPreferences::initializePreferenceDefinitions() {
    m_preferenceDefinitions.clear();

    // General preferences
    registerPreference({
        "general.auto_save", "Auto Save", "Automatically save configuration changes",
        ConfigValueType::Boolean, true, {}, PreferenceCategory::General, false, "CPP_SCAFFOLD_AUTO_SAVE"
    });

    registerPreference({
        "general.verbose_logging", "Verbose Logging", "Enable verbose logging output",
        ConfigValueType::Boolean, false, {}, PreferenceCategory::General, false, "CPP_SCAFFOLD_VERBOSE"
    });

    registerPreference({
        "general.check_for_updates", "Check for Updates", "Automatically check for updates",
        ConfigValueType::Boolean, true, {}, PreferenceCategory::General, false, "CPP_SCAFFOLD_CHECK_UPDATES"
    });

    // Default template preferences
    registerPreference({
        "default.template_type", "Default Template Type", "Default project template type",
        ConfigValueType::String, "console", {"console", "lib", "header-only-lib", "multi-executable", "gui", "network"},
        PreferenceCategory::Templates, false, "CPP_SCAFFOLD_DEFAULT_TEMPLATE"
    });

    registerPreference({
        "default.build_system", "Default Build System", "Default build system",
        ConfigValueType::String, "cmake", {"cmake", "meson", "bazel", "xmake", "premake"},
        PreferenceCategory::BuildSystems, false, "CPP_SCAFFOLD_DEFAULT_BUILD_SYSTEM"
    });

    registerPreference({
        "default.package_manager", "Default Package Manager", "Default package manager",
        ConfigValueType::String, "vcpkg", {"vcpkg", "conan", "none"},
        PreferenceCategory::BuildSystems, false, "CPP_SCAFFOLD_DEFAULT_PACKAGE_MANAGER"
    });

    registerPreference({
        "default.include_tests", "Include Tests by Default", "Include test framework by default",
        ConfigValueType::Boolean, false, {}, PreferenceCategory::Testing, false, "CPP_SCAFFOLD_DEFAULT_TESTS"
    });

    registerPreference({
        "default.test_framework", "Default Test Framework", "Default test framework",
        ConfigValueType::String, "gtest", {"gtest", "catch2", "doctest", "boost"},
        PreferenceCategory::Testing, false, "CPP_SCAFFOLD_DEFAULT_TEST_FRAMEWORK"
    });

    registerPreference({
        "default.include_documentation", "Include Documentation by Default", "Include documentation by default",
        ConfigValueType::Boolean, false, {}, PreferenceCategory::Documentation, false, "CPP_SCAFFOLD_DEFAULT_DOCS"
    });

    registerPreference({
        "default.include_code_style_tools", "Include Code Style Tools by Default", "Include code style tools by default",
        ConfigValueType::Boolean, false, {}, PreferenceCategory::CodeStyle, false, "CPP_SCAFFOLD_DEFAULT_CODE_STYLE"
    });

    registerPreference({
        "default.init_git", "Initialize Git by Default", "Initialize Git repository by default",
        ConfigValueType::Boolean, true, {}, PreferenceCategory::General, false, "CPP_SCAFFOLD_DEFAULT_GIT"
    });

    registerPreference({
        "default.language", "Default Language", "Default interface language",
        ConfigValueType::String, "en", {"en", "zh", "es", "jp", "de", "fr"},
        PreferenceCategory::General, false, "CPP_SCAFFOLD_DEFAULT_LANGUAGE"
    });
}

void UserPreferences::registerPreference(const PreferenceDefinition& definition) {
    m_preferenceDefinitions.push_back(definition);
}

// Template specializations
template<>
std::string UserPreferences::getPreference<std::string>(const std::string& key, const std::string& defaultValue) {
    json value = getPreferenceValue(key);
    if (value.is_string()) {
        return value.get<std::string>();
    }
    return defaultValue;
}

template<>
bool UserPreferences::getPreference<bool>(const std::string& key, const bool& defaultValue) {
    json value = getPreferenceValue(key);
    if (value.is_boolean()) {
        return value.get<bool>();
    }
    return defaultValue;
}

template<>
int UserPreferences::getPreference<int>(const std::string& key, const int& defaultValue) {
    json value = getPreferenceValue(key);
    if (value.is_number_integer()) {
        return value.get<int>();
    }
    return defaultValue;
}

template<>
bool UserPreferences::setPreference<std::string>(const std::string& key, const std::string& value) {
    return setPreferenceValue(key, json(value));
}

template<>
bool UserPreferences::setPreference<bool>(const std::string& key, const bool& value) {
    return setPreferenceValue(key, json(value));
}

template<>
bool UserPreferences::setPreference<int>(const std::string& key, const int& value) {
    return setPreferenceValue(key, json(value));
}

// Preference utility functions
namespace preferences {

std::vector<PreferenceCategoryInfo> getPreferenceCategories() {
    return {
        {PreferenceCategory::General, "General", "General application settings",
         {"general.auto_save", "general.verbose_logging", "general.check_for_updates", "default.init_git", "default.language"}},
        {PreferenceCategory::Templates, "Templates", "Default template settings",
         {"default.template_type"}},
        {PreferenceCategory::BuildSystems, "Build Systems", "Build system and package manager settings",
         {"default.build_system", "default.package_manager"}},
        {PreferenceCategory::Testing, "Testing", "Test framework settings",
         {"default.include_tests", "default.test_framework"}},
        {PreferenceCategory::Documentation, "Documentation", "Documentation settings",
         {"default.include_documentation"}},
        {PreferenceCategory::CodeStyle, "Code Style", "Code style and formatting settings",
         {"default.include_code_style_tools"}},
        {PreferenceCategory::Editors, "Editors", "Editor integration settings", {}},
        {PreferenceCategory::CI_CD, "CI/CD", "Continuous integration settings", {}},
        {PreferenceCategory::Advanced, "Advanced", "Advanced configuration options", {}}
    };
}

std::string getPreferenceDisplayName(const std::string& key) {
    auto& prefs = UserPreferences::getInstance();
    auto prefDef = prefs.getPreferenceDefinition(key);
    if (prefDef) {
        return prefDef->displayName;
    }
    return key;
}

std::string getPreferenceDescription(const std::string& key) {
    auto& prefs = UserPreferences::getInstance();
    auto prefDef = prefs.getPreferenceDefinition(key);
    if (prefDef) {
        return prefDef->description;
    }
    return "";
}

std::vector<std::string> getPreferenceAllowedValues(const std::string& key) {
    auto& prefs = UserPreferences::getInstance();
    auto prefDef = prefs.getPreferenceDefinition(key);
    if (prefDef) {
        return prefDef->allowedValues;
    }
    return {};
}

bool isValidPreferenceKey(const std::string& key) {
    if (key.empty() || key.length() > 128) {
        return false;
    }

    // Check for valid characters (alphanumeric, underscore, dot)
    for (char c : key) {
        if (!std::isalnum(c) && c != '_' && c != '.') {
            return false;
        }
    }

    return true;
}

bool isValidPreferenceValue(const std::string& key, const nlohmann::json& value) {
    auto& prefs = UserPreferences::getInstance();
    return prefs.validatePreference(key, value);
}

nlohmann::json getDefaultPreferences() {
    json defaults;

    // General preferences
    defaults["general"]["auto_save"] = true;
    defaults["general"]["verbose_logging"] = false;
    defaults["general"]["check_for_updates"] = true;

    // Default template preferences
    defaults["default"]["template_type"] = "console";
    defaults["default"]["build_system"] = "cmake";
    defaults["default"]["package_manager"] = "vcpkg";
    defaults["default"]["include_tests"] = false;
    defaults["default"]["test_framework"] = "gtest";
    defaults["default"]["include_documentation"] = false;
    defaults["default"]["include_code_style_tools"] = false;
    defaults["default"]["init_git"] = true;
    defaults["default"]["language"] = "en";

    return defaults;
}

std::string getEnvironmentVariableForPreference(const std::string& key) {
    auto& prefs = UserPreferences::getInstance();
    auto prefDef = prefs.getPreferenceDefinition(key);
    if (prefDef) {
        return prefDef->environmentVariable;
    }
    return "";
}

bool hasEnvironmentOverride(const std::string& key) {
    std::string envVar = getEnvironmentVariableForPreference(key);
    if (!envVar.empty()) {
        return std::getenv(envVar.c_str()) != nullptr;
    }
    return false;
}

void showPreferenceHelp(const std::string& key) {
    auto& prefs = UserPreferences::getInstance();
    auto prefDef = prefs.getPreferenceDefinition(key);

    if (!prefDef) {
        std::cout << "Unknown preference: " << key << std::endl;
        return;
    }

    std::cout << TerminalUtils::colorize("Preference: " + prefDef->displayName, Color::BrightCyan) << std::endl;
    std::cout << "Key: " << key << std::endl;
    std::cout << "Description: " << prefDef->description << std::endl;
    std::cout << "Type: ";

    switch (prefDef->type) {
        case ConfigValueType::String:
            std::cout << "String";
            break;
        case ConfigValueType::Boolean:
            std::cout << "Boolean";
            break;
        case ConfigValueType::Integer:
            std::cout << "Integer";
            break;
        case ConfigValueType::Array:
            std::cout << "Array";
            break;
        case ConfigValueType::Object:
            std::cout << "Object";
            break;
    }
    std::cout << std::endl;

    std::cout << "Default: " << prefDef->defaultValue.dump() << std::endl;

    if (!prefDef->allowedValues.empty()) {
        std::cout << "Allowed values: ";
        for (size_t i = 0; i < prefDef->allowedValues.size(); ++i) {
            if (i > 0) std::cout << ", ";
            std::cout << prefDef->allowedValues[i];
        }
        std::cout << std::endl;
    }

    if (!prefDef->environmentVariable.empty()) {
        std::cout << "Environment variable: " << prefDef->environmentVariable << std::endl;
    }

    if (prefDef->requiresRestart) {
        std::cout << TerminalUtils::colorize("Note: Changing this preference requires a restart", Color::Yellow) << std::endl;
    }
}

void showAllPreferences() {
    auto& prefs = UserPreferences::getInstance();
    auto allPrefs = prefs.getAllPreferences();

    std::cout << TerminalUtils::colorize("All Preferences:", Color::BrightCyan) << std::endl;
    std::cout << std::endl;

    for (const auto& pref : allPrefs) {
        std::cout << "  " << pref.key << " - " << pref.displayName << std::endl;
        std::cout << "    " << pref.description << std::endl;
        std::cout << std::endl;
    }
}

void showPreferencesByCategory(PreferenceCategory category) {
    auto& prefs = UserPreferences::getInstance();
    auto categoryPrefs = prefs.getPreferencesByCategory(category);

    auto categories = getPreferenceCategories();
    std::string categoryName = "Unknown";
    for (const auto& cat : categories) {
        if (cat.category == category) {
            categoryName = cat.displayName;
            break;
        }
    }

    std::cout << TerminalUtils::colorize("Preferences - " + categoryName + ":", Color::BrightCyan) << std::endl;
    std::cout << std::endl;

    for (const auto& pref : categoryPrefs) {
        std::cout << "  " << pref.key << " - " << pref.displayName << std::endl;
        std::cout << "    " << pref.description << std::endl;
        std::cout << std::endl;
    }
}

bool migratePreferences(const nlohmann::json& oldPreferences, nlohmann::json& newPreferences) {
    try {
        // Simple migration - copy all valid preferences
        newPreferences = oldPreferences;

        // Add any new default preferences that don't exist
        auto defaults = getDefaultPreferences();
        for (const auto& [key, value] : defaults.items()) {
            if (!newPreferences.contains(key)) {
                newPreferences[key] = value;
            }
        }

        return true;

    } catch (const std::exception& e) {
        spdlog::error("Error migrating preferences: {}", e.what());
        return false;
    }
}

} // namespace preferences

} // namespace config

#include "config_manager.h"
#include "../utils/file_utils.h"
#include "../utils/string_utils.h"
#include <spdlog/spdlog.h>
#include <fstream>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <cstdlib>

using namespace utils;
using namespace cli_enums;
using json = nlohmann::json;

namespace config {

ConfigManager& ConfigManager::getInstance() {
    static ConfigManager instance;
    return instance;
}

bool ConfigManager::loadConfiguration() {
    try {
        if (!ensureConfigDirectoryExists()) {
            spdlog::error("Failed to create configuration directory");
            return false;
        }

        auto configPath = getConfigFilePath();
        if (!FileUtils::fileExists(configPath.string())) {
            spdlog::info("Configuration file not found, creating default configuration");
            initializeDefaultConfiguration();
            return saveConfiguration();
        }

        std::string configContent = FileUtils::readFromFile(configPath.string());
        if (configContent.empty()) {
            spdlog::error("Failed to read configuration file");
            return false;
        }

        m_config = json::parse(configContent);

        // Check schema version and migrate if necessary
        int schemaVersion = m_config.value("schemaVersion", 0);
        if (schemaVersion < CONFIG_SCHEMA_VERSION) {
            spdlog::info("Migrating configuration from version {} to {}", schemaVersion, CONFIG_SCHEMA_VERSION);
            if (!migrateConfiguration(schemaVersion, CONFIG_SCHEMA_VERSION)) {
                spdlog::error("Failed to migrate configuration");
                return false;
            }
        }

        // Load profiles and custom templates
        if (!loadProfiles() || !loadCustomTemplates()) {
            spdlog::error("Failed to load profiles or custom templates");
            return false;
        }

        // Load environment variables
        loadEnvironmentVariables();

        spdlog::info("Configuration loaded successfully");
        return true;

    } catch (const std::exception& e) {
        spdlog::error("Error loading configuration: {}", e.what());
        return false;
    }
}

bool ConfigManager::saveConfiguration() {
    try {
        if (!ensureConfigDirectoryExists()) {
            return false;
        }

        // Update timestamp
        m_config["lastModified"] = getCurrentTimestamp();
        m_config["schemaVersion"] = CONFIG_SCHEMA_VERSION;

        auto configPath = getConfigFilePath();
        std::string configContent = m_config.dump(2);

        if (!FileUtils::writeToFile(configPath.string(), configContent)) {
            spdlog::error("Failed to write configuration file");
            return false;
        }

        // Save profiles and custom templates
        if (!saveProfiles() || !saveCustomTemplates()) {
            spdlog::error("Failed to save profiles or custom templates");
            return false;
        }

        spdlog::info("Configuration saved successfully");
        return true;

    } catch (const std::exception& e) {
        spdlog::error("Error saving configuration: {}", e.what());
        return false;
    }
}

bool ConfigManager::resetToDefaults() {
    try {
        spdlog::info("Resetting configuration to defaults");

        // Create backup before reset
        createBackup();

        // Clear current configuration
        m_config.clear();
        m_profiles.clear();
        m_customTemplates.clear();
        m_environmentOverrides.clear();

        // Initialize with defaults
        initializeDefaultConfiguration();

        return saveConfiguration();

    } catch (const std::exception& e) {
        spdlog::error("Error resetting configuration: {}", e.what());
        return false;
    }
}

bool ConfigManager::saveProfile(const std::string& profileName, const CliOptions& options) {
    try {
        if (!utils::isValidProfileName(profileName)) {
            spdlog::error("Invalid profile name: {}", profileName);
            return false;
        }

        ConfigProfile profile;
        profile.name = profileName;
        profile.description = "User-defined profile";
        profile.settings = cliOptionsToJson(options);
        profile.createdAt = getCurrentTimestamp();
        profile.lastModified = getCurrentTimestamp();
        profile.schemaVersion = CONFIG_SCHEMA_VERSION;

        m_profiles[profileName] = profile;

        spdlog::info("Profile '{}' saved successfully", profileName);
        return saveProfiles();

    } catch (const std::exception& e) {
        spdlog::error("Error saving profile '{}': {}", profileName, e.what());
        return false;
    }
}

std::optional<CliOptions> ConfigManager::loadProfile(const std::string& profileName) {
    try {
        auto it = m_profiles.find(profileName);
        if (it == m_profiles.end()) {
            spdlog::warn("Profile '{}' not found", profileName);
            return std::nullopt;
        }

        const auto& profile = it->second;

        // Check if profile needs migration
        if (profile.schemaVersion < CONFIG_SCHEMA_VERSION) {
            spdlog::warn("Profile '{}' uses old schema version {}, current is {}",
                        profileName, profile.schemaVersion, CONFIG_SCHEMA_VERSION);
        }

        CliOptions options = jsonToCliOptions(profile.settings);
        spdlog::info("Profile '{}' loaded successfully", profileName);
        return options;

    } catch (const std::exception& e) {
        spdlog::error("Error loading profile '{}': {}", profileName, e.what());
        return std::nullopt;
    }
}

std::vector<std::string> ConfigManager::listProfiles() {
    std::vector<std::string> profileNames;
    profileNames.reserve(m_profiles.size());

    for (const auto& [name, profile] : m_profiles) {
        profileNames.push_back(name);
    }

    return profileNames;
}

bool ConfigManager::deleteProfile(const std::string& profileName) {
    try {
        auto it = m_profiles.find(profileName);
        if (it == m_profiles.end()) {
            spdlog::warn("Profile '{}' not found", profileName);
            return false;
        }

        m_profiles.erase(it);
        spdlog::info("Profile '{}' deleted successfully", profileName);
        return saveProfiles();

    } catch (const std::exception& e) {
        spdlog::error("Error deleting profile '{}': {}", profileName, e.what());
        return false;
    }
}

CliOptions ConfigManager::loadDefaultOptions() {
    try {
        if (m_config.contains("defaultOptions")) {
            return jsonToCliOptions(m_config["defaultOptions"]);
        }

        // Return system defaults if no user defaults exist
        CliOptions defaults;
        defaults.templateType = TemplateType::Console;
        defaults.buildSystem = BuildSystem::CMake;
        defaults.packageManager = PackageManager::Vcpkg;
        defaults.testFramework = TestFramework::GTest;
        defaults.language = Language::English;
        defaults.initGit = true;

        return defaults;

    } catch (const std::exception& e) {
        spdlog::error("Error loading default options: {}", e.what());
        return CliOptions{};
    }
}

bool ConfigManager::saveOptionsAsDefaults(const CliOptions& options) {
    try {
        m_config["defaultOptions"] = cliOptionsToJson(options);
        spdlog::info("Default options saved successfully");
        return saveConfiguration();

    } catch (const std::exception& e) {
        spdlog::error("Error saving default options: {}", e.what());
        return false;
    }
}

void ConfigManager::loadEnvironmentVariables() {
    try {
        // Load relevant environment variables
        auto envVars = utils::getRelevantEnvironmentVariables();

        for (const auto& [key, value] : envVars) {
            m_environmentOverrides[key] = value;
        }

        spdlog::info("Loaded {} environment variables", envVars.size());

    } catch (const std::exception& e) {
        spdlog::error("Error loading environment variables: {}", e.what());
    }
}

std::optional<std::string> ConfigManager::getEnvironmentValue(const std::string& key) {
    auto it = m_environmentOverrides.find(key);
    if (it != m_environmentOverrides.end()) {
        return it->second;
    }
    return std::nullopt;
}

void ConfigManager::setEnvironmentOverride(const std::string& key, const std::string& value) {
    m_environmentOverrides[key] = value;
}

std::filesystem::path ConfigManager::getConfigDirectory() {
    // Check for environment variable override first (for testing)
    const char* overrideDir = std::getenv("CPP_SCAFFOLD_CONFIG_DIR");
    if (overrideDir) {
        return std::filesystem::path(overrideDir);
    }
    return utils::getUserConfigDirectory() / "cpp-scaffold";
}

std::filesystem::path ConfigManager::getConfigFilePath() {
    return getConfigDirectory() / "config.json";
}

std::filesystem::path ConfigManager::getProfilesDirectory() {
    return getConfigDirectory() / "profiles";
}

std::filesystem::path ConfigManager::getCustomTemplatesDirectory() {
    return getConfigDirectory() / "templates";
}

std::filesystem::path ConfigManager::getCacheDirectory() {
    return getConfigDirectory() / "cache";
}

bool ConfigManager::ensureConfigDirectoryExists() {
    try {
        auto configDir = getConfigDirectory();
        auto profilesDir = getProfilesDirectory();
        auto templatesDir = getCustomTemplatesDirectory();
        auto cacheDir = getCacheDirectory();

        return FileUtils::createDirectory(configDir.string()) &&
               FileUtils::createDirectory(profilesDir.string()) &&
               FileUtils::createDirectory(templatesDir.string()) &&
               FileUtils::createDirectory(cacheDir.string());

    } catch (const std::exception& e) {
        spdlog::error("Error creating configuration directories: {}", e.what());
        return false;
    }
}

void ConfigManager::initializeDefaultConfiguration() {
    m_config = json::object();
    m_config["schemaVersion"] = CONFIG_SCHEMA_VERSION;
    m_config["createdAt"] = getCurrentTimestamp();
    m_config["lastModified"] = getCurrentTimestamp();

    // Initialize default settings
    m_config["settings"] = json::object();
    m_config["settings"]["autoSave"] = true;
    m_config["settings"]["verboseLogging"] = false;
    m_config["settings"]["checkForUpdates"] = true;
    m_config["settings"]["createBackups"] = true;

    // Initialize default options
    CliOptions defaults;
    m_config["defaultOptions"] = cliOptionsToJson(defaults);

    registerDefaultConfigEntries();
}

std::string ConfigManager::getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::gmtime(&time_t), "%Y-%m-%dT%H:%M:%SZ");
    return ss.str();
}

bool ConfigManager::loadProfiles() {
    try {
        auto profilesDir = getProfilesDirectory();
        if (!FileUtils::directoryExists(profilesDir.string())) {
            return true; // No profiles directory is fine
        }

        auto profileFiles = FileUtils::listFiles(profilesDir.string());
        for (const auto& profileFile : profileFiles) {
            if (FileUtils::getFileExtension(profileFile) != ".json") {
                continue;
            }

            std::string content = FileUtils::readFromFile(profileFile);
            if (content.empty()) {
                continue;
            }

            json profileJson = json::parse(content);
            ConfigProfile profile;
            profile.name = profileJson.value("name", "");
            profile.description = profileJson.value("description", "");
            profile.settings = profileJson.value("settings", json::object());
            profile.createdAt = profileJson.value("createdAt", "");
            profile.lastModified = profileJson.value("lastModified", "");
            profile.schemaVersion = profileJson.value("schemaVersion", 0);

            if (!profile.name.empty()) {
                m_profiles[profile.name] = profile;
            }
        }

        spdlog::info("Loaded {} profiles", m_profiles.size());
        return true;

    } catch (const std::exception& e) {
        spdlog::error("Error loading profiles: {}", e.what());
        return false;
    }
}

bool ConfigManager::saveProfiles() {
    try {
        auto profilesDir = getProfilesDirectory();
        if (!FileUtils::createDirectory(profilesDir.string())) {
            return false;
        }

        for (const auto& [name, profile] : m_profiles) {
            json profileJson;
            profileJson["name"] = profile.name;
            profileJson["description"] = profile.description;
            profileJson["settings"] = profile.settings;
            profileJson["createdAt"] = profile.createdAt;
            profileJson["lastModified"] = profile.lastModified;
            profileJson["schemaVersion"] = profile.schemaVersion;

            auto profilePath = profilesDir / (name + ".json");
            if (!FileUtils::writeToFile(profilePath.string(), profileJson.dump(2))) {
                spdlog::error("Failed to save profile: {}", name);
                return false;
            }
        }

        return true;

    } catch (const std::exception& e) {
        spdlog::error("Error saving profiles: {}", e.what());
        return false;
    }
}

bool ConfigManager::loadCustomTemplates() {
    try {
        auto templatesDir = getCustomTemplatesDirectory();
        if (!FileUtils::directoryExists(templatesDir.string())) {
            return true; // No templates directory is fine
        }

        auto templateFiles = FileUtils::listFiles(templatesDir.string());
        for (const auto& templateFile : templateFiles) {
            if (FileUtils::getFileExtension(templateFile) != ".json") {
                continue;
            }

            std::string content = FileUtils::readFromFile(templateFile);
            if (content.empty()) {
                continue;
            }

            json templateJson = json::parse(content);
            CustomTemplateConfig templateConfig;
            templateConfig.name = templateJson.value("name", "");
            templateConfig.description = templateJson.value("description", "");
            templateConfig.templatePath = templateJson.value("templatePath", "");
            templateConfig.author = templateJson.value("author", "");
            templateConfig.version = templateJson.value("version", "1.0.0");
            templateConfig.createdAt = templateJson.value("createdAt", "");

            // Load variables
            if (templateJson.contains("variables")) {
                for (const auto& [key, value] : templateJson["variables"].items()) {
                    templateConfig.variables[key] = value.get<std::string>();
                }
            }

            // Load required files
            if (templateJson.contains("requiredFiles")) {
                for (const auto& file : templateJson["requiredFiles"]) {
                    templateConfig.requiredFiles.push_back(file.get<std::string>());
                }
            }

            if (!templateConfig.name.empty()) {
                m_customTemplates[templateConfig.name] = templateConfig;
            }
        }

        spdlog::info("Loaded {} custom templates", m_customTemplates.size());
        return true;

    } catch (const std::exception& e) {
        spdlog::error("Error loading custom templates: {}", e.what());
        return false;
    }
}

bool ConfigManager::saveCustomTemplates() {
    try {
        auto templatesDir = getCustomTemplatesDirectory();
        if (!FileUtils::createDirectory(templatesDir.string())) {
            return false;
        }

        for (const auto& [name, templateConfig] : m_customTemplates) {
            json templateJson;
            templateJson["name"] = templateConfig.name;
            templateJson["description"] = templateConfig.description;
            templateJson["templatePath"] = templateConfig.templatePath.string();
            templateJson["author"] = templateConfig.author;
            templateJson["version"] = templateConfig.version;
            templateJson["createdAt"] = templateConfig.createdAt;

            // Save variables
            templateJson["variables"] = json::object();
            for (const auto& [key, value] : templateConfig.variables) {
                templateJson["variables"][key] = value;
            }

            // Save required files
            templateJson["requiredFiles"] = json::array();
            for (const auto& file : templateConfig.requiredFiles) {
                templateJson["requiredFiles"].push_back(file);
            }

            auto templatePath = templatesDir / (name + ".json");
            if (!FileUtils::writeToFile(templatePath.string(), templateJson.dump(2))) {
                spdlog::error("Failed to save custom template: {}", name);
                return false;
            }
        }

        return true;

    } catch (const std::exception& e) {
        spdlog::error("Error saving custom templates: {}", e.what());
        return false;
    }
}

void ConfigManager::registerDefaultConfigEntries() {
    // This method would register all the default configuration entries
    // For now, we'll keep it simple and add entries as needed
}

CliOptions ConfigManager::jsonToCliOptions(const nlohmann::json& json) {
    CliOptions options;

    try {
        if (json.contains("projectName")) {
            options.projectName = json["projectName"].get<std::string>();
        }

        if (json.contains("templateType")) {
            std::string templateStr = json["templateType"].get<std::string>();
            auto templateType = to_template_type(templateStr);
            if (templateType) {
                options.templateType = *templateType;
            }
        }

        if (json.contains("buildSystem")) {
            std::string buildStr = json["buildSystem"].get<std::string>();
            auto buildSystem = to_build_system(buildStr);
            if (buildSystem) {
                options.buildSystem = *buildSystem;
            }
        }

        if (json.contains("packageManager")) {
            std::string packageStr = json["packageManager"].get<std::string>();
            auto packageManager = to_package_manager(packageStr);
            if (packageManager) {
                options.packageManager = *packageManager;
            }
        }

        if (json.contains("includeTests")) {
            options.includeTests = json["includeTests"].get<bool>();
        }

        if (json.contains("testFramework")) {
            std::string testStr = json["testFramework"].get<std::string>();
            auto testFramework = to_test_framework(testStr);
            if (testFramework) {
                options.testFramework = *testFramework;
            }
        }

        if (json.contains("includeDocumentation")) {
            options.includeDocumentation = json["includeDocumentation"].get<bool>();
        }

        if (json.contains("includeCodeStyleTools")) {
            options.includeCodeStyleTools = json["includeCodeStyleTools"].get<bool>();
        }

        if (json.contains("initGit")) {
            options.initGit = json["initGit"].get<bool>();
        }

        if (json.contains("verbose")) {
            options.verbose = json["verbose"].get<bool>();
        }

        if (json.contains("language")) {
            std::string langStr = json["language"].get<std::string>();
            auto language = to_language(langStr);
            if (language) {
                options.language = *language;
            }
        }

        if (json.contains("customTemplatePath")) {
            options.customTemplatePath = json["customTemplatePath"].get<std::string>();
        }

        // Load editor options
        if (json.contains("editorOptions")) {
            for (const auto& editorStr : json["editorOptions"]) {
                auto editor = to_editor_config(editorStr.get<std::string>());
                if (editor) {
                    options.editorOptions.push_back(*editor);
                }
            }
        }

        // Load CI options
        if (json.contains("ciOptions")) {
            for (const auto& ciStr : json["ciOptions"]) {
                auto ci = to_ci_system(ciStr.get<std::string>());
                if (ci) {
                    options.ciOptions.push_back(*ci);
                }
            }
        }

    } catch (const std::exception& e) {
        spdlog::error("Error converting JSON to CliOptions: {}", e.what());
    }

    return options;
}

nlohmann::json ConfigManager::cliOptionsToJson(const CliOptions& options) {
    json result;

    try {
        result["projectName"] = options.projectName;
        result["templateType"] = std::string(to_string(options.templateType));
        result["buildSystem"] = std::string(to_string(options.buildSystem));
        result["packageManager"] = std::string(to_string(options.packageManager));
        result["includeTests"] = options.includeTests;
        result["testFramework"] = std::string(to_string(options.testFramework));
        result["includeDocumentation"] = options.includeDocumentation;
        result["includeCodeStyleTools"] = options.includeCodeStyleTools;
        result["initGit"] = options.initGit;
        result["verbose"] = options.verbose;
        result["language"] = std::string(to_string(options.language));
        result["customTemplatePath"] = options.customTemplatePath.string();

        // Convert editor options
        result["editorOptions"] = json::array();
        for (const auto& editor : options.editorOptions) {
            result["editorOptions"].push_back(std::string(to_string(editor)));
        }

        // Convert CI options
        result["ciOptions"] = json::array();
        for (const auto& ci : options.ciOptions) {
            result["ciOptions"].push_back(std::string(to_string(ci)));
        }

    } catch (const std::exception& e) {
        spdlog::error("Error converting CliOptions to JSON: {}", e.what());
    }

    return result;
}

// Utility functions implementation
namespace utils {

std::string expandEnvironmentVariables(const std::string& input) {
    std::string result = input;
    size_t pos = 0;

    while ((pos = result.find("${", pos)) != std::string::npos) {
        size_t endPos = result.find("}", pos);
        if (endPos == std::string::npos) {
            break;
        }

        std::string varName = result.substr(pos + 2, endPos - pos - 2);
        const char* envValue = std::getenv(varName.c_str());

        if (envValue) {
            result.replace(pos, endPos - pos + 1, envValue);
            pos += strlen(envValue);
        } else {
            pos = endPos + 1;
        }
    }

    return result;
}

std::map<std::string, std::string> getRelevantEnvironmentVariables() {
    std::map<std::string, std::string> envVars;

    // List of environment variables relevant to C++ development
    std::vector<std::string> relevantVars = {
        "CC", "CXX", "CFLAGS", "CXXFLAGS", "LDFLAGS",
        "CMAKE_PREFIX_PATH", "CMAKE_GENERATOR", "CMAKE_BUILD_TYPE",
        "VCPKG_ROOT", "CONAN_HOME", "CONAN_USER_HOME",
        "BOOST_ROOT", "QT_DIR", "SFML_ROOT",
        "PATH", "LD_LIBRARY_PATH", "DYLD_LIBRARY_PATH",
        "CPP_SCAFFOLD_TEMPLATE_PATH", "CPP_SCAFFOLD_CONFIG_DIR",
        "CPP_SCAFFOLD_DEFAULT_BUILD_SYSTEM", "CPP_SCAFFOLD_DEFAULT_PACKAGE_MANAGER"
    };

    for (const auto& varName : relevantVars) {
        const char* value = std::getenv(varName.c_str());
        if (value) {
            envVars[varName] = value;
        }
    }

    return envVars;
}

std::filesystem::path resolveConfigPath(const std::string& relativePath) {
    auto configDir = getUserConfigDirectory() / "cpp-scaffold";
    return configDir / relativePath;
}

std::filesystem::path getUserConfigDirectory() {
#ifdef _WIN32
    const char* appData = std::getenv("APPDATA");
    if (appData) {
        return std::filesystem::path(appData);
    }
    const char* userProfile = std::getenv("USERPROFILE");
    if (userProfile) {
        return std::filesystem::path(userProfile) / "AppData" / "Roaming";
    }
    return std::filesystem::current_path();
#else
    const char* xdgConfigHome = std::getenv("XDG_CONFIG_HOME");
    if (xdgConfigHome) {
        return std::filesystem::path(xdgConfigHome);
    }
    const char* home = std::getenv("HOME");
    if (home) {
        return std::filesystem::path(home) / ".config";
    }
    return std::filesystem::current_path();
#endif
}

std::filesystem::path getSystemConfigDirectory() {
#ifdef _WIN32
    const char* programData = std::getenv("PROGRAMDATA");
    if (programData) {
        return std::filesystem::path(programData) / "cpp-scaffold";
    }
    return std::filesystem::path("C:") / "ProgramData" / "cpp-scaffold";
#else
    return std::filesystem::path("/etc/cpp-scaffold");
#endif
}

bool isValidProfileName(const std::string& name) {
    if (name.empty() || name.length() > 64) {
        return false;
    }

    // Check for valid characters (alphanumeric, underscore, hyphen)
    for (char c : name) {
        if (!std::isalnum(c) && c != '_' && c != '-') {
            return false;
        }
    }

    // Reserved names
    std::vector<std::string> reserved = {"default", "system", "global", "temp", "tmp"};
    for (const auto& reservedName : reserved) {
        if (name == reservedName) {
            return false;
        }
    }

    return true;
}

bool isValidTemplateName(const std::string& name) {
    if (name.empty() || name.length() > 64) {
        return false;
    }

    // Check for valid characters (alphanumeric, underscore, hyphen)
    for (char c : name) {
        if (!std::isalnum(c) && c != '_' && c != '-') {
            return false;
        }
    }

    return true;
}

bool isValidConfigKey(const std::string& key) {
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

nlohmann::json mergeJsonObjects(const nlohmann::json& base, const nlohmann::json& overlay) {
    if (!base.is_object() || !overlay.is_object()) {
        return overlay;
    }

    nlohmann::json result = base;

    for (const auto& [key, value] : overlay.items()) {
        if (result.contains(key) && result[key].is_object() && value.is_object()) {
            result[key] = mergeJsonObjects(result[key], value);
        } else {
            result[key] = value;
        }
    }

    return result;
}

bool isJsonEqual(const nlohmann::json& a, const nlohmann::json& b) {
    return a == b;
}

std::string formatJsonForDisplay(const nlohmann::json& json) {
    return json.dump(2);
}

} // namespace utils

bool ConfigManager::migrateConfiguration(int fromVersion, int toVersion) {
    try {
        spdlog::info("Migrating configuration from version {} to {}", fromVersion, toVersion);

        // Simple migration for now - just update the schema version
        m_config["schemaVersion"] = toVersion;

        return saveConfiguration();

    } catch (const std::exception& e) {
        spdlog::error("Error migrating configuration: {}", e.what());
        return false;
    }
}

bool ConfigManager::createBackup() {
    try {
        auto backupDir = getConfigDirectory() / "backups";
        if (!FileUtils::createDirectory(backupDir.string())) {
            return false;
        }

        std::string timestamp = getCurrentTimestamp();
        std::string backupName = "config_backup_" + timestamp + ".json";
        auto backupPath = backupDir / backupName;

        std::string configContent = m_config.dump(2);
        return FileUtils::writeToFile(backupPath.string(), configContent);

    } catch (const std::exception& e) {
        spdlog::error("Error creating backup: {}", e.what());
        return false;
    }
}

} // namespace config

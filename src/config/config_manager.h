#pragma once

#include "../cli/types/cli_options.h"
#include <filesystem>
#include <map>
#include <memory>
#include <optional>
#include <string>
#include <vector>
#include <nlohmann/json.hpp>

namespace config {

// Configuration schema version for migration support
constexpr int CONFIG_SCHEMA_VERSION = 1;

// Configuration categories
enum class ConfigCategory {
    Global,
    Template,
    BuildSystem,
    Editor,
    CI,
    CodeStyle,
    Custom
};

// Configuration value types
enum class ConfigValueType {
    String,
    Integer,
    Boolean,
    Array,
    Object
};

// Configuration entry structure
struct ConfigEntry {
    std::string key;
    nlohmann::json value;
    ConfigValueType type;
    std::string description;
    std::optional<nlohmann::json> defaultValue;
    std::vector<std::string> allowedValues; // For validation
    bool isRequired = false;
    bool isEnvironmentVariable = false;
    std::string environmentKey; // Environment variable name if applicable
};

// Configuration profile structure
struct ConfigProfile {
    std::string name;
    std::string description;
    std::map<std::string, nlohmann::json> settings;
    std::string createdAt;
    std::string lastModified;
    int schemaVersion = CONFIG_SCHEMA_VERSION;
};

// Custom template configuration
struct CustomTemplateConfig {
    std::string name;
    std::string description;
    std::filesystem::path templatePath;
    std::map<std::string, std::string> variables;
    std::vector<std::string> requiredFiles;
    std::string author;
    std::string version;
    std::string createdAt;
};

// Enhanced configuration manager
class ConfigManager {
public:
    // Singleton pattern
    static ConfigManager& getInstance();

    // Configuration file management
    bool loadConfiguration();
    bool saveConfiguration();
    bool resetToDefaults();

    // Profile management
    bool saveProfile(const std::string& profileName, const CliOptions& options);
    std::optional<CliOptions> loadProfile(const std::string& profileName);
    std::vector<std::string> listProfiles();
    bool deleteProfile(const std::string& profileName);
    bool exportProfile(const std::string& profileName, const std::filesystem::path& exportPath);
    bool importProfile(const std::filesystem::path& importPath);

    // Default options management
    CliOptions loadDefaultOptions();
    bool saveOptionsAsDefaults(const CliOptions& options);

    // Environment variable support
    void loadEnvironmentVariables();
    std::optional<std::string> getEnvironmentValue(const std::string& key);
    void setEnvironmentOverride(const std::string& key, const std::string& value);

    // Custom template management
    bool registerCustomTemplate(const CustomTemplateConfig& templateConfig);
    std::optional<CustomTemplateConfig> getCustomTemplate(const std::string& name);
    std::vector<CustomTemplateConfig> listCustomTemplates();
    bool removeCustomTemplate(const std::string& name);

    // Configuration validation
    bool validateConfiguration();
    std::vector<std::string> getValidationErrors();

    // Configuration paths
    std::filesystem::path getConfigDirectory();
    std::filesystem::path getConfigFilePath();
    std::filesystem::path getProfilesDirectory();
    std::filesystem::path getCustomTemplatesDirectory();
    std::filesystem::path getCacheDirectory();

    // Configuration queries
    template<typename T>
    std::optional<T> getValue(const std::string& key);

    template<typename T>
    bool setValue(const std::string& key, const T& value);

    bool hasKey(const std::string& key);
    std::vector<std::string> getKeys(ConfigCategory category = ConfigCategory::Global);

    // Configuration migration
    bool migrateConfiguration(int fromVersion, int toVersion);

    // Configuration backup and restore
    bool createBackup();
    bool restoreFromBackup(const std::string& backupName);
    std::vector<std::string> listBackups();

private:
    ConfigManager() = default;
    ~ConfigManager() = default;
    ConfigManager(const ConfigManager&) = delete;
    ConfigManager& operator=(const ConfigManager&) = delete;

    // Internal data
    nlohmann::json m_config;
    std::map<std::string, ConfigProfile> m_profiles;
    std::map<std::string, CustomTemplateConfig> m_customTemplates;
    std::map<std::string, std::string> m_environmentOverrides;
    std::vector<std::string> m_validationErrors;

    // Internal methods
    bool loadProfiles();
    bool saveProfiles();
    bool loadCustomTemplates();
    bool saveCustomTemplates();

    void initializeDefaultConfiguration();
    void registerDefaultConfigEntries();

    std::string getCurrentTimestamp();
    bool createDirectoryStructure();

    // Configuration conversion helpers
    CliOptions jsonToCliOptions(const nlohmann::json& json);
    nlohmann::json cliOptionsToJson(const CliOptions& options);

    // Validation helpers
    bool validateConfigEntry(const ConfigEntry& entry, const nlohmann::json& value);
    bool validateProfile(const ConfigProfile& profile);
    bool validateCustomTemplate(const CustomTemplateConfig& templateConfig);

    // File system helpers
    bool ensureConfigDirectoryExists();
    std::string generateBackupName();
};

// Configuration utility functions
namespace utils {
    // Environment variable helpers
    std::string expandEnvironmentVariables(const std::string& input);
    std::map<std::string, std::string> getRelevantEnvironmentVariables();

    // Path helpers
    std::filesystem::path resolveConfigPath(const std::string& relativePath);
    std::filesystem::path getUserConfigDirectory();
    std::filesystem::path getSystemConfigDirectory();

    // Validation helpers
    bool isValidProfileName(const std::string& name);
    bool isValidTemplateName(const std::string& name);
    bool isValidConfigKey(const std::string& key);

    // JSON helpers
    nlohmann::json mergeJsonObjects(const nlohmann::json& base, const nlohmann::json& overlay);
    bool isJsonEqual(const nlohmann::json& a, const nlohmann::json& b);
    std::string formatJsonForDisplay(const nlohmann::json& json);
}

} // namespace config

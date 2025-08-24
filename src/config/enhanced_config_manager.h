#pragma once

#include "config_manager.h"
#include "../cli/cli_parser.h"
#include <string>
#include <map>
#include <vector>
#include <optional>
#include <functional>
#include <filesystem>
#include <nlohmann/json.hpp>

namespace config {

// Configuration scope levels
enum class ConfigScope {
    Global,     // System-wide configuration
    User,       // User-specific configuration
    Project,    // Project-specific configuration
    Session     // Session-specific configuration
};

// Configuration source types
enum class ConfigSource {
    Default,        // Built-in defaults
    SystemConfig,   // System configuration file
    UserConfig,     // User configuration file
    ProjectConfig,  // Project configuration file
    Environment,    // Environment variables
    CommandLine,    // Command-line arguments
    Interactive     // Interactive input
};

// Enhanced configuration entry with metadata (extends base ConfigEntry)
struct EnhancedConfigEntry : public ConfigEntry {
    ConfigSource source = ConfigSource::Default;
    ConfigScope scope = ConfigScope::User;
    bool isReadOnly = false;
    bool isSecret = false;
    std::function<bool(const nlohmann::json&)> validator;
    std::string validationMessage;

    // Constructor to initialize base class properly
    EnhancedConfigEntry() {
        type = ConfigValueType::String; // Default type
    }
};

// Configuration change event
struct ConfigChangeEvent {
    std::string key;
    nlohmann::json oldValue;
    nlohmann::json newValue;
    ConfigSource source;
    std::string timestamp;
};

// Configuration template for different use cases
struct ConfigTemplate {
    std::string name;
    std::string description;
    std::string category;
    std::map<std::string, nlohmann::json> settings;
    std::vector<std::string> requiredKeys;
    std::vector<std::string> tags;
    bool isBuiltIn = false;
};

// Enhanced configuration manager
class EnhancedConfigManager {
public:
    static EnhancedConfigManager& getInstance();

    // Configuration loading and saving
    bool loadConfiguration(ConfigScope scope = ConfigScope::User);
    bool saveConfiguration(ConfigScope scope = ConfigScope::User);
    bool reloadConfiguration();
    bool resetConfiguration(ConfigScope scope = ConfigScope::User);

    // Configuration value management
    template<typename T>
    std::optional<T> getValue(const std::string& key, ConfigScope scope = ConfigScope::User);

    template<typename T>
    bool setValue(const std::string& key, const T& value, ConfigScope scope = ConfigScope::User);

    bool hasKey(const std::string& key, ConfigScope scope = ConfigScope::User);
    bool removeKey(const std::string& key, ConfigScope scope = ConfigScope::User);

    // Configuration hierarchy and inheritance
    nlohmann::json getEffectiveValue(const std::string& key);
    std::vector<EnhancedConfigEntry> getConfigHierarchy(const std::string& key);
    ConfigSource getValueSource(const std::string& key);

    // Default configuration management
    CliOptions getDefaultOptions();
    bool setDefaultOptions(const CliOptions& options);
    bool resetToDefaults();

    // Configuration templates
    std::vector<ConfigTemplate> getAvailableTemplates();
    std::optional<ConfigTemplate> getTemplate(const std::string& name);
    bool applyTemplate(const std::string& templateName, ConfigScope scope = ConfigScope::User);
    bool saveAsTemplate(const std::string& templateName, const std::string& description);
    bool removeTemplate(const std::string& templateName);

    // Configuration validation
    bool validateConfiguration(ConfigScope scope = ConfigScope::User);
    std::vector<std::string> getValidationErrors(ConfigScope scope = ConfigScope::User);
    bool validateKey(const std::string& key, const nlohmann::json& value);

    // Configuration migration
    bool migrateConfiguration(int fromVersion, int toVersion);
    bool backupConfiguration(const std::string& backupName = "");
    bool restoreConfiguration(const std::string& backupName);
    std::vector<std::string> listBackups();

    // Environment variable integration
    void loadEnvironmentVariables(const std::string& prefix = "CPP_SCAFFOLD_");
    std::map<std::string, std::string> getEnvironmentOverrides();
    void setEnvironmentOverride(const std::string& key, const std::string& value);
    void clearEnvironmentOverrides();

    // Configuration file management
    std::filesystem::path getConfigFilePath(ConfigScope scope);
    std::filesystem::path getConfigDirectory(ConfigScope scope);
    bool ensureConfigDirectoryExists(ConfigScope scope);

    // Configuration export/import
    bool exportConfiguration(const std::string& filePath, ConfigScope scope = ConfigScope::User);
    bool importConfiguration(const std::string& filePath, ConfigScope scope = ConfigScope::User);
    std::string exportToString(ConfigScope scope = ConfigScope::User);
    bool importFromString(const std::string& configData, ConfigScope scope = ConfigScope::User);

    // Configuration monitoring and events
    void addChangeListener(const std::string& key, std::function<void(const ConfigChangeEvent&)> listener);
    void removeChangeListener(const std::string& key);
    std::vector<ConfigChangeEvent> getChangeHistory(const std::string& key = "");
    void clearChangeHistory();

    // Interactive configuration
    bool configureInteractively();
    bool configureKey(const std::string& key);
    void showConfigurationSummary();
    void showConfigurationHelp(const std::string& key = "");

    // Configuration search and discovery
    std::vector<std::string> searchKeys(const std::string& pattern);
    std::vector<EnhancedConfigEntry> getKeysInCategory(const std::string& category);
    std::map<std::string, std::vector<std::string>> getConfigCategories();

    // Configuration profiles (enhanced)
    bool createProfile(const std::string& name, const CliOptions& options, const std::string& description = "");
    bool updateProfile(const std::string& name, const CliOptions& options);
    bool deleteProfile(const std::string& name);
    std::optional<CliOptions> loadProfile(const std::string& name);
    std::vector<std::string> listProfiles();
    bool setActiveProfile(const std::string& name);
    std::optional<std::string> getActiveProfile();

    // Configuration analysis and optimization
    std::map<std::string, std::string> analyzeConfiguration();
    std::vector<std::string> getOptimizationSuggestions();
    std::vector<std::string> findUnusedKeys();
    std::vector<std::string> findDuplicateValues();

    // Configuration security
    bool encryptSensitiveValues(bool enable = true);
    bool isValueEncrypted(const std::string& key);
    void markAsSecret(const std::string& key, bool isSecret = true);
    std::vector<std::string> getSecretKeys();

private:
    EnhancedConfigManager() = default;

    // Internal configuration storage
    std::map<ConfigScope, nlohmann::json> configurations_;
    std::map<std::string, EnhancedConfigEntry> configEntries_;
    std::map<std::string, ConfigTemplate> templates_;
    std::map<std::string, std::string> environmentOverrides_;
    std::vector<ConfigChangeEvent> changeHistory_;
    std::map<std::string, std::vector<std::function<void(const ConfigChangeEvent&)>>> changeListeners_;

    // Configuration metadata
    std::string activeProfile_;
    bool encryptionEnabled_ = false;
    std::string encryptionKey_;

    // Internal helper methods
    void initializeDefaultConfiguration();
    void initializeBuiltInTemplates();
    void registerConfigEntry(const EnhancedConfigEntry& entry);
    void notifyConfigChange(const std::string& key, const nlohmann::json& oldValue,
                           const nlohmann::json& newValue, ConfigSource source);

    // File operations
    bool loadConfigurationFile(const std::filesystem::path& filePath, ConfigScope scope);
    bool saveConfigurationFile(const std::filesystem::path& filePath, ConfigScope scope);

    // Validation helpers
    bool validateConfigEntry(const EnhancedConfigEntry& entry);
    std::string getValidationError(const std::string& key, const nlohmann::json& value);

    // Encryption helpers
    std::string encryptValue(const std::string& value);
    std::string decryptValue(const std::string& encryptedValue);

    // Template helpers
    void loadBuiltInTemplates();
    void loadUserTemplates();

    // Migration helpers
    bool migrateFromVersion(int version);
    void updateSchemaVersion(int version);

    // Utility methods
    std::string getCurrentTimestamp();
    std::string generateBackupName();
    bool isValidKey(const std::string& key);
    std::string normalizeKey(const std::string& key);
};

// Configuration utility functions
namespace config_utils {
    // Key validation and normalization
    bool isValidConfigKey(const std::string& key);
    std::string normalizeConfigKey(const std::string& key);
    std::vector<std::string> parseKeyPath(const std::string& key);
    std::string joinKeyPath(const std::vector<std::string>& path);

    // Value conversion utilities
    template<typename T>
    std::optional<T> convertValue(const nlohmann::json& value);

    nlohmann::json convertFromCliOptions(const CliOptions& options);
    CliOptions convertToCliOptions(const nlohmann::json& json);

    // Configuration file utilities
    bool isValidConfigFile(const std::filesystem::path& filePath);
    std::string detectConfigFormat(const std::filesystem::path& filePath);
    bool convertConfigFormat(const std::filesystem::path& inputPath,
                            const std::filesystem::path& outputPath,
                            const std::string& targetFormat);

    // Configuration comparison
    std::map<std::string, std::pair<nlohmann::json, nlohmann::json>>
    compareConfigurations(const nlohmann::json& config1, const nlohmann::json& config2);

    std::vector<std::string> findConfigDifferences(const nlohmann::json& config1,
                                                   const nlohmann::json& config2);

    // Configuration merging
    nlohmann::json mergeConfigurations(const std::vector<nlohmann::json>& configs);
    nlohmann::json mergeWithPriority(const nlohmann::json& base,
                                     const nlohmann::json& override);

    // Display utilities
    void printConfiguration(const nlohmann::json& config, int indent = 0);
    void printConfigEntry(const EnhancedConfigEntry& entry);
    void printConfigTemplate(const ConfigTemplate& template_);
    std::string formatConfigValue(const nlohmann::json& value);

    // Interactive utilities
    nlohmann::json promptForValue(const std::string& key, const EnhancedConfigEntry& entry);
    bool confirmConfigChange(const std::string& key, const nlohmann::json& oldValue,
                            const nlohmann::json& newValue);
}

} // namespace config

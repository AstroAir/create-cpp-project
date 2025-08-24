#pragma once

#include "config_manager.h"
#include "../cli/cli_parser.h"
#include <string>
#include <vector>
#include <map>
#include <optional>

namespace config {

// User preference categories
enum class PreferenceCategory {
    General,
    Templates,
    BuildSystems,
    Editors,
    CodeStyle,
    Testing,
    Documentation,
    CI_CD,
    Advanced
};

// Individual preference definition
struct PreferenceDefinition {
    std::string key;
    std::string displayName;
    std::string description;
    ConfigValueType type;
    nlohmann::json defaultValue;
    std::vector<std::string> allowedValues;
    PreferenceCategory category;
    bool requiresRestart = false;
    std::string environmentVariable; // Optional environment variable override
};

// User preferences manager
class UserPreferences {
public:
    // Singleton access
    static UserPreferences& getInstance();
    
    // Preference management
    bool loadPreferences();
    bool savePreferences();
    bool resetToDefaults();
    
    // Preference access
    template<typename T>
    T getPreference(const std::string& key, const T& defaultValue = T{});
    
    template<typename T>
    bool setPreference(const std::string& key, const T& value);
    
    bool hasPreference(const std::string& key);
    bool removePreference(const std::string& key);
    
    // Preference queries
    std::vector<PreferenceDefinition> getPreferencesByCategory(PreferenceCategory category);
    std::vector<PreferenceDefinition> getAllPreferences();
    std::optional<PreferenceDefinition> getPreferenceDefinition(const std::string& key);
    
    // Environment variable integration
    void applyEnvironmentOverrides();
    std::map<std::string, std::string> getEnvironmentOverrides();
    
    // Validation
    bool validatePreference(const std::string& key, const nlohmann::json& value);
    std::vector<std::string> getValidationErrors();
    
    // Import/Export
    bool exportPreferences(const std::filesystem::path& filePath);
    bool importPreferences(const std::filesystem::path& filePath);
    
    // CLI Options integration
    CliOptions applyPreferencesToOptions(const CliOptions& baseOptions);
    void updatePreferencesFromOptions(const CliOptions& options);
    
    // Interactive preference editor
    bool runInteractiveEditor();
    
private:
    UserPreferences() = default;
    ~UserPreferences() = default;
    UserPreferences(const UserPreferences&) = delete;
    UserPreferences& operator=(const UserPreferences&) = delete;
    
    // Internal data
    nlohmann::json m_preferences;
    std::vector<PreferenceDefinition> m_preferenceDefinitions;
    std::vector<std::string> m_validationErrors;
    
    // Internal methods
    void initializePreferenceDefinitions();
    void registerPreference(const PreferenceDefinition& definition);
    
    // Helper methods
    nlohmann::json getPreferenceValue(const std::string& key);
    bool setPreferenceValue(const std::string& key, const nlohmann::json& value);
};

// Preference categories and their display information
struct PreferenceCategoryInfo {
    PreferenceCategory category;
    std::string displayName;
    std::string description;
    std::vector<std::string> preferenceKeys;
};

// Preference management utilities
namespace preferences {
    // Get all preference categories with their information
    std::vector<PreferenceCategoryInfo> getPreferenceCategories();
    
    // Get display-friendly preference information
    std::string getPreferenceDisplayName(const std::string& key);
    std::string getPreferenceDescription(const std::string& key);
    std::vector<std::string> getPreferenceAllowedValues(const std::string& key);
    
    // Preference validation helpers
    bool isValidPreferenceKey(const std::string& key);
    bool isValidPreferenceValue(const std::string& key, const nlohmann::json& value);
    
    // Default preference values
    nlohmann::json getDefaultPreferences();
    
    // Environment variable helpers
    std::string getEnvironmentVariableForPreference(const std::string& key);
    bool hasEnvironmentOverride(const std::string& key);
    
    // Interactive helpers
    void showPreferenceHelp(const std::string& key);
    void showAllPreferences();
    void showPreferencesByCategory(PreferenceCategory category);
    
    // Preference migration
    bool migratePreferences(const nlohmann::json& oldPreferences, nlohmann::json& newPreferences);
}

// Template specializations for common types
template<>
std::string UserPreferences::getPreference<std::string>(const std::string& key, const std::string& defaultValue);

template<>
bool UserPreferences::getPreference<bool>(const std::string& key, const bool& defaultValue);

template<>
int UserPreferences::getPreference<int>(const std::string& key, const int& defaultValue);

template<>
bool UserPreferences::setPreference<std::string>(const std::string& key, const std::string& value);

template<>
bool UserPreferences::setPreference<bool>(const std::string& key, const bool& value);

template<>
bool UserPreferences::setPreference<int>(const std::string& key, const int& value);

} // namespace config

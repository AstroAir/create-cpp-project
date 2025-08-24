# Configuration System Component

The Configuration System component manages persistent settings, user preferences, project profiles, and environment integration for CPP-Scaffold.

## 📋 Overview

The Configuration System component provides:
- Persistent configuration storage and retrieval
- User preference management with hierarchical settings
- Project profile creation, storage, and management
- Environment variable integration and overrides
- Configuration validation and migration
- Multi-format configuration file support (JSON, YAML, TOML)

## 🏗️ Architecture

### Component Structure

```
src/config/
├── config_manager.h/.cpp         # Main configuration manager
├── user_preferences.h/.cpp       # User-specific settings
├── ci_config.h/.cpp              # CI/CD configuration
├── editor_config.h/.cpp          # IDE/editor settings
├── code_style_config.h/.cpp      # Code formatting settings
├── doc_config.h/.cpp             # Documentation configuration
└── interactive_config.h/.cpp     # Interactive configuration UI
```

### Class Hierarchy

```cpp
namespace config {
    class ConfigManager;           // Singleton configuration manager
    class UserPreferences;         # User preference handling
    class CiConfig;               # CI/CD system configuration
    class EditorConfig;           # Editor/IDE configuration
    class CodeStyleConfig;        # Code style and formatting
    class DocConfig;              # Documentation settings
    class InteractiveConfig;      # Interactive configuration UI
}
```

## 🔧 Core Classes

### ConfigManager

Main configuration manager implementing the Singleton pattern.

```cpp
class ConfigManager {
public:
    // Singleton access
    static ConfigManager& getInstance();
    
    // Configuration file management
    bool loadConfiguration();
    bool saveConfiguration();
    bool resetToDefaults();
    bool validateConfiguration();
    
    // Profile management
    bool saveProfile(const std::string& profileName, const CliOptions& options);
    std::optional<CliOptions> loadProfile(const std::string& profileName);
    std::vector<std::string> listProfiles();
    bool deleteProfile(const std::string& profileName);
    bool exportProfile(const std::string& profileName, const std::filesystem::path& exportPath);
    bool importProfile(const std::filesystem::path& importPath);
    
    // User preferences
    template<typename T>
    bool setUserPreference(const std::string& key, const T& value);
    
    template<typename T>
    std::optional<T> getUserPreference(const std::string& key);
    
    bool removeUserPreference(const std::string& key);
    std::vector<std::string> getUserPreferenceKeys();
    
    // Environment integration
    void loadEnvironmentVariables();
    bool setEnvironmentOverride(const std::string& key, const std::string& value);
    std::optional<std::string> getEnvironmentOverride(const std::string& key);
    
    // Configuration paths
    std::filesystem::path getConfigDirectory();
    std::filesystem::path getConfigFilePath();
    std::filesystem::path getProfilesDirectory();
    std::filesystem::path getCustomTemplatesDirectory();
    std::filesystem::path getCacheDirectory();
    std::filesystem::path getLogsDirectory();
    
    // Configuration queries
    template<typename T>
    std::optional<T> getValue(const std::string& key, ConfigScope scope = ConfigScope::Global);
    
    template<typename T>
    bool setValue(const std::string& key, const T& value, ConfigScope scope = ConfigScope::User);
    
    bool hasKey(const std::string& key, ConfigScope scope = ConfigScope::Any);
    std::vector<std::string> getKeys(ConfigScope scope = ConfigScope::All);
    
    // Configuration migration
    bool migrateConfiguration(int fromVersion, int toVersion);
    int getCurrentConfigVersion();
    bool isConfigurationOutdated();

private:
    ConfigManager() = default;
    
    // Configuration data
    nlohmann::json globalConfig_;
    nlohmann::json userConfig_;
    std::map<std::string, nlohmann::json> profiles_;
    std::map<std::string, std::string> environmentOverrides_;
    
    // Internal methods
    bool ensureConfigDirectoryExists();
    bool loadGlobalConfiguration();
    bool loadUserConfiguration();
    bool loadProfiles();
    bool saveGlobalConfiguration();
    bool saveUserConfiguration();
    bool saveProfiles();
    void initializeDefaultConfiguration();
    bool validateConfigurationStructure(const nlohmann::json& config);
    std::string getCurrentTimestamp();
    
    // Migration helpers
    bool migrateFromVersion1To2();
    bool migrateFromVersion2To3();
};
```

### Configuration Enums and Structures

```cpp
enum class ConfigScope {
    Global,      // System-wide configuration
    User,        // User-specific configuration
    Project,     // Project-specific configuration
    Environment, // Environment variables
    Any,         // Search all scopes
    All          // Return from all scopes
};

enum class ConfigFormat {
    JSON,        // JSON format (default)
    YAML,        // YAML format
    TOML         // TOML format
};

struct ConfigEntry {
    std::string key;
    nlohmann::json value;
    ConfigScope scope;
    std::string description;
    std::optional<nlohmann::json> defaultValue;
    std::vector<std::string> allowedValues;
    bool isRequired = false;
    bool isEnvironmentVariable = false;
    std::string environmentKey;
    std::function<bool(const nlohmann::json&)> validator;
};

struct ConfigProfile {
    std::string name;
    std::string description;
    std::map<std::string, nlohmann::json> settings;
    std::string createdAt;
    std::string lastModified;
    std::string author;
    int schemaVersion = 1;
    std::vector<std::string> tags;
    bool isBuiltIn = false;
};
```

### UserPreferences

Manages user-specific preferences and settings.

```cpp
class UserPreferences {
public:
    UserPreferences();
    
    // Preference management
    bool loadPreferences();
    bool savePreferences();
    bool resetToDefaults();
    
    // Default values
    TemplateType getDefaultTemplateType() const;
    BuildSystem getDefaultBuildSystem() const;
    PackageManager getDefaultPackageManager() const;
    TestFramework getDefaultTestFramework() const;
    Language getDefaultLanguage() const;
    
    void setDefaultTemplateType(TemplateType type);
    void setDefaultBuildSystem(BuildSystem system);
    void setDefaultPackageManager(PackageManager manager);
    void setDefaultTestFramework(TestFramework framework);
    void setDefaultLanguage(Language language);
    
    // UI preferences
    bool getVerboseOutput() const;
    bool getColorOutput() const;
    bool getInteractiveMode() const;
    bool getAutoSaveProfiles() const;
    
    void setVerboseOutput(bool verbose);
    void setColorOutput(bool color);
    void setInteractiveMode(bool interactive);
    void setAutoSaveProfiles(bool autoSave);
    
    // Development preferences
    std::vector<EditorConfig> getPreferredEditors() const;
    std::vector<CiSystem> getPreferredCiSystems() const;
    std::string getDefaultAuthor() const;
    std::string getDefaultEmail() const;
    std::string getDefaultLicense() const;
    
    void setPreferredEditors(const std::vector<EditorConfig>& editors);
    void setPreferredCiSystems(const std::vector<CiSystem>& systems);
    void setDefaultAuthor(const std::string& author);
    void setDefaultEmail(const std::string& email);
    void setDefaultLicense(const std::string& license);

private:
    nlohmann::json preferences_;
    std::filesystem::path preferencesPath_;
    
    void initializeDefaults();
    bool validatePreferences();
};
```

## 🔄 Configuration Hierarchy

### Configuration Resolution Order

```
1. Command Line Arguments (highest priority)
   ↓
2. Environment Variables
   ↓
3. Project-Specific Configuration
   ↓
4. User Profile Settings
   ↓
5. User Preferences
   ↓
6. Global Configuration
   ↓
7. Built-in Defaults (lowest priority)
```

### Configuration File Locations

#### Windows
```
Global Config:    %PROGRAMDATA%\CPP-Scaffold\config.json
User Config:      %APPDATA%\CPP-Scaffold\config.json
User Preferences: %APPDATA%\CPP-Scaffold\preferences.json
Profiles:         %APPDATA%\CPP-Scaffold\profiles\
Cache:            %LOCALAPPDATA%\CPP-Scaffold\cache\
Logs:             %LOCALAPPDATA%\CPP-Scaffold\logs\
```

#### Linux/macOS
```
Global Config:    /etc/cpp-scaffold/config.json
User Config:      ~/.config/cpp-scaffold/config.json
User Preferences: ~/.config/cpp-scaffold/preferences.json
Profiles:         ~/.config/cpp-scaffold/profiles/
Cache:            ~/.cache/cpp-scaffold/
Logs:             ~/.local/share/cpp-scaffold/logs/
```

## 📝 Configuration Schema

### Global Configuration

```json
{
  "version": 1,
  "defaults": {
    "templateType": "console",
    "buildSystem": "cmake",
    "packageManager": "vcpkg",
    "testFramework": "none",
    "language": "english"
  },
  "paths": {
    "templatesDirectory": "./templates",
    "cacheDirectory": "./cache",
    "logsDirectory": "./logs"
  },
  "features": {
    "autoUpdate": true,
    "telemetry": false,
    "experimentalFeatures": false
  },
  "logging": {
    "level": "info",
    "fileLogging": true,
    "maxLogFiles": 10,
    "maxLogSizeMB": 10
  }
}
```

### User Preferences

```json
{
  "version": 1,
  "defaults": {
    "templateType": "console",
    "buildSystem": "cmake",
    "packageManager": "vcpkg",
    "testFramework": "gtest",
    "language": "english",
    "author": "John Doe",
    "email": "john.doe@example.com",
    "license": "MIT"
  },
  "ui": {
    "verboseOutput": false,
    "colorOutput": true,
    "interactiveMode": true,
    "autoSaveProfiles": true
  },
  "development": {
    "preferredEditors": ["vscode", "clion"],
    "preferredCiSystems": ["github", "gitlab"],
    "codeStyle": "google",
    "documentationFormat": "markdown"
  }
}
```

### Profile Configuration

```json
{
  "name": "web-development",
  "description": "Configuration for web service development",
  "author": "John Doe",
  "createdAt": "2023-07-25T10:30:00Z",
  "lastModified": "2023-07-25T15:45:00Z",
  "schemaVersion": 1,
  "tags": ["web", "service", "api"],
  "isBuiltIn": false,
  "settings": {
    "templateType": "webservice",
    "buildSystem": "cmake",
    "packageManager": "conan",
    "testFramework": "gtest",
    "includeTests": true,
    "includeDocumentation": true,
    "ciOptions": ["github", "gitlab"],
    "editorOptions": ["vscode", "clion"]
  }
}
```

## 💡 Usage Examples

### Basic Configuration Management

```cpp
#include "config/config_manager.h"

int main() {
    auto& config = ConfigManager::getInstance();
    
    // Load configuration
    if (!config.loadConfiguration()) {
        std::cerr << "Failed to load configuration" << std::endl;
        return 1;
    }
    
    // Get user preference
    auto defaultTemplate = config.getUserPreference<std::string>("defaults.templateType");
    if (defaultTemplate.has_value()) {
        std::cout << "Default template: " << defaultTemplate.value() << std::endl;
    }
    
    // Set user preference
    config.setUserPreference("defaults.buildSystem", "cmake");
    
    // Save configuration
    config.saveConfiguration();
    
    return 0;
}
```

### Profile Management

```cpp
#include "config/config_manager.h"

void manageProfiles() {
    auto& config = ConfigManager::getInstance();
    
    // Create a new profile
    CliOptions webDevOptions;
    webDevOptions.templateType = TemplateType::WebService;
    webDevOptions.buildSystem = BuildSystem::CMake;
    webDevOptions.packageManager = PackageManager::Conan;
    webDevOptions.testFramework = TestFramework::GTest;
    webDevOptions.includeTests = true;
    webDevOptions.ciOptions = {CiSystem::GitHub, CiSystem::GitLab};
    
    // Save profile
    if (config.saveProfile("web-dev", webDevOptions)) {
        std::cout << "Profile 'web-dev' saved successfully" << std::endl;
    }
    
    // List all profiles
    auto profiles = config.listProfiles();
    std::cout << "Available profiles:" << std::endl;
    for (const auto& profile : profiles) {
        std::cout << "  - " << profile << std::endl;
    }
    
    // Load profile
    auto loadedOptions = config.loadProfile("web-dev");
    if (loadedOptions.has_value()) {
        std::cout << "Profile loaded successfully" << std::endl;
        // Use loaded options...
    }
}
```

### Environment Integration

```cpp
#include "config/config_manager.h"

void setupEnvironmentIntegration() {
    auto& config = ConfigManager::getInstance();
    
    // Load environment variables
    config.loadEnvironmentVariables();
    
    // Check for environment overrides
    auto envTemplate = config.getEnvironmentOverride("CPP_SCAFFOLD_TEMPLATE");
    if (envTemplate.has_value()) {
        std::cout << "Using template from environment: " << envTemplate.value() << std::endl;
    }
    
    // Set environment override
    config.setEnvironmentOverride("CPP_SCAFFOLD_BUILD_SYSTEM", "ninja");
}
```

## 🧪 Testing

### Configuration Testing

```cpp
// Test configuration loading
TEST(ConfigManagerTest, LoadConfiguration) {
    auto& config = ConfigManager::getInstance();
    EXPECT_TRUE(config.loadConfiguration());
}

// Test profile management
TEST(ConfigManagerTest, ProfileManagement) {
    auto& config = ConfigManager::getInstance();
    
    CliOptions testOptions;
    testOptions.projectName = "TestProject";
    testOptions.templateType = TemplateType::Console;
    
    // Save profile
    EXPECT_TRUE(config.saveProfile("test-profile", testOptions));
    
    // Load profile
    auto loaded = config.loadProfile("test-profile");
    ASSERT_TRUE(loaded.has_value());
    EXPECT_EQ(loaded->templateType, TemplateType::Console);
    
    // Delete profile
    EXPECT_TRUE(config.deleteProfile("test-profile"));
}

// Test user preferences
TEST(UserPreferencesTest, DefaultValues) {
    UserPreferences prefs;
    prefs.loadPreferences();
    
    EXPECT_EQ(prefs.getDefaultTemplateType(), TemplateType::Console);
    EXPECT_EQ(prefs.getDefaultBuildSystem(), BuildSystem::CMake);
    
    prefs.setDefaultTemplateType(TemplateType::Library);
    EXPECT_EQ(prefs.getDefaultTemplateType(), TemplateType::Library);
}
```

## 🔗 Dependencies

### External Dependencies
- **nlohmann/json**: JSON parsing and generation
- **std::filesystem**: File system operations
- **spdlog**: Logging and error reporting

### Internal Dependencies
- **Utility Classes**: File operations, string processing
- **CLI Parser**: Option validation and conversion

## 📊 Performance Considerations

- **Configuration loading**: Lazy loading with caching
- **Profile management**: Efficient JSON serialization
- **Memory usage**: Minimal, only active configuration in memory
- **File I/O**: Atomic writes to prevent corruption

## 🔧 Extension Points

### Adding New Configuration Options

1. **Define configuration entry** in schema
2. **Add getter/setter methods** in appropriate class
3. **Update validation logic** for new option
4. **Add migration code** if needed
5. **Update documentation** and tests

### Custom Configuration Formats

```cpp
// Register custom format handler
config.registerFormatHandler(ConfigFormat::YAML, 
    std::make_unique<YamlConfigHandler>());
```

---

For more information, see the [API Reference](../api-reference/API.md) and [Architecture Documentation](../developer-guide/ARCHITECTURE.md).

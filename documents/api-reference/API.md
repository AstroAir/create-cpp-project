# CPP-Scaffold API Reference Documentation

A comprehensive API reference for the CPP-Scaffold C++ project scaffolding tool.

## 📋 Table of Contents

1. [Overview](#overview)
2. [Installation](#installation)
3. [Quick Start](#quick-start)
4. [Core API](#core-api)
5. [Template System](#template-system)
6. [Configuration System](#configuration-system)
7. [Utility Classes](#utility-classes)
8. [Testing Framework](#testing-framework)
9. [Documentation System](#documentation-system)
10. [Enums and Type Definitions](#enums-and-type-definitions)
11. [Examples](#examples)
12. [Error Handling](#error-handling)

## 🔍 Overview

CPP-Scaffold is a powerful C++ project scaffolding tool that helps developers quickly create well-structured C++ projects with modern build systems, package managers, testing frameworks, and development tools.

### 🚀 Key Features

- **Multiple Project Templates**: Console apps, libraries, GUI applications, network services, embedded systems, web services
- **Build System Support**: CMake, Meson, Bazel, XMake, Premake, Make, Ninja
- **Package Managers**: vcpkg, Conan, Spack, Hunter with automatic dependency management
- **Testing Frameworks**: Google Test, Catch2, Doctest, Boost.Test with CI integration
- **IDE Integration**: VSCode, CLion, Visual Studio, Vim, Emacs with project-specific configurations
- **CI/CD Setup**: GitHub Actions, GitLab CI, Travis CI, AppVeyor, Azure DevOps, CircleCI
- **Multi-language Support**: English, Chinese, Spanish, Japanese, German, French
- **Interactive Wizard**: Step-by-step project configuration with styled terminal output

### 🏗️ Architecture Overview

```
CPP-Scaffold API Structure
├── Core API
│   ├── CliParser          # Command-line argument parsing
│   ├── TemplateManager    # Template factory and management
│   └── ConfigManager      # Configuration and profile management
├── Template System
│   ├── TemplateBase       # Abstract base for all templates
│   ├── ConsoleTemplate    # Console application template
│   ├── LibraryTemplate    # Library project template
│   ├── GuiTemplate        # GUI application template
│   └── CustomTemplate     # User-defined templates
├── Utility Classes
│   ├── FileUtils          # File system operations
│   ├── StringUtils        # String manipulation
│   ├── TerminalUtils      # Terminal styling and interaction
│   ├── GitUtils           # Git operations
│   ├── FrameworkIntegration # Third-party framework setup
│   ├── ProgressIndicator  # Progress visualization
│   └── InteractiveMenu    # Interactive user interfaces
├── Testing Framework
│   └── TestFrameworkManager # Testing framework integration
└── Documentation System
    └── DocGenerator       # Documentation generation
```

## 📦 Installation

### Prerequisites

- **C++17 compatible compiler**: GCC 7+, Clang 6+, MSVC 2017+
- **CMake**: 3.14 or later
- **Git**: For project initialization and dependency management
- **Ninja** (recommended): For faster builds

### Building from Source

```bash
# Clone the repository
git clone https://github.com/cpp-scaffold/cpp-scaffold.git
cd cpp-scaffold

# Build with CMake and Ninja
mkdir build && cd build
cmake .. -G Ninja -DCMAKE_BUILD_TYPE=Release
ninja

# Optional: Install system-wide
sudo ninja install  # Linux/macOS
```

### Dependencies

The project automatically manages its dependencies using CMake FetchContent:

- **spdlog v1.12.0** - High-performance logging library
- **nlohmann/json v3.11.2** - JSON parsing and generation
- **Google Test v1.14.0** - Testing framework (when BUILD_TESTING=ON)

## ⚡ Quick Start

### Command Line Usage

```bash
# Interactive mode (recommended for beginners)
./cpp-scaffold

# Quick project creation
./cpp-scaffold MyProject --type console --build cmake --package vcpkg

# Advanced options with full setup
./cpp-scaffold MyProject --type gui --build cmake --package conan --test gtest --ci github --editor vscode

# Using configuration profiles
./cpp-scaffold MyProject --profile webdev

# Custom template usage
./cpp-scaffold MyProject --custom-template ./my-templates/special-template
```

### Programmatic Usage

```cpp
#include "templates/template_manager.h"
#include "cli/cli_parser.h"
#include "config/config_manager.h"

int main() {
    try {
        // Create and configure options
        CliOptions options;
        options.projectName = "MyProject";
        options.templateType = TemplateType::Console;
        options.buildSystem = BuildSystem::CMake;
        options.packageManager = PackageManager::Vcpkg;
        options.includeTests = true;
        options.testFramework = TestFramework::GTest;
        options.editorOptions = {EditorConfig::VSCode};
        options.ciOptions = {CiSystem::GitHub};
        options.language = Language::English;

        // Validate options
        if (!CliParser::validateOptions(options)) {
            std::cerr << "Invalid options provided" << std::endl;
            return 1;
        }

        // Create project using template manager
        TemplateManager manager;
        bool success = manager.createProject(options);

        if (success) {
            std::cout << "Project created successfully!" << std::endl;

            // Optionally save configuration as profile
            ConfigManager::getInstance().saveProfile("my-profile", options);
        } else {
            std::cerr << "Failed to create project" << std::endl;
        }

        return success ? 0 : 1;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
```

### Interactive Configuration

```cpp
#include "cli/cli_parser.h"
#include "utils/interactive_menu.h"

int main() {
    // Run interactive wizard
    CliOptions options = CliParser::runInteractiveMode();

    // Or prompt for specific options
    CliOptions defaults;
    defaults.templateType = TemplateType::Console;
    defaults.buildSystem = BuildSystem::CMake;

    CliOptions userOptions = CliParser::promptUserForOptions(defaults);

    // Create project with user-selected options
    TemplateManager manager;
    return manager.createProject(userOptions) ? 0 : 1;
}
```

## 🔧 Core API

### CliOptions Structure

The central configuration structure that holds all project creation options.

```cpp
struct CliOptions {
    // Basic project information
    std::string projectName;
    TemplateType templateType = TemplateType::Console;
    BuildSystem buildSystem = BuildSystem::CMake;
    PackageManager packageManager = PackageManager::Vcpkg;

    // Framework and library options
    std::optional<std::string> networkLibrary;  // For network projects: asio, boost, poco

    // Testing configuration
    bool includeTests = false;
    TestFramework testFramework = TestFramework::GTest;

    // Documentation and code quality
    bool includeDocumentation = false;
    bool includeCodeStyleTools = false;

    // Development environment
    std::vector<EditorConfig> editorOptions;
    std::vector<CiSystem> ciOptions;

    // Version control
    bool initGit = true;

    // CLI behavior
    bool showHelp = false;
    bool version = false;
    bool verbose = false;

    // Internationalization
    Language language = Language::English;

    // Custom templates
    std::filesystem::path customTemplatePath;

    // Profile management
    std::optional<std::string> profileName;
    bool saveAsProfile = false;
    std::string saveProfileName;

    // Advanced options
    bool forceOverwrite = false;
    bool dryRun = false;
    std::map<std::string, std::string> templateVariables;
};
```

### TemplateBase

Abstract base class for all project templates implementing the Template Method pattern.

```cpp
class TemplateBase {
public:
    TemplateBase(const CliOptions& options);
    virtual ~TemplateBase() = default;

    // Main entry point for creating a project (Template Method)
    virtual bool create() = 0;

protected:
    CliOptions options_;

    // Pure virtual methods - must be implemented by derived classes
    virtual bool createProjectStructure() = 0;
    virtual bool createBuildSystem() = 0;
    virtual bool setupPackageManager() = 0;
    virtual bool setupTestFramework() = 0;

    // Common functionality provided by base class
    bool setupCICD(const std::string& projectPath);
    bool setupEditorConfig(const std::string& projectPath);
    bool setupCodeStyleConfig(const std::string& projectPath);
    bool setupDocConfig(const std::string& projectPath);
    bool initializeGit(const std::string& projectPath);

    // Content generators
    std::string getGitignoreContent();
    std::string getLicenseContent(const std::string& projectName);

    // File creation utilities
    bool createLicense(const std::string& projectPath);
    bool createClangFormat(const std::string& projectPath);
    bool createGitAttributes(const std::string& projectPath);

    // Post-creation actions
    void printUsageGuide();
    bool executePostCreationActions();
};
```

### TemplateManager

Factory class for creating and managing project templates using the Factory pattern.

```cpp
class TemplateManager {
public:
    TemplateManager();

    // Main project creation method
    bool createProject(const CliOptions& options);

private:
    // Template factory function type
    using TemplateFactory = std::function<std::unique_ptr<TemplateBase>(const CliOptions&)>;

    // Template factory registry
    std::map<std::string, TemplateFactory> templateFactories_;

    // Framework template creation
    static std::unique_ptr<TemplateBase> createFrameworkTemplate(
        const CliOptions& options,
        utils::FrameworkIntegration::Framework framework
    );
};
```

**Registered Templates:**
- `console` → `ConsoleTemplate`
- `lib` → `LibraryTemplate`
- `header-only-lib` → `HeaderOnlyLibTemplate`
- `gui` → `GuiTemplate`
- `network` → `NetworkTemplate`
- `webservice` → `WebServiceTemplate`
- `embedded` → `EmbeddedTemplate`
- `custom` → `CustomTemplate`

### CliParser

Handles command-line argument parsing, validation, and interactive configuration.

```cpp
class CliParser {
public:
    // Main parsing method
    [[nodiscard]] static CliOptions parse(int argc, char* argv[]);

    // Information display
    static void showHelp(Language lang = Language::English);
    static void showVersion();
    static void showAvailableTemplates();
    static void validateProject(const std::string& projectPath);

    // Interactive configuration
    [[nodiscard]] static CliOptions promptUserForOptions(
        const CliOptions& defaultOptions = {}
    );
    [[nodiscard]] static CliOptions runInteractiveMode();

    // Validation
    [[nodiscard]] static bool validateOptions(CliOptions& options);

private:
    // System detection and defaults
    static std::string detectOS();
    static CliOptions getSystemSuggestedDefaults();
    static void showCliHeader();
};
```

**Key Methods:**

- **`parse(argc, argv)`**: Parses command-line arguments and returns populated `CliOptions`
- **`showHelp(lang)`**: Displays localized help information
- **`showVersion()`**: Shows version and build information
- **`showAvailableTemplates()`**: Lists all available project templates
- **`promptUserForOptions(defaults)`**: Interactive prompt for configuration
- **`runInteractiveMode()`**: Full interactive wizard experience
- **`validateOptions(options)`**: Validates and sanitizes options

## 🏗️ Template System

### Available Templates

| Template | Description | Use Case | Key Features |
|----------|-------------|----------|--------------|
| `console` | Command-line application | CLI tools, utilities, system tools | Argument parsing, logging, cross-platform |
| `lib` | Static/shared library | Reusable components, SDKs | Export/import handling, pkg-config, versioning |
| `header-only-lib` | Header-only library | Template libraries, utilities | Single-header distribution, comprehensive docs |
| `gui` | GUI application | Desktop applications | Framework integration (Qt, SFML), resource management |
| `network` | Network service | Servers, clients, APIs | Async I/O, protocol handling, scalability |
| `embedded` | Embedded system | IoT, microcontrollers | Resource constraints, real-time, hardware abstraction |
| `webservice` | Web service | REST APIs, web backends | HTTP handling, JSON APIs, middleware |
| `custom` | User-defined template | Specialized projects | Template inheritance, variable substitution |

### Template Inheritance Hierarchy

```
TemplateBase (Abstract)
├── ConsoleTemplate
├── LibraryTemplate
├── HeaderOnlyLibTemplate
├── GuiTemplate
├── NetworkTemplate
├── WebServiceTemplate
├── EmbeddedTemplate
└── CustomTemplate
```

### Template Subclasses

#### ConsoleTemplate

Creates command-line applications with modern C++ structure and best practices.

```cpp
class ConsoleTemplate : public TemplateBase {
public:
    ConsoleTemplate(const CliOptions& options);
    bool create() override;

protected:
    bool createProjectStructure() override;
    bool createBuildSystem() override;
    bool setupPackageManager() override;
    bool setupTestFramework() override;

private:
    // Content generators
    std::string getMainCppContent();
    std::string getReadmeContent();
    std::string getArgumentParsingCode();
    std::string getLoggingSetupCode();

    // Build system generators
    std::string getCMakeContent();
    std::string getMesonContent();
    std::string getBazelContent();

    // Package manager generators
    std::string getVcpkgJsonContent();
    std::string getConanfileContent();

    // Test framework generators
    std::string getGTestContent();
    std::string getCatch2Content();
    std::string getDocTestContent();
};
```

**Generated Project Structure:**
```
MyConsoleApp/
├── src/
│   ├── main.cpp                 # Entry point with argument parsing
│   ├── MyConsoleApp/
│   │   ├── MyConsoleApp.h       # Main application class
│   │   ├── MyConsoleApp.cpp     # Implementation
│   │   ├── cli/
│   │   │   ├── ArgumentParser.h # Command-line argument handling
│   │   │   └── ArgumentParser.cpp
│   │   └── utils/
│   │       ├── Logger.h         # Logging utilities
│   │       └── Logger.cpp
├── tests/
│   ├── test_main.cpp           # Test runner
│   ├── test_MyConsoleApp.cpp   # Unit tests
│   └── test_ArgumentParser.cpp # CLI tests
├── CMakeLists.txt              # Build configuration
├── README.md                   # Project documentation
├── .gitignore                  # Git ignore rules
└── LICENSE                     # License file
```

#### LibraryTemplate

Creates static or shared libraries with proper export/import handling, versioning, and packaging support.

```cpp
class LibraryTemplate : public TemplateBase {
public:
    LibraryTemplate(const CliOptions& options);
    bool create() override;

protected:
    bool createProjectStructure() override;
    bool createBuildSystem() override;
    bool setupPackageManager() override;
    bool setupTestFramework() override;

private:
    // Content generators
    std::string getLibraryHeaderContent();
    std::string getLibrarySourceContent();
    std::string getExportHeaderContent();
    std::string getVersionHeader();

    // Packaging and distribution
    std::string getPkgConfigContent();
    std::string getCMakeConfigContent();
    std::string getConanPackageContent();

    // Documentation
    std::string getAPIDocumentation();
    std::string getUsageExamples();

    // Build configuration
    bool setupSharedLibraryConfig();
    bool setupStaticLibraryConfig();
    bool setupInstallTargets();
};
```

**Generated Project Structure:**

```
MyLibrary/
├── include/
│   └── MyLibrary/
│       ├── MyLibrary.h          # Main public header
│       ├── Version.h            # Version information
│       └── Export.h             # Export/import macros
├── src/
│   ├── MyLibrary.cpp           # Implementation
│   └── internal/
│       └── PrivateImpl.cpp     # Private implementation
├── tests/
│   ├── test_main.cpp           # Test runner
│   └── test_MyLibrary.cpp      # Unit tests
├── examples/
│   ├── basic_usage.cpp         # Basic usage example
│   └── advanced_usage.cpp      # Advanced usage example
├── docs/
│   ├── API.md                  # API documentation
│   └── USAGE.md                # Usage guide
├── cmake/
│   ├── MyLibraryConfig.cmake.in # CMake package config
│   └── MyLibraryConfigVersion.cmake.in
├── CMakeLists.txt              # Build configuration
├── MyLibrary.pc.in             # pkg-config template
└── conanfile.py                # Conan package recipe
```

#### HeaderOnlyLibTemplate

Creates header-only libraries with comprehensive documentation and testing.

```cpp
class HeaderOnlyLibTemplate : public TemplateBase {
public:
    HeaderOnlyLibTemplate(const CliOptions& options);
    bool create() override;

protected:
    bool createProjectStructure() override;
    bool createBuildSystem() override;
    bool setupPackageManager() override;
    bool setupTestFramework() override;
    bool setupDocumentation();
    bool setupContinuousIntegration();
    bool setupCodeFormatting();
    bool setupBenchmarking();
    bool setupVersionControl();

private:
    std::string getHeaderContent();
    std::string getExampleContent();
    std::string getBenchmarkContent();
};
```

#### GuiTemplate

Creates GUI applications with framework support (Qt, SFML, etc.).

```cpp
class GuiTemplate : public TemplateBase {
public:
    GuiTemplate(const CliOptions& options);
    bool create() override;

protected:
    bool createProjectStructure() override;
    bool createBuildSystem() override;
    bool setupPackageManager() override;
    bool setupTestFramework() override;

private:
    std::string getMainWindowContent();
    std::string getApplicationContent();
    std::string getResourceContent();
    std::string getUIContent();
};
```

#### NetworkTemplate

Creates network services and clients with modern networking libraries.

```cpp
class NetworkTemplate : public TemplateBase {
public:
    NetworkTemplate(const CliOptions& options);
    bool create() override;

protected:
    bool createProjectStructure() override;
    bool createBuildSystem() override;
    bool setupPackageManager() override;
    bool setupTestFramework() override;

private:
    std::string getServerContent();
    std::string getClientContent();
    std::string getProtocolContent();
    std::string getHandlerContent();
};
```

## ⚙️ Configuration System

### ConfigManager

Manages persistent configuration, user preferences, and project profiles with hierarchical settings.

```cpp
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

    // User preferences
    bool setUserPreference(const std::string& key, const nlohmann::json& value);
    std::optional<nlohmann::json> getUserPreference(const std::string& key);
    bool removeUserPreference(const std::string& key);

    // Environment integration
    void loadEnvironmentVariables();
    bool setEnvironmentOverride(const std::string& key, const std::string& value);

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

private:
    ConfigManager() = default;
    nlohmann::json m_config;
    std::map<std::string, ConfigProfile> m_profiles;
    std::map<std::string, std::string> m_customTemplates;

    bool ensureConfigDirectoryExists();
    bool loadProfiles();
    bool saveProfiles();
    bool loadCustomTemplates();
    bool saveCustomTemplates();
    void initializeDefaultConfiguration();
    std::string getCurrentTimestamp();
};
```

### Configuration Structure

```cpp
enum class ConfigCategory {
    Global,      // Global application settings
    User,        // User-specific preferences
    Project,     // Project-specific settings
    Template,    // Template-specific configuration
    Environment  // Environment variables
};

enum class ConfigValueType {
    String,
    Integer,
    Boolean,
    Array,
    Object
};

struct ConfigEntry {
    std::string key;
    nlohmann::json value;
    ConfigValueType type;
    std::string description;
    std::optional<nlohmann::json> defaultValue;
    std::vector<std::string> allowedValues;
    bool isRequired = false;
    bool isEnvironmentVariable = false;
    std::string environmentKey;
};

struct ConfigProfile {
    std::string name;
    std::string description;
    std::map<std::string, nlohmann::json> settings;
    std::string createdAt;
    std::string lastModified;
    int schemaVersion = 1;
};
```
    static bool saveProfile(std::string_view profileName, const CliOptions& options);
    static std::optional<CliOptions> loadProfile(std::string_view profileName);
    static std::vector<std::string> listProfiles();

private:
    static constexpr std::string_view DEFAULT_PROFILE = "default";
};
```

### ProjectWizard

Interactive project configuration wizard.

```cpp
class ProjectWizard {
public:
    // Main wizard entry points
    static CliOptions runWizard(const CliOptions& initialOptions = {});
    static CliOptions runQuickStartWizard();
    static CliOptions runAdvancedWizard(const CliOptions& baseOptions);
    static CliOptions runTemplateSelectionWizard();

    // Configuration steps
    static bool configureProjectDetails(CliOptions& options);
    static bool configureBuildSystem(CliOptions& options);
    static bool configureTestFramework(CliOptions& options);
    static bool configureDevTools(CliOptions& options);
    static bool configureCiCd(CliOptions& options);
    static bool showSummaryAndConfirm(const CliOptions& options);

private:
    static void showWelcomeScreen();
    static void showEnhancedWelcomeScreen();
    static void showWizardProgress(int currentStep, int totalSteps, const std::string& stepName);
    static CliOptions getRecommendedOptionsForTemplate(TemplateType type);
    static bool saveAsTemplate(const CliOptions& options, const std::string& templateName);
};
```

### UserInput

Type-safe user input handling with validation.

```cpp
class UserInput {
public:
    using ValidatorResult = std::variant<bool, std::string>;
    using ValidatorFunction = std::function<ValidatorResult(const std::string&)>;

    // Input methods
    static std::string read(
        std::string_view prompt,
        std::optional<std::string_view> defaultValue = std::nullopt,
        std::optional<ValidatorFunction> validator = std::nullopt
    );

    static bool readConfirmation(std::string_view prompt, bool defaultValue = true);
    static int readNumber(std::string_view prompt, int min, int max,
                         std::optional<int> defaultValue = std::nullopt);
    static std::string readChoice(
        std::string_view prompt,
        const std::vector<std::string_view>& choices,
        std::optional<std::string_view> defaultValue = std::nullopt
    );

    // Validators
    static ValidatorFunction notEmptyValidator();
    static ValidatorFunction numericValidator();
    static ValidatorFunction rangeValidator(int min, int max);
    static ValidatorFunction oneOfValidator(const std::vector<std::string_view>& validValues);

    // Styled input methods
    static std::string readWithHighlight(
        std::string_view prompt,
        std::optional<std::string_view> defaultValue = std::nullopt,
        utils::Color promptColor = utils::Color::BrightCyan
    );
};
```

### Configuration Classes

#### EditorConfig

```cpp
class EditorConfig {
public:
    static bool generateVSCodeConfig(const std::string& projectPath, const CliOptions& options);
    static bool generateCLionConfig(const std::string& projectPath, const CliOptions& options);
    static bool generateVisualStudioConfig(const std::string& projectPath, const CliOptions& options);
    static bool generateVimConfig(const std::string& projectPath, const CliOptions& options);
    static bool generateEmacsConfig(const std::string& projectPath, const CliOptions& options);
};
```

#### CiConfig

```cpp
class CiConfig {
public:
    static bool generateGitHubActions(const std::string& projectPath, const CliOptions& options);
    static bool generateGitLabCI(const std::string& projectPath, const CliOptions& options);
    static bool generateTravisCI(const std::string& projectPath, const CliOptions& options);
    static bool generateAppVeyorCI(const std::string& projectPath, const CliOptions& options);
    static bool generateAzureDevOps(const std::string& projectPath, const CliOptions& options);
};
```

#### CodeStyleConfig

```cpp
class CodeStyleConfig {
public:
    static bool generateClangFormat(const std::string& projectPath, const CliOptions& options);
    static bool generateClangTidy(const std::string& projectPath, const CliOptions& options);
    static bool generateEditorConfig(const std::string& projectPath, const CliOptions& options);
    static bool generateCppCheck(const std::string& projectPath, const CliOptions& options);
};
```

## 🛠️ Utility Classes

The utility classes provide cross-platform functionality for file operations, terminal interaction, Git integration, and framework setup.

### FileUtils

Cross-platform file system operations with error handling and logging.

```cpp
namespace utils {
class FileUtils {
public:
    // Directory operations
    static bool createDirectory(const std::string& path);
    static bool createDirectoryStructure(const std::vector<std::string>& paths,
                                       const std::string& basePath);
    static bool directoryExists(const std::string& path);
    static std::vector<std::string> listFiles(const std::string& directory);
    static std::vector<std::string> listDirectories(const std::string& directory);

    // File operations
    static bool writeToFile(const std::string& path, const std::string& content);
    static std::string readFromFile(const std::string& path);
    static bool fileExists(const std::string& path);
    static bool copyFile(const std::string& source, const std::string& destination);
    static bool copyDirectory(const std::string& source, const std::string& destination);
    static bool deleteFile(const std::string& path);
    static bool deleteDirectory(const std::string& path);

    // Path utilities
    static std::string getFileName(const std::string& path);
    static std::string getDirectoryName(const std::string& path);
    static std::string getFileExtension(const std::string& path);
    static std::string combinePath(const std::string& path1, const std::string& path2);
    static std::string getAbsolutePath(const std::string& path);
    static std::string getRelativePath(const std::string& from, const std::string& to);

    // Permissions and attributes
    static bool setExecutable(const std::string& filePath);
    static bool isExecutable(const std::string& filePath);
    static bool isReadable(const std::string& filePath);
    static bool isWritable(const std::string& filePath);

    // System utilities
    static bool commandExists(const std::string& command);
    static std::string getTempDirectory();
    static std::string getCurrentDirectory();
    static bool setCurrentDirectory(const std::string& path);
};
    static bool directoryExists(const std::string& path);
    static bool copyDirectory(const std::string& source, const std::string& destination);

    // File operations
    static bool writeToFile(const std::string& path, const std::string& content);
    static std::string readFromFile(const std::string& path);
    static bool copyFile(const std::string& source, const std::string& destination);
    static bool fileExists(const std::string& path);
    static bool setExecutable(const std::string& filePath);

    // Path utilities
    static std::string getFileName(const std::string& path);
    static std::string getDirectoryName(const std::string& path);
    static std::string getFileExtension(const std::string& path);
    static std::string combinePath(const std::string& path1, const std::string& path2);

    // File listing
    static std::vector<std::string> listFiles(const std::string& directory);

    // Command utilities
    static bool commandExists(const std::string& command);
};
} // namespace utils
```

### StringUtils

String manipulation and text processing utilities.

```cpp
namespace utils {
class StringUtils {
public:
    // Case conversion
    static std::string toLower(const std::string& str);
    static std::string toUpper(const std::string& str);
    static std::string toTitleCase(const std::string& str);
    static std::string toCamelCase(const std::string& str);
    static std::string toSnakeCase(const std::string& str);
    static std::string toKebabCase(const std::string& str);

    // String manipulation
    static std::string replace(const std::string& str, const std::string& from, const std::string& to);
    static std::string replaceAll(const std::string& str, const std::string& from, const std::string& to);
    static std::vector<std::string> split(const std::string& str, char delimiter);
    static std::vector<std::string> split(const std::string& str, const std::string& delimiter);
    static std::string join(const std::vector<std::string>& strings, const std::string& delimiter);

    // Whitespace handling
    static std::string trim(const std::string& str);
    static std::string trimLeft(const std::string& str);
    static std::string trimRight(const std::string& str);
    static std::string removeWhitespace(const std::string& str);

    // String queries
    static bool startsWith(const std::string& str, const std::string& prefix);
    static bool endsWith(const std::string& str, const std::string& suffix);
    static bool contains(const std::string& str, const std::string& substring);
    static bool isEmpty(const std::string& str);
    static bool isWhitespace(const std::string& str);

    // Validation
    static bool isValidIdentifier(const std::string& str);
    static bool isValidProjectName(const std::string& str);
    static bool isValidEmail(const std::string& str);
    static bool isValidUrl(const std::string& str);

    // Formatting
    static std::string format(const std::string& format, const std::vector<std::string>& args);
    static std::string padLeft(const std::string& str, size_t width, char fillChar = ' ');
    static std::string padRight(const std::string& str, size_t width, char fillChar = ' ');
    static std::string center(const std::string& str, size_t width, char fillChar = ' ');
};
} // namespace utils
```

### TerminalUtils

Terminal styling and formatting.

```cpp
namespace utils {
enum class Color {
    Reset, Black, Red, Green, Yellow, Blue, Magenta, Cyan, White,
    BrightBlack, BrightRed, BrightGreen, BrightYellow,
    BrightBlue, BrightMagenta, BrightCyan, BrightWhite
};

class TerminalUtils {
public:
    // Color and styling
    static std::string colorize(const std::string& text, Color color);
    static std::string bold(const std::string& text);
    static std::string underline(const std::string& text);

    // UI elements
    static void printHeader(const std::string& title);
    static void printSeparator(char character = '-', int length = 80);
    static void printTable(const std::vector<std::vector<std::string>>& data);

    // Progress indication
    static void showSpinner(const std::string& message);
    static void showProgressBar(int current, int total, const std::string& message = "");
    static void showModernProgressBar(int percent, const std::string& label = "",
                                    Color color = Color::BrightGreen);

    // Interactive elements
    static int showInteractiveMenu(const std::vector<std::string>& options,
                                  const std::string& prompt = "",
                                  int defaultSelection = 0);
    static bool showConfirmDialog(const std::string& message, bool defaultValue = true);
    static std::string showInputDialog(const std::string& prompt,
                                     const std::string& placeholder = "",
                                     const std::string& defaultValue = "");

    // Status messages
    static void showSuccess(const std::string& message);
    static void showError(const std::string& message);
    static void showWarning(const std::string& message);
    static void showInfo(const std::string& message);
};
}
```

### GitUtils

Git repository operations.

```cpp
namespace utils {
class GitUtils {
public:
    // Repository initialization
    static bool initializeRepository(const std::filesystem::path& projectPath);
    static bool isGitRepository(const std::filesystem::path& path);
    static bool hasGitInstalled();

    // .gitignore management
    static bool createGitignore(const std::filesystem::path& projectPath,
                               const std::string& templateType,
                               const std::string& buildSystem = "",
                               const std::string& packageManager = "");

    // Git configuration
    static bool configureRepository(const std::filesystem::path& projectPath,
                                  const std::string& userName = "",
                                  const std::string& userEmail = "");

    // Remote management
    static bool addRemote(const std::filesystem::path& projectPath,
                         const std::string& remoteName,
                         const std::string& remoteUrl);

    // Commit operations
    static bool createInitialCommit(const std::filesystem::path& projectPath,
                                   const std::string& message = "Initial commit");
    static bool stageAllFiles(const std::filesystem::path& projectPath);

    // Branch operations
    static bool createBranch(const std::filesystem::path& projectPath,
                            const std::string& branchName);
    static bool switchBranch(const std::filesystem::path& projectPath,
                            const std::string& branchName);

    // Git hooks
    static bool installPreCommitHook(const std::filesystem::path& projectPath);
    static bool installCommitMsgHook(const std::filesystem::path& projectPath);
};
}
```

### StringUtils

String manipulation utilities.

```cpp
namespace utils {
class StringUtils {
public:
    static std::string toLower(const std::string& str);
    static std::string toUpper(const std::string& str);
    static std::string replace(const std::string& str, const std::string& from,
                             const std::string& to);
    static std::vector<std::string> split(const std::string& str, char delimiter);
    static std::string trim(const std::string& str);
    static bool startsWith(const std::string& str, const std::string& prefix);
    static bool endsWith(const std::string& str, const std::string& suffix);

    // Template string processing
    static std::string replaceVariables(const std::string& templateStr,
                                       const std::map<std::string, std::string>& variables);
    static std::string camelCase(const std::string& str);
    static std::string snakeCase(const std::string& str);
    static std::string pascalCase(const std::string& str);
};
}
```

### FrameworkIntegration

Support for various C++ frameworks.

```cpp
namespace utils {
class FrameworkIntegration {
public:
    enum class Framework {
        Qt, SFML, Boost, GoogleTest, Catch2, OpenCV, Eigen, FLTK, GTK, wxWidgets,
        Dear_ImGui, GLFW, OpenGL, Vulkan, SDL2, Allegro, POCO, Asio, gRPC,
        Protobuf, JSON_nlohmann, fmt, spdlog, CLI11, Doctest
    };

    enum class PackageManager {
        vcpkg, Conan, Hunter, CPM, FetchContent, System
    };

    struct FrameworkInfo {
        std::string name;
        std::string description;
        std::string version;
        std::vector<std::string> dependencies;
        std::vector<std::string> includeDirectories;
        std::vector<std::string> libraries;
        std::map<std::string, std::string> cmakeConfig;
        bool requiresSpecialSetup;
    };

    // Framework detection and installation
    static bool isFrameworkAvailable(Framework framework,
                                   PackageManager packageManager = PackageManager::vcpkg);
    static bool installFramework(Framework framework,
                                const std::filesystem::path& projectPath,
                                PackageManager packageManager = PackageManager::vcpkg);
    static bool configureFramework(Framework framework,
                                  const std::filesystem::path& projectPath);

    // Framework information
    static std::optional<FrameworkInfo> getFrameworkInfo(Framework framework);
    static std::vector<Framework> listAvailableFrameworks();
    static std::vector<Framework> getCompatibleFrameworks(Framework baseFramework);

    // CMake integration
    static std::string generateCMakeConfig(Framework framework);
    static std::string generateFindPackageCall(Framework framework);
    static std::string generateTargetLinkLibraries(Framework framework,
                                                   const std::string& targetName);
};
}
```

## 🧪 Testing Framework

### TestFrameworkManager

Manages integration with various testing frameworks, providing unified setup, configuration, and execution.

```cpp
namespace testing {
class TestFrameworkManager {
public:
    static TestFrameworkManager& getInstance();

    // Framework enumeration
    enum class Framework {
        GoogleTest,    // Google Test and Google Mock
        Catch2,        // Catch2 testing framework
        Doctest,       // Doctest lightweight framework
        Boost_Test,    // Boost.Test framework
        Custom         // User-defined testing framework
    };

    // Framework information structure
    struct FrameworkInfo {
        Framework framework;
        std::string name;
        std::string description;
        std::string version;
        std::vector<std::string> features;
        std::map<std::string, std::string> packageNames; // Package manager -> package name
        std::vector<std::string> cmakeTargets;
        bool supportsParameterizedTests;
        bool supportsFixtures;
        bool supportsMocking;
        bool supportsBenchmarking;
        bool supportsParallelExecution;
    };

    // Test configuration structure
    struct TestConfig {
        Framework framework = Framework::GoogleTest;
        std::string version = "latest";
        bool generateSampleTests = true;
        bool enableMocking = false;
        bool enableBenchmarking = false;
        bool enableParallelExecution = true;
        std::vector<std::string> additionalDependencies;
        std::map<std::string, std::string> frameworkOptions;
    };

    // Framework management
    std::vector<Framework> listSupportedFrameworks() const;
    std::optional<FrameworkInfo> getFrameworkInfo(Framework framework) const;
    bool isFrameworkSupported(Framework framework) const;

    // Framework setup and configuration
    bool setupFramework(Framework framework, const std::filesystem::path& projectPath,
                       const TestConfig& config = {});
    bool generateTestStructure(const std::filesystem::path& projectPath, const TestConfig& config);
    bool generateSampleTests(const std::filesystem::path& projectPath, Framework framework);
    bool generateTestMain(const std::filesystem::path& projectPath, Framework framework);

    // CMake integration
    std::string generateCMakeTestConfig(Framework framework, const TestConfig& config);
    std::string generateCTestConfig(const std::filesystem::path& projectPath);

    // Test discovery and execution
    std::vector<std::string> discoverTests(const std::filesystem::path& projectPath) const;
    bool runTests(const std::filesystem::path& projectPath,
                 const std::vector<std::string>& testNames = {}) const;
    bool generateTestReport(const std::filesystem::path& projectPath,
                           const std::filesystem::path& outputPath) const;

    // Framework-specific setup
    bool setupGoogleTest(const std::filesystem::path& projectPath, const TestConfig& config);
    bool setupCatch2(const std::filesystem::path& projectPath, const TestConfig& config);
    bool setupDoctest(const std::filesystem::path& projectPath, const TestConfig& config);
    bool setupBoostTest(const std::filesystem::path& projectPath, const TestConfig& config);

private:
    TestFrameworkManager() = default;
    std::map<Framework, FrameworkInfo> m_frameworks;

    void initializeFrameworks();
    bool validateTestConfig(const TestConfig& config) const;
    std::string generateTestTemplate(Framework framework, const std::string& testName) const;
};
}
```

### Test Utilities

```cpp
namespace test_utils {
    // Framework detection
    TestFramework detectFramework(const std::filesystem::path& projectPath);
    std::string frameworkToString(TestFramework framework);
    std::optional<TestFramework> stringToFramework(const std::string& name);

    // Test file utilities
    bool isTestFile(const std::filesystem::path& filePath);
    std::string getTestFilePattern(TestFramework framework);
    std::vector<std::filesystem::path> findTestFiles(const std::filesystem::path& directory);

    // CMake utilities
    std::string generateTestTarget(const std::string& targetName,
                                  const std::vector<std::string>& sources,
                                  TestFramework framework);
    std::string generateCTestConfiguration(const std::vector<std::string>& testTargets);

    // Test templates
    std::string generateBasicTestFile(const std::string& className, TestFramework framework);
    std::string generateMockTestFile(const std::string& className, TestFramework framework);
    std::string generateIntegrationTestFile(const std::string& moduleName, TestFramework framework);
}
```

## 🔧 Enums and Type Definitions

### Core Enums

```cpp
// Project template types
enum class TemplateType {
    Console,           // Command-line applications
    Lib,              // Static/shared libraries
    HeaderOnlyLib,    // Header-only libraries
    MultiExecutable,  // Projects with multiple executables
    Gui,              // GUI applications
    Network,          // Network services and clients
    Embedded,         // Embedded systems
    WebService,       // Web services and APIs
    GameEngine,       // Game development
    QtApp,            // Qt-specific applications
    SfmlApp,          // SFML-specific applications
    BoostApp,         // Boost-heavy applications
    TestProject,      // Testing-focused projects
    Custom            // User-defined templates
};

// Build system options
enum class BuildSystem {
    CMake,            // CMake build system
    Meson,            // Meson build system
    Bazel,            // Bazel build system
    XMake,            // XMake build system
    Premake,          // Premake build system
    Make,             // Traditional Makefiles
    Ninja             // Ninja build system
};

// Package manager options
enum class PackageManager {
    Vcpkg,            // Microsoft vcpkg
    Conan,            // Conan package manager
    Spack,            // Spack package manager
    Hunter,           // Hunter package manager
    None              // No package manager
};

// Testing framework options
enum class TestFramework {
    GTest,            // Google Test
    Catch2,           // Catch2
    Doctest,          // Doctest
    Boost,            // Boost.Test
    None              // No testing framework
};

// Editor configuration options
enum class EditorConfig {
    VSCode,           // Visual Studio Code
    CLion,            // JetBrains CLion
    VS,               // Visual Studio
    Vim,              // Vim/Neovim
    Emacs,            // Emacs
    Sublime           // Sublime Text
};

// CI/CD system options
enum class CiSystem {
    GitHub,           // GitHub Actions
    GitLab,           // GitLab CI
    Travis,           // Travis CI
    AppVeyor,         // AppVeyor
    AzureDevOps,      // Azure DevOps
    CircleCI          // CircleCI
};

// Language options for internationalization
enum class Language {
    English,          // English
    Chinese,          // Chinese (Simplified)
    Spanish,          // Spanish
    Japanese,         // Japanese
    German,           // German
    French            // French
};
```

### Enum Utilities

```cpp
namespace enums {
    // Convert enums to strings
    std::string_view to_string(TemplateType type);
    std::string_view to_string(BuildSystem system);
    std::string_view to_string(PackageManager manager);
    std::string_view to_string(TestFramework framework);
    std::string_view to_string(EditorConfig editor);
    std::string_view to_string(CiSystem ci);
    std::string_view to_string(Language lang);

    // Convert strings to enums
    std::optional<TemplateType> to_template_type(std::string_view str);
    std::optional<BuildSystem> to_build_system(std::string_view str);
    std::optional<PackageManager> to_package_manager(std::string_view str);
    std::optional<TestFramework> to_test_framework(std::string_view str);
    std::optional<EditorConfig> to_editor_config(std::string_view str);
    std::optional<CiSystem> to_ci_system(std::string_view str);
    std::optional<Language> to_language(std::string_view str);

    // Get all enum values as strings
    std::vector<std::string_view> all_template_types();
    std::vector<std::string_view> all_build_systems();
    std::vector<std::string_view> all_package_managers();
    std::vector<std::string_view> all_test_frameworks();
    std::vector<std::string_view> all_editor_configs();
    std::vector<std::string_view> all_ci_systems();
    std::vector<std::string_view> all_languages();
}
```

## Examples

### Creating a Console Application

```cpp
#include "templates/template_manager.h"
#include "cli/cli_parser.h"

int main() {
    CliOptions options;
    options.projectName = "MyConsoleApp";
    options.templateType = TemplateType::Console;
    options.buildSystem = BuildSystem::CMake;
    options.packageManager = PackageManager::Vcpkg;
    options.includeTests = true;
    options.testFramework = TestFramework::GTest;
    options.editorOptions = {EditorConfig::VSCode};
    options.ciOptions = {CiSystem::GitHub};

    TemplateManager manager;
    if (manager.createProject(options)) {
        std::cout << "Project created successfully!\n";
        return 0;
    } else {
        std::cerr << "Failed to create project.\n";
        return 1;
    }
}
```

### Creating a Header-Only Library

```cpp
#include "templates/template_manager.h"

int main() {
    CliOptions options;
    options.projectName = "MyHeaderLib";
    options.templateType = TemplateType::HeaderOnlyLib;
    options.buildSystem = BuildSystem::CMake;
    options.packageManager = PackageManager::Vcpkg;
    options.includeTests = true;
    options.testFramework = TestFramework::Doctest;
    options.includeDocumentation = true;
    options.includeCodeStyleTools = true;

    TemplateManager manager;
    return manager.createProject(options) ? 0 : 1;
}
```

### Interactive Project Creation

```cpp
#include "cli/cli_parser.h"
#include "templates/template_manager.h"

int main() {
    // Run interactive wizard
    CliOptions options = CliParser::runInteractiveMode();

    // Validate options
    if (!CliParser::validateOptions(options)) {
        std::cerr << "Invalid project configuration.\n";
        return 1;
    }

    // Create project
    TemplateManager manager;
    return manager.createProject(options) ? 0 : 1;
}
```

### Using Framework Integration

```cpp
#include "utils/framework_integration.h"

int main() {
    using namespace utils;

    // Check if Qt is available
    if (FrameworkIntegration::isFrameworkAvailable(
            FrameworkIntegration::Framework::Qt,
            FrameworkIntegration::PackageManager::vcpkg)) {

        std::filesystem::path projectPath = "MyQtApp";

        // Install Qt framework
        if (FrameworkIntegration::installFramework(
                FrameworkIntegration::Framework::Qt,
                projectPath,
                FrameworkIntegration::PackageManager::vcpkg)) {

            // Configure Qt in the project
            FrameworkIntegration::configureFramework(
                FrameworkIntegration::Framework::Qt,
                projectPath);
        }
    }

    return 0;
}
```

## Error Handling

The API uses a combination of error handling strategies:

### Return Value Patterns

- **Boolean return values** for success/failure operations
- **`std::optional<T>`** for operations that may not return a value
- **Exceptions** for critical errors that should not be ignored
- **Comprehensive logging** through spdlog for debugging and monitoring

### Error Categories

#### File System Errors

```cpp
// Example: File operations return false on failure
if (!utils::FileUtils::writeToFile("config.json", content)) {
    spdlog::error("Failed to write configuration file");
    return false;
}

// Example: Optional return for file reading
auto content = utils::FileUtils::readFromFile("config.json");
if (content.empty()) {
    spdlog::warn("Configuration file is empty or could not be read");
}
```

#### Validation Errors

```cpp
// Example: Option validation
CliOptions options = /* ... */;
if (!CliParser::validateOptions(options)) {
    std::cerr << "Invalid project configuration provided\n";
    return 1;
}
```

#### Framework Integration Errors

```cpp
// Example: Framework availability check
if (!FrameworkIntegration::isFrameworkAvailable(Framework::Qt)) {
    spdlog::error("Qt framework is not available with the selected package manager");
    return false;
}
```

### Logging Levels

The project uses spdlog with the following levels:

- **trace**: Detailed execution flow
- **debug**: Debug information for development
- **info**: General information about operations
- **warn**: Warning messages for non-critical issues
- **error**: Error messages for failed operations
- **critical**: Critical errors that may cause application termination

## Command Line Reference

### Basic Syntax

```bash
cpp-scaffold [PROJECT_NAME] [OPTIONS]
```

### Core Options

| Option | Short | Description | Default |
|--------|-------|-------------|---------|
| `--type` | `-t` | Project template type | `console` |
| `--build` | `-b` | Build system | `cmake` |
| `--package` | `-p` | Package manager | `vcpkg` |
| `--test` | | Testing framework | `none` |
| `--ci` | | CI/CD system | `none` |
| `--editor` | | Editor configuration | `none` |
| `--language` | `-l` | Interface language | `english` |
| `--verbose` | `-v` | Enable verbose output | `false` |
| `--help` | `-h` | Show help information | |
| `--version` | | Show version information | |

### Template Types

| Type | Description |
|------|-------------|
| `console` | Command-line application |
| `lib` | Static/shared library |
| `header-only-lib` | Header-only library |
| `gui` | GUI application |
| `network` | Network service |
| `embedded` | Embedded system project |
| `webservice` | Web service application |
| `multi-executable` | Multiple executables |

### Build Systems

| System | Description |
|--------|-------------|
| `cmake` | CMake build system |
| `meson` | Meson build system |
| `bazel` | Bazel build system |
| `xmake` | XMake build system |
| `premake` | Premake build system |
| `make` | Traditional Makefile |
| `ninja` | Ninja build system |

### Package Managers

| Manager | Description |
|---------|-------------|
| `vcpkg` | Microsoft vcpkg |
| `conan` | Conan package manager |
| `spack` | Spack package manager |
| `hunter` | Hunter package manager |
| `none` | No package manager |

### Testing Frameworks

| Framework | Description |
|-----------|-------------|
| `gtest` | Google Test |
| `catch2` | Catch2 testing framework |
| `doctest` | Doctest framework |
| `boost` | Boost.Test |
| `none` | No testing framework |

### Example Commands

```bash
# Create a simple console application
cpp-scaffold MyApp --type console --build cmake

# Create a library with testing
cpp-scaffold MyLib --type lib --build cmake --package vcpkg --test gtest

# Create a GUI application with full setup
cpp-scaffold MyGuiApp --type gui --build cmake --package conan --test catch2 --ci github --editor vscode

# Interactive mode
cpp-scaffold

# Quick start wizard
cpp-scaffold --wizard quick

# Advanced configuration wizard
cpp-scaffold --wizard advanced
```

### Configuration Profiles

```bash
# Save current configuration as a profile
cpp-scaffold --save-profile myprofile

# Use a saved profile
cpp-scaffold MyProject --profile myprofile

# List available profiles
cpp-scaffold --list-profiles
```

## Getting Started Guide

### 1. Installation

Follow the [installation instructions](#installation) to build CPP-Scaffold from source.

### 2. Your First Project

Create a simple console application:

```bash
./cpp-scaffold HelloWorld --type console --build cmake --package vcpkg --test gtest
cd HelloWorld
mkdir build && cd build
cmake .. -G Ninja
ninja
./HelloWorld
```

### 3. Exploring Templates

Try different project templates:

```bash
# Header-only library
./cpp-scaffold MyHeaderLib --type header-only-lib --test doctest

# GUI application
./cpp-scaffold MyGuiApp --type gui --build cmake --package vcpkg

# Network service
./cpp-scaffold MyServer --type network --build cmake --package vcpkg --test gtest
```

### 4. Interactive Mode

For beginners, use the interactive wizard:

```bash
./cpp-scaffold
```

This will guide you through all available options with explanations and recommendations.

### 5. Advanced Usage

Explore advanced features like:

- Custom templates with `--template-path`
- Multiple CI/CD systems with `--ci github,gitlab`
- Multiple editor configurations with `--editor vscode,clion`
- Framework integration for GUI applications

## ❌ Error Handling

CPP-Scaffold uses a comprehensive error handling strategy that combines multiple approaches for different types of operations.

### Error Handling Patterns

#### 1. Boolean Return Values
Most operations return `bool` to indicate success or failure:

```cpp
bool success = manager.createProject(options);
if (!success) {
    std::cerr << "Failed to create project" << std::endl;
    return 1;
}
```

#### 2. Optional Return Values
Operations that may not return a value use `std::optional<T>`:

```cpp
auto profile = ConfigManager::getInstance().loadProfile("my-profile");
if (profile.has_value()) {
    CliOptions options = profile.value();
    // Use the loaded profile
} else {
    std::cerr << "Profile not found" << std::endl;
}
```

#### 3. Exceptions for Critical Errors
Critical errors that should not be ignored throw exceptions:

```cpp
try {
    TemplateManager manager;
    manager.createProject(options);
} catch (const std::filesystem::filesystem_error& e) {
    std::cerr << "Filesystem error: " << e.what() << std::endl;
    return 1;
} catch (const std::runtime_error& e) {
    std::cerr << "Runtime error: " << e.what() << std::endl;
    return 1;
}
```

#### 4. Comprehensive Logging
All operations are logged using spdlog for debugging and monitoring:

```cpp
#include <spdlog/spdlog.h>

// Logging is automatically handled by the API
// You can configure log levels:
spdlog::set_level(spdlog::level::debug);  // Show all logs
spdlog::set_level(spdlog::level::info);   // Show info and above
spdlog::set_level(spdlog::level::warn);   // Show warnings and errors only
```

### Error Categories

- **Configuration Errors**: Invalid project names, unsupported template types, missing required options
- **File System Errors**: Permission denied, disk space insufficient, path not found, file already exists
- **Network Errors**: Dependency download failures, Git clone failures, package manager errors
- **Build System Errors**: CMake configuration failures, missing build tools, compiler not found

### Best Practices

1. **Always Check Return Values**: Never ignore boolean return values from API functions
2. **Use RAII for Resource Management**: Ensure proper cleanup with smart pointers and RAII
3. **Provide Meaningful Error Messages**: Include context and suggestions for resolution
4. **Log Context Information**: Include relevant details in log messages for debugging

## 🔗 Support and Contributing

- 📖 **Documentation**: See the [docs/](docs/) directory for additional documentation
- 🐛 **Bug Reports**: Open an issue on GitHub with detailed reproduction steps
- 💡 **Feature Requests**: Open an issue with the "enhancement" label
- 💬 **Discussions**: Use GitHub Discussions for questions and community support
- 🤝 **Contributing**: See [CONTRIBUTING.md](CONTRIBUTING.md) for contribution guidelines

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

# CLI Parser Component

The CLI Parser component handles command-line argument parsing, validation, and interactive user configuration for CPP-Scaffold.

## 📋 Overview

The CLI Parser component is responsible for:
- Parsing command-line arguments into structured options
- Validating user input and providing meaningful error messages
- Providing interactive wizards for project configuration
- Displaying help information and usage examples
- Converting between different data types and formats

## 🏗️ Architecture

### Component Structure

```
src/cli/
├── cli_parser.h          # Main CLI parser interface
├── cli_parser.cpp        # CLI parsing implementation
└── project_wizard.cpp   # Interactive wizard implementation
```

### Class Hierarchy

```cpp
namespace cli {
    class CliParser {
        // Static interface for parsing and validation
    };
    
    class ProjectWizard {
        // Interactive configuration wizard
    };
}
```

## 🔧 Core Classes

### CliParser

The main class responsible for command-line argument parsing and validation.

```cpp
class CliParser {
public:
    // Main parsing method
    static CliOptions parse(int argc, char* argv[]);
    
    // Information display
    static void showHelp(Language lang = Language::English);
    static void showVersion();
    static void showAvailableTemplates();
    
    // Interactive configuration
    static CliOptions promptUserForOptions(const CliOptions& defaults = {});
    static CliOptions runInteractiveMode();
    
    // Validation
    static bool validateOptions(CliOptions& options);
    static bool validateProjectName(const std::string& name);
    
private:
    // Internal parsing helpers
    static TemplateType parseTemplateType(const std::string& type);
    static BuildSystem parseBuildSystem(const std::string& system);
    static PackageManager parsePackageManager(const std::string& manager);
    static TestFramework parseTestFramework(const std::string& framework);
    static std::vector<EditorConfig> parseEditorConfigs(const std::string& editors);
    static std::vector<CiSystem> parseCiSystems(const std::string& systems);
    static Language parseLanguage(const std::string& language);
    
    // Validation helpers
    static bool isValidProjectName(const std::string& name);
    static bool isValidPath(const std::string& path);
    static bool isValidUrl(const std::string& url);
    
    // System detection
    static std::string detectOperatingSystem();
    static CliOptions getSystemDefaults();
};
```

### ProjectWizard

Interactive wizard for guided project configuration.

```cpp
class ProjectWizard {
public:
    ProjectWizard();
    
    // Main wizard interface
    CliOptions runWizard(const CliOptions& defaults = {});
    CliOptions runQuickWizard();
    CliOptions runAdvancedWizard();
    
    // Individual configuration steps
    std::string promptProjectName(const std::string& defaultName = "");
    TemplateType promptTemplateType(TemplateType defaultType = TemplateType::Console);
    BuildSystem promptBuildSystem(BuildSystem defaultSystem = BuildSystem::CMake);
    PackageManager promptPackageManager(PackageManager defaultManager = PackageManager::Vcpkg);
    TestFramework promptTestFramework(TestFramework defaultFramework = TestFramework::None);
    std::vector<EditorConfig> promptEditorConfigs();
    std::vector<CiSystem> promptCiSystems();
    Language promptLanguage(Language defaultLang = Language::English);
    
    // Configuration validation and preview
    bool validateConfiguration(const CliOptions& options);
    void showConfigurationPreview(const CliOptions& options);
    bool confirmConfiguration(const CliOptions& options);

private:
    // UI helpers
    void showWelcomeMessage();
    void showStepProgress(int current, int total);
    template<typename T>
    T promptChoice(const std::string& prompt, 
                   const std::vector<std::pair<T, std::string>>& choices,
                   T defaultChoice);
    
    bool promptYesNo(const std::string& prompt, bool defaultValue = false);
    std::string promptString(const std::string& prompt, 
                           const std::string& defaultValue = "",
                           const std::function<bool(const std::string&)>& validator = nullptr);
};
```

## 🔄 Data Flow

### Command-Line Parsing Flow

```
1. Raw Arguments (argc, argv)
   ↓
2. Argument Tokenization
   ↓
3. Option Recognition
   ↓
4. Type Conversion
   ↓
5. Validation
   ↓
6. CliOptions Structure
```

### Interactive Wizard Flow

```
1. Welcome & Introduction
   ↓
2. Project Name Input
   ↓
3. Template Type Selection
   ↓
4. Build System Selection
   ↓
5. Package Manager Selection
   ↓
6. Testing Framework Selection
   ↓
7. Additional Options
   ↓
8. Configuration Preview
   ↓
9. Confirmation
   ↓
10. CliOptions Structure
```

## 📝 Configuration Options

### Supported Command-Line Options

| Option | Short | Type | Description | Default |
|--------|-------|------|-------------|---------|
| `--type` | `-t` | `TemplateType` | Project template | `console` |
| `--build` | `-b` | `BuildSystem` | Build system | `cmake` |
| `--package` | `-p` | `PackageManager` | Package manager | `vcpkg` |
| `--test` | | `TestFramework` | Testing framework | `none` |
| `--ci` | | `CiSystem[]` | CI/CD systems | `[]` |
| `--editor` | | `EditorConfig[]` | Editor configs | `[]` |
| `--lang` | `-l` | `Language` | Interface language | `english` |
| `--verbose` | `-v` | `bool` | Verbose output | `false` |
| `--help` | `-h` | `bool` | Show help | `false` |
| `--version` | | `bool` | Show version | `false` |

### Advanced Options

| Option | Type | Description |
|--------|------|-------------|
| `--profile` | `string` | Use saved profile |
| `--save-profile` | `string` | Save as profile |
| `--custom-template` | `path` | Custom template path |
| `--no-git` | `bool` | Skip Git initialization |
| `--no-tests` | `bool` | Skip test setup |
| `--no-docs` | `bool` | Skip documentation |
| `--force` | `bool` | Overwrite existing |
| `--dry-run` | `bool` | Preview only |

## 🔍 Validation Rules

### Project Name Validation

```cpp
bool validateProjectName(const std::string& name) {
    // Rules:
    // 1. Not empty
    // 2. Length between 1-255 characters
    // 3. Starts with letter or underscore
    // 4. Contains only letters, numbers, underscores, hyphens
    // 5. No reserved names (con, aux, prn, etc. on Windows)
    
    if (name.empty() || name.length() > 255) return false;
    
    if (!std::isalpha(name[0]) && name[0] != '_') return false;
    
    for (char c : name) {
        if (!std::isalnum(c) && c != '_' && c != '-') return false;
    }
    
    return !isReservedName(name);
}
```

### Option Compatibility Validation

```cpp
bool validateOptionCompatibility(const CliOptions& options) {
    // Check template-specific requirements
    if (options.templateType == TemplateType::Embedded) {
        if (options.packageManager == PackageManager::Vcpkg) {
            // vcpkg may not be suitable for embedded
            return false;
        }
    }
    
    // Check build system compatibility
    if (options.buildSystem == BuildSystem::Bazel) {
        if (options.packageManager != PackageManager::None) {
            // Bazel has its own dependency management
            return false;
        }
    }
    
    return true;
}
```

## 💡 Usage Examples

### Basic Command-Line Parsing

```cpp
#include "cli/cli_parser.h"

int main(int argc, char* argv[]) {
    try {
        // Parse command-line arguments
        CliOptions options = CliParser::parse(argc, argv);
        
        // Handle special cases
        if (options.showHelp) {
            CliParser::showHelp(options.language);
            return 0;
        }
        
        if (options.version) {
            CliParser::showVersion();
            return 0;
        }
        
        // Validate options
        if (!CliParser::validateOptions(options)) {
            std::cerr << "Invalid configuration. Use --help for usage information." << std::endl;
            return 1;
        }
        
        // Use options for project creation
        // ...
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
```

### Interactive Wizard Usage

```cpp
#include "cli/cli_parser.h"

int main() {
    try {
        // Run interactive wizard
        CliOptions options = CliParser::runInteractiveMode();
        
        // Options are already validated by the wizard
        std::cout << "Configuration complete!" << std::endl;
        
        // Use options for project creation
        // ...
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
```

### Custom Validation

```cpp
#include "cli/cli_parser.h"

bool customValidation(CliOptions& options) {
    // Custom business logic validation
    if (options.templateType == TemplateType::Gui && 
        options.editorOptions.empty()) {
        // Suggest IDE for GUI development
        options.editorOptions.push_back(EditorConfig::VSCode);
        std::cout << "Added VSCode configuration for GUI development" << std::endl;
    }
    
    return CliParser::validateOptions(options);
}
```

## 🧪 Testing

### Unit Tests

The CLI Parser component includes comprehensive unit tests:

```cpp
// Test basic parsing
TEST(CliParserTest, ParseBasicOptions) {
    const char* argv[] = {"cpp-scaffold", "MyProject", "--type", "console", "--build", "cmake"};
    int argc = sizeof(argv) / sizeof(argv[0]);
    
    CliOptions options = CliParser::parse(argc, argv);
    
    EXPECT_EQ(options.projectName, "MyProject");
    EXPECT_EQ(options.templateType, TemplateType::Console);
    EXPECT_EQ(options.buildSystem, BuildSystem::CMake);
}

// Test validation
TEST(CliParserTest, ValidateProjectName) {
    EXPECT_TRUE(CliParser::validateProjectName("MyProject"));
    EXPECT_TRUE(CliParser::validateProjectName("my_project"));
    EXPECT_TRUE(CliParser::validateProjectName("my-project"));
    EXPECT_FALSE(CliParser::validateProjectName(""));
    EXPECT_FALSE(CliParser::validateProjectName("123project"));
    EXPECT_FALSE(CliParser::validateProjectName("my project"));
}

// Test error handling
TEST(CliParserTest, HandleInvalidOptions) {
    const char* argv[] = {"cpp-scaffold", "MyProject", "--type", "invalid"};
    int argc = sizeof(argv) / sizeof(argv[0]);
    
    EXPECT_THROW(CliParser::parse(argc, argv), std::invalid_argument);
}
```

## 🔗 Dependencies

### External Dependencies
- **Standard Library**: `<string>`, `<vector>`, `<map>`, `<optional>`
- **spdlog**: For logging and error reporting

### Internal Dependencies
- **Configuration System**: For loading default preferences
- **Utility Classes**: For string manipulation and validation
- **Terminal Utils**: For styled interactive output

## 📊 Performance Considerations

- **Argument parsing**: O(n) where n is the number of arguments
- **Validation**: O(1) for most validations, O(n) for string validation
- **Memory usage**: Minimal, only stores parsed options
- **Interactive mode**: Responsive with immediate feedback

## 🔧 Extension Points

### Adding New Options

1. **Add to CliOptions structure**
2. **Update parsing logic** in `CliParser::parse()`
3. **Add validation rules** in `CliParser::validateOptions()`
4. **Update help text** in `CliParser::showHelp()`
5. **Add interactive prompts** in `ProjectWizard`

### Custom Validators

```cpp
// Register custom validator
CliParser::registerValidator("project-name", [](const std::string& name) {
    return myCustomValidation(name);
});
```

---

For more information, see the [API Reference](../api-reference/api.md) and [Architecture Documentation](../developer-guide/architecture.md).

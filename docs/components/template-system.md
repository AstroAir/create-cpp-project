# Template System Component

The Template System component is the core of CPP-Scaffold, responsible for generating project structures, build configurations, and development environments based on predefined or custom templates.

## 📋 Overview

The Template System component provides:
- Abstract template base class with Template Method pattern
- Concrete template implementations for different project types
- Template factory for creating appropriate template instances
- Project structure generation with customizable layouts
- Build system integration and configuration
- Package manager setup and dependency management

## 🏗️ Architecture

### Component Structure

```
src/templates/
├── template_base.h               # Abstract base class
├── template_base.cpp             # Common functionality
├── template_manager.h            # Template factory
├── template_manager.cpp          # Factory implementation
├── console_template.h/.cpp       # Console application template
├── library_template.h/.cpp       # Library project template
├── header_only_lib_template.h/.cpp # Header-only library template
├── gui_template.h/.cpp           # GUI application template
├── network_template.h/.cpp       # Network service template
├── webservice_template.h/.cpp    # Web service template
├── embedded_template.h/.cpp      # Embedded system template
├── multi_executable_template.h/.cpp # Multi-executable template
└── custom_template.h/.cpp        # User-defined template
```

### Class Hierarchy

```cpp
TemplateBase (Abstract)
├── ConsoleTemplate
├── LibraryTemplate
├── HeaderOnlyLibTemplate
├── GuiTemplate
├── NetworkTemplate
├── WebServiceTemplate
├── EmbeddedTemplate
├── MultiExecutableTemplate
└── CustomTemplate
```

## 🔧 Core Classes

### TemplateBase

Abstract base class implementing the Template Method pattern.

```cpp
class TemplateBase {
public:
    TemplateBase(const CliOptions& options);
    virtual ~TemplateBase() = default;

    // Main template method - defines the algorithm
    virtual bool create() = 0;

protected:
    CliOptions options_;

    // Pure virtual methods - must be implemented by subclasses
    virtual bool createProjectStructure() = 0;
    virtual bool createBuildSystem() = 0;
    virtual bool setupPackageManager() = 0;
    virtual bool setupTestFramework() = 0;

    // Common functionality - implemented in base class
    bool setupCICD(const std::string& projectPath);
    bool setupEditorConfig(const std::string& projectPath);
    bool setupCodeStyleConfig(const std::string& projectPath);
    bool setupDocConfig(const std::string& projectPath);
    bool initializeGit(const std::string& projectPath);
    
    // Content generators
    std::string getGitignoreContent();
    std::string getLicenseContent(const std::string& projectName);
    std::string getClangFormatContent();
    std::string getClangTidyContent();
    std::string getEditorConfigContent();
    
    // File creation utilities
    bool createFile(const std::string& path, const std::string& content);
    bool createDirectory(const std::string& path);
    bool copyTemplate(const std::string& templatePath, const std::string& targetPath);
    
    // Template variable substitution
    std::string processTemplate(const std::string& templateContent);
    void setTemplateVariable(const std::string& key, const std::string& value);
    std::string getTemplateVariable(const std::string& key) const;
    
    // Post-creation actions
    void printUsageGuide();
    bool executePostCreationActions();
    bool runInitialBuild();

private:
    std::map<std::string, std::string> templateVariables_;
    
    void initializeTemplateVariables();
    std::string replaceVariables(const std::string& content) const;
};
```

### TemplateManager

Factory class for creating and managing template instances.

```cpp
class TemplateManager {
public:
    TemplateManager();
    
    // Main project creation method
    bool createProject(const CliOptions& options);
    
    // Template management
    std::vector<std::string> getAvailableTemplates() const;
    bool isTemplateAvailable(const std::string& templateName) const;
    std::string getTemplateDescription(const std::string& templateName) const;
    
    // Custom template support
    bool registerCustomTemplate(const std::string& name, 
                               std::unique_ptr<TemplateBase> (*factory)(const CliOptions&));
    bool loadCustomTemplate(const std::filesystem::path& templatePath);

private:
    // Template factory function type
    using TemplateFactory = std::function<std::unique_ptr<TemplateBase>(const CliOptions&)>;
    
    // Template registry
    std::map<std::string, TemplateFactory> templateFactories_;
    std::map<std::string, std::string> templateDescriptions_;
    
    // Template creation helpers
    std::unique_ptr<TemplateBase> createTemplate(const CliOptions& options);
    std::unique_ptr<TemplateBase> createFrameworkTemplate(const CliOptions& options);
    
    // Initialization
    void initializeBuiltinTemplates();
    void registerTemplate(const std::string& name, 
                         TemplateFactory factory, 
                         const std::string& description);
};
```

## 🎯 Template Implementations

### ConsoleTemplate

Creates command-line applications with modern C++ structure.

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
    std::string getApplicationHeaderContent();
    std::string getApplicationSourceContent();
    std::string getArgumentParserContent();
    std::string getReadmeContent();
    
    // Build system generators
    std::string getCMakeListsContent();
    std::string getMesonBuildContent();
    std::string getBazelBuildContent();
    
    // Package manager generators
    std::string getVcpkgJsonContent();
    std::string getConanfileContent();
    
    // Test generators
    std::string getTestMainContent();
    std::string getUnitTestContent();
    
    // Project structure
    bool createSourceStructure();
    bool createTestStructure();
    bool createDocumentationStructure();
};
```

**Generated Structure:**
```
MyConsoleApp/
├── src/
│   ├── main.cpp
│   ├── MyConsoleApp/
│   │   ├── Application.h
│   │   ├── Application.cpp
│   │   ├── ArgumentParser.h
│   │   └── ArgumentParser.cpp
│   └── utils/
│       ├── Logger.h
│       └── Logger.cpp
├── tests/
│   ├── test_main.cpp
│   ├── test_Application.cpp
│   └── test_ArgumentParser.cpp
├── docs/
│   ├── README.md
│   └── USAGE.md
├── CMakeLists.txt
├── vcpkg.json
├── .gitignore
├── .clang-format
└── LICENSE
```

### LibraryTemplate

Creates static or shared libraries with proper export/import handling.

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
    std::string getPublicHeaderContent();
    std::string getPrivateHeaderContent();
    std::string getSourceContent();
    std::string getExportHeaderContent();
    std::string getVersionHeaderContent();
    std::string getConfigHeaderContent();
    
    // Build system generators
    std::string getCMakeListsContent();
    std::string getCMakeConfigContent();
    std::string getPkgConfigContent();
    
    // Documentation generators
    std::string getAPIDocumentationContent();
    std::string getUsageExamplesContent();
    
    // Installation and packaging
    bool setupInstallTargets();
    bool createPackageConfig();
    bool generateExamples();
    
    // Library-specific setup
    bool setupSharedLibraryConfig();
    bool setupStaticLibraryConfig();
    bool setupHeaderOnlyConfig();
};
```

### CustomTemplate

Supports user-defined templates with inheritance and variable substitution.

```cpp
class CustomTemplate : public TemplateBase {
public:
    CustomTemplate(const CliOptions& options);
    bool create() override;
    
    // Template loading and management
    bool loadFromDirectory(const std::filesystem::path& templatePath);
    bool loadFromArchive(const std::filesystem::path& archivePath);
    bool saveToDirectory(const std::filesystem::path& outputPath) const;
    
    // Template inheritance
    bool inheritFrom(const std::string& baseTemplateName);
    bool mergeWithTemplate(const CustomTemplate& other);
    
    // Variable management
    void setVariable(const std::string& key, const std::string& value);
    std::string getVariable(const std::string& key) const;
    bool hasVariable(const std::string& key) const;
    std::map<std::string, std::string> getAllVariables() const;

protected:
    bool createProjectStructure() override;
    bool createBuildSystem() override;
    bool setupPackageManager() override;
    bool setupTestFramework() override;

private:
    struct TemplateMetadata {
        std::string name;
        std::string description;
        std::string version;
        std::string author;
        std::string baseTemplate;
        std::vector<std::string> requiredVariables;
        std::map<std::string, std::string> defaultVariables;
        std::vector<std::string> supportedBuildSystems;
        std::vector<std::string> supportedPackageManagers;
    };
    
    TemplateMetadata metadata_;
    std::filesystem::path templatePath_;
    std::map<std::string, std::string> variables_;
    
    // Template processing
    bool loadMetadata();
    bool validateTemplate() const;
    bool processTemplateFiles();
    std::string processTemplateContent(const std::string& content) const;
    
    // File operations
    bool copyTemplateFiles();
    bool processTemplateFile(const std::filesystem::path& source, 
                           const std::filesystem::path& target);
};
```

## 🔄 Template Creation Process

### Standard Template Flow

```
1. Template Selection
   ├── Parse template type from options
   ├── Create template instance via factory
   └── Initialize with CliOptions

2. Project Structure Creation
   ├── Create root directory
   ├── Create source directories
   ├── Create test directories
   ├── Create documentation directories
   └── Create configuration directories

3. Build System Setup
   ├── Generate CMakeLists.txt / build files
   ├── Configure compiler settings
   ├── Setup build targets
   └── Configure installation rules

4. Package Manager Integration
   ├── Generate package configuration files
   ├── Setup dependency management
   ├── Configure package repositories
   └── Install initial dependencies

5. Test Framework Setup
   ├── Configure test framework
   ├── Generate test templates
   ├── Setup test targets
   └── Configure test runners

6. Development Environment Setup
   ├── Initialize Git repository
   ├── Setup CI/CD configurations
   ├── Configure editor settings
   ├── Setup code style tools
   └── Generate documentation

7. Post-Creation Actions
   ├── Run initial build (optional)
   ├── Generate usage guide
   ├── Display next steps
   └── Save project profile (optional)
```

## 💡 Usage Examples

### Basic Template Usage

```cpp
#include "templates/template_manager.h"

int main() {
    CliOptions options;
    options.projectName = "MyProject";
    options.templateType = TemplateType::Console;
    options.buildSystem = BuildSystem::CMake;
    options.packageManager = PackageManager::Vcpkg;
    options.includeTests = true;
    options.testFramework = TestFramework::GTest;
    
    TemplateManager manager;
    bool success = manager.createProject(options);
    
    if (success) {
        std::cout << "Project created successfully!" << std::endl;
    } else {
        std::cerr << "Failed to create project" << std::endl;
    }
    
    return success ? 0 : 1;
}
```

### Custom Template Creation

```cpp
#include "templates/custom_template.h"

// Create a custom template
CustomTemplate customTemplate(options);

// Set template variables
customTemplate.setVariable("PROJECT_NAME", "MyCustomProject");
customTemplate.setVariable("AUTHOR_NAME", "John Doe");
customTemplate.setVariable("LICENSE_TYPE", "MIT");

// Load template from directory
if (customTemplate.loadFromDirectory("./my-custom-template")) {
    // Create project using custom template
    bool success = customTemplate.create();
    
    if (success) {
        std::cout << "Custom project created!" << std::endl;
    }
}
```

### Template Extension

```cpp
// Extend existing template
class MySpecialTemplate : public ConsoleTemplate {
public:
    MySpecialTemplate(const CliOptions& options) : ConsoleTemplate(options) {}
    
protected:
    bool createProjectStructure() override {
        // Call base implementation
        if (!ConsoleTemplate::createProjectStructure()) {
            return false;
        }
        
        // Add custom directories
        createDirectory(options_.projectName + "/special");
        createDirectory(options_.projectName + "/plugins");
        
        // Add custom files
        createFile(options_.projectName + "/special/config.json", getConfigContent());
        
        return true;
    }
    
private:
    std::string getConfigContent() {
        return R"({
            "version": "1.0.0",
            "features": ["special", "plugins"]
        })";
    }
};

// Register custom template
TemplateManager manager;
manager.registerCustomTemplate("special-console", 
    [](const CliOptions& opts) {
        return std::make_unique<MySpecialTemplate>(opts);
    });
```

## 🧪 Testing

### Template Testing Strategy

```cpp
// Test template creation
TEST(TemplateSystemTest, CreateConsoleTemplate) {
    CliOptions options;
    options.projectName = "TestProject";
    options.templateType = TemplateType::Console;
    
    ConsoleTemplate template(options);
    EXPECT_TRUE(template.create());
    
    // Verify project structure
    EXPECT_TRUE(std::filesystem::exists("TestProject/src/main.cpp"));
    EXPECT_TRUE(std::filesystem::exists("TestProject/CMakeLists.txt"));
    EXPECT_TRUE(std::filesystem::exists("TestProject/README.md"));
}

// Test template validation
TEST(TemplateSystemTest, ValidateTemplateStructure) {
    TemplateManager manager;
    
    // Test all available templates
    auto templates = manager.getAvailableTemplates();
    for (const auto& templateName : templates) {
        EXPECT_TRUE(manager.isTemplateAvailable(templateName));
        EXPECT_FALSE(manager.getTemplateDescription(templateName).empty());
    }
}
```

## 🔗 Dependencies

### External Dependencies
- **std::filesystem**: File system operations
- **spdlog**: Logging and error reporting
- **nlohmann/json**: JSON configuration files

### Internal Dependencies
- **Utility Classes**: File operations, string processing
- **Configuration System**: Template preferences and defaults
- **Testing Framework**: Test setup and configuration

## 📊 Performance Considerations

- **Template instantiation**: O(1) factory lookup
- **File generation**: O(n) where n is number of files
- **Template processing**: O(m) where m is template content size
- **Memory usage**: Minimal, templates are stateless

## 🔧 Extension Points

### Adding New Templates

1. **Create template class** inheriting from `TemplateBase`
2. **Implement required methods**: `createProjectStructure()`, `createBuildSystem()`, etc.
3. **Register template** in `TemplateManager`
4. **Add template tests** and documentation
5. **Update template list** in help and documentation

### Template Customization

Templates support extensive customization through:
- **Template variables**: Dynamic content substitution
- **Conditional generation**: Feature-based file creation
- **Inheritance**: Building on existing templates
- **Plugin system**: Modular template extensions

---

For more information, see the [API Reference](../api-reference/api.md) and [Architecture Documentation](../developer-guide/architecture.md).

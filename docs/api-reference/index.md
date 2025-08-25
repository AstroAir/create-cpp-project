# API Reference

Complete API documentation for CPP-Scaffold's public interfaces and extension points.

## Overview

This section provides comprehensive documentation for all public APIs, interfaces, and extension points in CPP-Scaffold. Whether you're integrating CPP-Scaffold into your tools or creating custom extensions, you'll find the technical details you need here.

## What's in This Section

### [API Documentation](api.md)
Complete reference for all public classes, methods, and interfaces including:

- **Core API** - Main application interfaces
- **Template System** - Template creation and management
- **Configuration System** - Settings and profile management
- **Utility Classes** - Helper functions and cross-platform utilities
- **CLI Interface** - Command-line parsing and validation
- **Testing Framework** - Test integration and management

## API Categories

### Core Interfaces

The main interfaces for interacting with CPP-Scaffold programmatically:

```cpp
namespace cpp_scaffold {
    class Application;           // Main application interface
    class TemplateManager;       // Template factory and management
    class ConfigurationManager;  // Settings and profiles
    class ProjectGenerator;      // Project creation orchestration
}
```

### Template System

Interfaces for creating and managing project templates:

```cpp
namespace cpp_scaffold::templates {
    class TemplateBase;         // Abstract base for all templates
    class TemplateFactory;      // Template creation factory
    class TemplateRegistry;     // Template registration and discovery
    class TemplateValidator;    // Template validation and testing
}
```

### Configuration Management

APIs for managing application settings and user preferences:

```cpp
namespace cpp_scaffold::config {
    class ConfigManager;        // Configuration management
    class ProfileManager;       // User profile management
    class SettingsValidator;    // Configuration validation
    class PreferencesStore;     // Persistent storage interface
}
```

### Utility APIs

Cross-platform utilities and helper functions:

```cpp
namespace cpp_scaffold::utils {
    class FileUtils;           // File system operations
    class GitUtils;            // Git integration utilities
    class TerminalUtils;       // Terminal styling and interaction
    class StringUtils;         // String manipulation helpers
    class ProcessUtils;        // Process execution utilities
}
```

## Usage Patterns

### Basic Integration

```cpp
#include <cpp_scaffold/application.h>

int main() {
    cpp_scaffold::Application app;
    
    // Configure application
    app.set_verbose(true);
    app.set_output_directory("./projects");
    
    // Create project
    cpp_scaffold::ProjectOptions options;
    options.name = "MyProject";
    options.template_type = "console";
    options.build_system = "cmake";
    
    return app.create_project(options) ? 0 : 1;
}
```

### Custom Template Creation

```cpp
#include <cpp_scaffold/templates/template_base.h>

class MyCustomTemplate : public cpp_scaffold::templates::TemplateBase {
public:
    bool create() override {
        // Implement template creation logic
        return create_project_structure() && 
               setup_build_system() && 
               generate_documentation();
    }
    
    std::string get_name() const override {
        return "my-custom-template";
    }
    
    std::string get_description() const override {
        return "My custom project template";
    }
};
```

### Configuration Management

```cpp
#include <cpp_scaffold/config/config_manager.h>

void setup_configuration() {
    auto& config = cpp_scaffold::config::ConfigManager::instance();
    
    // Set default values
    config.set_default("build_system", "cmake");
    config.set_default("package_manager", "vcpkg");
    
    // Load user preferences
    config.load_user_preferences();
    
    // Save profile
    config.save_profile("my-profile", current_settings);
}
```

## Extension Points

### Template Registration

Register custom templates with the template system:

```cpp
// Register template
cpp_scaffold::templates::TemplateRegistry::register_template<MyCustomTemplate>();

// Register template factory
cpp_scaffold::templates::TemplateRegistry::register_factory(
    "my-template", 
    []() { return std::make_unique<MyCustomTemplate>(); }
);
```

### Configuration Hooks

Extend configuration system with custom settings:

```cpp
// Add custom configuration section
config.add_section("my_extension", {
    {"custom_setting", "default_value"},
    {"another_setting", 42}
});

// Register validation
config.add_validator("my_extension.custom_setting", 
    [](const auto& value) { return !value.empty(); });
```

### CLI Extensions

Add custom command-line options:

```cpp
// Extend CLI parser
cli_parser.add_argument("--my-option")
    .help("My custom option")
    .default_value(false);

// Register option handler
cli_parser.add_handler("--my-option", 
    [](const auto& value) { handle_my_option(value); });
```

## Error Handling

### Exception Hierarchy

```cpp
namespace cpp_scaffold {
    class Exception;                    // Base exception
    class TemplateException;           // Template-related errors
    class ConfigurationException;      // Configuration errors
    class FileSystemException;         // File system errors
    class ValidationException;         // Validation errors
}
```

### Error Codes

```cpp
enum class ErrorCode {
    Success = 0,
    InvalidArguments,
    TemplateNotFound,
    ConfigurationError,
    FileSystemError,
    BuildSystemError,
    PackageManagerError
};
```

## Thread Safety

### Thread-Safe Components
- **ConfigurationManager** - Thread-safe singleton
- **TemplateRegistry** - Thread-safe registration
- **Logger** - Thread-safe logging

### Non-Thread-Safe Components
- **TemplateBase** instances - Single-threaded usage
- **ProjectGenerator** - Single-threaded project creation
- **CLI Parser** - Single-threaded argument parsing

## Performance Considerations

### Memory Management
- **RAII** - Automatic resource management
- **Smart pointers** - Shared and unique ownership
- **Move semantics** - Efficient object transfer

### I/O Operations
- **Async file operations** - Non-blocking file I/O where possible
- **Batch operations** - Efficient bulk file operations
- **Caching** - Template and configuration caching

## Versioning and Compatibility

### API Versioning
- **Semantic versioning** - Major.Minor.Patch
- **Backward compatibility** - Within major versions
- **Deprecation policy** - Gradual deprecation with warnings

### ABI Stability
- **Stable ABI** - Within minor versions
- **Symbol versioning** - For shared library usage
- **Header compatibility** - Forward compatibility

## Examples and Samples

### Complete Examples
See the [API Documentation](api.md) for complete code examples including:

- **Basic project creation**
- **Custom template implementation**
- **Configuration management**
- **CLI integration**
- **Error handling patterns**

### Sample Projects
The `examples/` directory contains:

- **api_usage/** - Basic API usage examples
- **custom_templates/** - Custom template examples
- **integrations/** - IDE and tool integrations
- **extensions/** - Plugin and extension examples

## Migration Guides

### Upgrading from v1.x to v2.x
- **Breaking changes** - API changes and removals
- **Migration steps** - Step-by-step upgrade guide
- **Compatibility layer** - Temporary compatibility support

### Best Practices
- **Modern C++** - Use C++17 features appropriately
- **Error handling** - Prefer exceptions for error conditions
- **Resource management** - Use RAII and smart pointers
- **Testing** - Write unit tests for custom components

Ready to dive deeper? Check out the [complete API documentation](api.md) for detailed interface specifications and examples.

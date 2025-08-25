# Extending CPP-Scaffold

Learn how to extend CPP-Scaffold with custom templates, plugins, and integrations to meet your specific development needs.

## Overview

CPP-Scaffold is designed with extensibility in mind. You can extend it in several ways:

- **Custom Templates** - Create project templates for your specific needs
- **Template Inheritance** - Extend existing templates with additional features
- **Plugin System** - Add new functionality through plugins
- **IDE Integrations** - Create integrations for your favorite development environment
- **Build System Support** - Add support for new build systems
- **Package Manager Integration** - Integrate with additional package managers

## Custom Templates

### Template Structure

A custom template consists of:

```
my-template/
├── template.json         # Template configuration
├── CMakeLists.txt.template
├── README.md.template
├── src/
│   └── main.cpp.template
├── include/
│   └── {{project_name}}/
│       └── {{project_name}}.h.template
├── tests/
│   └── test_{{project_name}}.cpp.template
└── docs/
    └── api.md.template
```

### Template Configuration

The `template.json` file defines template metadata and variables:

```json
{
    "name": "my-custom-template",
    "description": "My custom project template",
    "version": "1.0.0",
    "author": "Your Name <your.email@example.com>",
    "license": "MIT",
    "category": "application",
    "tags": ["custom", "specialized"],
    "variables": {
        "project_name": {
            "type": "string",
            "description": "Name of the project",
            "required": true,
            "validation": "^[a-zA-Z][a-zA-Z0-9_]*$"
        },
        "namespace": {
            "type": "string",
            "description": "C++ namespace",
            "default": "{{project_name}}",
            "validation": "^[a-zA-Z][a-zA-Z0-9_]*(::[a-zA-Z][a-zA-Z0-9_]*)*$"
        },
        "use_database": {
            "type": "boolean",
            "description": "Include database support",
            "default": false
        },
        "database_type": {
            "type": "choice",
            "description": "Database type",
            "choices": ["postgresql", "mysql", "sqlite"],
            "default": "postgresql",
            "condition": "use_database == true"
        }
    },
    "dependencies": {
        "vcpkg": [
            "spdlog",
            "nlohmann-json"
        ],
        "conan": [
            "spdlog/1.12.0",
            "nlohmann_json/3.11.2"
        ]
    },
    "build_systems": ["cmake", "meson"],
    "platforms": ["windows", "linux", "macos"],
    "languages": ["cpp17", "cpp20"]
}
```

### Template Files

Template files use Jinja2 syntax for variable substitution:

```cpp title="src/main.cpp.template"
#include <iostream>
#include <spdlog/spdlog.h>
{% if use_database %}
#include "{{namespace}}/database/connection.h"
{% endif %}

int main() {
    spdlog::info("Starting {{project_name}}");
    
    {% if use_database %}
    // Initialize database connection
    {{namespace}}::database::Connection db;
    if (!db.connect("{{database_type}}://localhost/{{project_name}}")) {
        spdlog::error("Failed to connect to database");
        return 1;
    }
    {% endif %}
    
    std::cout << "Hello from {{project_name}}!" << std::endl;
    return 0;
}
```

### Template Installation

Install custom templates in the user template directory:

```bash
# Copy template to user directory
cp -r my-template ~/.local/share/cpp-scaffold/templates/

# Or use symbolic link for development
ln -s /path/to/my-template ~/.local/share/cpp-scaffold/templates/my-template

# Verify installation
cpp-scaffold --list-templates
```

## Template Inheritance

### Extending Existing Templates

Create templates that inherit from existing ones:

```json title="enhanced-console/template.json"
{
    "name": "enhanced-console",
    "description": "Console template with additional features",
    "inherits": "console",
    "version": "1.0.0",
    "variables": {
        "include_config_file": {
            "type": "boolean",
            "description": "Include configuration file support",
            "default": true
        },
        "include_database": {
            "type": "boolean", 
            "description": "Include database support",
            "default": false
        }
    },
    "additional_dependencies": {
        "vcpkg": ["toml11"],
        "conan": ["toml11/3.7.1"]
    }
}
```

### Override Files

Override specific files from the parent template:

```
enhanced-console/
├── template.json
├── overrides/
│   ├── src/
│   │   └── main.cpp.template    # Overrides parent main.cpp
│   └── CMakeLists.txt.template  # Overrides parent CMakeLists.txt
└── additions/
    ├── config/
    │   └── app.toml.template    # New file
    └── src/
        └── config_manager.cpp.template  # New file
```

## Plugin System

### Plugin Interface

Create plugins by implementing the plugin interface:

```cpp title="my_plugin.h"
#pragma once
#include <cpp_scaffold/plugin_interface.h>

class MyPlugin : public cpp_scaffold::PluginInterface {
public:
    std::string name() const override { return "MyPlugin"; }
    std::string version() const override { return "1.0.0"; }
    std::string description() const override { return "My custom plugin"; }
    
    bool initialize() override;
    void shutdown() override;
    
    // Hook implementations
    void pre_generation(const ProjectOptions& options) override;
    void post_generation(const ProjectOptions& options) override;
    
    // Command extensions
    std::vector<Command> get_commands() const override;
    
    // Template extensions
    std::vector<std::string> get_template_paths() const override;
};
```

### Plugin Implementation

```cpp title="my_plugin.cpp"
#include "my_plugin.h"
#include <iostream>

bool MyPlugin::initialize() {
    std::cout << "Initializing MyPlugin" << std::endl;
    return true;
}

void MyPlugin::pre_generation(const ProjectOptions& options) {
    // Custom logic before project generation
    if (options.template_name == "my-template") {
        // Perform custom setup
        setup_custom_environment(options);
    }
}

void MyPlugin::post_generation(const ProjectOptions& options) {
    // Custom logic after project generation
    if (options.enable_custom_feature) {
        generate_custom_files(options);
    }
}

std::vector<Command> MyPlugin::get_commands() const {
    return {
        Command{
            .name = "my-command",
            .description = "My custom command",
            .handler = [](const std::vector<std::string>& args) {
                handle_my_command(args);
            }
        }
    };
}

// Plugin registration
extern "C" {
    cpp_scaffold::PluginInterface* create_plugin() {
        return new MyPlugin();
    }
    
    void destroy_plugin(cpp_scaffold::PluginInterface* plugin) {
        delete plugin;
    }
}
```

### Plugin Configuration

```json title="plugin.json"
{
    "name": "MyPlugin",
    "version": "1.0.0",
    "description": "My custom plugin for CPP-Scaffold",
    "author": "Your Name",
    "license": "MIT",
    "api_version": "1.0",
    "dependencies": [],
    "platforms": ["windows", "linux", "macos"],
    "entry_point": "libmyplugin.so",
    "configuration": {
        "enable_feature_x": true,
        "custom_path": "/opt/custom"
    }
}
```

## IDE Integrations

### VSCode Extension

Create a VSCode extension for CPP-Scaffold integration:

```typescript title="extension.ts"
import * as vscode from 'vscode';
import { exec } from 'child_process';

export function activate(context: vscode.ExtensionContext) {
    // Register command
    let disposable = vscode.commands.registerCommand(
        'cpp-scaffold.createProject', 
        async () => {
            const projectName = await vscode.window.showInputBox({
                prompt: 'Enter project name'
            });
            
            const template = await vscode.window.showQuickPick([
                'console', 'lib', 'gui', 'webservice'
            ], {
                placeHolder: 'Select project template'
            });
            
            if (projectName && template) {
                createProject(projectName, template);
            }
        }
    );
    
    context.subscriptions.push(disposable);
}

function createProject(name: string, template: string) {
    const workspaceFolder = vscode.workspace.workspaceFolders?.[0];
    const cwd = workspaceFolder?.uri.fsPath || process.cwd();
    
    const command = `cpp-scaffold ${name} --type ${template}`;
    
    exec(command, { cwd }, (error, stdout, stderr) => {
        if (error) {
            vscode.window.showErrorMessage(`Error: ${error.message}`);
            return;
        }
        
        vscode.window.showInformationMessage(
            `Project ${name} created successfully!`
        );
        
        // Open the new project
        const projectPath = vscode.Uri.file(`${cwd}/${name}`);
        vscode.commands.executeCommand('vscode.openFolder', projectPath);
    });
}
```

### CLion Plugin

Create a CLion plugin for CPP-Scaffold:

```kotlin title="CppScaffoldAction.kt"
class CppScaffoldAction : AnAction() {
    override fun actionPerformed(e: AnActionEvent) {
        val project = e.project ?: return
        
        val dialog = CppScaffoldDialog(project)
        if (dialog.showAndGet()) {
            val options = dialog.getProjectOptions()
            createProject(project, options)
        }
    }
    
    private fun createProject(project: Project, options: ProjectOptions) {
        val command = buildCommand(options)
        
        ProgressManager.getInstance().run(
            object : Task.Backgroundable(project, "Creating CPP-Scaffold Project") {
                override fun run(indicator: ProgressIndicator) {
                    try {
                        val process = Runtime.getRuntime().exec(command)
                        process.waitFor()
                        
                        ApplicationManager.getApplication().invokeLater {
                            if (process.exitValue() == 0) {
                                showSuccessNotification(project, options.name)
                            } else {
                                showErrorNotification(project, "Project creation failed")
                            }
                        }
                    } catch (e: Exception) {
                        showErrorNotification(project, e.message ?: "Unknown error")
                    }
                }
            }
        )
    }
}
```

## Build System Extensions

### Adding New Build System Support

Implement the build system interface:

```cpp title="my_build_system.h"
#pragma once
#include <cpp_scaffold/build_system_interface.h>

class MyBuildSystem : public cpp_scaffold::BuildSystemInterface {
public:
    std::string name() const override { return "my-build-system"; }
    std::string version() const override { return "1.0.0"; }
    
    bool is_available() const override;
    bool generate_build_files(const ProjectOptions& options) override;
    bool configure_project(const ProjectOptions& options) override;
    bool build_project(const std::string& project_path) override;
    
private:
    void generate_build_script(const ProjectOptions& options);
    void generate_config_file(const ProjectOptions& options);
};
```

### Build System Registration

```cpp title="my_build_system.cpp"
#include "my_build_system.h"
#include <cpp_scaffold/build_system_registry.h>

bool MyBuildSystem::is_available() const {
    // Check if build system is installed
    return system("my-build-tool --version > /dev/null 2>&1") == 0;
}

bool MyBuildSystem::generate_build_files(const ProjectOptions& options) {
    generate_build_script(options);
    generate_config_file(options);
    return true;
}

// Register the build system
static bool registered = cpp_scaffold::BuildSystemRegistry::register_build_system(
    std::make_unique<MyBuildSystem>()
);
```

## Package Manager Integration

### Custom Package Manager

```cpp title="my_package_manager.h"
#pragma once
#include <cpp_scaffold/package_manager_interface.h>

class MyPackageManager : public cpp_scaffold::PackageManagerInterface {
public:
    std::string name() const override { return "my-pkg-manager"; }
    
    bool is_available() const override;
    bool initialize_project(const ProjectOptions& options) override;
    bool add_dependency(const std::string& package) override;
    bool install_dependencies() override;
    
private:
    void generate_manifest_file(const ProjectOptions& options);
    void update_build_configuration(const ProjectOptions& options);
};
```

## Testing Extensions

### Custom Test Framework Integration

```cpp title="my_test_framework.h"
#pragma once
#include <cpp_scaffold/test_framework_interface.h>

class MyTestFramework : public cpp_scaffold::TestFrameworkInterface {
public:
    std::string name() const override { return "my-test-framework"; }
    
    bool setup_testing(const ProjectOptions& options) override;
    bool generate_test_files(const ProjectOptions& options) override;
    bool configure_build_system(const ProjectOptions& options) override;
    
private:
    void generate_test_main(const ProjectOptions& options);
    void generate_sample_tests(const ProjectOptions& options);
    void setup_test_discovery(const ProjectOptions& options);
};
```

## Distribution and Sharing

### Template Marketplace

Share templates with the community:

```bash
# Package template for distribution
cpp-scaffold --package-template my-template

# Publish to marketplace
cpp-scaffold --publish-template my-template.cppst

# Install from marketplace
cpp-scaffold --install-template community/awesome-template
```

### Plugin Distribution

```bash
# Package plugin
cpp-scaffold --package-plugin my-plugin

# Install plugin
cpp-scaffold --install-plugin my-plugin.cpps

# List installed plugins
cpp-scaffold --list-plugins
```

## Best Practices

### Template Development
1. **Follow conventions** - Use consistent naming and structure
2. **Comprehensive testing** - Test on all supported platforms
3. **Clear documentation** - Provide usage examples and guides
4. **Version management** - Use semantic versioning

### Plugin Development
1. **Minimal dependencies** - Keep plugin dependencies minimal
2. **Error handling** - Implement robust error handling
3. **Performance** - Avoid blocking operations in main thread
4. **Compatibility** - Ensure compatibility with CPP-Scaffold versions

### Extension Maintenance
1. **Regular updates** - Keep extensions up to date
2. **Community feedback** - Listen to user feedback
3. **Testing** - Maintain comprehensive test suites
4. **Documentation** - Keep documentation current

Ready to extend CPP-Scaffold? Start with creating your first custom template!

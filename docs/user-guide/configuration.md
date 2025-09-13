# Configuration

Learn how to configure CPP-Scaffold, manage profiles, and customize default settings for optimal workflow.

## Configuration Overview

CPP-Scaffold uses a hierarchical configuration system that allows you to:

- Set global defaults for all projects
- Create and manage reusable profiles
- Override settings per project
- Customize template behavior
- Configure IDE and tool integrations

## Configuration Files

### Location

Configuration files are stored in platform-specific directories:

| Platform    | Location                                      |
| ----------- | --------------------------------------------- |
| **Linux**   | `~/.config/cpp-scaffold/`                     |
| **macOS**   | `~/Library/Application Support/cpp-scaffold/` |
| **Windows** | `%APPDATA%\cpp-scaffold\`                     |

### File Structure

```
cpp-scaffold/
├── config.json          # Global configuration
├── profiles.json        # Saved profiles
├── templates/           # Custom templates
│   └── my-template/
└── cache/              # Template cache
    └── downloads/
```

## Global Configuration

### Default Settings

The `config.json` file contains global defaults:

```json
{
  "defaults": {
    "template": "console",
    "build_system": "cmake",
    "package_manager": "vcpkg",
    "test_framework": "gtest",
    "ci_system": "github",
    "editor": "vscode",
    "language": "english"
  },
  "preferences": {
    "verbose": false,
    "auto_git_init": true,
    "auto_open_editor": false,
    "check_updates": true,
    "telemetry": false
  },
  "paths": {
    "default_project_dir": "~/Projects",
    "template_cache_dir": "~/.cache/cpp-scaffold",
    "custom_templates_dir": "~/.local/share/cpp-scaffold/templates"
  }
}
```

### Setting Global Defaults

```bash
# Set default template
cpp-scaffold --set-default template=lib

# Set default build system
cpp-scaffold --set-default build_system=meson

# Set default package manager
cpp-scaffold --set-default package_manager=conan

# View current defaults
cpp-scaffold --show-defaults
```

### Environment Variables

Override configuration with environment variables:

```bash
# Default template
export CPP_SCAFFOLD_DEFAULT_TEMPLATE=console

# Default build system
export CPP_SCAFFOLD_DEFAULT_BUILD=cmake

# Configuration directory
export CPP_SCAFFOLD_CONFIG_DIR=~/.config/cpp-scaffold

# Disable telemetry
export CPP_SCAFFOLD_TELEMETRY=false
```

## Profile Management

### Creating Profiles

Profiles save frequently used configurations:

```bash
# Save current settings as a profile
cpp-scaffold --save-profile webdev \
  --type webservice \
  --build cmake \
  --package vcpkg \
  --test gtest \
  --ci github \
  --editor vscode

# Interactive profile creation
cpp-scaffold --create-profile
```

### Using Profiles

```bash
# Use a saved profile
cpp-scaffold MyProject --profile webdev

# Override profile settings
cpp-scaffold MyProject --profile webdev --package conan

# List available profiles
cpp-scaffold --list-profiles
```

### Profile Configuration

Profiles are stored in `profiles.json`:

```json
{
  "webdev": {
    "description": "Web development profile",
    "template": "webservice",
    "build_system": "cmake",
    "package_manager": "vcpkg",
    "test_framework": "gtest",
    "ci_system": "github",
    "editor": "vscode",
    "options": {
      "database": "postgresql",
      "auth": "jwt",
      "cors": true,
      "docker": true
    }
  },
  "embedded": {
    "description": "Embedded development profile",
    "template": "embedded",
    "build_system": "cmake",
    "package_manager": "none",
    "test_framework": "catch2",
    "ci_system": "gitlab",
    "editor": "clion",
    "options": {
      "target": "arm-cortex-m4",
      "optimization": "size",
      "rtos": "freertos"
    }
  },
  "gamedev": {
    "description": "Game development profile",
    "template": "gui",
    "build_system": "cmake",
    "package_manager": "vcpkg",
    "test_framework": "gtest",
    "ci_system": "github",
    "editor": "vs",
    "options": {
      "framework": "sfml",
      "graphics": "opengl",
      "audio": true
    }
  }
}
```

### Managing Profiles

```bash
# Edit profile
cpp-scaffold --edit-profile webdev

# Delete profile
cpp-scaffold --delete-profile old-profile

# Export profile
cpp-scaffold --export-profile webdev > webdev-profile.json

# Import profile
cpp-scaffold --import-profile webdev-profile.json
```

## Template Configuration

### Template-Specific Settings

Configure behavior for specific templates:

```json
{
  "templates": {
    "console": {
      "default_namespace": "{{project_name}}",
      "include_logging": true,
      "include_config": true,
      "argument_parser": "argparse"
    },
    "lib": {
      "default_namespace": "{{project_name}}",
      "generate_pkg_config": true,
      "include_examples": true,
      "api_documentation": "doxygen"
    },
    "webservice": {
      "default_port": 8080,
      "default_database": "postgresql",
      "include_swagger": true,
      "cors_enabled": true
    }
  }
}
```

### Custom Template Paths

Add custom template directories:

```bash
# Add custom template directory
cpp-scaffold --add-template-path ~/my-templates

# List template paths
cpp-scaffold --list-template-paths

# Remove template path
cpp-scaffold --remove-template-path ~/old-templates
```

## IDE Integration

### VSCode Configuration

```json
{
  "ide_configs": {
    "vscode": {
      "extensions": [
        "ms-vscode.cpptools",
        "ms-vscode.cmake-tools",
        "ms-vscode.cpptools-extension-pack"
      ],
      "settings": {
        "C_Cpp.default.cppStandard": "c++17",
        "cmake.configureOnOpen": true,
        "files.associations": {
          "*.hpp": "cpp"
        }
      },
      "launch_configs": {
        "console": "launch_console.json",
        "lib": "launch_tests.json"
      }
    }
  }
}
```

### CLion Configuration

```json
{
  "ide_configs": {
    "clion": {
      "cmake_options": "-DCMAKE_BUILD_TYPE=Debug",
      "toolchain": "default",
      "build_directory": "cmake-build-debug",
      "code_style": "Google"
    }
  }
}
```

## Build System Configuration

### CMake Settings

```json
{
  "build_systems": {
    "cmake": {
      "minimum_version": "3.14",
      "default_generator": "Ninja",
      "default_build_type": "Release",
      "options": {
        "BUILD_TESTING": true,
        "BUILD_SHARED_LIBS": false,
        "CMAKE_EXPORT_COMPILE_COMMANDS": true
      }
    }
  }
}
```

### Meson Settings

```json
{
  "build_systems": {
    "meson": {
      "default_buildtype": "release",
      "default_backend": "ninja",
      "options": {
        "warning_level": "3",
        "werror": false,
        "cpp_std": "c++17"
      }
    }
  }
}
```

## Package Manager Configuration

### vcpkg Settings

```json
{
  "package_managers": {
    "vcpkg": {
      "root": "/opt/vcpkg",
      "default_triplet": "x64-linux",
      "manifest_mode": true,
      "binary_caching": true,
      "common_dependencies": ["spdlog", "nlohmann-json", "fmt"]
    }
  }
}
```

### Conan Settings

```json
{
  "package_managers": {
    "conan": {
      "profile": "default",
      "build_missing": true,
      "remote": "conancenter",
      "common_dependencies": [
        "spdlog/1.12.0",
        "nlohmann_json/3.11.2",
        "fmt/10.1.1"
      ]
    }
  }
}
```

## Advanced Configuration

### Hooks and Scripts

Configure pre/post generation hooks:

```json
{
  "hooks": {
    "pre_generation": [
      "echo 'Starting project generation...'",
      "mkdir -p ~/Projects/{{project_name}}"
    ],
    "post_generation": [
      "cd {{project_path}} && git init",
      "cd {{project_path}} && git add .",
      "cd {{project_path}} && git commit -m 'Initial commit'"
    ]
  }
}
```

### Custom Variables

Define custom variables for templates:

```json
{
  "variables": {
    "company_name": "My Company",
    "copyright_year": "2024",
    "license_header": "// Copyright (c) {{copyright_year}} {{company_name}}",
    "default_author": "John Doe",
    "default_email": "john@example.com"
  }
}
```

## Configuration Commands

### Viewing Configuration

```bash
# Show all configuration
cpp-scaffold --show-config

# Show specific section
cpp-scaffold --show-config defaults

# Show profile configuration
cpp-scaffold --show-profile webdev
```

### Editing Configuration

```bash
# Edit global configuration
cpp-scaffold --edit-config

# Edit specific profile
cpp-scaffold --edit-profile webdev

# Reset to defaults
cpp-scaffold --reset-config
```

### Validation

```bash
# Validate configuration
cpp-scaffold --validate-config

# Check profile validity
cpp-scaffold --validate-profile webdev

# Test template with configuration
cpp-scaffold --test-template console --profile webdev
```

## Troubleshooting Configuration

### Common Issues

| Issue                          | Solution                         |
| ------------------------------ | -------------------------------- |
| Configuration not found        | Check file paths and permissions |
| Profile not loading            | Validate JSON syntax             |
| Template variables not working | Check variable names and syntax  |
| IDE integration failing        | Verify IDE-specific settings     |

### Debug Configuration

```bash
# Enable verbose configuration logging
cpp-scaffold --verbose --show-config

# Test configuration loading
cpp-scaffold --debug-config

# Validate all profiles
cpp-scaffold --validate-all-profiles
```

### Reset Configuration

```bash
# Reset global configuration
cpp-scaffold --reset-config

# Reset specific profile
cpp-scaffold --reset-profile webdev

# Reset all profiles
cpp-scaffold --reset-all-profiles
```

## Best Practices

### Configuration Management

1. **Use profiles** for different project types
2. **Version control** your configuration files
3. **Document custom settings** for team members
4. **Test configurations** before sharing

### Profile Design

1. **Meaningful names** - Use descriptive profile names
2. **Logical grouping** - Group related settings together
3. **Minimal overrides** - Only override necessary settings
4. **Regular maintenance** - Keep profiles up to date

### Team Collaboration

1. **Shared profiles** - Create team-specific profiles
2. **Consistent settings** - Align on common configurations
3. **Documentation** - Document custom configurations
4. **Version control** - Track configuration changes

Ready to customize your CPP-Scaffold experience? Start with creating your first profile!

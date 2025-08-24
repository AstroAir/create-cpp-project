# CPP-Scaffold Usage Guide

This comprehensive guide covers all aspects of using CPP-Scaffold to create and manage C++ projects.

## 📋 Table of Contents

- [Getting Started](#getting-started)
- [Command Line Interface](#command-line-interface)
- [Interactive Mode](#interactive-mode)
- [Project Templates](#project-templates)
- [Configuration Profiles](#configuration-profiles)
- [Advanced Usage](#advanced-usage)
- [Examples](#examples)
- [Best Practices](#best-practices)

## 🚀 Getting Started

### Basic Syntax

```bash
cpp-scaffold [PROJECT_NAME] [OPTIONS]
```

### Your First Project

Create a simple console application:

```bash
# Interactive mode (recommended for beginners)
cpp-scaffold

# Direct command
cpp-scaffold MyFirstApp --type console --build cmake --package vcpkg
```

### Quick Examples

```bash
# Console application with testing
cpp-scaffold MyApp --type console --test gtest

# Library with documentation
cpp-scaffold MyLib --type lib --test catch2 --docs

# GUI application with Qt
cpp-scaffold MyGuiApp --type gui --framework qt --editor vscode

# Web service with full setup
cpp-scaffold MyAPI --type webservice --build cmake --package conan --test gtest --ci github
```

## 🖥️ Command Line Interface

### Core Options

| Option | Short | Description | Values | Default |
|--------|-------|-------------|--------|---------|
| `--type` | `-t` | Project template | `console`, `lib`, `header-only-lib`, `gui`, `network`, `embedded`, `webservice`, `multi-executable` | `console` |
| `--build` | `-b` | Build system | `cmake`, `meson`, `bazel`, `xmake`, `premake`, `make`, `ninja` | `cmake` |
| `--package` | `-p` | Package manager | `vcpkg`, `conan`, `spack`, `hunter`, `none` | `vcpkg` |
| `--test` | | Testing framework | `gtest`, `catch2`, `doctest`, `boost`, `none` | `none` |
| `--ci` | | CI/CD system | `github`, `gitlab`, `travis`, `appveyor`, `azuredevops`, `circleci` | `none` |
| `--editor` | | Editor config | `vscode`, `clion`, `vs`, `vim`, `emacs`, `sublime` | `none` |
| `--lang` | `-l` | Interface language | `english`, `chinese`, `spanish`, `japanese`, `german`, `french` | `english` |
| `--verbose` | `-v` | Verbose output | - | `false` |
| `--help` | `-h` | Show help | - | - |
| `--version` | | Show version | - | - |

### Advanced Options

| Option | Description | Example |
|--------|-------------|---------|
| `--profile <name>` | Use saved profile | `--profile webdev` |
| `--save-profile <name>` | Save current config | `--save-profile mysetup` |
| `--list-profiles` | List profiles | - |
| `--custom-template <path>` | Custom template | `--custom-template ./my-template` |
| `--network-lib <lib>` | Network library | `--network-lib asio` |
| `--framework <name>` | GUI framework | `--framework qt` |
| `--no-git` | Skip Git init | - |
| `--no-tests` | Skip tests | - |
| `--no-docs` | Skip docs | - |

### Boolean Flags

```bash
# Enable features
cpp-scaffold MyProject --type console --git --tests --docs --verbose

# Disable features
cpp-scaffold MyProject --type console --no-git --no-tests --no-docs
```

### Multiple Values

```bash
# Multiple CI systems
cpp-scaffold MyProject --ci github,gitlab,travis

# Multiple editors
cpp-scaffold MyProject --editor vscode,clion

# Multiple options
cpp-scaffold MyProject --type gui --framework qt --editor vscode,clion --ci github,gitlab
```

## 🎯 Interactive Mode

### Basic Interactive Mode

```bash
# Launch interactive wizard
cpp-scaffold

# Follow the prompts:
# 1. Project name
# 2. Template type
# 3. Build system
# 4. Package manager
# 5. Testing framework
# 6. Additional options
```

### Wizard Modes

```bash
# Quick start wizard (smart defaults)
cpp-scaffold --wizard quick

# Advanced wizard (all options)
cpp-scaffold --wizard advanced

# Template selection wizard
cpp-scaffold --wizard templates

# Configuration wizard
cpp-scaffold --config
```

### Interactive Features

- **Smart Defaults**: Based on project type and system detection
- **Validation**: Real-time option validation and suggestions
- **Help**: Contextual help for each option
- **Preview**: Show what will be created before confirmation
- **Progress**: Visual progress indicators during creation

## 🏗️ Project Templates

### Console Applications

```bash
# Basic console app
cpp-scaffold MyTool --type console

# Console app with full setup
cpp-scaffold MyTool --type console --build cmake --package vcpkg --test gtest --ci github --editor vscode

# Cross-platform console app
cpp-scaffold MyTool --type console --build cmake --package conan --test catch2
```

**Generated Structure:**
```
MyTool/
├── src/
│   ├── main.cpp
│   └── MyTool/
│       ├── MyTool.h
│       └── MyTool.cpp
├── tests/
│   └── test_MyTool.cpp
├── CMakeLists.txt
├── README.md
└── .gitignore
```

### Libraries

```bash
# Static library
cpp-scaffold MyLib --type lib --build cmake --package vcpkg

# Header-only library
cpp-scaffold MyHeaderLib --type header-only-lib --test doctest --docs

# Shared library with versioning
cpp-scaffold MySharedLib --type lib --build cmake --package conan --shared
```

**Generated Structure:**
```
MyLib/
├── include/
│   └── MyLib/
│       └── MyLib.h
├── src/
│   └── MyLib.cpp
├── tests/
│   └── test_MyLib.cpp
├── examples/
│   └── example.cpp
├── CMakeLists.txt
└── README.md
```

### GUI Applications

```bash
# Qt application
cpp-scaffold MyGuiApp --type gui --framework qt --build cmake --package vcpkg

# SFML game
cpp-scaffold MyGame --type gui --framework sfml --build cmake --package conan

# Cross-platform GUI
cpp-scaffold MyApp --type gui --framework imgui --build cmake
```

### Network Services

```bash
# HTTP server
cpp-scaffold MyServer --type network --network-lib asio --build cmake --package vcpkg

# TCP/UDP service
cpp-scaffold MyService --type network --network-lib boost --test gtest

# Web service
cpp-scaffold MyAPI --type webservice --build cmake --package conan --test catch2
```

### Embedded Projects

```bash
# Microcontroller project
cpp-scaffold MyEmbedded --type embedded --build cmake --no-package

# IoT device
cpp-scaffold MyIoT --type embedded --build cmake --test doctest --no-ci
```

## 📁 Configuration Profiles

### Creating Profiles

```bash
# Save current configuration as profile
cpp-scaffold MyProject --type gui --framework qt --build cmake --package vcpkg --test gtest --save-profile qt-dev

# Create profile interactively
cpp-scaffold --config --save-profile web-dev
```

### Using Profiles

```bash
# List available profiles
cpp-scaffold --list-profiles

# Use a profile
cpp-scaffold MyNewProject --profile qt-dev

# Override profile options
cpp-scaffold MyProject --profile qt-dev --test catch2
```

### Built-in Profiles

| Profile | Description | Configuration |
|---------|-------------|---------------|
| `minimal` | Minimal setup | Console, CMake, no packages, no tests |
| `standard` | Standard C++ project | Console, CMake, vcpkg, GTest |
| `library` | Library development | Library, CMake, vcpkg, GTest, docs |
| `gui-qt` | Qt application | GUI, Qt, CMake, vcpkg, GTest |
| `web-service` | Web service | WebService, CMake, Conan, GTest, GitHub CI |

### Profile Management

```bash
# Edit profile
cpp-scaffold --edit-profile qt-dev

# Delete profile
cpp-scaffold --delete-profile old-profile

# Export profile
cpp-scaffold --export-profile qt-dev > qt-dev.json

# Import profile
cpp-scaffold --import-profile qt-dev.json
```

## 🔧 Advanced Usage

### Custom Templates

```bash
# Use custom template directory
cpp-scaffold MyProject --custom-template ./my-templates/special-template

# Create custom template
cpp-scaffold --create-template my-template --base console
```

### Environment Variables

```bash
# Set default options via environment
export CPP_SCAFFOLD_BUILD_SYSTEM=cmake
export CPP_SCAFFOLD_PACKAGE_MANAGER=vcpkg
export CPP_SCAFFOLD_TEST_FRAMEWORK=gtest

cpp-scaffold MyProject  # Uses environment defaults
```

### Configuration Files

```bash
# Global configuration file
~/.config/cpp-scaffold/config.json  # Linux/macOS
%APPDATA%\cpp-scaffold\config.json  # Windows

# Project-specific configuration
.cpp-scaffold.json  # In project directory
```

### Batch Operations

```bash
# Create multiple projects from list
cpp-scaffold --batch projects.txt

# projects.txt format:
# ProjectName1 --type console --test gtest
# ProjectName2 --type lib --package conan
# ProjectName3 --type gui --framework qt
```

### Integration with IDEs

```bash
# Generate IDE project files
cpp-scaffold MyProject --type console --editor vscode --generate-ide

# Open in IDE after creation
cpp-scaffold MyProject --type gui --framework qt --editor clion --open
```

## 📚 Examples

### Example 1: Command-Line Tool

```bash
# Create a command-line utility with argument parsing
cpp-scaffold CLITool --type console --build cmake --package vcpkg --test gtest --ci github --editor vscode

cd CLITool
# Edit src/main.cpp to add CLI argument parsing
# Build and test
mkdir build && cd build
cmake .. -G Ninja
ninja
ctest
```

### Example 2: Cross-Platform Library

```bash
# Create a cross-platform library
cpp-scaffold MathLib --type lib --build cmake --package conan --test catch2 --docs --ci github,appveyor --editor vscode,clion

cd MathLib
# Implement library in src/
# Add tests in tests/
# Build and package
mkdir build && cd build
cmake .. -G Ninja -DCMAKE_BUILD_TYPE=Release
ninja
ctest
ninja package
```

### Example 3: Qt Desktop Application

```bash
# Create Qt application with full setup
cpp-scaffold MyQtApp --type gui --framework qt --build cmake --package vcpkg --test gtest --ci github --editor vscode

cd MyQtApp
# Design UI with Qt Designer
# Implement application logic
# Build and run
mkdir build && cd build
cmake .. -G Ninja
ninja
./MyQtApp
```

### Example 4: Web Service API

```bash
# Create REST API service
cpp-scaffold MyAPI --type webservice --build cmake --package conan --test gtest --ci github --editor vscode

cd MyAPI
# Implement REST endpoints
# Add database integration
# Build and deploy
mkdir build && cd build
cmake .. -G Ninja -DCMAKE_BUILD_TYPE=Release
ninja
./MyAPI --port 8080
```

### Example 5: Header-Only Library

```bash
# Create header-only template library
cpp-scaffold TemplateLib --type header-only-lib --test doctest --docs --ci github --editor vscode

cd TemplateLib
# Implement templates in include/
# Add comprehensive tests
# Generate documentation
mkdir build && cd build
cmake .. -G Ninja
ninja
ctest
ninja docs
```

## 🎯 Best Practices

### Project Naming

```bash
# Good project names
cpp-scaffold MyLibrary --type lib
cpp-scaffold my-tool --type console
cpp-scaffold MyGuiApp --type gui

# Avoid special characters and spaces
# Use PascalCase or kebab-case consistently
```

### Template Selection

- **Console**: For CLI tools, utilities, and simple applications
- **Library**: For reusable components and SDKs
- **Header-Only-Lib**: For template libraries and utilities
- **GUI**: For desktop applications with user interfaces
- **Network**: For servers, clients, and network services
- **WebService**: For REST APIs and web backends
- **Embedded**: For microcontroller and IoT projects

### Build System Choice

- **CMake**: Most versatile, best ecosystem support
- **Meson**: Fast builds, good for large projects
- **Bazel**: Google's build system, excellent for monorepos
- **XMake**: Lua-based, simple configuration

### Package Manager Selection

- **vcpkg**: Microsoft's package manager, excellent Windows support
- **Conan**: Cross-platform, good for complex dependencies
- **Spack**: Scientific computing focus
- **Hunter**: CMake-based, good for CMake projects

### Testing Framework Choice

- **Google Test**: Industry standard, comprehensive features
- **Catch2**: Header-only, easy to integrate
- **Doctest**: Lightweight, fast compilation
- **Boost.Test**: Part of Boost ecosystem

### Workflow Recommendations

1. **Start with Interactive Mode**: Use `cpp-scaffold` without arguments for first-time setup
2. **Create Profiles**: Save frequently used configurations as profiles
3. **Use Version Control**: Always initialize Git repositories
4. **Enable Testing**: Include testing framework from the start
5. **Set Up CI/CD**: Configure continuous integration early
6. **Document Everything**: Enable documentation generation
7. **Follow Conventions**: Use consistent naming and structure

---

For more information, see the [Installation Guide](INSTALLATION.md), [Architecture Documentation](ARCHITECTURE.md), or visit our [GitHub repository](https://github.com/cpp-scaffold/cpp-scaffold).

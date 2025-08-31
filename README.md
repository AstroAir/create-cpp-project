# CPP-Scaffold

[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)
[![Build Status](https://img.shields.io/badge/build-passing-brightgreen.svg)](https://github.com/cpp-scaffold/cpp-scaffold)
[![C++17](https://img.shields.io/badge/C%2B%2B-17-blue.svg)](https://en.cppreference.com/w/cpp/17)
[![CMake](https://img.shields.io/badge/CMake-3.14+-blue.svg)](https://cmake.org/)

A powerful, modern C++ project scaffolding tool that helps developers quickly create well-structured C++ projects with industry best practices, modern build systems, package managers, testing frameworks, and development tools.

## 🚀 Quick Start

### Installation

**Quick Install (Recommended):**

```bash
# Linux/macOS
curl -sSL https://raw.githubusercontent.com/cpp-scaffold/cpp-scaffold/main/scripts/web-install.sh | bash

# Windows (PowerShell)
iwr -useb https://raw.githubusercontent.com/cpp-scaffold/cpp-scaffold/main/scripts/install.ps1 | iex
```

**Package Managers:**

```bash
# Homebrew (macOS/Linux)
brew install cpp-scaffold

# Chocolatey (Windows)
choco install cpp-scaffold

# Winget (Windows)
winget install cpp-scaffold
```

**Build from Source:**

```bash
# Clone and build
git clone https://github.com/cpp-scaffold/cpp-scaffold.git
cd cpp-scaffold
cmake --preset release
cmake --build --preset release
cmake --install build --prefix /usr/local
```

📖 **[Complete Installation Guide](docs/installation/README.md)**

### Create Your First Project

```bash
# Interactive mode (recommended)
./cpp-scaffold

# Quick project creation
./cpp-scaffold MyProject --type console --build cmake --package vcpkg --test gtest

# Advanced setup
./cpp-scaffold MyApp --type gui --framework qt --build cmake --package conan --test gtest --ci github --editor vscode
```

## 🏗️ Features

- **🎯 Multiple Project Templates**: Console apps, libraries, GUI applications, network services, embedded systems, web services
- **🔧 Build System Support**: CMake, Meson, Bazel, XMake, Premake, Make, Ninja
- **📦 Package Managers**: vcpkg, Conan, Spack, Hunter with automatic dependency management
- **🧪 Testing Frameworks**: Google Test, Catch2, Doctest, Boost.Test with CI integration
- **🎨 IDE Integration**: VSCode, CLion, Visual Studio, Vim, Emacs with project-specific configurations
- **🔄 CI/CD Setup**: GitHub Actions, GitLab CI, Travis CI, AppVeyor, Azure DevOps, CircleCI
- **🌍 Multi-language Support**: English, Chinese, Spanish, Japanese, German, French
- **📊 Interactive Wizard**: Step-by-step project configuration with styled terminal output

## 📚 Documentation

Our comprehensive documentation is built with MkDocs and provides detailed guides, tutorials, and API references.

### 🌐 Online Documentation

**📖 [View Documentation](https://cpp-scaffold.github.io/cpp-scaffold/)** - Complete online documentation

### 🏠 Local Documentation

Build and serve the documentation locally:

```bash
# Install MkDocs and dependencies
pip install -r requirements.txt

# Serve documentation locally
mkdocs serve

# Build static documentation
mkdocs build
```

### 📋 Quick Links

- **[Getting Started](docs/getting-started/index.md)** - Installation and first project
- **[User Guide](docs/user-guide/index.md)** - Complete usage documentation
- **[Tutorials](docs/tutorials/index.md)** - Step-by-step examples
- **[Developer Guide](docs/developer-guide/index.md)** - Contributing and extending
- **[API Reference](docs/api-reference/index.md)** - Complete API documentation

### Quick Links

| For | Documentation | Description |
|-----|---------------|-------------|
| **New Users** | [Getting Started](docs/getting-started/index.md) | Installation and first project |
| **All Users** | [User Guide](docs/user-guide/index.md) | Complete usage documentation |
| **Developers** | [API Reference](docs/api-reference/index.md) | Complete API documentation |
| **Contributors** | [Developer Guide](docs/developer-guide/index.md) | Contributing and extending |
| **Troubleshooting** | [Troubleshooting](docs/user-guide/troubleshooting.md) | Common issues and solutions |

### Documentation Structure

The documentation is organized using MkDocs with the following structure:

```text
docs/                           # MkDocs documentation source
├── index.md                    # Documentation homepage
├── getting-started/            # Getting started guides
│   ├── installation.md         # Installation instructions
│   ├── quick-start.md          # Quick start guide
│   └── first-project.md        # First project tutorial
├── user-guide/                 # User documentation
│   ├── usage.md                # Usage guide and examples
│   ├── templates.md            # Project templates guide
│   ├── configuration.md        # Configuration management
│   └── troubleshooting.md      # Common issues and solutions
├── tutorials/                  # Step-by-step tutorials
│   ├── console-app.md          # Console application tutorial
│   ├── library-project.md      # Library project tutorial
│   ├── gui-application.md      # GUI application tutorial
│   └── advanced-features.md    # Advanced features tutorial
├── developer-guide/            # Developer documentation
│   ├── architecture.md         # System architecture
│   ├── building.md             # Build system documentation
│   ├── contributing.md         # Contributing guidelines
│   └── extending.md            # Extending CPP-Scaffold
├── components/                 # Component documentation
│   ├── cli-parser.md           # CLI parser component
│   ├── template-system.md      # Template system component
│   └── configuration-system.md # Configuration system component
├── api-reference/              # API documentation
│   └── api.md                  # Complete API reference
└── reference/                  # Additional references
    └── enhancements.md         # Recent improvements

mkdocs.yml                      # MkDocs configuration
requirements.txt                # Python dependencies for docs
```

## 🎯 Project Templates

| Template | Description | Use Case |
|----------|-------------|----------|
| `console` | Command-line application | CLI tools, utilities, system tools |
| `lib` | Static/shared library | Reusable components, SDKs |
| `header-only-lib` | Header-only library | Template libraries, utilities |
| `gui` | GUI application | Desktop applications |
| `network` | Network service | Servers, clients, APIs |
| `embedded` | Embedded system | IoT, microcontrollers |
| `webservice` | Web service | REST APIs, web backends |
| `custom` | User-defined template | Specialized projects |

## 💡 Examples

### Quick Start Examples

#### Console Application
Create a command-line tool with modern C++ practices:

```bash
# Basic console app
cpp-scaffold MyTool --type console --build cmake --package vcpkg --test gtest

# Advanced console app with CI/CD and editor setup
cpp-scaffold FileProcessor \
  --type console \
  --build cmake \
  --package vcpkg \
  --test gtest \
  --ci github \
  --editor vscode \
  --std cpp20 \
  --docs
```

**Generated structure:**
```
FileProcessor/
├── CMakeLists.txt          # CMake build configuration
├── vcpkg.json             # Package dependencies
├── .github/workflows/     # GitHub Actions CI/CD
├── .vscode/              # VSCode configuration
├── src/main.cpp          # Application entry point
├── include/              # Public headers
├── tests/                # Unit tests with GTest
└── docs/                 # Documentation
```

#### C++ Library
Create a reusable library with comprehensive tooling:

```bash
# Static/shared library
cpp-scaffold MathLib \
  --type lib \
  --build cmake \
  --package conan \
  --test catch2 \
  --docs \
  --ci github,gitlab \
  --editor clion

# Header-only library
cpp-scaffold UtilsLib \
  --type header-only-lib \
  --build cmake \
  --package vcpkg \
  --test doctest \
  --std cpp20
```

**Features included:**
- Modern CMake with proper target exports
- Package manager integration (Conan/vcpkg)
- Comprehensive testing setup
- API documentation generation
- CI/CD for multiple platforms

#### Qt GUI Application
Build desktop applications with Qt framework:

```bash
# Qt6 application with modern setup
cpp-scaffold MyGuiApp \
  --type gui \
  --framework qt \
  --build cmake \
  --package vcpkg \
  --test gtest \
  --editor vscode \
  --ci github

# Qt application with custom widgets
cpp-scaffold ImageEditor \
  --type qtapp \
  --build cmake \
  --package conan \
  --std cpp20 \
  --docs
```

#### Network Service
Create high-performance network applications:

```bash
# REST API server
cpp-scaffold MyAPI \
  --type webservice \
  --build cmake \
  --package conan \
  --test gtest \
  --ci github,gitlab \
  --network-lib boost

# TCP/UDP network service
cpp-scaffold NetworkService \
  --type network \
  --build cmake \
  --package vcpkg \
  --test catch2 \
  --network-lib asio
```

### Complete Project Walkthroughs

#### Example 1: File Processing Utility

```bash
# Create the project
cpp-scaffold FileProcessor \
  --type console \
  --build cmake \
  --package vcpkg \
  --test gtest \
  --ci github \
  --editor vscode \
  --std cpp17 \
  --docs

cd FileProcessor

# Build and test
mkdir build && cd build
cmake .. -G Ninja
ninja
ctest

# Run the application
./FileProcessor --help
./FileProcessor --count *.txt
```

**What you get:**
- Complete CLI argument parsing
- File I/O operations with error handling
- Unit tests with Google Test
- GitHub Actions CI/CD
- VSCode debugging configuration
- Documentation generation

#### Example 2: Math Library with Conan

```bash
# Create library project
cpp-scaffold MathLib \
  --type lib \
  --build cmake \
  --package conan \
  --test catch2 \
  --docs \
  --std cpp20

cd MathLib

# Install dependencies and build
conan install . --build=missing
cmake --preset conan-default
cmake --build --preset conan-release

# Run tests
ctest --preset conan-release
```

**Features demonstrated:**
- Modern C++20 modules (optional)
- Conan package management
- Catch2 testing framework
- Doxygen documentation
- CMake package config generation

#### Example 3: Qt Desktop Application

```bash
# Create Qt application
cpp-scaffold ImageViewer \
  --type gui \
  --framework qt \
  --build cmake \
  --package vcpkg \
  --test gtest \
  --editor vscode

cd ImageViewer

# Install Qt and build
vcpkg install qt6-base qt6-imageformats
cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=vcpkg/scripts/buildsystems/vcpkg.cmake
cmake --build build

# Run the application
./build/ImageViewer
```

**Includes:**
- Qt6 integration with CMake
- Modern Qt widgets and layouts
- Image processing capabilities
- Cross-platform deployment
- Unit testing for Qt components

## 🤝 Contributing

We welcome contributions! Please see our [Contributing Guide](documents/developer-guide/CONTRIBUTING.md) for details on:

- Code of conduct
- Development setup
- Coding standards
- Pull request process
- Issue reporting

## 📄 License

This project is licensed under the GPL v3 License - see the [LICENSE](LICENSE) file for details.

## 🔗 Links

- **📖 [Documentation](documents/README.md)** - Complete documentation
- **🐛 [Issues](https://github.com/cpp-scaffold/cpp-scaffold/issues)** - Bug reports and feature requests
- **💬 [Discussions](https://github.com/cpp-scaffold/cpp-scaffold/discussions)** - Questions and community support
- **🚀 [Releases](https://github.com/cpp-scaffold/cpp-scaffold/releases)** - Latest releases and changelogs

## ⭐ Support

If you find CPP-Scaffold useful, please consider:

- ⭐ **Starring** the repository
- 🐛 **Reporting** bugs and issues
- 💡 **Suggesting** new features
- 🤝 **Contributing** code or documentation
- 💬 **Sharing** with other developers

---

**Get started today**: [Installation Guide](documents/user-guide/INSTALLATION.md) → [Usage Guide](documents/user-guide/USAGE.md) → [Create your first project!](documents/user-guide/USAGE.md#your-first-project)

# CPP-Scaffold (Documents Version - Backup)

[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)
[![Build Status](https://img.shields.io/badge/build-passing-brightgreen.svg)](https://github.com/cpp-scaffold/cpp-scaffold)
[![C++17](https://img.shields.io/badge/C%2B%2B-17-blue.svg)](https://en.cppreference.com/w/cpp/17)
[![CMake](https://img.shields.io/badge/CMake-3.14+-blue.svg)](https://cmake.org/)

Welcome to CPP-Scaffold, a powerful, modern C++ project scaffolding tool that helps developers quickly create well-structured C++ projects with industry best practices, modern build systems, package managers, testing frameworks, and development tools.

## 🚀 Features

### Core Capabilities

- 🏗️ **Multiple Project Templates**: Console apps, libraries, GUI applications, network services, embedded systems, web services, and more
- 🔧 **Build System Support**: CMake, Meson, Bazel, XMake, Premake, Make, Ninja with optimized configurations
- 📦 **Package Managers**: vcpkg, Conan, Spack, Hunter with automatic dependency management
- 🧪 **Testing Frameworks**: Google Test, Catch2, Doctest, Boost.Test with CI integration
- 🎨 **IDE Integration**: VSCode, CLion, Visual Studio, Vim, Emacs with project-specific configurations
- 🔄 **CI/CD Setup**: GitHub Actions, GitLab CI, Travis CI, AppVeyor, Azure DevOps, CircleCI
- 🌍 **Multi-language Support**: English, Chinese, Spanish, Japanese, German, French
- 📊 **Interactive Wizard**: Step-by-step project configuration with styled terminal output

### Advanced Features

- ⚙️ **Configuration Management**: Save and reuse project profiles with hierarchical preferences
- 🎯 **Framework Integration**: Automatic setup for Qt, SFML, Boost, OpenCV, Dear ImGui, and more
- 📝 **Documentation Generation**: Automated README, API docs, and user guides in multiple formats
- 🔍 **Code Quality Tools**: Integrated clang-format, clang-tidy, and static analysis
- 🎨 **Enhanced Terminal Experience**: Progress indicators, interactive menus, and colored output
- 🔧 **Custom Templates**: User-defined templates with inheritance and variable substitution
- 🚦 **Git Integration**: Comprehensive .gitignore, .gitattributes, and Git hooks setup

## 📚 Documentation Structure

This documentation is organized into several categories to help you find the information you need:

### 👥 For Users

| Document | Description | Location |
|----------|-------------|----------|
| **[Installation Guide](user-guide/INSTALLATION.md)** | Complete installation instructions for all platforms | `user-guide/` |
| **[Usage Guide](user-guide/USAGE.md)** | Comprehensive usage examples and command reference | `user-guide/` |
| **[Troubleshooting](user-guide/TROUBLESHOOTING.md)** | Common issues and solutions | `user-guide/` |

### 👨‍💻 For Developers

| Document | Description | Location |
|----------|-------------|----------|
| **[Architecture Guide](developer-guide/ARCHITECTURE.md)** | System design and component interactions | `developer-guide/` |
| **[Build System](developer-guide/BUILD.md)** | Build process and configuration options | `developer-guide/` |
| **[Contributing Guide](developer-guide/CONTRIBUTING.md)** | Guidelines for contributors | `developer-guide/` |

### 📖 Reference Documentation

| Document | Description | Location |
|----------|-------------|----------|
| **[API Reference](api-reference/API.md)** | Complete API documentation | `api-reference/` |
| **[Component Documentation](components/README.md)** | Detailed component documentation | `components/` |
| **[Enhancement History](reference/ENHANCEMENTS.md)** | Recent improvements and features | `reference/` |

## 🎯 Quick Navigation

### By User Type

| User Type | Start Here | Then Read |
|-----------|------------|-----------|
| **New Users** | [Installation Guide](user-guide/INSTALLATION.md) | [Usage Guide](user-guide/USAGE.md) |
| **Power Users** | [Usage Guide](user-guide/USAGE.md) | [API Reference](api-reference/API.md) |
| **Contributors** | [Architecture Guide](developer-guide/ARCHITECTURE.md) | [Contributing Guide](developer-guide/CONTRIBUTING.md) |
| **Maintainers** | [Build System](developer-guide/BUILD.md) | [Component Documentation](components/README.md) |

### By Task

| Task | Documentation |
|------|---------------|
| **Install CPP-Scaffold** | [Installation Guide](user-guide/INSTALLATION.md) |
| **Create your first project** | [Usage Guide](user-guide/USAGE.md#your-first-project) |
| **Understand the architecture** | [Architecture Guide](developer-guide/ARCHITECTURE.md) |
| **Extend functionality** | [API Reference](api-reference/API.md) + [Component Documentation](components/README.md) |
| **Report issues** | [Troubleshooting](user-guide/TROUBLESHOOTING.md) + [Contributing Guide](developer-guide/CONTRIBUTING.md) |
| **Build from source** | [Build System](developer-guide/BUILD.md) |

## ⚡ Quick Start

### Prerequisites

- **C++17 compatible compiler**: GCC 7+, Clang 6+, MSVC 2017+
- **CMake**: 3.14 or later
- **Git**: For project initialization and version control
- **Ninja** (recommended): For faster builds

### Installation

#### Option 1: Build from Source (Recommended)

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
# or copy cpp-scaffold.exe to PATH on Windows
```

#### Option 2: Quick Build Script

```bash
# Use the provided build script
./build.bat  # Windows
# or
chmod +x build.sh && ./build.sh  # Linux/macOS
```

### Basic Usage

```bash
# Interactive mode (recommended for beginners)
./cpp-scaffold

# Quick project creation
./cpp-scaffold MyProject --type console --build cmake --package vcpkg

# Advanced options with full setup
./cpp-scaffold MyProject --type gui --build cmake --package conan --test gtest --ci github --editor vscode

# Use a saved profile
./cpp-scaffold MyProject --profile webdev
```

### Your First Project

Create a simple console application:

```bash
# Create a console app with testing
./cpp-scaffold HelloWorld --type console --build cmake --package vcpkg --test gtest

# Navigate to the project and build
cd HelloWorld
mkdir build && cd build
cmake .. -G Ninja
ninja

# Run the application
./HelloWorld  # Linux/macOS
# or
HelloWorld.exe  # Windows

# Run tests
ctest
```

## 🏗️ Project Templates

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
| `multi-executable` | Multiple executables | Complex projects, tool suites | Shared libraries, modular architecture |

### Template Features

Each template includes:

- ✅ **Modern C++17 project structure** with best practices
- ✅ **Configured build system** optimized for the template type
- ✅ **Package manager integration** with dependency management
- ✅ **Testing framework setup** with sample tests and CI integration
- ✅ **CI/CD configuration** for multiple platforms and services
- ✅ **Code formatting** (.clang-format, .clang-tidy, .editorconfig)
- ✅ **Git initialization** with comprehensive .gitignore and .gitattributes
- ✅ **License file** with proper formatting and copyright
- ✅ **Documentation structure** with README, API docs, and user guides
- ✅ **IDE configurations** for popular development environments
- ✅ **Code quality tools** integration and automation

### Template-Specific Features

#### Console Template
- Command-line argument parsing with validation
- Logging configuration with multiple output formats
- Cross-platform compatibility utilities
- Signal handling and graceful shutdown

#### Library Template
- Symbol export/import macros for Windows/Linux
- pkg-config file generation
- Version management and semantic versioning
- Installation and packaging support

#### Header-Only Library Template
- Single-header distribution option
- Comprehensive documentation with examples
- Benchmark integration for performance testing
- Continuous integration for multiple compilers

#### GUI Template
- Framework-specific project structure (Qt, SFML, etc.)
- Resource file management
- Cross-platform deployment scripts
- UI testing framework integration

#### Network Template
- Asynchronous I/O with modern C++ patterns
- Protocol buffer integration
- Load balancing and scaling considerations
- Security best practices implementation

#### Web Service Template
- RESTful API structure with routing
- JSON request/response handling
- Authentication and authorization middleware
- Database integration patterns

## Command Line Options

### Core Options

```bash
Options:
  -t, --type <template>     Project template (console, lib, gui, network, etc.)
  -b, --build <system>      Build system (cmake, meson, bazel, xmake, etc.)
  -p, --package <manager>   Package manager (vcpkg, conan, spack, hunter, none)
  --test <framework>        Testing framework (gtest, catch2, doctest, boost, none)
  --ci <system>            CI/CD system (github, gitlab, travis, appveyor, etc.)
  --editor <config>        Editor configuration (vscode, clion, vs, vim, etc.)
  --lang <language>        Interface language (english, chinese, spanish, etc.)
  -v, --verbose            Enable verbose output
  -h, --help              Show help information
  --version               Show version information
```

### Interactive Mode

```bash
# Launch interactive wizard
./cpp-scaffold

# Quick start wizard
./cpp-scaffold --wizard quick

# Advanced configuration wizard
./cpp-scaffold --wizard advanced
```

## Examples

### Console Application with Testing

```bash
./cpp-scaffold MyConsoleApp \
  --type console \
  --build cmake \
  --package vcpkg \
  --test gtest \
  --ci github \
  --editor vscode
```

### GUI Application with Qt

```bash
./cpp-scaffold MyGuiApp \
  --type gui \
  --build cmake \
  --package conan \
  --framework qt \
  --test catch2
```

### Network Service

```bash
./cpp-scaffold MyNetworkService \
  --type network \
  --build cmake \
  --package vcpkg \
  --network-lib asio \
  --test gtest \
  --ci gitlab
```

### Header-Only Library

```bash
./cpp-scaffold MyHeaderLib \
  --type header-only-lib \
  --build cmake \
  --test doctest \
  --ci github
```

## Configuration Profiles

Save frequently used configurations as profiles:

```bash
# Save current configuration as a profile
./cpp-scaffold --save-profile myprofile

# Use a saved profile
./cpp-scaffold MyProject --profile myprofile

# List available profiles
./cpp-scaffold --list-profiles
```

## Build and Development

### Prerequisites

- C++17 compatible compiler (GCC 7+, Clang 6+, MSVC 2017+)
- CMake 3.14 or later
- Git (for project initialization)

### Building

```bash
# Standard build
mkdir build && cd build
cmake .. -G Ninja
ninja

# Debug build
cmake .. -G Ninja -DCMAKE_BUILD_TYPE=Debug
ninja

# Release build
cmake .. -G Ninja -DCMAKE_BUILD_TYPE=Release
ninja
```

### Testing

```bash
# Run all tests
cd build
ctest

# Run specific tests
./test_template_manager
./test_cli_parser
./all_tests
```

### Dependencies

The project automatically manages its dependencies using CMake FetchContent:

- **spdlog v1.12.0** - High-performance logging library
- **nlohmann/json v3.11.2** - JSON parsing and generation
- **Google Test v1.14.0** - Testing framework (when BUILD_TESTING=ON)

Dependencies are cached in `deps_cache/` to avoid re-downloading.

## Architecture

### Core Components

- **Template System**: Factory pattern with pluggable templates
- **CLI Parser**: Type-safe argument parsing with rich validation
- **Configuration Manager**: Persistent settings and profiles
- **Interactive Wizard**: Step-by-step project creation
- **Utility Libraries**: File operations, terminal styling, Git integration

### Template Architecture

```cpp
// Base template interface
class TemplateBase {
    virtual bool create() = 0;
    virtual bool createProjectStructure() = 0;
    virtual bool createBuildSystem() = 0;
    virtual bool setupPackageManager() = 0;
    virtual bool setupTestFramework() = 0;
};

// Template factory
class TemplateManager {
    std::unique_ptr<TemplateBase> createTemplate(const CliOptions& options);
};
```

## Contributing

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/amazing-feature`)
3. Make your changes
4. Add tests for new functionality
5. Ensure all tests pass (`ctest`)
6. Commit your changes (`git commit -m 'Add amazing feature'`)
7. Push to the branch (`git push origin feature/amazing-feature`)
8. Open a Pull Request

### Adding New Templates

1. Create a new template class inheriting from `TemplateBase`
2. Implement all virtual methods
3. Register the template in `TemplateManager`
4. Add tests for the new template
5. Update documentation

### Code Style

- Follow modern C++17 best practices
- Use clang-format for code formatting
- Include comprehensive unit tests
- Document public APIs with Doxygen comments

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Support

- 📖 **Documentation**: See the [docs/](docs/) directory
- 🐛 **Bug Reports**: Open an issue on GitHub
- 💡 **Feature Requests**: Open an issue with the "enhancement" label
- 💬 **Discussions**: Use GitHub Discussions for questions

## Acknowledgments

- [spdlog](https://github.com/gabime/spdlog) for excellent logging
- [nlohmann/json](https://github.com/nlohmann/json) for JSON handling
- [Google Test](https://github.com/google/googletest) for testing framework
- The C++ community for inspiration and feedback

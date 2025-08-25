# CPP-Scaffold

[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)
[![Build Status](https://img.shields.io/badge/build-passing-brightgreen.svg)](https://github.com/cpp-scaffold/cpp-scaffold)
[![C++17](https://img.shields.io/badge/C%2B%2B-17-blue.svg)](https://en.cppreference.com/w/cpp/17)
[![CMake](https://img.shields.io/badge/CMake-3.14+-blue.svg)](https://cmake.org/)

A powerful, modern C++ project scaffolding tool that helps developers quickly create well-structured C++ projects with industry best practices, modern build systems, package managers, testing frameworks, and development tools.

## 🚀 Quick Start

### Installation

```bash
# Clone and build
git clone https://github.com/cpp-scaffold/cpp-scaffold.git
cd cpp-scaffold
mkdir build && cd build
cmake .. -G Ninja -DCMAKE_BUILD_TYPE=Release
ninja
```

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

### Console Application
```bash
cpp-scaffold MyTool --type console --build cmake --package vcpkg --test gtest --ci github
```

### C++ Library
```bash
cpp-scaffold MyLib --type lib --build cmake --package conan --test catch2 --docs
```

### Qt GUI Application
```bash
cpp-scaffold MyGuiApp --type gui --framework qt --build cmake --package vcpkg --editor vscode
```

### Web Service
```bash
cpp-scaffold MyAPI --type webservice --build cmake --package conan --test gtest --ci github,gitlab
```

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

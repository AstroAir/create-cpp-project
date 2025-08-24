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

**📖 [Complete Documentation](documents/README.md)** - Start here for comprehensive guides and references

### Quick Links

| For | Documentation | Description |
|-----|---------------|-------------|
| **New Users** | [Installation Guide](documents/user-guide/INSTALLATION.md) | Get CPP-Scaffold up and running |
| **All Users** | [Usage Guide](documents/user-guide/USAGE.md) | Learn how to use CPP-Scaffold effectively |
| **Developers** | [API Reference](documents/api-reference/API.md) | Complete API documentation |
| **Contributors** | [Contributing Guide](documents/developer-guide/CONTRIBUTING.md) | How to contribute to the project |
| **Troubleshooting** | [Troubleshooting Guide](documents/user-guide/TROUBLESHOOTING.md) | Common issues and solutions |

### Documentation Structure

```
documents/
├── README.md                    # Documentation index and navigation
├── user-guide/                 # User-facing documentation
│   ├── INSTALLATION.md          # Installation instructions
│   ├── USAGE.md                 # Usage guide and examples
│   └── TROUBLESHOOTING.md       # Common issues and solutions
├── developer-guide/             # Developer documentation
│   ├── ARCHITECTURE.md          # System architecture
│   ├── BUILD.md                 # Build system documentation
│   └── CONTRIBUTING.md          # Contributing guidelines
├── api-reference/               # API documentation
│   └── API.md                   # Complete API reference
├── components/                  # Component documentation
│   ├── README.md                # Component overview
│   ├── cli-parser.md            # CLI parser component
│   ├── template-system.md       # Template system component
│   └── configuration-system.md # Configuration system component
└── reference/                   # Additional references
    └── ENHANCEMENTS.md          # Recent improvements
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

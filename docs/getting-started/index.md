# Getting Started with CPP-Scaffold

Welcome to CPP-Scaffold! This section will help you get up and running quickly with our powerful C++ project scaffolding tool.

## What is CPP-Scaffold?

CPP-Scaffold is a modern C++ project scaffolding tool that helps developers quickly create well-structured C++ projects with industry best practices, modern build systems, package managers, testing frameworks, and development tools.

## Quick Overview

CPP-Scaffold provides:

- **Multiple Project Templates**: Console apps, libraries, GUI applications, network services, embedded systems, web services
- **Build System Support**: CMake, Meson, Bazel, XMake, Premake, Make, Ninja
- **Package Managers**: vcpkg, Conan, Spack, Hunter with automatic dependency management
- **Testing Frameworks**: Google Test, Catch2, Doctest, Boost.Test with CI integration
- **IDE Integration**: VSCode, CLion, Visual Studio, Vim, Emacs
- **CI/CD Setup**: GitHub Actions, GitLab CI, Travis CI, AppVeyor, Azure DevOps, CircleCI

## Getting Started Steps

1. **[Install CPP-Scaffold](installation.md)** - Set up the tool on your system
2. **[Quick Start Guide](quick-start.md)** - Learn the basic commands and workflow
3. **[Create Your First Project](first-project.md)** - Walk through creating a complete project

## 🚀 Quick Start

Ready to create your first project? Use command-line arguments for project creation:

```bash
# Create a project with command-line arguments
./cpp-scaffold MyProject --type console --build cmake --package vcpkg --test gtest

# See all available options
./cpp-scaffold --help
```

### Interactive Mode _(Coming Soon)_

An interactive wizard is planned for future releases:

```bash
# Interactive wizard (not yet implemented)
# ./cpp-scaffold

# When available, will provide step-by-step project configuration
```

## Prerequisites

Before you begin, make sure you have:

- **C++17 compatible compiler**: GCC 7+, Clang 6+, MSVC 2017+
- **CMake**: 3.14 or later
- **Git**: For project initialization and version control
- **Ninja** (recommended): For faster builds

## Next Steps

Once you've completed the getting started guide, you might want to:

- Explore the [User Guide](../user-guide/index.md) for detailed usage information
- Try the [Tutorials](../tutorials/index.md) for hands-on examples
- Check out the [Developer Guide](../developer-guide/index.md) if you want to contribute

## Need Help?

If you run into any issues:

- Check the [Troubleshooting Guide](../user-guide/troubleshooting.md)
- Visit our [GitHub repository](https://github.com/cpp-scaffold/cpp-scaffold)
- Open an issue for bug reports or feature requests

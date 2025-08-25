# User Guide

This comprehensive user guide covers all aspects of using CPP-Scaffold effectively.

## Overview

CPP-Scaffold is designed to streamline C++ project creation with modern best practices. This guide will help you master all its features and capabilities.

## What's in This Section

### [Usage](usage.md)
Complete command-line reference and usage examples covering all available options and flags.

### [Project Templates](templates.md)
Detailed information about all available project templates, their features, and when to use each one.

### [Configuration](configuration.md)
Learn how to configure CPP-Scaffold, manage profiles, and customize default settings.

### [Troubleshooting](troubleshooting.md)
Common issues and their solutions, plus debugging tips and best practices.

## Quick Reference

### Common Commands

```bash
# Interactive mode
cpp-scaffold

# Quick project creation
cpp-scaffold MyProject --type console --build cmake

# With full configuration
cpp-scaffold MyProject --type gui --build cmake --package vcpkg --test gtest --ci github
```

### Project Types

| Template | Use Case | Key Features |
|----------|----------|--------------|
| `console` | CLI applications | Argument parsing, logging |
| `lib` | Libraries | Export/import, versioning |
| `header-only-lib` | Header-only libraries | Single-header distribution |
| `gui` | Desktop applications | Framework integration |
| `network` | Network services | Async I/O, protocols |
| `webservice` | Web APIs | HTTP handling, JSON |
| `embedded` | Embedded systems | Resource constraints |

### Build Systems

- **CMake** (recommended) - Cross-platform, widely supported
- **Meson** - Fast, user-friendly
- **Bazel** - Scalable, reproducible builds
- **XMake** - Lua-based, modern
- **Premake** - Lua-based, simple

### Package Managers

- **vcpkg** - Microsoft's C++ package manager
- **Conan** - Cross-platform, Python-based
- **Spack** - Scientific computing focus
- **Hunter** - CMake-driven

## Getting Help

### Command Line Help

```bash
# General help
cpp-scaffold --help

# Verbose output for debugging
cpp-scaffold MyProject --verbose

# Version information
cpp-scaffold --version
```

### Documentation

- **[Getting Started](../getting-started/index.md)** - New to CPP-Scaffold?
- **[Tutorials](../tutorials/index.md)** - Step-by-step examples
- **[Developer Guide](../developer-guide/index.md)** - Contributing and extending
- **[API Reference](../api-reference/index.md)** - Complete API documentation

### Community Support

- **GitHub Issues** - Bug reports and feature requests
- **Discussions** - Questions and community help
- **Wiki** - Community-contributed guides and tips

## Best Practices

### Project Organization

1. **Use meaningful names** for projects and components
2. **Follow naming conventions** for your target platform
3. **Include comprehensive documentation** from the start
4. **Set up CI/CD early** in the development process

### Template Selection

1. **Start simple** - Use console template for learning
2. **Choose appropriate complexity** - Don't over-engineer
3. **Consider future needs** - Libraries vs applications
4. **Match your deployment target** - Desktop, server, embedded

### Configuration Management

1. **Use profiles** for repeated project types
2. **Version control** your configuration files
3. **Document custom settings** for team members
4. **Test configurations** before committing

## Advanced Topics

### Custom Templates

Learn how to create and use custom project templates for your specific needs.

### Integration with IDEs

Configure your favorite development environment for optimal CPP-Scaffold integration.

### CI/CD Pipeline Setup

Automate your build, test, and deployment processes with generated CI/CD configurations.

### Multi-Project Workspaces

Manage complex projects with multiple components and dependencies.

## Next Steps

After reading this user guide:

1. **Practice** with different project types
2. **Experiment** with various build systems and package managers
3. **Contribute** to the project or create custom templates
4. **Share** your experiences with the community

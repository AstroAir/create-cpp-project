# CPP-Scaffold Manual Page

## NAME
cpp-scaffold - C++ project scaffolding tool

## SYNOPSIS
**cpp-scaffold** [*OPTIONS*] [*PROJECT_NAME*]

## DESCRIPTION
**cpp-scaffold** is a powerful, modern C++ project scaffolding tool that helps developers quickly create well-structured C++ projects with industry best practices, modern build systems, package managers, testing frameworks, and development tools.

## OPTIONS

### General Options
- **-h, --help**  
  Show help message and exit

- **-v, --version**  
  Show version information and exit

### Project Configuration
- **-t, --type** *TYPE*  
  Project type (console, library, gui, network, embedded, webservice)

- **-b, --build-system** *SYSTEM*  
  Build system (xmake, cmake, meson, bazel, premake, make, ninja)

- **-p, --package-manager** *MANAGER*  
  Package manager (vcpkg, conan, spack, hunter)

- **-f, --framework** *FRAMEWORK*  
  Framework (qt, sfml, boost, opencv, imgui)

### Advanced Options
- **--config** *FILE*  
  Use configuration file

- **--template** *TEMPLATE*  
  Use custom template

- **--interactive**  
  Run in interactive mode

- **--no-git**  
  Skip Git initialization

- **--no-deps**  
  Skip dependency installation

## EXAMPLES

### Basic Usage
```bash
# Create a console application
cpp-scaffold my-app --type console

# Create a library with CMake
cpp-scaffold my-lib --type library --build-system cmake

# Create a Qt GUI application
cpp-scaffold my-gui --type gui --framework qt
```

### Advanced Usage
```bash
# Interactive mode
cpp-scaffold --interactive

# Use custom configuration
cpp-scaffold my-project --config project.json

# Skip Git initialization
cpp-scaffold my-project --no-git
```

## FILES
- **~/.config/cpp-scaffold/config.json**  
  User configuration file

- **./cpp-scaffold.json**  
  Project-specific configuration

## EXIT STATUS
- **0** - Success
- **1** - General error
- **2** - Invalid arguments
- **3** - File system error

## SEE ALSO
- [Getting Started Guide](../getting-started/index.md)
- [User Guide](../user-guide/index.md)
- [Developer Guide](../developer-guide/index.md)

## AUTHOR
CPP-Scaffold Team

## COPYRIGHT
Copyright © 2024 CPP-Scaffold Team. Licensed under GPL v3.

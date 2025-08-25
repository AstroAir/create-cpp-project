# Quick Start Guide

This guide will get you up and running with CPP-Scaffold in just a few minutes.

## Installation Check

First, verify that CPP-Scaffold is properly installed:

```bash
cpp-scaffold --version
```

If you haven't installed CPP-Scaffold yet, see the [Installation Guide](installation.md).

## Basic Usage

### Interactive Mode (Recommended for Beginners)

The easiest way to get started is with the interactive wizard:

```bash
cpp-scaffold
```

This will launch an interactive wizard that guides you through:

1. Project name selection
2. Template type choice
3. Build system configuration
4. Package manager setup
5. Testing framework selection
6. IDE configuration

### Command Line Mode

For quick project creation, use the command line interface:

```bash
# Create a simple console application
cpp-scaffold MyProject --type console --build cmake --package vcpkg

# Create a library with testing
cpp-scaffold MyLibrary --type lib --build cmake --package conan --test gtest

# Create a GUI application
cpp-scaffold MyGuiApp --type gui --build cmake --package vcpkg --framework qt
```

## Common Commands

### Project Templates

```bash
# Console application
cpp-scaffold MyConsoleApp --type console

# Static/shared library
cpp-scaffold MyLibrary --type lib

# Header-only library
cpp-scaffold MyHeaderLib --type header-only-lib

# GUI application
cpp-scaffold MyGuiApp --type gui

# Network service
cpp-scaffold MyNetworkService --type network

# Web service
cpp-scaffold MyWebService --type webservice
```

### Build Systems

```bash
# CMake (recommended)
cpp-scaffold MyProject --build cmake

# Meson
cpp-scaffold MyProject --build meson

# Bazel
cpp-scaffold MyProject --build bazel
```

### Package Managers

```bash
# vcpkg (recommended for Windows)
cpp-scaffold MyProject --package vcpkg

# Conan (cross-platform)
cpp-scaffold MyProject --package conan

# No package manager
cpp-scaffold MyProject --package none
```

## Your First Project

Let's create a simple "Hello World" console application:

```bash
# Create the project
cpp-scaffold HelloWorld --type console --build cmake --package vcpkg --test gtest

# Navigate to the project directory
cd HelloWorld

# Build the project
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

## Project Structure

After creating a project, you'll see a structure like this:

```
HelloWorld/
├── CMakeLists.txt          # Build configuration
├── README.md               # Project documentation
├── LICENSE                 # License file
├── .gitignore             # Git ignore rules
├── .clang-format          # Code formatting rules
├── vcpkg.json             # Package dependencies
├── src/
│   └── main.cpp           # Main source file
├── include/
│   └── HelloWorld/        # Public headers
├── tests/
│   └── test_main.cpp      # Unit tests
└── docs/
    └── README.md          # Additional documentation
```

## Configuration Profiles

Save frequently used configurations as profiles:

```bash
# Save current configuration
cpp-scaffold --save-profile myprofile

# Use a saved profile
cpp-scaffold MyProject --profile myprofile

# List available profiles
cpp-scaffold --list-profiles
```

## Next Steps

Now that you've created your first project:

1. **[Create Your First Project](first-project.md)** - Detailed walkthrough
2. **[User Guide](../user-guide/index.md)** - Comprehensive usage documentation
3. **[Tutorials](../tutorials/index.md)** - Step-by-step examples

## Getting Help

```bash
# Show help
cpp-scaffold --help

# Show version
cpp-scaffold --version

# Enable verbose output
cpp-scaffold MyProject --verbose
```

For more detailed information, see the [User Guide](../user-guide/index.md).

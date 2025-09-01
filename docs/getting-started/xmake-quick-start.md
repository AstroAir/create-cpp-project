# XMake Quick Start Guide

This guide helps you get started with XMake build system for cpp-scaffold projects.

## What is XMake?

XMake is a modern Lua-based build system that provides:

- **Simple Configuration**: Lua-based configuration files
- **Built-in Package Management**: No need for external package managers
- **Cross-Platform**: Works on Windows, Linux, macOS
- **Fast Builds**: Optimized incremental compilation
- **MSYS2 Integration**: Native Windows development support

## Installation

### Windows (MSYS2)

```bash
# Install via MSYS2 (recommended for Windows)
pacman -S mingw-w64-x86_64-xmake

# Or download from https://xmake.io/
```

### Linux

```bash
# Ubuntu/Debian
sudo add-apt-repository ppa:xmake-io/xmake
sudo apt update
sudo apt install xmake

# Arch Linux
sudo pacman -S xmake

# Or install via script
curl -fsSL https://xmake.io/shget.text | bash
```

### macOS

```bash
# Homebrew
brew install xmake

# Or install via script
curl -fsSL https://xmake.io/shget.text | bash
```

## Basic Usage

### 1. Create a New Project

```bash
# Create console application with XMake
cpp-scaffold MyApp --type console --build xmake --test gtest

# Create library with XMake
cpp-scaffold MyLib --type lib --build xmake --test catch2

# Create with MSYS2 support (Windows)
cpp-scaffold MyApp --type console --build xmake --msys2
```

### 2. Build the Project

```bash
# Navigate to project directory
cd MyApp

# Configure project
xmake config

# Build all targets
xmake build

# Run the application
xmake run MyApp
```

### 3. Development Workflow

```bash
# Configure for development with tests
xmake config --mode=debug --tests=true

# Build and run tests
xmake build
xmake run-tests

# Run specific test
xmake run test_myapp

# Show project information
xmake info
```

## Configuration Options

### Build Modes

```bash
# Debug mode (default)
xmake config --mode=debug

# Release mode with optimizations
xmake config --mode=release
```

### Enable Features

```bash
# Enable tests
xmake config --tests=true

# Enable examples
xmake config --examples=true

# Enable code coverage
xmake config --coverage=true
```

### MSYS2 Options (Windows)

```bash
# Enable MSYS2 optimizations
xmake config --msys2_optimizations=true

# Build with static linking (portable)
xmake config --msys2_static=true

# Prepare for MSYS2 packaging
xmake config --msys2_package=true
```

## Common Commands

### Building

```bash
# Build all targets
xmake build

# Build specific target
xmake build MyApp

# Build with verbose output
xmake build -v

# Clean build artifacts
xmake clean
```

### Testing

```bash
# Run all tests
xmake run-tests

# Run tests with verbose output
xmake run-tests --verbose

# Run specific test
xmake run test_myapp

# Generate coverage report
xmake coverage
```

### Package Management

```bash
# Show package information
xmake require --info spdlog

# Search for packages
xmake require --search json

# Install MSYS2 dependencies (Windows)
xmake msys2-install-deps
```

### Project Information

```bash
# Show project information
xmake info

# Show available targets
xmake show -l targets

# Show configuration
xmake show -c

# Check MSYS2 environment (Windows)
xmake msys2-info
```

## Project Structure

A typical XMake project structure:

```text
MyApp/
├── xmake.lua              # Main build configuration
├── xmake/                 # Modular build files
│   ├── packages.lua       # Package dependencies
│   ├── components.lua     # Component definitions
│   ├── tests.lua          # Test configurations
│   └── tasks.lua          # Custom tasks
├── src/                   # Source code
│   ├── main.cpp           # Application entry point
│   └── myapp/             # Application modules
├── include/               # Public headers
│   └── myapp/
├── tests/                 # Unit tests
│   ├── test_main.cpp
│   └── unit/
└── docs/                  # Documentation
```

## IDE Integration

### Visual Studio Code

1. Install XMake extension
2. Configure settings in `.vscode/settings.json`:

```json
{
  "xmake.workingDirectory": "${workspaceFolder}",
  "xmake.buildDirectory": "build",
  "xmake.configArguments": ["--tests=true"]
}
```

### CLion

1. Install XMake plugin
2. Configure XMake toolchain in settings
3. Set build directory to `build`

## Comparison with CMake

| Feature                | XMake     | CMake                   |
| ---------------------- | --------- | ----------------------- |
| Configuration Language | Lua       | CMake Script            |
| Package Management     | Built-in  | External (vcpkg, Conan) |
| Learning Curve         | Easy      | Moderate                |
| Build Speed            | Fast      | Good                    |
| MSYS2 Support          | Excellent | Manual                  |
| Cross-Platform         | Excellent | Excellent               |

## Migration from CMake

If you have an existing CMake project:

1. **Keep CMake**: Both systems can coexist
2. **Try XMake**: Create new components with XMake
3. **Gradual migration**: Move components incrementally
4. **Compare results**: Ensure feature parity

## Troubleshooting

### Common Issues

1. **XMake not found**

   ```bash
   # Check installation
   which xmake
   xmake --version

   # Reinstall if needed
   pacman -S mingw-w64-x86_64-xmake  # MSYS2
   ```

2. **Package not found**

   ```bash
   # Search for package
   xmake require --search package-name

   # Install via system package manager
   pacman -S mingw-w64-x86_64-package-name  # MSYS2
   ```

3. **Build fails**

   ```bash
   # Verbose build
   xmake build -v

   # Clean and rebuild
   xmake clean
   xmake build
   ```

### Getting Help

```bash
# Show help
xmake --help

# Show command help
xmake build --help

# Show project information
xmake info

# Check environment (MSYS2)
xmake msys2-info
```

## Next Steps

- **[XMake Build System Guide](../developer-guide/xmake-build-system.md)** - Comprehensive XMake documentation
- **[MSYS2 Integration](../developer-guide/xmake-msys2-integration.md)** - Windows development with MSYS2
- **[Build System Comparison](../developer-guide/build-system.md)** - XMake vs CMake detailed comparison
- **[Package Management](../reference/package-management.md)** - Managing dependencies

## Resources

- [XMake Official Documentation](https://xmake.io/)
- [XMake GitHub Repository](https://github.com/xmake-io/xmake)
- [XMake Package Repository](https://github.com/xmake-io/xmake-repo)
- [MSYS2 Official Website](https://www.msys2.org/)

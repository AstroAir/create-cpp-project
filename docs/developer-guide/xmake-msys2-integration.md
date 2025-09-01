# XMake MSYS2 Integration Guide

This guide provides comprehensive instructions for using XMake with MSYS2 environment for optimal Windows development experience.

## Overview

The XMake build system includes extensive MSYS2 integration that provides:

- **Automatic MSYS2 environment detection**
- **Native MSYS2 package management**
- **Cross-architecture support** (MinGW64, MinGW32, UCRT64, Clang64, ClangARM64)
- **Performance optimizations** for MSYS2 toolchains
- **MSYS2 package creation** (PKGBUILD generation)
- **Seamless development workflow**

## Prerequisites

### Installing MSYS2

1. **Download MSYS2**

   - Visit [https://www.msys2.org/](https://www.msys2.org/)
   - Download the installer for your architecture

2. **Install MSYS2**

   - Run the installer with default settings
   - Follow the installation wizard

3. **Update MSYS2**

   ```bash
   # Update package database and core packages
   pacman -Syu

   # Restart MSYS2 terminal and update remaining packages
   pacman -Su
   ```

### Installing XMake

```bash
# Method 1: Install via MSYS2 package manager (recommended)
pacman -S mingw-w64-x86_64-xmake

# Method 2: Install from official installer
# Download from https://xmake.io/ and install

# Method 3: Install via script
curl -fsSL https://xmake.io/shget.text | bash
```

### Installing Development Tools

```bash
# Essential development packages
pacman -S mingw-w64-x86_64-toolchain \
          mingw-w64-x86_64-cmake \
          mingw-w64-x86_64-ninja \
          mingw-w64-x86_64-make \
          mingw-w64-x86_64-gdb \
          git

# Or use the automated setup
xmake msys2-setup-dev
```

## MSYS2 Environments

### Available Environments

MSYS2 provides multiple environments for different use cases:

| Environment    | Description                 | Use Case             |
| -------------- | --------------------------- | -------------------- |
| **MinGW64**    | 64-bit Windows applications | General development  |
| **MinGW32**    | 32-bit Windows applications | Legacy support       |
| **UCRT64**     | Universal C Runtime         | Modern Windows apps  |
| **Clang64**    | Clang-based 64-bit builds   | Alternative compiler |
| **ClangARM64** | ARM64 builds                | ARM development      |
| **MSYS**       | Unix-like environment       | Build tools only     |

### Choosing the Right Environment

- **MinGW64**: Recommended for most Windows development
- **UCRT64**: For modern Windows applications (Windows 10+)
- **Clang64**: When using Clang compiler features
- **MinGW32**: Only for legacy 32-bit requirements

### Launching Environments

```bash
# From Windows Start Menu:
# - MSYS2 MinGW 64-bit
# - MSYS2 MinGW 32-bit
# - MSYS2 UCRT64
# - MSYS2 Clang64

# Or from command line:
C:\msys64\mingw64.exe    # MinGW64
C:\msys64\ucrt64.exe     # UCRT64
C:\msys64\clang64.exe    # Clang64
```

## Quick Start

### 1. Environment Setup

```bash
# Launch MinGW64 shell
# Verify environment
echo $MSYSTEM          # Should show MINGW64
echo $MINGW_PREFIX     # Should show /mingw64

# Check XMake installation
xmake --version

# Setup development environment (one-time)
xmake msys2-setup-dev
```

### 2. Project Configuration

```bash
# Navigate to project directory
cd /path/to/cpp-scaffold

# Basic configuration
xmake config

# Configuration with tests and examples
xmake config --tests=true --examples=true

# Configuration with MSYS2 optimizations
xmake config --msys2_optimizations=true --mode=release
```

### 3. Building

```bash
# Build all targets
xmake build

# Build specific component
xmake build cpp_scaffold_utils

# Build with verbose output
xmake build -v
```

### 4. Testing

```bash
# Run all tests
xmake run-tests

# Run specific test
xmake run test_file_utils

# Run tests with coverage
xmake config --coverage=true
xmake run-tests
xmake coverage
```

## MSYS2-Specific Features

### Automatic Environment Detection

XMake automatically detects and configures for MSYS2:

```bash
# Check environment detection
xmake msys2-info
```

**Output example:**

```
MSYS2 Environment Information
============================
Environment: MINGW64
MinGW Prefix: /mingw64
MSYS2 Root: /
Architecture Support:
  MinGW64: Yes
  MinGW32: No
  UCRT64:  No
  Clang64: No
  ClangARM64: No

Available Tools:
  gcc: Yes
  g++: Yes
  make: Yes
  cmake: Yes
  ninja: Yes
  pacman: Yes
```

### Package Management Integration

#### Automatic Package Mapping

XMake automatically maps package names to MSYS2 equivalents:

| XMake Package   | MSYS2 Package             |
| --------------- | ------------------------- |
| `spdlog`        | `mingw-w64-spdlog`        |
| `nlohmann_json` | `mingw-w64-nlohmann-json` |
| `gtest`         | `mingw-w64-gtest`         |
| `boost`         | `mingw-w64-boost`         |
| `qt5widgets`    | `mingw-w64-qt5-base`      |
| `sfml`          | `mingw-w64-sfml`          |

#### Installing Dependencies

```bash
# Automatic installation of project dependencies
xmake msys2-install-deps

# Manual installation
pacman -S mingw-w64-x86_64-spdlog \
          mingw-w64-x86_64-nlohmann-json \
          mingw-w64-x86_64-gtest \
          mingw-w64-x86_64-boost
```

### Build Optimizations

#### Compiler Optimizations

For release builds, XMake automatically applies MSYS2-specific optimizations:

```bash
# Native CPU optimization
-march=native -mtune=native

# Performance optimizations
-ffast-math -funroll-loops

# Link-time optimization
-flto
```

#### Linker Optimizations

```bash
# Dead code elimination
-Wl,--gc-sections

# Symbol stripping (release)
-Wl,--strip-all

# MSYS2-specific linking
-Wl,--enable-auto-import
-Wl,--enable-runtime-pseudo-reloc
```

### Static Linking

For portable executables:

```bash
# Configure for static linking
xmake config --msys2_static=true --mode=release

# Build portable executable
xmake build

# The resulting executable will be self-contained
```

## Advanced Configuration

### Build Options

```bash
# Enable MSYS2 optimizations
xmake config --msys2_optimizations=true

# Static linking for portability
xmake config --msys2_static=true

# Prepare for MSYS2 packaging
xmake config --msys2_package=true

# Combine multiple options
xmake config --mode=release \
              --msys2_optimizations=true \
              --msys2_static=true \
              --tests=true
```

### Cross-Architecture Building

```bash
# Build for different architectures
# (requires switching MSYS2 environment)

# MinGW64 (64-bit)
# Launch MinGW64 shell
xmake config --arch=x86_64
xmake build

# MinGW32 (32-bit)
# Launch MinGW32 shell
xmake config --arch=i386
xmake build

# UCRT64
# Launch UCRT64 shell
xmake config --arch=x86_64
xmake build
```

### Custom Toolchains

```bash
# Use specific compiler
xmake config --toolchain=gcc
xmake config --toolchain=clang

# Use specific compiler version
xmake config --cc=gcc-11 --cxx=g++-11
```

## Package Creation

### Generating PKGBUILD

```bash
# Generate MSYS2 package files
xmake msys2-package
```

This creates a `PKGBUILD` file with proper MSYS2 package structure.

### Building Package

```bash
# Build the package
makepkg -s

# Install locally
pacman -U mingw-w64-x86_64-cpp-scaffold-1.0.0-1-any.pkg.tar.zst

# Clean build files
makepkg --clean
```

## Development Workflow

### Daily Development

```bash
# 1. Start MSYS2 environment
# Launch MinGW64 shell

# 2. Navigate to project
cd /c/path/to/cpp-scaffold

# 3. Configure for development
xmake config --mode=debug --tests=true --msys2_optimizations=true

# 4. Incremental build and test
xmake build
xmake run-tests

# 5. Run specific tests during development
xmake run test_file_utils
```

### Release Workflow

```bash
# 1. Configure for release
xmake config --mode=release --msys2_static=true

# 2. Clean build
xmake clean
xmake build

# 3. Run full test suite
xmake config --tests=true
xmake run-tests

# 4. Create package
xmake msys2-package

# 5. Build and test package
makepkg -s
pacman -U *.pkg.tar.zst
```

### Debugging

```bash
# Build with debug symbols
xmake config --mode=debug
xmake build

# Debug with GDB
gdb bin/cpp-scaffold.exe

# Or use XMake's debug runner
xmake run -d cpp-scaffold
```

## Best Practices

### 1. Environment Management

- **Use appropriate shell**: Always use MinGW64/MinGW32/UCRT64 shells, not MSYS2 shell
- **Consistent environment**: Stick to one environment per project
- **Regular updates**: Keep MSYS2 packages updated with `pacman -Syu`

### 2. Package Management

- **Prefer system packages**: Use MSYS2 packages over bundled ones
- **Check dependencies**: Use `xmake msys2-install-deps` for automatic setup
- **Version consistency**: Ensure all dependencies use compatible versions

### 3. Build Configuration

- **Development**: Use `--mode=debug --tests=true --msys2_optimizations=true`
- **Release**: Use `--mode=release --msys2_static=true` for distribution
- **Testing**: Always enable tests during development

### 4. Performance

- **Parallel builds**: Use `-j$(nproc)` for faster builds
- **Incremental builds**: Avoid unnecessary clean builds
- **Native optimizations**: Enable `--msys2_optimizations=true` for release

### 5. Distribution

- **Static linking**: Use `--msys2_static=true` for portable executables
- **Package creation**: Use `xmake msys2-package` for proper MSYS2 packages
- **Testing**: Test packages on clean MSYS2 installations

## See Also

- [XMake Build System Guide](xmake-build-system.md)
- [General Build System Documentation](build-system.md)
- [Package Management Guide](../reference/package-management.md)

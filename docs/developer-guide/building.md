# CPP-Scaffold Build System Documentation

This document provides comprehensive information about building CPP-Scaffold from source, understanding the build system, and configuring build options.

## 📋 Table of Contents

- [Build System Overview](#build-system-overview)
- [Prerequisites](#prerequisites)
- [Build Configuration](#build-configuration)
- [Build Targets](#build-targets)
- [Platform-Specific Builds](#platform-specific-builds)
- [Dependency Management](#dependency-management)
- [Testing](#testing)
- [Packaging](#packaging)
- [Troubleshooting](#troubleshooting)

## 🏗️ Build System Overview

CPP-Scaffold supports **dual build systems**:

### Primary: CMake with Presets

- **Cross-platform compatibility** across Windows, macOS, and Linux
- **CMake presets** for standardized build configurations
- **Automatic dependency management** using CMake FetchContent
- **Multiple build configurations** (debug, release, optimized, minimal)
- **Integrated testing** with CTest
- **Packaging support** with CPack
- **IDE integration** for Visual Studio, Xcode, and others

### Alternative: XMake (Modern)

- **Lua-based configuration** with simpler syntax
- **Built-in package management** and dependency resolution
- **MSYS2 integration** for Windows development
- **Fast compilation** and intelligent caching

### CMake Version Requirements

- **Minimum**: CMake 3.14
- **Recommended**: CMake 3.20+
- **Features used**: FetchContent, target_link_libraries, modern CMake practices

## 🔧 Prerequisites

### Required Tools

| Tool         | Minimum Version  | Purpose                |
| ------------ | ---------------- | ---------------------- |
| CMake        | 3.14             | Build system generator |
| C++ Compiler | C++17 compatible | Source compilation     |
| Git          | 2.0+             | Dependency fetching    |

### Supported Compilers

| Compiler    | Minimum Version | Platforms              |
| ----------- | --------------- | ---------------------- |
| GCC         | 7.0             | Linux, Windows (MinGW) |
| Clang       | 6.0             | Linux, macOS, Windows  |
| MSVC        | 2017 (15.7)     | Windows                |
| Apple Clang | 10.0            | macOS                  |

### Optional Tools

| Tool         | Purpose           | Benefit            |
| ------------ | ----------------- | ------------------ |
| Ninja        | Fast build system | Faster compilation |
| ccache       | Compiler cache    | Faster rebuilds    |
| Clang-tidy   | Static analysis   | Code quality       |
| Clang-format | Code formatting   | Consistent style   |

## ⚙️ Build Configuration

### Basic Build Commands

```bash
# Standard build process
git clone https://github.com/cpp-scaffold/cpp-scaffold.git
cd cpp-scaffold
mkdir build && cd build
cmake .. -G Ninja
ninja
```

### CMake Configuration Options

| Option                 | Type   | Default      | Description            |
| ---------------------- | ------ | ------------ | ---------------------- |
| `CMAKE_BUILD_TYPE`     | STRING | `Release`    | Build configuration    |
| `BUILD_TESTING`        | BOOL   | `ON`         | Build unit tests       |
| `CMAKE_INSTALL_PREFIX` | PATH   | `/usr/local` | Installation directory |
| `CMAKE_CXX_STANDARD`   | STRING | `17`         | C++ standard version   |
| `CMAKE_CXX_EXTENSIONS` | BOOL   | `OFF`        | Compiler extensions    |

### Build Type Configurations

#### Debug Build

```bash
cmake .. -G Ninja -DCMAKE_BUILD_TYPE=Debug
```

- **Optimization**: None (-O0)
- **Debug Info**: Full debug symbols
- **Assertions**: Enabled
- **Warnings**: All warnings enabled (-Wall -Wextra -Wpedantic)
- **Use Case**: Development and debugging

#### Release Build

```bash
cmake .. -G Ninja -DCMAKE_BUILD_TYPE=Release
```

- **Optimization**: Maximum (-O3)
- **Debug Info**: None
- **Assertions**: Disabled (NDEBUG)
- **Warnings**: Error on warnings (-Werror)
- **Use Case**: Production deployment

#### RelWithDebInfo Build

```bash
cmake .. -G Ninja -DCMAKE_BUILD_TYPE=RelWithDebInfo
```

- **Optimization**: High (-O2)
- **Debug Info**: Included
- **Use Case**: Performance profiling

#### MinSizeRel Build

```bash
cmake .. -G Ninja -DCMAKE_BUILD_TYPE=MinSizeRel
```

- **Optimization**: Size optimization (-Os)
- **Use Case**: Embedded systems, minimal distributions

### Advanced Configuration

```bash
# Custom compiler
cmake .. -DCMAKE_CXX_COMPILER=clang++

# Custom installation prefix
cmake .. -DCMAKE_INSTALL_PREFIX=/opt/cpp-scaffold

# Disable testing
cmake .. -DBUILD_TESTING=OFF

# Static linking (where supported)
cmake .. -DBUILD_SHARED_LIBS=OFF

# Verbose build output
cmake .. -DCMAKE_VERBOSE_MAKEFILE=ON
```

## 🎯 Build Targets

### Primary Targets

| Target             | Description                | Command                  |
| ------------------ | -------------------------- | ------------------------ |
| `cpp-scaffold`     | Main executable            | `ninja cpp-scaffold`     |
| `cpp_scaffold_lib` | Core library (for testing) | `ninja cpp_scaffold_lib` |
| `all`              | Build everything           | `ninja` or `ninja all`   |
| `clean`            | Clean build artifacts      | `ninja clean`            |

### Test Targets

| Target                  | Description              | Command                       |
| ----------------------- | ------------------------ | ----------------------------- |
| `test_file_utils`       | File utilities tests     | `ninja test_file_utils`       |
| `test_string_utils`     | String utilities tests   | `ninja test_string_utils`     |
| `test_cli_parser`       | CLI parser tests         | `ninja test_cli_parser`       |
| `test_template_manager` | Template manager tests   | `ninja test_template_manager` |
| `test_terminal_utils`   | Terminal utilities tests | `ninja test_terminal_utils`   |
| `test_integration`      | Integration tests        | `ninja test_integration`      |
| `all_tests`             | Comprehensive test suite | `ninja all_tests`             |

### Utility Targets

| Target    | Description                 | Command                 |
| --------- | --------------------------- | ----------------------- |
| `install` | Install to system           | `ninja install`         |
| `package` | Create distribution package | `ninja package`         |
| `test`    | Run all tests               | `ninja test` or `ctest` |

## 🖥️ Platform-Specific Builds

### Windows

#### Visual Studio

```powershell
# Generate Visual Studio solution
cmake .. -G "Visual Studio 17 2022" -A x64

# Build from command line
cmake --build . --config Release

# Or open in Visual Studio
start cpp-scaffold.sln
```

#### MinGW

```bash
# Using MinGW-w64
cmake .. -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release
mingw32-make

# Using MSYS2
cmake .. -G Ninja -DCMAKE_BUILD_TYPE=Release
ninja
```

### macOS

#### Xcode

```bash
# Generate Xcode project
cmake .. -G Xcode

# Build from command line
cmake --build . --config Release

# Or open in Xcode
open cpp-scaffold.xcodeproj
```

#### Command Line

```bash
# Using system clang
cmake .. -G Ninja -DCMAKE_BUILD_TYPE=Release
ninja

# Using Homebrew clang
cmake .. -G Ninja -DCMAKE_CXX_COMPILER=/usr/local/bin/clang++
ninja
```

### Linux

#### GCC

```bash
# Default GCC
cmake .. -G Ninja -DCMAKE_BUILD_TYPE=Release
ninja

# Specific GCC version
cmake .. -G Ninja -DCMAKE_CXX_COMPILER=g++-9
ninja
```

#### Clang

```bash
# System Clang
cmake .. -G Ninja -DCMAKE_CXX_COMPILER=clang++
ninja

# Specific Clang version
cmake .. -G Ninja -DCMAKE_CXX_COMPILER=clang++-12
ninja
```

## 📦 Dependency Management

### Automatic Dependencies

CPP-Scaffold uses CMake FetchContent to automatically manage dependencies:

```cmake
# Dependencies are automatically downloaded and built
include(FetchContent)

# spdlog for logging
FetchContent_Declare(spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG v1.12.0
)

# nlohmann/json for JSON handling
FetchContent_Declare(json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.2
)

# Google Test for testing (when BUILD_TESTING=ON)
FetchContent_Declare(googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.14.0
)
```

### Dependency Caching

Dependencies are cached in `deps_cache/` to avoid re-downloading:

```bash
# Cache structure
deps_cache/
├── spdlog_https___github_com_gabime_spdlog_git_v1.12.0/
├── json_https___github_com_nlohmann_json_git_v3.11.2/
└── googletest_https___github_com_google_googletest_git_v1.14.0/

# Clear cache if needed
rm -rf deps_cache/
```

### System Dependencies

If system packages are available, they will be used instead:

```bash
# Check for system packages
find_package(spdlog QUIET)
find_package(nlohmann_json QUIET)

# Use system package if found, otherwise fetch
if(NOT spdlog_FOUND)
    fetch_and_cache(spdlog ...)
endif()
```

## 🧪 Testing

### Running Tests

```bash
# Run all tests
cd build
ctest

# Run specific test
ctest -R test_file_utils

# Run tests with verbose output
ctest --verbose

# Run tests in parallel
ctest -j4

# Run tests with specific configuration
ctest -C Release
```

### Test Configuration

```bash
# Build with testing enabled (default)
cmake .. -DBUILD_TESTING=ON

# Build without testing
cmake .. -DBUILD_TESTING=OFF

# Run tests during build
ninja test
```

### Test Coverage

```bash
# Enable coverage (GCC/Clang)
cmake .. -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_FLAGS="--coverage"
ninja
ctest
gcov src/*.cpp
lcov --capture --directory . --output-file coverage.info
genhtml coverage.info --output-directory coverage_html
```

## 📦 Packaging

### CPack Configuration

CPP-Scaffold includes CPack configuration for creating distribution packages:

```bash
# Create packages
ninja package

# Available package types (platform-dependent):
# - ZIP archives
# - NSIS installer (Windows)
# - DEB packages (Debian/Ubuntu)
# - RPM packages (Red Hat/CentOS)
# - DMG images (macOS)
```

### Package Types by Platform

#### Windows

```bash
# NSIS installer
cpack -G NSIS

# ZIP archive
cpack -G ZIP
```

#### Linux

```bash
# DEB package
cpack -G DEB

# RPM package
cpack -G RPM

# TAR.GZ archive
cpack -G TGZ
```

#### macOS

```bash
# DMG image
cpack -G DragNDrop

# TAR.GZ archive
cpack -G TGZ
```

## 🔧 Troubleshooting

### Common Build Issues

#### CMake Version Too Old

```bash
# Error: CMake 3.14 or higher is required
# Solution: Update CMake
sudo snap install cmake --classic  # Ubuntu
brew upgrade cmake                  # macOS
winget upgrade Kitware.CMake       # Windows
```

#### Compiler Not Found

```bash
# Error: No suitable C++ compiler found
# Solution: Install compiler
sudo apt install build-essential   # Ubuntu
xcode-select --install             # macOS
# Install Visual Studio on Windows
```

#### Dependency Download Failures

```bash
# Error: Failed to download dependencies
# Solution: Check network connection and clear cache
rm -rf deps_cache/
cmake .. -G Ninja
ninja
```

#### Filesystem Library Issues

```bash
# Error: filesystem library not found (older compilers)
# Solution: Link filesystem library explicitly
cmake .. -DCMAKE_CXX_FLAGS="-lstdc++fs"  # GCC < 9
cmake .. -DCMAKE_CXX_FLAGS="-lc++fs"     # Clang < 9
```

### Build Performance

#### Parallel Builds

```bash
# Use all CPU cores
ninja -j$(nproc)                    # Linux
ninja -j$(sysctl -n hw.ncpu)        # macOS
ninja -j%NUMBER_OF_PROCESSORS%      # Windows
```

#### Compiler Cache

```bash
# Install and configure ccache
sudo apt install ccache             # Ubuntu
brew install ccache                 # macOS

# Configure CMake to use ccache
cmake .. -DCMAKE_CXX_COMPILER_LAUNCHER=ccache
```

#### Memory Usage

```bash
# Limit parallel jobs for low-memory systems
ninja -j2

# Use gold linker for faster linking (Linux)
cmake .. -DCMAKE_EXE_LINKER_FLAGS="-fuse-ld=gold"
```

### Debug Builds

#### Verbose Output

```bash
# CMake verbose output
cmake .. --debug-output

# Make verbose output
ninja -v

# Show all commands
cmake .. -DCMAKE_VERBOSE_MAKEFILE=ON
```

#### Debugging CMake

```bash
# Print all variables
cmake .. -LAH

# Debug find_package
cmake .. --debug-find

# Trace CMake execution
cmake .. --trace
```

---

For additional build support, see the [Installation Guide](../getting-started/installation.md) or visit our [GitHub repository](https://github.com/cpp-scaffold/cpp-scaffold).

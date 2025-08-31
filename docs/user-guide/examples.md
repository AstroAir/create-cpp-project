# Comprehensive Examples

This guide provides detailed examples of using CPP-Scaffold to create various types of C++ projects with different configurations, frameworks, and tools.

## 🚀 Quick Start Examples

### Basic Project Creation

#### Minimal Console Application
```bash
# Simplest possible console app
cpp-scaffold hello-world --type console

# With basic testing
cpp-scaffold hello-world --type console --test gtest
```

#### Standard Library Project
```bash
# Create a library with modern setup
cpp-scaffold my-lib --type lib --build cmake --package vcpkg --test catch2
```

### Interactive Mode Examples

Start the interactive wizard for guided project creation:

```bash
# Launch interactive mode
cpp-scaffold

# Follow the prompts:
# Project name: MyProject
# Template type: [1] Console, [2] Library, [3] GUI... → 1
# Build system: [1] CMake, [2] Meson... → 1
# Package manager: [1] vcpkg, [2] Conan... → 1
# Include tests? [y/N] → y
# Test framework: [1] GTest, [2] Catch2... → 1
```

## 🏗️ Template-Specific Examples

### Console Applications

#### CLI Tool with Argument Parsing
```bash
cpp-scaffold file-analyzer \
  --type console \
  --build cmake \
  --package vcpkg \
  --test gtest \
  --ci github \
  --editor vscode \
  --std cpp17
```

**Generated features:**
- Command-line argument parsing with `argparse` or `CLI11`
- File I/O operations with error handling
- Logging with `spdlog`
- Unit tests with Google Test
- GitHub Actions CI/CD pipeline
- VSCode debugging configuration

**Usage after creation:**
```bash
cd file-analyzer
mkdir build && cd build
cmake .. -G Ninja
ninja

# Run the tool
./file-analyzer --help
./file-analyzer --count file1.txt file2.txt
./file-analyzer --search "pattern" *.cpp
```

#### System Utility
```bash
cpp-scaffold sys-monitor \
  --type console \
  --build cmake \
  --package conan \
  --test doctest \
  --std cpp20 \
  --docs
```

### Library Projects

#### Header-Only Utility Library
```bash
cpp-scaffold utils-lib \
  --type header-only-lib \
  --build cmake \
  --package vcpkg \
  --test catch2 \
  --docs \
  --ci github,gitlab
```

**Generated structure:**
```
utils-lib/
├── include/utils-lib/
│   ├── algorithm.hpp
│   ├── string_utils.hpp
│   └── math_utils.hpp
├── tests/
├── examples/
├── docs/
└── CMakeLists.txt
```

#### Shared Library with C API
```bash
cpp-scaffold math-engine \
  --type lib \
  --build cmake \
  --package conan \
  --test gtest \
  --docs \
  --std cpp20
```

### GUI Applications

#### Qt Desktop Application
```bash
cpp-scaffold image-editor \
  --type gui \
  --framework qt \
  --build cmake \
  --package vcpkg \
  --test gtest \
  --editor vscode
```

**Includes:**
- Qt6 main window application
- Modern Qt widgets and layouts
- Resource management (images, icons)
- Settings persistence
- Cross-platform deployment scripts

#### SFML Game Application
```bash
cpp-scaffold space-game \
  --type gui \
  --framework sfml \
  --build cmake \
  --package vcpkg \
  --test catch2
```

### Network Applications

#### REST API Server
```bash
cpp-scaffold api-server \
  --type webservice \
  --build cmake \
  --package conan \
  --test gtest \
  --ci github \
  --network-lib boost
```

**Features:**
- HTTP server with routing
- JSON request/response handling
- Database integration options
- Authentication middleware
- API documentation generation

#### TCP Client/Server
```bash
cpp-scaffold chat-server \
  --type network \
  --build cmake \
  --package vcpkg \
  --test catch2 \
  --network-lib asio
```

## 🔧 Build System Examples

### CMake Projects

#### Modern CMake with vcpkg
```bash
cpp-scaffold modern-app \
  --type console \
  --build cmake \
  --package vcpkg \
  --test gtest
```

**Generated CMakeLists.txt features:**
- Modern CMake 3.14+ practices
- Target-based dependency management
- Proper include directories and compile features
- Testing integration with CTest
- Package configuration generation

#### CMake with Conan
```bash
cpp-scaffold conan-app \
  --type lib \
  --build cmake \
  --package conan \
  --test catch2
```

### Meson Projects

```bash
cpp-scaffold meson-app \
  --type console \
  --build meson \
  --package none \
  --test gtest
```

**Generated meson.build features:**
- Modern Meson syntax
- Dependency management
- Testing integration
- Cross-compilation support

### Bazel Projects

```bash
cpp-scaffold bazel-lib \
  --type lib \
  --build bazel \
  --package none \
  --test gtest
```

## 📦 Package Manager Examples

### vcpkg Integration

```bash
cpp-scaffold vcpkg-demo \
  --type console \
  --build cmake \
  --package vcpkg \
  --test gtest

cd vcpkg-demo

# Install dependencies
vcpkg install fmt spdlog nlohmann-json

# Build with vcpkg toolchain
cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=vcpkg/scripts/buildsystems/vcpkg.cmake
cmake --build build
```

### Conan Integration

```bash
cpp-scaffold conan-demo \
  --type lib \
  --build cmake \
  --package conan \
  --test catch2

cd conan-demo

# Install dependencies
conan install . --build=missing

# Build with Conan
cmake --preset conan-default
cmake --build --preset conan-release
```

### MSYS2 Integration

```bash
cpp-scaffold msys2-app \
  --type console \
  --build cmake \
  --package msys2 \
  --test gtest

cd msys2-app

# In MSYS2 MinGW64 shell:
pacman -S mingw-w64-x86_64-cmake mingw-w64-x86_64-ninja
mkdir build && cd build
cmake .. -G Ninja
ninja
```

## 🧪 Testing Framework Examples

### Google Test
```bash
cpp-scaffold gtest-demo \
  --type lib \
  --build cmake \
  --package vcpkg \
  --test gtest
```

**Generated test structure:**
```cpp
// tests/test_my_class.cpp
#include <gtest/gtest.h>
#include "my_class.h"

class MyClassTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Setup code
    }
};

TEST_F(MyClassTest, BasicFunctionality) {
    EXPECT_EQ(42, my_function());
}
```

### Catch2
```bash
cpp-scaffold catch2-demo \
  --type lib \
  --build cmake \
  --package conan \
  --test catch2
```

### Doctest
```bash
cpp-scaffold doctest-demo \
  --type header-only-lib \
  --build cmake \
  --package vcpkg \
  --test doctest
```

## 🔄 CI/CD Examples

### GitHub Actions
```bash
cpp-scaffold github-ci \
  --type lib \
  --build cmake \
  --package vcpkg \
  --test gtest \
  --ci github
```

**Generated workflow features:**
- Multi-platform builds (Windows, Linux, macOS)
- Multiple compiler support (GCC, Clang, MSVC)
- Dependency caching
- Test execution and reporting
- Release automation

### GitLab CI
```bash
cpp-scaffold gitlab-ci \
  --type console \
  --build cmake \
  --package conan \
  --test catch2 \
  --ci gitlab
```

### Multiple CI Systems
```bash
cpp-scaffold multi-ci \
  --type lib \
  --build cmake \
  --package vcpkg \
  --test gtest \
  --ci github,gitlab,travis
```

## 🎯 Real-World Project Examples

### Complete Examples Directory

The `examples/` directory contains complete, buildable projects:

- `examples/console-file-processor/` - File processing utility
- `examples/math-library/` - Mathematical computation library  
- `examples/qt-image-viewer/` - Qt-based image viewer
- `examples/rest-api-server/` - HTTP REST API server
- `examples/game-engine-demo/` - Simple game engine with SFML

Each example includes:
- Complete source code
- Build instructions
- Usage documentation
- Test suite
- CI/CD configuration

## 🔍 Next Steps

After creating your project:

1. **Explore the generated code** - Understand the project structure
2. **Build and test** - Verify everything works
3. **Customize** - Modify templates and configurations
4. **Extend** - Add your own features and dependencies
5. **Deploy** - Use the CI/CD pipeline for releases

## 📚 Related Documentation

- [Template System Guide](../components/template-system.md)
- [Build System Configuration](../user-guide/configuration.md)
- [Testing Framework Integration](../components/testing-framework.md)
- [CI/CD Setup Guide](../tutorials/advanced-features.md)

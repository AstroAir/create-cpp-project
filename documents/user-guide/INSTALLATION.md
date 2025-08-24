# CPP-Scaffold Installation Guide

This guide provides comprehensive installation instructions for CPP-Scaffold on all supported platforms.

## 📋 Table of Contents

- [System Requirements](#system-requirements)
- [Quick Installation](#quick-installation)
- [Platform-Specific Instructions](#platform-specific-instructions)
- [Build from Source](#build-from-source)
- [Dependencies](#dependencies)
- [Verification](#verification)
- [Troubleshooting](#troubleshooting)
- [Uninstallation](#uninstallation)

## 🔧 System Requirements

### Minimum Requirements

- **Operating System**: Windows 10+, macOS 10.15+, or Linux (Ubuntu 18.04+, CentOS 7+)
- **Compiler**: C++17 compatible compiler
  - GCC 7.0 or later
  - Clang 6.0 or later
  - MSVC 2017 (Visual Studio 15.7) or later
- **CMake**: 3.14 or later
- **Git**: 2.0 or later (for project initialization)
- **Memory**: 512 MB RAM minimum, 1 GB recommended
- **Disk Space**: 100 MB for installation, additional space for generated projects

### Recommended Requirements

- **Build System**: Ninja (for faster builds)
- **Package Managers**: vcpkg, Conan (for dependency management)
- **IDE**: VSCode, CLion, or Visual Studio (for enhanced development experience)

## ⚡ Quick Installation

### Option 1: Pre-built Binaries (Coming Soon)

```bash
# Download and install (when available)
curl -L https://github.com/cpp-scaffold/releases/latest/download/cpp-scaffold-linux.tar.gz | tar xz
sudo mv cpp-scaffold /usr/local/bin/
```

### Option 2: Build from Source (Current Method)

```bash
# Clone and build
git clone https://github.com/cpp-scaffold/cpp-scaffold.git
cd cpp-scaffold
mkdir build && cd build
cmake .. -G Ninja -DCMAKE_BUILD_TYPE=Release
ninja
```

## 🖥️ Platform-Specific Instructions

### Windows

#### Prerequisites Installation

1. **Install Visual Studio 2019/2022** with C++ development tools:
   ```powershell
   # Using winget (Windows 10 1709+)
   winget install Microsoft.VisualStudio.2022.Community
   ```

2. **Install CMake**:
   ```powershell
   # Using winget
   winget install Kitware.CMake
   
   # Or using Chocolatey
   choco install cmake
   ```

3. **Install Git**:
   ```powershell
   winget install Git.Git
   ```

4. **Install Ninja** (recommended):
   ```powershell
   winget install Ninja-build.Ninja
   ```

#### Building CPP-Scaffold

```powershell
# Clone the repository
git clone https://github.com/cpp-scaffold/cpp-scaffold.git
cd cpp-scaffold

# Build using the provided script
.\build.bat

# Or build manually
mkdir build
cd build
cmake .. -G "Visual Studio 17 2022" -A x64
cmake --build . --config Release

# Or with Ninja
cmake .. -G Ninja -DCMAKE_BUILD_TYPE=Release
ninja
```

#### Installation

```powershell
# Copy to a directory in PATH
copy build\cpp-scaffold.exe C:\Windows\System32\
# Or add build directory to PATH
```

### macOS

#### Prerequisites Installation

1. **Install Xcode Command Line Tools**:
   ```bash
   xcode-select --install
   ```

2. **Install Homebrew** (if not already installed):
   ```bash
   /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
   ```

3. **Install dependencies**:
   ```bash
   brew install cmake ninja git
   ```

#### Building CPP-Scaffold

```bash
# Clone the repository
git clone https://github.com/cpp-scaffold/cpp-scaffold.git
cd cpp-scaffold

# Build
mkdir build && cd build
cmake .. -G Ninja -DCMAKE_BUILD_TYPE=Release
ninja

# Install system-wide (optional)
sudo ninja install
```

### Linux

#### Ubuntu/Debian

```bash
# Update package list
sudo apt update

# Install prerequisites
sudo apt install -y build-essential cmake ninja-build git

# For older Ubuntu versions, you might need a newer CMake
# sudo snap install cmake --classic

# Clone and build
git clone https://github.com/cpp-scaffold/cpp-scaffold.git
cd cpp-scaffold
mkdir build && cd build
cmake .. -G Ninja -DCMAKE_BUILD_TYPE=Release
ninja

# Install system-wide
sudo ninja install
```

#### CentOS/RHEL/Fedora

```bash
# CentOS/RHEL 8+
sudo dnf groupinstall "Development Tools"
sudo dnf install cmake ninja-build git

# CentOS/RHEL 7 (requires newer CMake)
sudo yum groupinstall "Development Tools"
sudo yum install git
# Install CMake 3.14+ from source or EPEL

# Fedora
sudo dnf install gcc-c++ cmake ninja-build git

# Build and install
git clone https://github.com/cpp-scaffold/cpp-scaffold.git
cd cpp-scaffold
mkdir build && cd build
cmake .. -G Ninja -DCMAKE_BUILD_TYPE=Release
ninja
sudo ninja install
```

#### Arch Linux

```bash
# Install prerequisites
sudo pacman -S base-devel cmake ninja git

# Build and install
git clone https://github.com/cpp-scaffold/cpp-scaffold.git
cd cpp-scaffold
mkdir build && cd build
cmake .. -G Ninja -DCMAKE_BUILD_TYPE=Release
ninja
sudo ninja install
```

## 🔨 Build from Source

### Standard Build Process

```bash
# 1. Clone the repository
git clone https://github.com/cpp-scaffold/cpp-scaffold.git
cd cpp-scaffold

# 2. Create build directory
mkdir build && cd build

# 3. Configure with CMake
cmake .. -G Ninja -DCMAKE_BUILD_TYPE=Release

# 4. Build
ninja

# 5. Run tests (optional)
ctest

# 6. Install (optional)
sudo ninja install  # Linux/macOS
# or copy executable to PATH on Windows
```

### Build Options

| Option | Description | Default |
|--------|-------------|---------|
| `CMAKE_BUILD_TYPE` | Build type (Debug, Release, RelWithDebInfo) | `Release` |
| `BUILD_TESTING` | Build unit tests | `ON` |
| `CMAKE_INSTALL_PREFIX` | Installation directory | `/usr/local` (Unix) |

### Advanced Build Configuration

```bash
# Debug build with tests
cmake .. -G Ninja -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTING=ON

# Release build without tests
cmake .. -G Ninja -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTING=OFF

# Custom installation prefix
cmake .. -G Ninja -DCMAKE_INSTALL_PREFIX=/opt/cpp-scaffold

# Static linking (if supported)
cmake .. -G Ninja -DBUILD_SHARED_LIBS=OFF
```

## 📦 Dependencies

CPP-Scaffold automatically manages its dependencies using CMake FetchContent. The following libraries are downloaded and built automatically:

### Runtime Dependencies

- **spdlog v1.12.0**: High-performance logging library
- **nlohmann/json v3.11.2**: JSON parsing and generation
- **fmt**: Modern formatting library (included with spdlog)

### Build Dependencies

- **Google Test v1.14.0**: Testing framework (when `BUILD_TESTING=ON`)

### System Dependencies

- **Filesystem library**: Automatically detected and linked for older compilers
- **Threading library**: Required for concurrent operations

### Dependency Caching

Dependencies are cached in the `deps_cache/` directory to avoid re-downloading:

```bash
# Clear dependency cache if needed
rm -rf deps_cache/
```

## ✅ Verification

### Basic Verification

```bash
# Check if cpp-scaffold is installed and working
cpp-scaffold --version
cpp-scaffold --help

# Create a test project
cpp-scaffold TestProject --type console --build cmake
cd TestProject
mkdir build && cd build
cmake .. -G Ninja
ninja
./TestProject  # Should run successfully
```

### Comprehensive Testing

```bash
# Run the built-in test suite
cd cpp-scaffold/build
ctest --verbose

# Test different project types
cpp-scaffold TestConsole --type console
cpp-scaffold TestLib --type lib
cpp-scaffold TestGui --type gui --framework qt
```

## 🔧 Troubleshooting

### Common Issues

#### CMake Version Too Old

**Problem**: `CMake 3.14 or higher is required`

**Solution**:
```bash
# Ubuntu/Debian
sudo snap install cmake --classic

# macOS
brew upgrade cmake

# Windows
winget upgrade Kitware.CMake
```

#### Compiler Not Found

**Problem**: `No suitable C++ compiler found`

**Solution**:
```bash
# Ubuntu/Debian
sudo apt install build-essential

# macOS
xcode-select --install

# Windows - Install Visual Studio with C++ tools
```

#### Git Not Found

**Problem**: `Git not found in PATH`

**Solution**:
```bash
# Ubuntu/Debian
sudo apt install git

# macOS
brew install git

# Windows
winget install Git.Git
```

#### Permission Denied During Installation

**Problem**: Permission errors during `ninja install`

**Solution**:
```bash
# Use sudo on Unix systems
sudo ninja install

# Or install to user directory
cmake .. -DCMAKE_INSTALL_PREFIX=$HOME/.local
ninja install
```

#### Build Failures

**Problem**: Compilation errors or linking failures

**Solution**:
1. Ensure all prerequisites are installed
2. Clear build directory and rebuild:
   ```bash
   rm -rf build/
   mkdir build && cd build
   cmake .. -G Ninja
   ninja
   ```
3. Check compiler compatibility
4. Update dependencies

### Getting Help

If you encounter issues not covered here:

1. Check the [Troubleshooting Guide](TROUBLESHOOTING.md)
2. Search existing [GitHub Issues](https://github.com/cpp-scaffold/cpp-scaffold/issues)
3. Create a new issue with:
   - Operating system and version
   - Compiler and version
   - CMake version
   - Complete error messages
   - Build configuration used

## 🗑️ Uninstallation

### If Installed via `ninja install`

```bash
# From the build directory
sudo ninja uninstall  # If supported
# or manually remove files listed in install_manifest.txt
```

### Manual Installation

```bash
# Remove the executable
sudo rm /usr/local/bin/cpp-scaffold  # Linux/macOS
# or remove from Windows PATH

# Remove configuration files (optional)
rm -rf ~/.config/cpp-scaffold/  # Linux
rm -rf ~/Library/Preferences/cpp-scaffold/  # macOS
rmdir /S %APPDATA%\cpp-scaffold\  # Windows
```

### Complete Cleanup

```bash
# Remove source directory
rm -rf cpp-scaffold/

# Remove any generated projects (if desired)
# rm -rf <project-directories>
```

---

For additional help, see the [main documentation](README.md) or visit our [GitHub repository](https://github.com/cpp-scaffold/cpp-scaffold).

# CPP-Scaffold Troubleshooting Guide

This guide helps you diagnose and resolve common issues when using CPP-Scaffold.

## 📋 Table of Contents

- [Installation Issues](#installation-issues)
- [Build Problems](#build-problems)
- [Runtime Errors](#runtime-errors)
- [Project Creation Issues](#project-creation-issues)
- [Platform-Specific Issues](#platform-specific-issues)
- [Performance Issues](#performance-issues)
- [Getting Help](#getting-help)

## 🔧 Installation Issues

### CMake Version Too Old

**Problem**: `CMake 3.14 or higher is required. You are running version X.X.X`

**Solutions**:

```bash
# Ubuntu/Debian
sudo snap install cmake --classic
# or
wget -O - https://apt.kitware.com/keys/kitware-archive-latest.asc | sudo apt-key add -
sudo apt-add-repository 'deb https://apt.kitware.com/ubuntu/ focal main'
sudo apt update && sudo apt install cmake

# macOS
brew upgrade cmake

# Windows
winget upgrade Kitware.CMake
# or download from https://cmake.org/download/
```

### Compiler Not Found

**Problem**: `No suitable C++ compiler found` or `C++ compiler not working`

**Solutions**:

```bash
# Ubuntu/Debian
sudo apt update
sudo apt install build-essential

# CentOS/RHEL
sudo yum groupinstall "Development Tools"
# or
sudo dnf groupinstall "Development Tools"

# macOS
xcode-select --install

# Windows - Install Visual Studio with C++ tools
# Download from: https://visualstudio.microsoft.com/downloads/
```

### Git Not Found

**Problem**: `Git not found in PATH`

**Solutions**:

```bash
# Ubuntu/Debian
sudo apt install git

# CentOS/RHEL
sudo yum install git
# or
sudo dnf install git

# macOS
brew install git

# Windows
winget install Git.Git
# or download from: https://git-scm.com/download/win
```

### Permission Denied

**Problem**: Permission errors during installation or execution

**Solutions**:

```bash
# Linux/macOS - Use sudo for system installation
sudo ninja install

# Or install to user directory
cmake .. -DCMAKE_INSTALL_PREFIX=$HOME/.local
ninja install
export PATH=$HOME/.local/bin:$PATH

# Windows - Run as Administrator or install to user directory
```

## 🏗️ Build Problems

### Dependency Download Failures

**Problem**: `Failed to download dependency` or network timeouts

**Solutions**:

1. **Check network connection**:
   ```bash
   ping github.com
   curl -I https://github.com/gabime/spdlog.git
   ```

2. **Clear dependency cache**:
   ```bash
   rm -rf deps_cache/
   cmake .. -G Ninja
   ninja
   ```

3. **Use system packages** (if available):
   ```bash
   # Ubuntu/Debian
   sudo apt install libspdlog-dev nlohmann-json3-dev
   
   # macOS
   brew install spdlog nlohmann-json
   
   # Then rebuild
   cmake .. -G Ninja
   ninja
   ```

4. **Configure proxy** (if behind corporate firewall):
   ```bash
   git config --global http.proxy http://proxy.company.com:8080
   git config --global https.proxy https://proxy.company.com:8080
   ```

### Compilation Errors

**Problem**: Compiler errors during build

**Common Solutions**:

1. **C++17 support**:
   ```bash
   # Ensure compiler supports C++17
   g++ --version  # Should be 7.0+
   clang++ --version  # Should be 6.0+
   
   # Force C++17 standard
   cmake .. -DCMAKE_CXX_STANDARD=17
   ```

2. **Filesystem library issues** (older compilers):
   ```bash
   # GCC < 9 or Clang < 9
   cmake .. -DCMAKE_CXX_FLAGS="-lstdc++fs"  # GCC
   cmake .. -DCMAKE_CXX_FLAGS="-lc++fs"     # Clang
   ```

3. **Missing headers**:
   ```bash
   # Install development packages
   sudo apt install libc++-dev libc++abi-dev  # Ubuntu
   ```

### Linking Errors

**Problem**: Undefined symbols or linking failures

**Solutions**:

1. **Clean rebuild**:
   ```bash
   rm -rf build/
   mkdir build && cd build
   cmake .. -G Ninja
   ninja
   ```

2. **Check library paths**:
   ```bash
   # Linux
   export LD_LIBRARY_PATH=/usr/local/lib:$LD_LIBRARY_PATH
   
   # macOS
   export DYLD_LIBRARY_PATH=/usr/local/lib:$DYLD_LIBRARY_PATH
   ```

3. **Static linking** (if dynamic linking fails):
   ```bash
   cmake .. -DBUILD_SHARED_LIBS=OFF
   ```

## 🚀 Runtime Errors

### Segmentation Fault

**Problem**: Application crashes with segmentation fault

**Debugging Steps**:

1. **Run with debugger**:
   ```bash
   # Build debug version
   cmake .. -DCMAKE_BUILD_TYPE=Debug
   ninja
   
   # Run with gdb
   gdb ./cpp-scaffold
   (gdb) run --your-arguments
   (gdb) bt  # Show backtrace when it crashes
   ```

2. **Use AddressSanitizer**:
   ```bash
   cmake .. -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_FLAGS="-fsanitize=address"
   ninja
   ./cpp-scaffold --your-arguments
   ```

3. **Check for null pointers**:
   ```bash
   # Enable additional debugging
   cmake .. -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_FLAGS="-g -O0 -DDEBUG"
   ```

### Configuration File Errors

**Problem**: `Failed to load configuration` or JSON parsing errors

**Solutions**:

1. **Reset configuration**:
   ```bash
   # Remove corrupted config files
   rm -rf ~/.config/cpp-scaffold/  # Linux
   rm -rf ~/Library/Preferences/cpp-scaffold/  # macOS
   rmdir /S %APPDATA%\cpp-scaffold\  # Windows
   ```

2. **Validate JSON**:
   ```bash
   # Check configuration file syntax
   python -m json.tool ~/.config/cpp-scaffold/config.json
   ```

3. **Use default configuration**:
   ```bash
   cpp-scaffold --reset-config
   ```

## 📁 Project Creation Issues

### Project Already Exists

**Problem**: `Directory already exists` error

**Solutions**:

```bash
# Remove existing directory
rm -rf ProjectName/

# Or use different name
cpp-scaffold ProjectName2 --type console

# Or force overwrite (if supported)
cpp-scaffold ProjectName --force --type console
```

### Template Not Found

**Problem**: `Unknown template type` or template errors

**Solutions**:

1. **List available templates**:
   ```bash
   cpp-scaffold --list-templates
   ```

2. **Check spelling**:
   ```bash
   # Correct template names
   cpp-scaffold MyProject --type console      # ✓
   cpp-scaffold MyProject --type library      # ✗ (should be 'lib')
   cpp-scaffold MyProject --type lib          # ✓
   ```

3. **Update to latest version**:
   ```bash
   git pull origin main
   mkdir build && cd build
   cmake .. -G Ninja
   ninja
   ```

### Package Manager Issues

**Problem**: Package manager not found or dependency installation fails

**Solutions**:

1. **Install package manager**:
   ```bash
   # vcpkg
   git clone https://github.com/Microsoft/vcpkg.git
   cd vcpkg
   ./bootstrap-vcpkg.sh  # Linux/macOS
   .\bootstrap-vcpkg.bat  # Windows
   
   # Conan
   pip install conan
   ```

2. **Skip package manager**:
   ```bash
   cpp-scaffold MyProject --type console --package none
   ```

3. **Use alternative package manager**:
   ```bash
   cpp-scaffold MyProject --type console --package conan
   ```

## 🖥️ Platform-Specific Issues

### Windows Issues

#### MSVC Compiler Issues
```powershell
# Ensure Visual Studio is properly installed
# Install "C++ build tools" workload

# Use Developer Command Prompt
# Start -> Visual Studio 2022 -> Developer Command Prompt

# Or set environment manually
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"
```

#### Path Length Limitations
```powershell
# Enable long path support (Windows 10 1607+)
# Run as Administrator
New-ItemProperty -Path "HKLM:\SYSTEM\CurrentControlSet\Control\FileSystem" -Name "LongPathsEnabled" -Value 1 -PropertyType DWORD -Force

# Or use shorter paths
cpp-scaffold MyProj --type console  # Instead of MyVeryLongProjectName
```

### macOS Issues

#### Xcode Command Line Tools
```bash
# Install if missing
xcode-select --install

# Reset if corrupted
sudo xcode-select --reset
xcode-select --install
```

#### Homebrew Issues
```bash
# Update Homebrew
brew update
brew upgrade

# Fix permissions
sudo chown -R $(whoami) $(brew --prefix)/*
```

### Linux Issues

#### Missing Development Packages
```bash
# Ubuntu/Debian
sudo apt update
sudo apt install build-essential cmake ninja-build git pkg-config

# CentOS/RHEL 8+
sudo dnf groupinstall "Development Tools"
sudo dnf install cmake ninja-build git

# Arch Linux
sudo pacman -S base-devel cmake ninja git
```

#### Library Path Issues
```bash
# Add to ~/.bashrc or ~/.zshrc
export LD_LIBRARY_PATH=/usr/local/lib:$LD_LIBRARY_PATH
export PKG_CONFIG_PATH=/usr/local/lib/pkgconfig:$PKG_CONFIG_PATH

# Or use ldconfig
echo "/usr/local/lib" | sudo tee /etc/ld.so.conf.d/local.conf
sudo ldconfig
```

## ⚡ Performance Issues

### Slow Build Times

**Solutions**:

1. **Use Ninja instead of Make**:
   ```bash
   cmake .. -G Ninja  # Instead of default generator
   ```

2. **Parallel builds**:
   ```bash
   ninja -j$(nproc)  # Use all CPU cores
   ninja -j4         # Use 4 cores
   ```

3. **Use compiler cache**:
   ```bash
   # Install ccache
   sudo apt install ccache  # Ubuntu
   brew install ccache      # macOS
   
   # Configure CMake
   cmake .. -DCMAKE_CXX_COMPILER_LAUNCHER=ccache
   ```

4. **Reduce debug info**:
   ```bash
   cmake .. -DCMAKE_BUILD_TYPE=RelWithDebInfo  # Instead of Debug
   ```

### High Memory Usage

**Solutions**:

1. **Limit parallel jobs**:
   ```bash
   ninja -j2  # Reduce from default
   ```

2. **Use gold linker** (Linux):
   ```bash
   cmake .. -DCMAKE_EXE_LINKER_FLAGS="-fuse-ld=gold"
   ```

3. **Split compilation**:
   ```bash
   # Build specific targets
   ninja cpp-scaffold  # Just the main executable
   ninja test_file_utils  # Just one test
   ```

### Slow Project Creation

**Solutions**:

1. **Skip unnecessary features**:
   ```bash
   cpp-scaffold MyProject --type console --no-tests --no-docs --no-git
   ```

2. **Use local templates**:
   ```bash
   # Copy templates locally for faster access
   cpp-scaffold MyProject --custom-template ./local-templates/console
   ```

## 🆘 Getting Help

### Diagnostic Information

When reporting issues, include:

```bash
# System information
uname -a                    # Linux/macOS
systeminfo                 # Windows

# Compiler information
g++ --version
clang++ --version

# CMake information
cmake --version

# CPP-Scaffold information
cpp-scaffold --version

# Build log
cmake .. -G Ninja 2>&1 | tee cmake.log
ninja 2>&1 | tee build.log
```

### Log Files

Check these locations for detailed logs:

```bash
# Application logs
./cpp-scaffold.log         # Current directory
~/.config/cpp-scaffold/logs/  # Linux
~/Library/Logs/cpp-scaffold/  # macOS
%APPDATA%\cpp-scaffold\logs\  # Windows

# Build logs
build/CMakeFiles/CMakeOutput.log
build/CMakeFiles/CMakeError.log
```

### Support Channels

1. **GitHub Issues**: [Report bugs and request features](https://github.com/cpp-scaffold/cpp-scaffold/issues)
2. **GitHub Discussions**: [Ask questions and get help](https://github.com/cpp-scaffold/cpp-scaffold/discussions)
3. **Documentation**: [Read the docs](https://github.com/cpp-scaffold/cpp-scaffold/tree/main/docs)

### Before Reporting Issues

1. **Search existing issues**: Check if your problem is already reported
2. **Try latest version**: Update to the latest release
3. **Minimal reproduction**: Create a minimal example that reproduces the issue
4. **Include logs**: Attach relevant log files and error messages
5. **Environment details**: Include OS, compiler, and version information

---

If this guide doesn't solve your problem, please [open an issue](https://github.com/cpp-scaffold/cpp-scaffold/issues/new) with detailed information about your environment and the specific problem you're encountering.

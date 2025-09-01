# Package Management Guide

## Overview

This guide covers package management strategies and tools for C++ project dependencies.

## Supported Package Managers

### vcpkg
Microsoft's C++ package manager with extensive library support.

```bash
# Install vcpkg
git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg
./bootstrap-vcpkg.sh  # Linux/macOS
./bootstrap-vcpkg.bat # Windows

# Install packages
./vcpkg install nlohmann-json spdlog fmt
```

### Conan
Decentralized package manager with binary packages.

```bash
# Install Conan
pip install conan

# Create profile
conan profile detect --force

# Install dependencies
conan install . --build=missing
```

### MSYS2 (Windows)
Native package manager for MSYS2 environment.

```bash
# Update package database
pacman -Sy

# Install development tools
pacman -S mingw-w64-x86_64-toolchain
pacman -S mingw-w64-x86_64-cmake
pacman -S mingw-w64-x86_64-ninja

# Install libraries
pacman -S mingw-w64-x86_64-boost
pacman -S mingw-w64-x86_64-qt6
```

### System Package Managers

#### Ubuntu/Debian
```bash
# Install build tools
sudo apt update
sudo apt install build-essential cmake ninja-build

# Install libraries
sudo apt install libboost-all-dev libfmt-dev nlohmann-json3-dev
```

#### macOS (Homebrew)
```bash
# Install build tools
brew install cmake ninja pkg-config

# Install libraries
brew install boost fmt nlohmann-json
```

## Integration with Build Systems

### XMake Integration
```lua
-- xmake.lua
add_requires("nlohmann_json", "spdlog", "fmt")

target("myapp")
    set_kind("binary")
    add_files("src/*.cpp")
    add_packages("nlohmann_json", "spdlog", "fmt")
```

### CMake Integration
```cmake
# CMakeLists.txt
find_package(nlohmann_json REQUIRED)
find_package(spdlog REQUIRED)
find_package(fmt REQUIRED)

target_link_libraries(myapp 
    nlohmann_json::nlohmann_json
    spdlog::spdlog
    fmt::fmt
)
```

## Best Practices

### Version Management
- Pin specific versions in production
- Use version ranges for development
- Document minimum required versions

### Dependency Resolution
- Prefer system packages when available
- Use package managers for missing dependencies
- Provide fallback build instructions

### Cross-Platform Compatibility
- Test package resolution on all target platforms
- Handle platform-specific dependencies
- Use conditional dependencies when needed

## Troubleshooting

### Common Issues
- **Package not found**: Update package database
- **Version conflicts**: Check dependency constraints
- **Build failures**: Verify compiler compatibility

### Debug Commands
```bash
# vcpkg
./vcpkg list
./vcpkg search <package>

# Conan
conan search <package>
conan info . --graph=graph.html

# MSYS2
pacman -Q  # List installed packages
pacman -Ss <package>  # Search packages
```

## See Also

- [Build System Documentation](../developer-guide/build-system.md)
- [XMake Build System](../developer-guide/xmake-build-system.md)
- [Installation Guide](../installation/index.md)

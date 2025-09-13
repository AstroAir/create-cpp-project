# Cross-Compilation Guide

## Overview

This guide covers cross-compilation techniques for building C++ projects across different target platforms and architectures.

## Supported Targets

### Windows

- **x86_64-w64-mingw32** - 64-bit Windows
- **i686-w64-mingw32** - 32-bit Windows

### Linux

- **x86_64-linux-gnu** - 64-bit Linux
- **aarch64-linux-gnu** - ARM64 Linux
- **arm-linux-gnueabihf** - ARM32 Linux

### macOS

- **x86_64-apple-darwin** - Intel macOS
- **aarch64-apple-darwin** - Apple Silicon macOS

## Toolchain Setup

### Using XMake

```bash
# Configure for Windows target
xmake config --plat=windows --arch=x64 --toolchain=mingw

# Configure for Linux target
xmake config --plat=linux --arch=x86_64 --toolchain=gcc

# Configure for macOS target
xmake config --plat=macosx --arch=x86_64 --toolchain=clang
```

### Using CMake

```bash
# Windows cross-compilation
cmake -DCMAKE_TOOLCHAIN_FILE=cmake/toolchains/mingw-w64.cmake ..

# Linux cross-compilation
cmake -DCMAKE_TOOLCHAIN_FILE=cmake/toolchains/linux-gcc.cmake ..
```

## Common Issues

### Path Separators

- Use forward slashes in build scripts
- Handle Windows path conversion in MSYS2

### Library Dependencies

- Ensure target-specific libraries are available
- Use package managers that support cross-compilation

### Testing

- Use emulation or target hardware for testing
- Set up CI/CD for multiple platforms

## See Also

- [Build System Documentation](../developer-guide/build-system.md)
- [XMake Build System](../developer-guide/xmake-build-system.md)
- [Platform-Specific Notes](platforms.md)

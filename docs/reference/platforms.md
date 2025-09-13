# Platform-Specific Notes

## Overview

This document contains platform-specific considerations, workarounds, and best practices for building C++ projects.

## Windows

### MSYS2 Environment

- Use MinGW64 shell for 64-bit builds
- Use MinGW32 shell for 32-bit builds
- Avoid MSYS2 shell for native Windows builds

### Path Handling

```bash
# Prevent path conversion issues
export MSYS2_ARG_CONV_EXCL="-DCMAKE_INSTALL_PREFIX="
```

### Common Issues

- **DLL not found**: Ensure runtime dependencies are in PATH
- **Permission denied**: Don't run as administrator
- **Path too long**: Use shorter build directory paths

## Linux

### Distribution Differences

- **Ubuntu/Debian**: Use `apt` for dependencies
- **CentOS/RHEL**: Use `yum` or `dnf` for dependencies
- **Arch Linux**: Use `pacman` for dependencies

### Compiler Versions

```bash
# Check GCC version
gcc --version

# Install specific GCC version (Ubuntu)
sudo apt install gcc-11 g++-11
```

### Library Paths

```bash
# Add custom library paths
export LD_LIBRARY_PATH=/usr/local/lib:$LD_LIBRARY_PATH
```

## macOS

### Xcode Command Line Tools

```bash
# Install Xcode command line tools
xcode-select --install
```

### Homebrew Integration

```bash
# Install dependencies via Homebrew
brew install cmake ninja pkg-config
```

### Apple Silicon Considerations

- Use universal binaries for compatibility
- Test on both Intel and Apple Silicon hardware
- Consider Rosetta 2 compatibility

## Cross-Platform Best Practices

### File System

- Use forward slashes in paths
- Handle case sensitivity differences
- Avoid special characters in filenames

### Environment Variables

- Use consistent variable naming
- Provide platform-specific defaults
- Document required environment setup

### Dependencies

- Use package managers when available
- Provide fallback build instructions
- Test dependency resolution on all platforms

## See Also

- [Cross-Compilation Guide](cross-compilation.md)
- [Build System Documentation](../developer-guide/build-system.md)
- [MSYS2 Integration](../getting-started/msys2-integration.md)

# Build System Documentation

This document describes the enhanced, modular build system with comprehensive cross-platform support.

## Overview

The build system has been completely redesigned to be more modular and support a wide range of platforms including:

- **Desktop Platforms**: Windows, Linux, macOS
- **Mobile Platforms**: Android, iOS
- **Web Platforms**: WebAssembly/Emscripten
- **Embedded Systems**: ARM Cortex-M series
- **Container Environments**: Docker-based builds

## Architecture

### Modular Preset System

The build system uses a hierarchical preset system with inheritance:

```
cmake/presets/
├── base-presets.json          # Base configurations
├── platform-presets.json     # Platform-specific settings
├── optimization-presets.json # Optimization configurations
└── container-presets.json    # Container-based builds
```

### CMake Modules

The system is organized into specialized modules:

```
cmake/modules/
├── PlatformSupport.cmake           # Enhanced platform detection
├── PresetUtils.cmake               # Preset validation utilities
├── ContainerUtils.cmake            # Docker integration
├── WebAssemblySupport.cmake        # WebAssembly/Emscripten support
├── EmbeddedSupport.cmake           # Embedded systems support
├── MobileSupport.cmake             # Android/iOS support
└── CrossCompilationWorkflows.cmake # Advanced cross-compilation
```

## Quick Start

### Basic Usage

```bash
# Configure with default preset
cmake --preset default

# Build with default configuration
cmake --build --preset default

# Run tests
ctest --preset default
```

### Platform-Specific Builds

```bash
# Android build
cmake --preset android-arm64
cmake --build --preset android-arm64

# iOS build (macOS only)
cmake --preset ios-device
cmake --build --preset ios-device

# WebAssembly build
cmake --preset wasm-web
cmake --build --preset wasm-web

# Embedded ARM build
cmake --preset cortex-m4
cmake --build --preset cortex-m4
```

### Container-Based Builds

```bash
# Build using Ubuntu container
cmake --preset ubuntu-container
cmake --build --preset ubuntu-container

# Build using Alpine container
cmake --preset alpine-container
cmake --build --preset alpine-container
```

## Available Presets

### Base Presets

- `default` - Default optimized build
- `debug` - Debug build with all diagnostics
- `release` - Optimized release build
- `fast-build` - Maximum build speed
- `minimal` - Minimal build without optimizations
- `optimized` - Highly optimized build with LTO
- `debug-sanitized` - Debug build with sanitizers
- `profiling` - Build with profiling enabled

### Platform-Specific Presets

#### Desktop Platforms
- `windows-*` - Windows-specific configurations
- `linux-*` - Linux-specific configurations
- `macos-*` - macOS-specific configurations
- `msys2-*` - MSYS2/MinGW configurations

#### Mobile Platforms
- `android-arm64` - Android ARM64 build
- `android-arm32` - Android ARM32 build
- `android-x64` - Android x64 build
- `ios-device` - iOS device build
- `ios-simulator` - iOS simulator build

#### Web Platforms
- `wasm-web` - WebAssembly for web browsers
- `wasm-node` - WebAssembly for Node.js

#### Embedded Platforms
- `cortex-m0` - ARM Cortex-M0 build
- `cortex-m3` - ARM Cortex-M3 build
- `cortex-m4` - ARM Cortex-M4 build

#### Container Platforms
- `ubuntu-container` - Ubuntu development container
- `alpine-container` - Alpine minimal container
- `android-container` - Android development container
- `wasm-container` - WebAssembly container
- `ci-container` - CI/CD optimized container

## Toolchain Requirements

### Android Development
- Android NDK (r21 or later)
- Android SDK (API level 21 or later)

### iOS Development (macOS only)
- Xcode (12.0 or later)
- iOS SDK (12.0 or later)

### WebAssembly Development
- Emscripten SDK (3.1.0 or later)
- Node.js (for testing)

### Embedded Development
- ARM GNU Toolchain (arm-none-eabi-gcc)
- OpenOCD (for flashing)

### Container Development
- Docker (20.10 or later)
- Docker Compose (optional)

## Configuration

### Environment Variables

```bash
# Android
export ANDROID_NDK_ROOT=/path/to/android-ndk
export ANDROID_SDK_ROOT=/path/to/android-sdk

# WebAssembly
export EMSDK=/path/to/emsdk

# Embedded
export ARM_TOOLCHAIN_PATH=/path/to/arm-toolchain

# Container
export USER_ID=$(id -u)
export GROUP_ID=$(id -g)
```

### CMake Variables

Common variables that can be set:

```cmake
# Platform-specific
set(ANDROID_ABI "arm64-v8a")           # Android ABI
set(IOS_PLATFORM "OS")                 # iOS platform
set(ARM_TARGET "cortex-m4")            # ARM target
set(WASM_TARGET "web")                 # WebAssembly target

# Optimization
set(ENABLE_LTO ON)                     # Link-time optimization
set(ENABLE_CCACHE ON)                  # Compiler cache
set(ENABLE_UNITY_BUILD ON)             # Unity builds

# Container
set(CONTAINER_BUILD ON)                # Enable container builds
set(CONTAINER_IMAGE "cpp-scaffold:ubuntu-dev")
```

## Advanced Usage

### Custom Presets

Create custom presets by inheriting from base configurations:

```json
{
    "name": "my-custom-preset",
    "displayName": "My Custom Configuration",
    "inherits": ["base-release", "fast-build-base"],
    "cacheVariables": {
        "MY_CUSTOM_OPTION": "ON"
    }
}
```

### Cross-Compilation Workflows

```cmake
# Create multi-platform workflow
create_cross_compilation_workflow(multi-platform
    TARGETS myapp mylib
    PLATFORMS android ios wasm
    CONFIGURATIONS Debug Release
)
```

### Container Management

```bash
# Build all container images
./scripts/manage-containers.sh build --all

# Run interactive shell
./scripts/manage-containers.sh shell ubuntu-dev

# Clean up images
./scripts/manage-containers.sh clean --dangling
```

## Testing

### Platform-Specific Testing

```bash
# Native tests
ctest --preset default

# Container tests
ctest --preset ubuntu-container

# Cross-compilation tests (with emulator/simulator)
ctest --preset android-container
ctest --preset ios-simulator
```

### Validation

```bash
# Validate presets
cmake -P cmake/scripts/validate-presets.cmake --verbose

# Validate container images
./scripts/manage-containers.sh validate ubuntu-dev
```

## Troubleshooting

### Common Issues

1. **Preset not found**: Ensure all preset files are included in CMakePresets.json
2. **Toolchain not found**: Check environment variables and PATH
3. **Container build fails**: Verify Docker is running and images are built
4. **Cross-compilation errors**: Validate toolchain installation and configuration

### Debug Information

```bash
# Show preset information
cmake --list-presets

# Verbose configuration
cmake --preset default -DCMAKE_VERBOSE_MAKEFILE=ON

# Container information
./scripts/manage-containers.sh status
```

## Migration Guide

### From Old System

1. Update CMakePresets.json to use new inheritance system
2. Replace manual toolchain setup with preset-based configuration
3. Update CI/CD scripts to use new preset names
4. Test all configurations with validation scripts

### Breaking Changes

- Preset names have changed to follow new naming convention
- Some CMake variables have been renamed for consistency
- Container-based builds require Docker setup

## Contributing

When adding new platforms or features:

1. Create appropriate preset configurations
2. Add platform detection in PlatformSupport.cmake
3. Update documentation
4. Add validation tests
5. Update CI/CD workflows

## See Also

- [Container Documentation](CONTAINERS.md)
- [Cross-Compilation Guide](CROSS_COMPILATION.md)
- [Platform-Specific Notes](PLATFORMS.md)

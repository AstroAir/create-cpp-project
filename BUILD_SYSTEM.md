# Build System Documentation

This document describes the modernized build system for the cpp-scaffold project.

## Overview

The build system has been modernized to use:
- **CMake 3.20+** with modern target-based configuration
- **C++20 support** with C++17 fallback for compatibility
- **Ninja generator** for fast parallel builds
- **Precompiled headers** for faster compilation
- **Unity builds** option for maximum build speed
- **ccache support** for faster rebuilds
- **CMake presets** for different build configurations

## Quick Start

### Using CMake Presets (Recommended)

```bash
# Configure and build with default settings
cmake --preset default
cmake --build --preset default

# Debug build with all optimizations
cmake --preset debug
cmake --build --preset debug

# Release build with maximum optimizations
cmake --preset release
cmake --build --preset release

# Fast build for development
cmake --preset fast-build
cmake --build --preset fast-build
```

### Using Build Scripts

#### Windows (PowerShell)
```powershell
# Debug build with tests
.\scripts\build.ps1 -Config debug -Tests

# Release build with installation
.\scripts\build.ps1 -Config release -Clean -Install

# Fast development build
.\scripts\build.ps1 -Config fast-build
```

#### Linux/macOS (Bash)
```bash
# Debug build with tests
./scripts/build.sh --config debug --tests

# Release build with installation
./scripts/build.sh --config release --clean --install

# Fast development build
./scripts/build.sh --config fast-build
```

### Legacy Build (Windows)
```batch
# Simple parallel build
build.bat
```

## Build Configurations

### Available Presets

1. **default** - Balanced configuration for general use
   - Release build type
   - Precompiled headers enabled
   - ccache enabled

2. **debug** - Development configuration
   - Debug build type
   - All warnings enabled
   - Testing enabled
   - Precompiled headers enabled

3. **release** - Production configuration
   - Release build type
   - Unity builds enabled
   - Maximum optimizations
   - Testing disabled

4. **fast-build** - Maximum build speed
   - Debug build type
   - Unity builds enabled
   - Precompiled headers enabled
   - Testing disabled

5. **minimal** - Minimal configuration for debugging build issues
   - Debug build type
   - No build optimizations
   - Testing enabled

## Build Options

### CMake Options

- `ENABLE_UNITY_BUILD` (OFF) - Enable Unity builds for faster compilation
- `ENABLE_PRECOMPILED_HEADERS` (ON) - Enable precompiled headers
- `ENABLE_CCACHE` (ON) - Enable ccache for faster rebuilds
- `BUILD_TESTING` (ON) - Build and enable tests

### Example Custom Configuration

```bash
cmake -B build \
  -G Ninja \
  -DCMAKE_BUILD_TYPE=Release \
  -DENABLE_UNITY_BUILD=ON \
  -DENABLE_PRECOMPILED_HEADERS=ON \
  -DENABLE_CCACHE=ON \
  -DBUILD_TESTING=OFF

cmake --build build --parallel
```

## Performance Features

### Parallel Compilation
- Automatically detects CPU cores
- Uses optimal number of parallel jobs
- Caps at 8 jobs to avoid system overload

### Precompiled Headers
Includes commonly used headers:
- Standard library headers (iostream, string, vector, etc.)
- Third-party libraries (spdlog, nlohmann_json)
- System headers (filesystem)

### Unity Builds
- Combines multiple source files into single compilation units
- Significantly reduces build times
- May break some code patterns (use with caution)

### ccache Support
- Automatically detected and enabled
- Caches compilation results
- Dramatically speeds up rebuilds

## Testing

### Running Tests

```bash
# Run all tests
ctest --preset default

# Run tests in parallel
ctest --preset parallel

# Run tests with verbose output
cmake --build build --target run_tests_verbose
```

### Test Organization
- Individual test executables for focused testing
- Comprehensive `all_tests` executable
- Optimized test library with precompiled headers
- Parallel test discovery and execution

## IDE Integration

### Compile Commands
The build system automatically generates `compile_commands.json` for IDE integration:
- Enables IntelliSense/code completion
- Supports clang-tidy integration
- Works with VS Code, CLion, and other modern IDEs

### Project Structure
- Organized folder structure in IDEs
- Proper target dependencies
- Debug configuration support

## Troubleshooting

### Common Issues

1. **CMake version too old**
   ```
   Solution: Update to CMake 3.20 or newer
   ```

2. **ccache not found**
   ```
   Solution: Install ccache or disable with -DENABLE_CCACHE=OFF
   ```

3. **Unity build failures**
   ```
   Solution: Disable Unity builds with -DENABLE_UNITY_BUILD=OFF
   ```

4. **Precompiled header issues**
   ```
   Solution: Disable PCH with -DENABLE_PRECOMPILED_HEADERS=OFF
   ```

### Build from Scratch
```bash
# Clean all build directories
rm -rf build build-*

# Reconfigure and build
cmake --preset default
cmake --build --preset default
```

## Migration from Old Build System

The modernized build system maintains backward compatibility:
- All existing targets still work
- Same dependencies and libraries
- Improved performance and developer experience
- Original CMakeLists.txt backed up as CMakeLists.txt.backup

## Advanced Usage

### Custom Compiler Flags
```bash
cmake --preset debug -DCMAKE_CXX_FLAGS_DEBUG="-g -O0 -fsanitize=address"
```

### Static Analysis Integration
```bash
# Run clang-tidy
cmake --build build --target clang-tidy

# Generate documentation
cmake --build build --target docs
```

### Packaging
```bash
# Create packages
cmake --build build --target package
```

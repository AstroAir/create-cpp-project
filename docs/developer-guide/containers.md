# Container-Based Development

This document describes the container-based development environment for consistent cross-platform builds.

## Overview

The project provides Docker containers for reproducible development environments across different platforms and use cases:

- **ubuntu-dev**: Full-featured Ubuntu development environment
- **alpine-minimal**: Lightweight Alpine-based environment
- **android-dev**: Android cross-compilation environment
- **wasm-dev**: WebAssembly/Emscripten development environment
- **ci-builder**: Optimized for CI/CD pipelines

## Quick Start

### Using Docker Compose

```bash
# Start Ubuntu development environment
docker-compose -f docker/docker-compose.yml run ubuntu-dev

# Start Alpine minimal environment
docker-compose -f docker/docker-compose.yml run alpine-minimal

# Start Android development environment
docker-compose -f docker/docker-compose.yml run android-dev
```

### Using Management Script

```bash
# Build all container images
./scripts/manage-containers.sh build --all

# Build specific image
./scripts/manage-containers.sh build ubuntu-dev

# Run interactive shell
./scripts/manage-containers.sh shell ubuntu-dev

# Run build command
./scripts/manage-containers.sh run ubuntu-dev build --type Release

# Run tests
./scripts/manage-containers.sh run ubuntu-dev test
```

## Container Images

### Ubuntu Development (ubuntu-dev)

**Purpose**: Full-featured C++ development environment with modern tools

**Includes**:

- GCC 12, Clang 15
- CMake, Ninja
- Cross-compilation toolchains (ARM, AArch64)
- Static analysis tools (clang-tidy, cppcheck)
- Build optimization tools (ccache, distcc)
- vcpkg, Conan package managers
- Development utilities

**Usage**:

```bash
# Interactive development
./scripts/manage-containers.sh shell ubuntu-dev

# Build project
./scripts/manage-containers.sh run ubuntu-dev build --preset release

# Run with custom volume
docker run --rm -v $(pwd):/workspace -it cpp-scaffold:ubuntu-dev
```

### Alpine Minimal (alpine-minimal)

**Purpose**: Lightweight environment for minimal builds

**Includes**:

- GCC, Clang
- CMake, Ninja
- Essential build tools
- vcpkg, Conan
- Basic utilities

**Usage**:

```bash
# Minimal build environment
./scripts/manage-containers.sh shell alpine-minimal

# Size-optimized builds
./scripts/manage-containers.sh run alpine-minimal build --preset minimal
```

### Android Development (android-dev)

**Purpose**: Android cross-compilation environment

**Includes**:

- Android NDK
- Cross-compilation toolchains
- Android SDK tools
- Build tools for APK creation

**Usage**:

```bash
# Android development
./scripts/manage-containers.sh shell android-dev

# Build for Android
./scripts/manage-containers.sh run android-dev build --preset android-arm64
```

### WebAssembly Development (wasm-dev)

**Purpose**: WebAssembly compilation with Emscripten

**Includes**:

- Emscripten SDK
- Node.js for testing
- WebAssembly tools

**Usage**:

```bash
# WebAssembly development
./scripts/manage-containers.sh shell wasm-dev

# Build for web
./scripts/manage-containers.sh run wasm-dev build --preset wasm-web
```

### CI Builder (ci-builder)

**Purpose**: Optimized for automated builds and testing

**Includes**:

- Essential build tools only
- Optimized for speed
- Minimal dependencies

**Usage**:

```bash
# CI/CD builds
./scripts/manage-containers.sh run ci-builder build --preset ci-container
```

## Configuration

### Environment Variables

```bash
# User mapping (Linux/macOS)
export USER_ID=$(id -u)
export GROUP_ID=$(id -g)

# Git configuration
export GIT_USER_NAME="Your Name"
export GIT_USER_EMAIL="your.email@example.com"

# Build optimization
export CCACHE_MAX_SIZE=5G
export VCPKG_ROOT=/opt/vcpkg

# Platform-specific
export ANDROID_NDK_ROOT=/opt/android-ndk
export EMSDK=/opt/emsdk
```

### Volume Mounts

Default volume mounts:

- `$(pwd):/workspace` - Project source code
- `ccache-*:/root/.ccache` - Compiler cache
- `vcpkg-*:/opt/vcpkg/buildtrees` - vcpkg build cache
- `vcpkg-downloads:/opt/vcpkg/downloads` - vcpkg download cache

### Custom Configuration

Create `.env` file for custom settings:

```bash
# .env
USER_ID=1000
GROUP_ID=1000
GIT_USER_NAME=Developer
GIT_USER_EMAIL=dev@example.com
CCACHE_MAX_SIZE=10G
```

## Building Images

### Manual Build

```bash
# Build Ubuntu development image
docker build -f docker/ubuntu-dev.Dockerfile -t cpp-scaffold:ubuntu-dev .

# Build with no cache
docker build --no-cache -f docker/ubuntu-dev.Dockerfile -t cpp-scaffold:ubuntu-dev .
```

### Using Management Script

```bash
# Build all images
./scripts/manage-containers.sh build --all

# Build specific image
./scripts/manage-containers.sh build ubuntu-dev

# Force rebuild
./scripts/manage-containers.sh build ubuntu-dev --force

# Build without cache
./scripts/manage-containers.sh build ubuntu-dev --no-cache
```

## Development Workflow

### Interactive Development

```bash
# Start development container
./scripts/manage-containers.sh shell ubuntu-dev

# Inside container:
cd /workspace
cmake --preset debug
cmake --build --preset debug
ctest --preset default
```

### Automated Builds

```bash
# One-command build
./scripts/manage-containers.sh run ubuntu-dev build --preset release

# Run tests
./scripts/manage-containers.sh run ubuntu-dev test

# Custom command
./scripts/manage-containers.sh run ubuntu-dev cmake --list-presets
```

### Multi-Platform Builds

```bash
# Build for multiple platforms
for platform in ubuntu-dev alpine-minimal android-dev; do
    ./scripts/manage-containers.sh run $platform build --preset release
done
```

## Troubleshooting

### Common Issues

1. **Permission denied**: Ensure USER_ID and GROUP_ID are set correctly
2. **Container not found**: Build the image first
3. **Volume mount issues**: Check Docker volume permissions
4. **Out of disk space**: Clean up unused images and volumes

### Debugging

```bash
# Check container status
./scripts/manage-containers.sh status

# Validate container image
./scripts/manage-containers.sh validate ubuntu-dev

# Show container information
./scripts/manage-containers.sh run ubuntu-dev info

# Debug build issues
./scripts/manage-containers.sh run ubuntu-dev build --verbose
```

### Cleanup

```bash
# Remove dangling images
./scripts/manage-containers.sh clean --dangling

# Remove all unused images
./scripts/manage-containers.sh clean --all

# Force cleanup
./scripts/manage-containers.sh clean --all --force

# Clean Docker system
docker system prune -af
```

## Performance Optimization

### Build Speed

- Use ccache for faster rebuilds
- Enable Unity builds for large projects
- Use parallel builds with appropriate job count
- Mount ccache volume for persistent cache

### Resource Usage

- Limit container memory if needed: `--memory=4g`
- Use multi-stage builds to reduce image size
- Clean up build artifacts regularly
- Use .dockerignore to exclude unnecessary files

### Network Optimization

- Use local package mirrors when possible
- Cache package downloads in volumes
- Use multi-stage builds to minimize layers

## Security Considerations

- Containers run with user privileges when USER_ID is set
- Avoid running as root in production
- Use specific image tags instead of 'latest'
- Regularly update base images for security patches
- Scan images for vulnerabilities

## Integration with CI/CD

### GitHub Actions

```yaml
- name: Build in container
  run: |
    ./scripts/manage-containers.sh build ci-builder
    ./scripts/manage-containers.sh run ci-builder build --preset ci-container
```

### GitLab CI

```yaml
build:
  script:
    - ./scripts/manage-containers.sh build ci-builder
    - ./scripts/manage-containers.sh run ci-builder build --preset ci-container
```

## Customization

### Custom Images

Create custom Dockerfile:

```dockerfile
FROM cpp-scaffold:ubuntu-dev
RUN apt-get update && apt-get install -y my-custom-tool
```

### Custom Scripts

Add custom scripts to `docker/scripts/` directory and they will be available in containers.

### Environment Customization

Modify `docker/scripts/entrypoint.sh` to customize container initialization.

## See Also

- [Build System Documentation](build-system.md)
- [Cross-Compilation Guide](../reference/cross-compilation.md)
- [Platform-Specific Notes](../reference/platforms.md)

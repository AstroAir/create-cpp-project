# MSYS2 Integration Guide

This guide explains how to use the MSYS2 package manager integration in the C++ project creation tool.

## What is MSYS2?

MSYS2 is a software distribution and building platform for Windows that provides a Unix-like environment. It includes:

- **pacman package manager** - For installing and managing packages
- **MinGW-w64 toolchain** - For native Windows development
- **POSIX compatibility layer** - For running Unix-like tools on Windows
- **Extensive package repository** - Thousands of pre-built packages

## Prerequisites

### Installing MSYS2

1. Download MSYS2 from [https://www.msys2.org/](https://www.msys2.org/)
2. Run the installer and follow the installation instructions
3. Update the package database:
   ```bash
   pacman -Syu
   ```
4. Install development tools:
   ```bash
   pacman -S mingw-w64-x86_64-toolchain
   pacman -S mingw-w64-x86_64-cmake
   pacman -S mingw-w64-x86_64-ninja
   ```

### Environment Setup

Always use the **MinGW64** or **MinGW32** shell (not the MSYS2 shell) for C++ development:

- **MinGW64 shell** - For 64-bit Windows applications
- **MinGW32 shell** - For 32-bit Windows applications

## Using MSYS2 with the Project Creator

### Command Line Usage

Create a new project with MSYS2 package manager:

```bash
./create-cpp-project MyProject --package msys2 --type console
```

Available options:
- `--package msys2` - Use MSYS2 package manager
- `--type` - Project type (console, library, gui, etc.)
- `--test-framework` - Testing framework (gtest, catch2)
- `--build-system` - Build system (cmake, meson)

### Interactive Wizard

Run the interactive wizard and select MSYS2 when prompted for package manager:

```bash
./create-cpp-project --wizard
```

The wizard will guide you through:
1. Project name and type selection
2. Package manager selection (choose MSYS2)
3. Testing framework options
4. Build system configuration

## Generated Files

When you select MSYS2 as the package manager, the tool generates:

### PKGBUILD File

The `PKGBUILD` file contains package metadata and build instructions:

```bash
# Maintainer: Your Name <your.email@example.com>
_realname=myproject
pkgbase=mingw-w64-${_realname}
pkgname="${MINGW_PACKAGE_PREFIX}-${_realname}"
pkgver=1.0.0
pkgrel=1
pkgdesc="A C++ console application (mingw-w64)"
arch=('any')
mingw_arch=('mingw32' 'mingw64' 'ucrt64' 'clang64' 'clangarm64')
url="https://github.com/yourname/myproject"
license=('MIT')
makedepends=(
  "${MINGW_PACKAGE_PREFIX}-cc"
  "${MINGW_PACKAGE_PREFIX}-cmake"
  "${MINGW_PACKAGE_PREFIX}-ninja"
)
depends=(
  "${MINGW_PACKAGE_PREFIX}-gcc-libs"
)
source=("${_realname}-${pkgver}.tar.gz")
sha256sums=('SKIP')

build() {
  cd "${srcdir}/${_realname}-${pkgver}"
  
  mkdir -p build && cd build
  
  MSYS2_ARG_CONV_EXCL="-DCMAKE_INSTALL_PREFIX=" \
  ${MINGW_PREFIX}/bin/cmake.exe \
    -GNinja \
    -DCMAKE_INSTALL_PREFIX=${MINGW_PREFIX} \
    -DCMAKE_BUILD_TYPE=Release \
    ..
  
  ${MINGW_PREFIX}/bin/cmake.exe --build .
}

check() {
  cd "${srcdir}/${_realname}-${pkgver}/build"
  
  # Run tests if available
  if [ -f "test_myproject" ]; then
    ./test_myproject
  fi
}

package() {
  cd "${srcdir}/${_realname}-${pkgver}/build"
  
  DESTDIR="${pkgdir}" ${MINGW_PREFIX}/bin/cmake.exe --install .
  
  # Install license
  install -Dm644 "${srcdir}/${_realname}-${pkgver}/LICENSE" \
    "${pkgdir}${MINGW_PREFIX}/share/licenses/${_realname}/LICENSE"
}
```

### CMake Configuration

The generated CMakeLists.txt includes MSYS2-specific configurations:

- Proper compiler detection for MinGW
- MSYS2-compatible installation paths
- Windows-specific resource handling

## Building Your Project

### Development Build

For development, build normally with CMake:

```bash
mkdir build && cd build
cmake .. -G Ninja
ninja
```

### Creating MSYS2 Package

To create an MSYS2 package for distribution:

1. Create a source archive:
   ```bash
   tar -czf myproject-1.0.0.tar.gz --exclude=build --exclude=.git .
   ```

2. Build the package:
   ```bash
   makepkg -s
   ```

3. Install the package:
   ```bash
   pacman -U mingw-w64-x86_64-myproject-1.0.0-1-any.pkg.tar.zst
   ```

## Package Dependencies

### Common MSYS2 Packages for C++ Development

- `${MINGW_PACKAGE_PREFIX}-gcc` - GCC compiler
- `${MINGW_PACKAGE_PREFIX}-cmake` - CMake build system
- `${MINGW_PACKAGE_PREFIX}-ninja` - Ninja build tool
- `${MINGW_PACKAGE_PREFIX}-gdb` - GNU Debugger
- `${MINGW_PACKAGE_PREFIX}-pkg-config` - Package configuration tool

### GUI Framework Dependencies

For GUI applications, common dependencies include:

- **Qt**: `${MINGW_PACKAGE_PREFIX}-qt6-base`, `${MINGW_PACKAGE_PREFIX}-qt6-tools`
- **GTK**: `${MINGW_PACKAGE_PREFIX}-gtk3`, `${MINGW_PACKAGE_PREFIX}-gtk4`
- **wxWidgets**: `${MINGW_PACKAGE_PREFIX}-wxwidgets`

### Testing Framework Dependencies

- **Google Test**: `${MINGW_PACKAGE_PREFIX}-gtest`
- **Catch2**: `${MINGW_PACKAGE_PREFIX}-catch2`

## Troubleshooting

### Common Issues

1. **Path Conversion Problems**
   - Use `MSYS2_ARG_CONV_EXCL` to prevent automatic path conversion
   - Always use forward slashes in CMake paths

2. **Missing Dependencies**
   - Check if packages are installed: `pacman -Q package-name`
   - Install missing packages: `pacman -S package-name`

3. **Build Failures**
   - Ensure you're using the correct MinGW shell (not MSYS2 shell)
   - Check that `MINGW_PREFIX` environment variable is set

4. **Package Creation Issues**
   - Validate PKGBUILD syntax with the built-in validator
   - Ensure all source files are included in the archive

### Validation Tools

The project creator includes MSYS2 validation utilities:

```cpp
#include "utils/msys2_validator.h"

// Validate MSYS2 environment
auto envResult = MSYS2Validator::validateMSYS2Environment();
if (!envResult.isValid) {
    // Handle environment issues
}

// Validate PKGBUILD file
auto pkgResult = MSYS2Validator::validatePKGBUILD("PKGBUILD");
if (!pkgResult.isValid) {
    // Handle PKGBUILD issues
}
```

## Best Practices

1. **Use MinGW Shell**: Always develop in MinGW64/MinGW32 shell, not MSYS2 shell
2. **Package Naming**: Follow MSYS2 naming conventions (`mingw-w64-*`)
3. **Dependencies**: Specify all runtime and build dependencies
4. **Testing**: Include comprehensive tests in your PKGBUILD
5. **Documentation**: Provide clear installation and usage instructions
6. **Licensing**: Include proper license information in your package

## Resources

- [MSYS2 Official Website](https://www.msys2.org/)
- [MSYS2 Package Guidelines](https://www.msys2.org/wiki/Creating-Packages/)
- [MinGW-w64 Documentation](https://www.mingw-w64.org/)
- [PKGBUILD Reference](https://wiki.archlinux.org/title/PKGBUILD)

## Support

For issues specific to MSYS2 integration in this tool, please check:

1. Validate your MSYS2 environment setup
2. Check the generated PKGBUILD for syntax errors
3. Ensure all dependencies are properly specified
4. Review the build logs for specific error messages

For general MSYS2 support, consult the MSYS2 community resources and documentation.

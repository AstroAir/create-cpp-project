# MSYS2 Quick Reference Guide

## Essential Commands

### Package Management (pacman)

```bash
# Update package database
pacman -Sy

# Update all packages
pacman -Syu

# Install a package
pacman -S package-name

# Search for packages
pacman -Ss search-term

# List installed packages
pacman -Q

# Get package information
pacman -Si package-name

# Remove a package
pacman -R package-name

# Remove package and dependencies
pacman -Rs package-name

# Clean package cache
pacman -Sc
```

### Common Development Packages

```bash
# Essential build tools
pacman -S mingw-w64-x86_64-toolchain
pacman -S mingw-w64-x86_64-cmake
pacman -S mingw-w64-x86_64-ninja
pacman -S mingw-w64-x86_64-make
pacman -S mingw-w64-x86_64-gdb

# Version control
pacman -S git

# Text editors
pacman -S mingw-w64-x86_64-vim
pacman -S mingw-w64-x86_64-emacs

# Libraries
pacman -S mingw-w64-x86_64-boost
pacman -S mingw-w64-x86_64-fmt
pacman -S mingw-w64-x86_64-spdlog
pacman -S mingw-w64-x86_64-nlohmann-json

# Testing frameworks
pacman -S mingw-w64-x86_64-gtest
pacman -S mingw-w64-x86_64-catch2

# GUI frameworks
pacman -S mingw-w64-x86_64-qt6-base
pacman -S mingw-w64-x86_64-gtk3
pacman -S mingw-w64-x86_64-wxwidgets
```

## Project Creation Commands

### Using the C++ Project Creator

```bash
# Console application with MSYS2
./create-cpp-project MyApp --package msys2 --type console

# Library with MSYS2 and Google Test
./create-cpp-project MyLib --package msys2 --type library --test-framework gtest

# GUI application with MSYS2 and Qt
./create-cpp-project MyGUI --package msys2 --type gui --gui-framework qt

# Interactive wizard
./create-cpp-project --wizard
```

## Build Commands

### CMake with MSYS2

```bash
# Configure project
cmake .. -G Ninja -DCMAKE_BUILD_TYPE=Release

# Build project
ninja

# Install project
ninja install

# Run tests
ninja test
# or
ctest
```

### Makepkg (Package Creation)

```bash
# Build package from PKGBUILD
makepkg

# Build with dependency installation
makepkg -s

# Build and install
makepkg -si

# Force rebuild
makepkg -f

# Clean build directory
makepkg -c

# Generate source package only
makepkg --source
```

## Environment Variables

### Important MSYS2 Variables

```bash
# Check current environment
echo $MSYSTEM          # Current MSYS2 environment (MINGW64, MINGW32, etc.)
echo $MINGW_PREFIX     # Installation prefix (/mingw64, /mingw32, etc.)
echo $PATH             # System PATH

# Set environment for cross-compilation
export MSYSTEM=MINGW64
export MINGW_PREFIX=/mingw64
```

### CMake-specific Variables

```bash
# Prevent path conversion issues
export MSYS2_ARG_CONV_EXCL="-DCMAKE_INSTALL_PREFIX="

# Set CMake generator
export CMAKE_GENERATOR=Ninja
```

## File Paths and Conventions

### MSYS2 Path Structure

```
/mingw64/          # 64-bit MinGW installation
├── bin/           # Executables
├── include/       # Header files
├── lib/           # Libraries
└── share/         # Shared data

/mingw32/          # 32-bit MinGW installation
/ucrt64/           # UCRT 64-bit installation
/clang64/          # Clang 64-bit installation
```

### Package Naming Conventions

```bash
# MSYS2 package names
mingw-w64-x86_64-package-name    # 64-bit package
mingw-w64-i686-package-name      # 32-bit package
mingw-w64-ucrt-x86_64-package    # UCRT 64-bit package
mingw-w64-clang-x86_64-package   # Clang 64-bit package
```

## PKGBUILD Template

### Basic Structure

```bash
# Maintainer: Your Name <email@example.com>
_realname=myproject
pkgbase=mingw-w64-${_realname}
pkgname="${MINGW_PACKAGE_PREFIX}-${_realname}"
pkgver=1.0.0
pkgrel=1
pkgdesc="Description of your project"
arch=('any')
mingw_arch=('mingw32' 'mingw64' 'ucrt64' 'clang64')
url="https://github.com/user/project"
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
  ${MINGW_PREFIX}/bin/cmake.exe --build . --target test
}

package() {
  cd "${srcdir}/${_realname}-${pkgver}/build"
  DESTDIR="${pkgdir}" ${MINGW_PREFIX}/bin/cmake.exe --install .
}
```

## Troubleshooting

### Common Issues and Solutions

| Issue                      | Solution                               |
| -------------------------- | -------------------------------------- |
| `cmake: command not found` | `pacman -S mingw-w64-x86_64-cmake`     |
| `ninja: command not found` | `pacman -S mingw-w64-x86_64-ninja`     |
| `gcc: command not found`   | `pacman -S mingw-w64-x86_64-toolchain` |
| Path conversion errors     | Use `MSYS2_ARG_CONV_EXCL`              |
| Wrong shell environment    | Use MinGW64/MinGW32 shell, not MSYS2   |
| Package not found          | Update database: `pacman -Sy`          |
| Permission denied          | Don't run as administrator             |
| DLL not found              | Install runtime dependencies           |

### Validation Commands

```bash
# Check MSYS2 environment
echo $MSYSTEM
echo $MINGW_PREFIX

# Verify tools are available
which cmake
which ninja
which gcc

# Test package installation
pacman -Q mingw-w64-x86_64-cmake

# Validate PKGBUILD syntax
makepkg --printsrcinfo

# Check package dependencies
pactree package-name
```

## Useful Aliases

Add to your `~/.bashrc`:

```bash
# MSYS2 aliases
alias ll='ls -la'
alias la='ls -A'
alias l='ls -CF'

# Package management
alias pacs='pacman -S'
alias pacss='pacman -Ss'
alias pacu='pacman -Syu'
alias pacr='pacman -R'
alias pacq='pacman -Q'

# Build shortcuts
alias cmakebuild='cmake .. -G Ninja && ninja'
alias maketest='ninja test'
alias makeclean='ninja clean'

# MSYS2 specific
alias mingw64='export MSYSTEM=MINGW64 && cd /mingw64'
alias mingw32='export MSYSTEM=MINGW32 && cd /mingw32'
```

## Resources

- [MSYS2 Official Documentation](https://www.msys2.org/)
- [Pacman Command Reference](https://wiki.archlinux.org/title/Pacman)
- [PKGBUILD Guidelines](https://wiki.archlinux.org/title/PKGBUILD)
- [CMake Documentation](https://cmake.org/documentation/)
- [MinGW-w64 Project](https://www.mingw-w64.org/)

# MSYS2 Console Application Example

This example demonstrates how to create a C++ console application with MSYS2 package manager integration.

## Project Structure

```
msys2_console_example/
├── src/
│   └── main.cpp
├── tests/
│   └── test_main.cpp
├── CMakeLists.txt
├── PKGBUILD
├── LICENSE
└── README.md
```

## Generated with Command

This project was generated using:

```bash
./create-cpp-project msys2_console_example --package msys2 --type console --test-framework gtest
```

## Building the Project

### Development Build

1. Open MSYS2 MinGW64 shell
2. Navigate to the project directory
3. Build with CMake:

```bash
mkdir build && cd build
cmake .. -G Ninja
ninja
```

4. Run the application:

```bash
./msys2_console_example
```

### Running Tests

```bash
# In the build directory
./test_msys2_console_example
```

## Creating MSYS2 Package

### Prerequisites

Ensure you have the required build tools:

```bash
pacman -S mingw-w64-x86_64-toolchain
pacman -S mingw-w64-x86_64-cmake
pacman -S mingw-w64-x86_64-ninja
pacman -S mingw-w64-x86_64-gtest
```

### Package Creation Steps

1. Create source archive:

```bash
# From the project root directory
tar -czf msys2_console_example-1.0.0.tar.gz \
    --exclude=build \
    --exclude=.git \
    --exclude='*.tar.gz' \
    .
```

2. Build the package:

```bash
makepkg -s
```

This will:

- Download and verify dependencies
- Extract the source archive
- Run the build() function
- Execute tests via check() function
- Create the package via package() function
- Generate `mingw-w64-x86_64-msys2_console_example-1.0.0-1-any.pkg.tar.zst`

3. Install the package:

```bash
pacman -U mingw-w64-x86_64-msys2_console_example-1.0.0-1-any.pkg.tar.zst
```

4. Test the installed package:

```bash
msys2_console_example
```

## PKGBUILD Explanation

The generated `PKGBUILD` file contains:

### Package Metadata

- `_realname`: Base package name
- `pkgname`: Full MSYS2 package name with prefix
- `pkgver`/`pkgrel`: Version information
- `pkgdesc`: Package description
- `arch`: Supported architectures
- `mingw_arch`: Supported MinGW architectures

### Dependencies

- `makedepends`: Build-time dependencies (compiler, cmake, ninja)
- `depends`: Runtime dependencies (gcc-libs, gtest for testing)

### Build Functions

- `build()`: Compiles the project using CMake and Ninja
- `check()`: Runs tests if available
- `package()`: Installs files to the package directory

## Key Features Demonstrated

1. **MSYS2 Environment Integration**: Proper use of `MINGW_PREFIX` and MSYS2-specific paths
2. **CMake Configuration**: MSYS2-compatible build system setup
3. **Testing Integration**: Google Test framework integration
4. **Package Creation**: Complete PKGBUILD for distribution
5. **Cross-Architecture Support**: Supports all MSYS2 architectures

## Customization

### Adding Dependencies

To add new dependencies, update the `depends` array in PKGBUILD:

```bash
depends=(
  "${MINGW_PACKAGE_PREFIX}-gcc-libs"
  "${MINGW_PACKAGE_PREFIX}-boost"
  "${MINGW_PACKAGE_PREFIX}-fmt"
)
```

### Modifying Build Options

Customize the CMake configuration in the `build()` function:

```bash
MSYS2_ARG_CONV_EXCL="-DCMAKE_INSTALL_PREFIX=" \
${MINGW_PREFIX}/bin/cmake.exe \
  -GNinja \
  -DCMAKE_INSTALL_PREFIX=${MINGW_PREFIX} \
  -DCMAKE_BUILD_TYPE=Release \
  -DENABLE_TESTING=ON \
  -DUSE_STATIC_LIBS=OFF \
  ..
```

### Adding Custom Tests

Extend the `check()` function for additional testing:

```bash
check() {
  cd "${srcdir}/${_realname}-${pkgver}/build"
  
  # Run unit tests
  if [ -f "test_${_realname}" ]; then
    ./test_${_realname}
  fi
  
  # Run integration tests
  if [ -f "integration_tests" ]; then
    ./integration_tests
  fi
  
  # Run benchmarks
  if [ -f "benchmarks" ]; then
    ./benchmarks --quick
  fi
}
```

## Troubleshooting

### Common Build Issues

1. **CMake not found**: Install `mingw-w64-x86_64-cmake`
2. **Ninja not found**: Install `mingw-w64-x86_64-ninja`
3. **Compiler errors**: Install `mingw-w64-x86_64-toolchain`
4. **Test failures**: Install `mingw-w64-x86_64-gtest`

### Package Creation Issues

1. **Missing source archive**: Ensure `msys2_console_example-1.0.0.tar.gz` exists
2. **Permission errors**: Run makepkg as regular user (not root)
3. **Dependency conflicts**: Update package database with `pacman -Sy`

### Runtime Issues

1. **DLL not found**: Ensure all dependencies are installed
2. **Path issues**: Use MinGW shell, not MSYS2 shell
3. **Permission denied**: Check file permissions and antivirus settings

## Next Steps

1. **Extend functionality**: Add more features to the console application
2. **Add libraries**: Integrate additional C++ libraries via MSYS2 packages
3. **Create GUI version**: Use the GUI template with Qt or GTK
4. **Distribute package**: Submit to MSYS2 package repository (if appropriate)

## Resources

- [MSYS2 Package Guidelines](https://www.msys2.org/wiki/Creating-Packages/)
- [CMake MSYS2 Integration](https://cmake.org/cmake/help/latest/manual/cmake-toolchains.7.html)
- [Google Test Documentation](https://google.github.io/googletest/)
- [PKGBUILD Reference](https://wiki.archlinux.org/title/PKGBUILD)

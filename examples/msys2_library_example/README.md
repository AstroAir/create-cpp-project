# MSYS2 Library Example

This example demonstrates how to create a C++ library project with MSYS2 packaging support.

## Project Structure

```
msys2_library_example/
├── PKGBUILD                 # MSYS2 package build script
├── CMakeLists.txt          # CMake build configuration
├── README.md               # This file
├── LICENSE                 # MIT License
├── include/
│   └── mathlib/
│       └── calculator.h    # Public header
├── src/
│   └── calculator.cpp      # Implementation
├── tests/
│   ├── CMakeLists.txt     # Test configuration
│   └── test_calculator.cpp # Unit tests
└── scripts/
    ├── build.sh           # Build script
    └── package.sh         # Package creation script
```

## Building the Library

### Prerequisites

Ensure you have MSYS2 installed with the following packages:

```bash
# Install build tools
pacman -S mingw-w64-x86_64-toolchain
pacman -S mingw-w64-x86_64-cmake
pacman -S mingw-w64-x86_64-ninja

# Install test framework
pacman -S mingw-w64-x86_64-gtest
```

### Development Build

```bash
# Create build directory
mkdir build && cd build

# Configure with CMake
cmake .. -G Ninja -DCMAKE_BUILD_TYPE=Debug

# Build the library
ninja

# Run tests
ninja test
```

### Release Build

```bash
# Create release build
mkdir build-release && cd build-release

# Configure for release
cmake .. -G Ninja -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=ON

# Build
ninja

# Install locally (optional)
ninja install
```

## Creating MSYS2 Package

### Method 1: Using makepkg

```bash
# Create source archive
tar -czf mathlib-1.0.0.tar.gz --exclude=build* --exclude=.git .

# Build package
makepkg -s

# Install package
pacman -U mingw-w64-x86_64-mathlib-1.0.0-1-any.pkg.tar.zst
```

### Method 2: Using provided scripts

```bash
# Build the project
./scripts/build.sh

# Create package
./scripts/package.sh
```

## Using the Library

### In CMake Projects

```cmake
find_package(mathlib REQUIRED)
target_link_libraries(your_target mathlib::mathlib)
```

### In MSYS2 Projects

```bash
# Install the library
pacman -S mingw-w64-x86_64-mathlib

# Use in your project
g++ -o myapp main.cpp -lmathlib
```

## Library API

### Calculator Class

```cpp
#include <mathlib/calculator.h>

mathlib::Calculator calc;

// Basic operations
double result = calc.add(5.0, 3.0);        // 8.0
result = calc.subtract(10.0, 4.0);         // 6.0
result = calc.multiply(3.0, 7.0);          // 21.0
result = calc.divide(15.0, 3.0);           // 5.0

// Advanced operations
result = calc.power(2.0, 3.0);             // 8.0
result = calc.sqrt(16.0);                  // 4.0
result = calc.factorial(5);                // 120.0
```

## PKGBUILD Features

This example demonstrates:

- **Shared Library Building**: Uses `-DBUILD_SHARED_LIBS=ON`
- **Development Package**: Includes headers and CMake config files
- **Test Integration**: Runs tests during package build
- **Proper Dependencies**: Specifies runtime and build dependencies
- **License Installation**: Installs license file correctly
- **CMake Integration**: Provides find_package() support

## Testing

The library includes comprehensive unit tests using Google Test:

```bash
# Run all tests
cd build && ninja test

# Run tests with verbose output
cd build && ctest --verbose

# Run specific test
cd build && ./tests/test_calculator
```

## Customization

### Adding New Functions

1. Add declaration to `include/mathlib/calculator.h`
2. Add implementation to `src/calculator.cpp`
3. Add tests to `tests/test_calculator.cpp`
4. Update version in `PKGBUILD` and `CMakeLists.txt`

### Changing Build Options

Edit `CMakeLists.txt` to modify:

- C++ standard version
- Compiler flags
- Installation paths
- Package configuration

### MSYS2 Package Options

Edit `PKGBUILD` to modify:

- Package metadata
- Dependencies
- Build configuration
- Installation rules

## Troubleshooting

### Common Issues

1. **Missing Dependencies**

   ```bash
   # Install missing packages
   pacman -S mingw-w64-x86_64-pkg-config
   ```

2. **Build Failures**

   ```bash
   # Clean build directory
   rm -rf build && mkdir build
   ```

3. **Test Failures**

   ```bash
   # Check test output
   cd build && ctest --output-on-failure
   ```

### Getting Help

- Check MSYS2 documentation: <https://www.msys2.org/docs/>
- Review CMake documentation: <https://cmake.org/documentation/>
- Consult Google Test guide: <https://google.github.io/googletest/>

## License

This example is provided under the MIT License. See LICENSE file for details.

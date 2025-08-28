# C++20 Modules Examples

This directory contains example projects demonstrating various aspects of C++20 modules usage with cpp-scaffold.

## Examples

### 1. Basic Module (`basic-module/`)

A simple example showing:

- Basic module interface and implementation
- Module imports
- CMake configuration for modules

**Create with:**

```bash
cpp-scaffold basic-module --template modules --build cmake --std cpp20
```

### 2. Math Library Module (`math-library/`)

A mathematical library demonstrating:

- Multiple module interfaces
- Module partitions
- Exporting classes and functions
- Unit testing with modules

**Create with:**

```bash
cpp-scaffold math-library --template modules --build cmake --std cpp20 --tests --test-framework gtest
```

### 3. Header Units Example (`header-units/`)

Shows how to use header units alongside modules:

- Traditional headers as header units
- Mixed module and header usage
- Gradual migration strategy

**Create with:**

```bash
cpp-scaffold header-units --template modules --build cmake --std cpp20 --package vcpkg
```

### 4. Module Partitions (`partitions/`)

Advanced example with module partitions:

- Interface partitions
- Implementation partitions
- Internal module organization

**Create with:**

```bash
cpp-scaffold partitions --template modules --build cmake --std cpp23
```

## Building Examples

Each example includes:

- Complete CMake configuration
- Build instructions
- Usage examples
- Documentation

## Requirements

- C++20 compatible compiler:
  - GCC 11+ (partial support)
  - Clang 15+ (good support)
  - MSVC 19.28+ (Visual Studio 2019 16.8+)
- CMake 3.28+
- cpp-scaffold tool

## Notes

- C++20 modules are still evolving, some features may not work on all compilers
- Use C++23 for the most complete modules experience
- Check compiler documentation for specific modules support details

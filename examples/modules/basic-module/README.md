# Basic C++20 Module Example

This example demonstrates the most basic usage of C++20 modules with cpp-scaffold.

## What's Included

- **Module Interface** (`modules/basic_module.cppm`): Exports a simple class and function
- **Module Implementation** (`src/basic_module.cpp`): Implements the module
- **Main Application** (`src/main.cpp`): Uses the module
- **CMake Configuration**: Proper modules support setup
- **Tests** (optional): Basic unit tests for the module

## Generated Structure

```
basic-module/
├── CMakeLists.txt          # CMake with modules support
├── modules/
│   └── basic_module.cppm   # Module interface
├── src/
│   ├── basic_module.cpp    # Module implementation  
│   └── main.cpp           # Main application
├── tests/                 # Unit tests (if --tests specified)
│   └── test_basic_module.cpp
└── README.md
```

## Key Features Demonstrated

1. **Module Declaration**: `export module basic_module;`
2. **Module Imports**: `import basic_module;` and `import std;`
3. **Exported Entities**: Classes, functions, and constants
4. **CMake Integration**: FILE_SET CXX_MODULES configuration
5. **Testing**: How to test modules with popular frameworks

## Building

```bash
# Create the project
cpp-scaffold basic-module --template modules --build cmake --std cpp20

# Build
cd basic-module
mkdir build && cd build
cmake .. -DCMAKE_CXX_STANDARD=20
cmake --build .

# Run
./basic-module
```

## With Testing

```bash
# Create with tests
cpp-scaffold basic-module --template modules --build cmake --std cpp20 --tests --test-framework gtest

# Build and test
cd basic-module
mkdir build && cd build
cmake .. -DCMAKE_CXX_STANDARD=20
cmake --build .
ctest
```

## Expected Output

```
Welcome to basic-module!
Example value: 42
3 + 4 = 7
Version: 1.0.0
```

## Compiler Requirements

- **GCC 11+**: Basic modules support
- **Clang 15+**: Good modules support
- **MSVC 19.28+**: Full modules support

## Notes

- This example uses the standard library modules (`import std;`)
- Module interface files use `.cppm` extension (configurable)
- CMake 3.28+ required for stable modules support

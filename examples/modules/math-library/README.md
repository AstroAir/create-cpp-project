# Math Library Module Example

This example demonstrates a more complex C++20 modules project with multiple interfaces and comprehensive testing.

## What's Included

- **Multiple Module Interfaces**: Separate modules for different mathematical operations
- **Module Partitions**: Internal organization using partitions
- **Comprehensive Testing**: Full test coverage with Google Test
- **Documentation**: API documentation and usage examples
- **Package Management**: Integration with vcpkg for dependencies

## Generated Structure

```
math-library/
├── CMakeLists.txt
├── modules/
│   ├── math_library.cppm      # Main module interface
│   ├── math_basic.cppm        # Basic operations partition
│   ├── math_advanced.cppm     # Advanced operations partition
│   └── math_constants.cppm    # Mathematical constants
├── src/
│   ├── math_library.cpp       # Main module implementation
│   ├── math_basic.cpp         # Basic operations implementation
│   ├── math_advanced.cpp      # Advanced operations implementation
│   └── main.cpp              # Demo application
├── tests/
│   ├── test_basic.cpp         # Tests for basic operations
│   ├── test_advanced.cpp      # Tests for advanced operations
│   └── test_constants.cpp     # Tests for constants
├── include/                   # Traditional headers (for comparison)
├── docs/                      # Documentation
└── examples/                  # Usage examples
```

## Key Features Demonstrated

1. **Module Partitions**: Organizing large modules into logical partitions
2. **Multiple Modules**: How different modules can work together
3. **Export/Import**: Advanced export patterns and selective imports
4. **Template Modules**: Exporting template classes and functions
5. **Constants and Types**: Exporting compile-time constants and type aliases
6. **Mixed Usage**: Using modules alongside traditional headers

## Mathematical Operations Included

### Basic Operations (`math_basic`)

- Addition, subtraction, multiplication, division
- Power and root operations
- Factorial and combinations

### Advanced Operations (`math_advanced`)

- Trigonometric functions
- Logarithmic functions
- Statistical operations
- Linear algebra basics

### Constants (`math_constants`)

- Mathematical constants (π, e, φ, etc.)
- Physical constants
- Conversion factors

## Building

```bash
# Create the project
cpp-scaffold math-library --template modules --build cmake --std cpp20 --tests --test-framework gtest --package vcpkg --docs

# Build
cd math-library
mkdir build && cd build
cmake .. -DCMAKE_CXX_STANDARD=20
cmake --build .

# Run tests
ctest

# Run demo
./math-library
```

## Usage Example

```cpp
import math_library;
import std;

int main() {
    using namespace math_library;
    
    // Basic operations
    auto result = basic::add(3.14, 2.86);
    auto power = basic::pow(2, 8);
    
    // Advanced operations
    auto sine = advanced::sin(constants::PI / 2);
    auto log = advanced::log(constants::E);
    
    // Statistical operations
    std::vector<double> data = {1, 2, 3, 4, 5};
    auto mean = advanced::mean(data);
    auto stddev = advanced::standard_deviation(data);
    
    return 0;
}
```

## Testing

The example includes comprehensive tests:

```bash
# Run all tests
ctest

# Run specific test suites
ctest -R basic
ctest -R advanced
ctest -R constants
```

## Performance Considerations

- Modules can improve compilation times for large projects
- Template instantiation is more efficient with modules
- Better optimization opportunities for the compiler

## Migration Strategy

This example shows how to gradually migrate from headers to modules:

1. Start with new code using modules
2. Convert stable headers to header units
3. Gradually refactor headers into proper modules
4. Maintain compatibility during transition

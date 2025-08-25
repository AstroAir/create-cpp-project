# Testing Framework Component

The Testing Framework component provides comprehensive support for multiple testing frameworks and automated test setup in generated C++ projects.

## Overview

This component handles:

- **Multiple Framework Support** - Google Test, Catch2, Doctest, Boost.Test
- **Test Project Structure** - Automated test directory and file generation
- **Build System Integration** - CMake, Meson, and other build system test configuration
- **Test Discovery** - Automatic test discovery and execution setup
- **CI/CD Integration** - Test execution in continuous integration pipelines

## Supported Testing Frameworks

### Google Test (gtest)
- **Description**: Google's C++ testing framework
- **Features**: Rich assertion macros, test fixtures, parameterized tests
- **Best for**: Most C++ projects, comprehensive testing needs
- **CMake Integration**: `find_package(GTest REQUIRED)`

### Catch2
- **Description**: Modern, header-only C++ testing framework
- **Features**: BDD-style tests, self-contained, easy setup
- **Best for**: Header-only libraries, rapid prototyping
- **CMake Integration**: `find_package(Catch2 REQUIRED)`

### Doctest
- **Description**: Lightweight, fast-compiling testing framework
- **Features**: Fast compilation, minimal overhead, header-only
- **Best for**: Large codebases where compile time matters
- **CMake Integration**: `find_package(doctest REQUIRED)`

### Boost.Test
- **Description**: Part of the Boost C++ libraries
- **Features**: Mature framework, extensive features, XML output
- **Best for**: Projects already using Boost libraries
- **CMake Integration**: `find_package(Boost REQUIRED COMPONENTS unit_test_framework)`

## Component Architecture

```cpp
namespace cpp_scaffold::testing {
    class TestFrameworkInterface {
    public:
        virtual ~TestFrameworkInterface() = default;
        virtual std::string name() const = 0;
        virtual bool is_available() const = 0;
        virtual bool setup_testing(const ProjectOptions& options) = 0;
        virtual bool generate_test_files(const ProjectOptions& options) = 0;
        virtual bool configure_build_system(const ProjectOptions& options) = 0;
    };
    
    class TestFrameworkFactory {
    public:
        static std::unique_ptr<TestFrameworkInterface> create(const std::string& framework);
        static std::vector<std::string> available_frameworks();
    };
    
    class TestGenerator {
    public:
        bool generate_test_structure(const ProjectOptions& options);
        bool generate_sample_tests(const ProjectOptions& options);
        bool setup_test_discovery(const ProjectOptions& options);
    };
}
```

## Generated Test Structure

### Basic Test Structure
```
tests/
├── CMakeLists.txt          # Test build configuration
├── test_main.cpp           # Test runner main function
├── unit/                   # Unit tests
│   ├── test_component1.cpp
│   └── test_component2.cpp
├── integration/            # Integration tests
│   └── test_integration.cpp
└── fixtures/               # Test fixtures and data
    └── test_data.json
```

### Google Test Example
```cpp
#include <gtest/gtest.h>
#include "MyProject/MyClass.h"

class MyClassTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Setup code
    }
    
    void TearDown() override {
        // Cleanup code
    }
    
    MyClass obj;
};

TEST_F(MyClassTest, BasicFunctionality) {
    EXPECT_EQ(obj.getValue(), 42);
    EXPECT_TRUE(obj.isValid());
}

TEST_F(MyClassTest, EdgeCases) {
    EXPECT_THROW(obj.setInvalidValue(-1), std::invalid_argument);
}
```

### Catch2 Example
```cpp
#include <catch2/catch_test_macros.hpp>
#include "MyProject/MyClass.h"

TEST_CASE("MyClass basic functionality", "[MyClass]") {
    MyClass obj;
    
    SECTION("getValue returns correct value") {
        REQUIRE(obj.getValue() == 42);
    }
    
    SECTION("isValid returns true for valid objects") {
        REQUIRE(obj.isValid());
    }
}

TEST_CASE("MyClass edge cases", "[MyClass][edge]") {
    MyClass obj;
    
    REQUIRE_THROWS_AS(obj.setInvalidValue(-1), std::invalid_argument);
}
```

## Build System Integration

### CMake Configuration
```cmake
# Enable testing
enable_testing()

# Find testing framework
find_package(GTest REQUIRED)

# Create test executable
add_executable(tests
    test_main.cpp
    unit/test_component1.cpp
    unit/test_component2.cpp
)

# Link libraries
target_link_libraries(tests
    PRIVATE
        MyProject::MyProject
        GTest::gtest
        GTest::gtest_main
)

# Add tests to CTest
include(GoogleTest)
gtest_discover_tests(tests)
```

### Meson Configuration
```meson
# Testing framework dependency
gtest_dep = dependency('gtest', main: true)

# Test executable
test_exe = executable('tests',
    'test_main.cpp',
    'unit/test_component1.cpp',
    'unit/test_component2.cpp',
    dependencies: [myproject_dep, gtest_dep]
)

# Register tests
test('unit_tests', test_exe)
```

## CI/CD Integration

### GitHub Actions
```yaml
- name: Run Tests
  run: |
    cd build
    ctest --output-on-failure --parallel 4
```

### GitLab CI
```yaml
test:
  stage: test
  script:
    - cd build
    - ctest --output-on-failure
  artifacts:
    reports:
      junit: build/test_results.xml
```

## Configuration Options

### Framework Selection
```bash
# Google Test
cpp-scaffold MyProject --test gtest

# Catch2
cpp-scaffold MyProject --test catch2

# Doctest
cpp-scaffold MyProject --test doctest

# Boost.Test
cpp-scaffold MyProject --test boost
```

### Advanced Options
```bash
# Enable code coverage
cpp-scaffold MyProject --test gtest --coverage

# Generate performance tests
cpp-scaffold MyProject --test catch2 --benchmarks

# Custom test directory
cpp-scaffold MyProject --test gtest --test-dir custom_tests
```

## Best Practices

### Test Organization
1. **Separate unit and integration tests**
2. **Use descriptive test names**
3. **Group related tests in test suites**
4. **Keep tests independent and isolated**

### Test Writing
1. **Follow AAA pattern** (Arrange, Act, Assert)
2. **Test one thing at a time**
3. **Use meaningful assertions**
4. **Include edge cases and error conditions**

### Performance
1. **Use test fixtures for expensive setup**
2. **Avoid file I/O in unit tests when possible**
3. **Consider test execution time**
4. **Use parameterized tests for similar test cases**

## Related Documentation

- [User Guide - Testing](../user-guide/index.md)
- [Developer Guide - Contributing](../developer-guide/contributing.md)
- [API Reference](../api-reference/api.md)

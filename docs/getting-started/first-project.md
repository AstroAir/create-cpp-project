# Your First Project

This tutorial walks you through creating your first C++ project with CPP-Scaffold, from initial setup to running tests.

## Overview

We'll create a simple calculator library with:

- A `Calculator` class with basic arithmetic operations
- Unit tests using Google Test
- CMake build system
- vcpkg for dependency management
- Proper project structure and documentation

## Step 1: Create the Project

Let's create a library project called "MathCalculator":

```bash
cpp-scaffold MathCalculator \
  --type lib \
  --build cmake \
  --package vcpkg \
  --test gtest \
  --editor vscode
```

This command creates:

- A **library project** (`--type lib`)
- Using **CMake** as the build system (`--build cmake`)
- With **vcpkg** for package management (`--package vcpkg`)
- **Google Test** for unit testing (`--test gtest`)
- **VSCode** configuration files (`--editor vscode`)

## Step 2: Explore the Project Structure

Navigate to the project directory and examine the structure:

```bash
cd MathCalculator
tree  # or ls -la on systems without tree
```

You should see:

```
MathCalculator/
├── CMakeLists.txt              # Main build configuration
├── README.md                   # Project documentation
├── LICENSE                     # MIT license
├── .gitignore                  # Git ignore rules
├── .clang-format              # Code formatting configuration
├── .clang-tidy                # Static analysis configuration
├── vcpkg.json                 # Package dependencies
├── .vscode/                   # VSCode configuration
│   ├── settings.json
│   ├── tasks.json
│   └── launch.json
├── include/
│   └── MathCalculator/
│       └── Calculator.h       # Public header
├── src/
│   └── Calculator.cpp         # Implementation
├── tests/
│   ├── CMakeLists.txt
│   └── test_calculator.cpp    # Unit tests
└── docs/
    └── README.md              # Additional documentation
```

## Step 3: Examine the Generated Code

Let's look at the generated header file:

```cpp title="include/MathCalculator/Calculator.h"
#pragma once

namespace MathCalculator {
    class Calculator {
    public:
        Calculator() = default;
        ~Calculator() = default;

        // Basic arithmetic operations
        double add(double a, double b) const;
        double subtract(double a, double b) const;
        double multiply(double a, double b) const;
        double divide(double a, double b) const;
    };
}
```

And the implementation:

```cpp title="src/Calculator.cpp"
#include "MathCalculator/Calculator.h"
#include <stdexcept>

namespace MathCalculator {
    double Calculator::add(double a, double b) const {
        return a + b;
    }

    double Calculator::subtract(double a, double b) const {
        return a - b;
    }

    double Calculator::multiply(double a, double b) const {
        return a * b;
    }

    double Calculator::divide(double a, double b) const {
        if (b == 0.0) {
            throw std::invalid_argument("Division by zero");
        }
        return a / b;
    }
}
```

## Step 4: Build the Project

Create a build directory and compile:

```bash
# Create build directory
mkdir build && cd build

# Configure with CMake
cmake .. -G Ninja

# Build the project
ninja

# Alternative: use make if Ninja is not available
# cmake .. && make
```

## Step 5: Run the Tests

Execute the unit tests to verify everything works:

```bash
# Run all tests
ctest

# Run tests with verbose output
ctest --verbose

# Run specific test executable
./tests/test_calculator
```

You should see output like:

```
Test project /path/to/MathCalculator/build
    Start 1: test_calculator
1/1 Test #1: test_calculator ..................   Passed    0.01 sec

100% tests passed, 0 tests failed out of 1
```

## Step 6: Examine the Test Code

Look at the generated test file:

```cpp title="tests/test_calculator.cpp"
#include <gtest/gtest.h>
#include "MathCalculator/Calculator.h"

using namespace MathCalculator;

class CalculatorTest : public ::testing::Test {
protected:
    Calculator calc;
};

TEST_F(CalculatorTest, Addition) {
    EXPECT_DOUBLE_EQ(calc.add(2.0, 3.0), 5.0);
    EXPECT_DOUBLE_EQ(calc.add(-1.0, 1.0), 0.0);
}

TEST_F(CalculatorTest, Subtraction) {
    EXPECT_DOUBLE_EQ(calc.subtract(5.0, 3.0), 2.0);
    EXPECT_DOUBLE_EQ(calc.subtract(1.0, 1.0), 0.0);
}

TEST_F(CalculatorTest, Multiplication) {
    EXPECT_DOUBLE_EQ(calc.multiply(3.0, 4.0), 12.0);
    EXPECT_DOUBLE_EQ(calc.multiply(-2.0, 3.0), -6.0);
}

TEST_F(CalculatorTest, Division) {
    EXPECT_DOUBLE_EQ(calc.divide(10.0, 2.0), 5.0);
    EXPECT_DOUBLE_EQ(calc.divide(7.0, 2.0), 3.5);
}

TEST_F(CalculatorTest, DivisionByZero) {
    EXPECT_THROW(calc.divide(5.0, 0.0), std::invalid_argument);
}
```

## Step 7: Install the Library (Optional)

To install the library system-wide:

```bash
# Install to default location
ninja install

# Or specify custom installation prefix
cmake .. -DCMAKE_INSTALL_PREFIX=/usr/local
ninja install
```

## Step 8: Use the Library in Another Project

Create a simple application that uses your library:

```cpp title="example_usage.cpp"
#include <iostream>
#include "MathCalculator/Calculator.h"

int main() {
    MathCalculator::Calculator calc;
    
    std::cout << "Calculator Demo\n";
    std::cout << "2 + 3 = " << calc.add(2, 3) << "\n";
    std::cout << "10 - 4 = " << calc.subtract(10, 4) << "\n";
    std::cout << "5 * 6 = " << calc.multiply(5, 6) << "\n";
    std::cout << "15 / 3 = " << calc.divide(15, 3) << "\n";
    
    return 0;
}
```

## What You've Learned

Congratulations! You've successfully:

✅ Created a C++ library project with CPP-Scaffold  
✅ Explored the generated project structure  
✅ Built the project using CMake and Ninja  
✅ Ran unit tests with Google Test  
✅ Understood the code organization  

## Next Steps

Now you're ready to:

1. **Explore Templates**: Try different project types (console, gui, network)
2. **Learn Advanced Features**: Check out the [User Guide](../user-guide/index.md)
3. **Follow Tutorials**: Work through specific scenarios in [Tutorials](../tutorials/index.md)
4. **Customize**: Learn about configuration and profiles

## Troubleshooting

If you encounter issues:

- Check the [Troubleshooting Guide](../user-guide/troubleshooting.md)
- Ensure all prerequisites are installed
- Verify your compiler supports C++17
- Make sure CMake version is 3.14 or later

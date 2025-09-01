# test-premake-lib

A C++ library created with CPP-Scaffold.

## Features

- Feature 1
- Feature 2
- Feature 3

## Installation

### Prerequisites

- C++ compiler with C++17 support
- premake build system
- vcpkg package manager

### Build and Install

```bash
premake5 gmake2
make config=release
make install
```

## Usage

```cpp
#include <test-premake-lib/test-premake-lib.h>

int main() {
    // Create an Example object
    test-premake-lib::Example example(42);

    // Use the example object
    int value = example.getValue();

    // Use a free function
    int sum = test-premake-lib::add(3, 4);

    // Get library version
    std::string version = test-premake-lib::getVersion();

    return 0;
}
```

## Running Tests

```bash
bazel test //...
```

## License

This project is licensed under the MIT License - see the LICENSE file for details.

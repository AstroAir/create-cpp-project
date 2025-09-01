# test-xmake-modules

A C++20/C++23 modules-based project created with CPP-Scaffold.

## Features

- Modern C++20/C++23 modules
- xmake build system
- vcpkg package manager
- Testing with gtest

## Prerequisites

- C++ compiler with C++20 modules support:
  - GCC 11+ (partial support)
  - Clang 15+ (good support)
  - MSVC 19.28+ (Visual Studio 2019 16.8+)
- xmake build system
- vcpkg package manager

## Building

```bash
xmake
```

## Usage

```cpp
import test-xmake-modules;
import std;

int main() {
    test-xmake-modules::Example example(42);
    int value = example.getValue();
    int sum = test-xmake-modules::add(3, 4);
    std::string version = test-xmake-modules::getVersion();
    return 0;
}
```


## Testing

Run tests with:

```bash
xmake test
```


## License

This project is licensed under the MIT License - see the LICENSE file for details.

# CPP-Scaffold Examples Collection

A comprehensive collection of examples demonstrating the capabilities of CPP-Scaffold for creating modern C++ projects with various configurations, frameworks, and tools.

## 📚 Overview

This examples collection showcases real-world usage patterns for CPP-Scaffold, providing complete, buildable projects that demonstrate best practices in modern C++ development. Each example includes full source code, documentation, tests, and deployment configurations.

## 🎯 Example Categories

### 1. Console Applications
- **[Console File Processor](console-file-processor/)** - Complete CLI tool with argument parsing, file I/O, logging, and comprehensive testing
- **[MSYS2 Console Example](msys2_console_example/)** - Windows-specific console app with MSYS2 package management

### 2. GUI Applications
- **[Qt Image Viewer](qt-image-viewer/)** - Modern Qt6 desktop application with image processing, settings persistence, and cross-platform deployment
- **[SFML Game Demo](sfml-game-demo/)** - Complete 2D game with Entity Component System, graphics, audio, and game state management

### 3. Network Services
- **[Boost Network Server](boost-network-server/)** - Enterprise-grade HTTP/WebSocket server with SSL, authentication, database integration, and microservices architecture

### 4. Library Projects
- **[Math Library](math-library/)** - Modern C++ library with Conan package management, comprehensive API documentation, and distribution packaging

### 5. Testing and Quality Assurance
- **[Testing Patterns](testing-patterns/)** - Comprehensive testing examples including unit tests, integration tests, mocking, TDD/BDD patterns, and performance testing

### 6. Build Systems and Package Management
- **[Build Systems Showcase](build-systems-showcase/)** - Examples for CMake, Meson, Bazel, XMake with various package managers (vcpkg, Conan, MSYS2)

### 7. Framework Integrations
- **[C++20 Modules](modules/)** - Modern C++20 modules examples with different module patterns and build system integration

## 🚀 Quick Start Guide

### Prerequisites

Before running any examples, ensure you have:

- **C++17/20** compatible compiler (GCC 7+, Clang 6+, MSVC 2017+)
- **CMake 3.14+** (or other build system as specified)
- **Git** for version control
- **Package manager** (vcpkg, Conan, or MSYS2 as needed)

### Running Examples

Each example includes detailed build instructions, but here's the general pattern:

```bash
# 1. Navigate to example directory
cd examples/console-file-processor

# 2. Install dependencies (example with vcpkg)
vcpkg install fmt spdlog nlohmann-json gtest

# 3. Configure build
mkdir build && cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=path/to/vcpkg.cmake

# 4. Build project
cmake --build . --config Release

# 5. Run tests
ctest --output-on-failure

# 6. Run application
./console-file-processor --help
```

## 📖 Learning Path

### Beginner Path
1. **[Console File Processor](console-file-processor/)** - Learn basic project structure and CLI development
2. **[Testing Patterns](testing-patterns/)** - Understand testing fundamentals
3. **[Build Systems Showcase](build-systems-showcase/)** - Master build system basics

### Intermediate Path
1. **[Qt Image Viewer](qt-image-viewer/)** - GUI development with Qt
2. **[Math Library](math-library/)** - Library development and packaging
3. **[C++20 Modules](modules/)** - Modern C++ features

### Advanced Path
1. **[SFML Game Demo](sfml-game-demo/)** - Complex application architecture
2. **[Boost Network Server](boost-network-server/)** - Enterprise server development
3. **[Testing Patterns](testing-patterns/)** - Advanced testing strategies

## 🛠️ Technologies Demonstrated

### Programming Languages & Standards
- **C++17** - Modern C++ features and best practices
- **C++20** - Latest standard with modules, concepts, and ranges

### Build Systems
- **CMake** - Modern CMake practices with targets and packages
- **Meson** - Fast and user-friendly build system
- **Bazel** - Scalable build system for large projects
- **XMake** - Lightweight Lua-based build system

### Package Managers
- **vcpkg** - Microsoft's C++ package manager
- **Conan** - Decentralized package manager
- **MSYS2** - Windows package management system

### Frameworks & Libraries
- **Qt6** - Cross-platform GUI framework
- **SFML** - Simple and Fast Multimedia Library
- **Boost** - Peer-reviewed portable C++ libraries
- **Google Test** - C++ testing framework
- **Catch2** - Modern C++ testing framework

### Development Tools
- **Git** - Version control integration
- **GitHub Actions** - CI/CD automation
- **Docker** - Containerization
- **VSCode** - Editor configuration
- **Doxygen** - Documentation generation

## 📊 Example Comparison Matrix

| Example | Type | Build System | Package Manager | Framework | Testing | CI/CD |
|---------|------|--------------|-----------------|-----------|---------|-------|
| Console File Processor | CLI | CMake | vcpkg | - | GTest | ✅ |
| Qt Image Viewer | GUI | CMake | vcpkg | Qt6 | GTest | ✅ |
| SFML Game Demo | Game | CMake | vcpkg | SFML | GTest | ✅ |
| Boost Network Server | Network | CMake | vcpkg | Boost | GTest | ✅ |
| Math Library | Library | CMake | Conan | - | Catch2 | ✅ |
| Testing Patterns | Library | CMake | vcpkg | - | Multiple | ✅ |
| Build Systems Showcase | Multiple | Multiple | Multiple | - | Multiple | ✅ |

## 🎯 Key Features Demonstrated

### Modern C++ Practices
- **RAII** and smart pointers
- **Move semantics** and perfect forwarding
- **Template metaprogramming** and concepts
- **Standard library** algorithms and containers
- **Exception safety** and error handling

### Software Architecture
- **SOLID principles** implementation
- **Design patterns** (Factory, Observer, Strategy)
- **Entity Component System** for games
- **Microservices** architecture for servers
- **Plugin architecture** for extensibility

### Development Workflows
- **Test-Driven Development** (TDD)
- **Behavior-Driven Development** (BDD)
- **Continuous Integration** with GitHub Actions
- **Code coverage** analysis
- **Static analysis** with clang-tidy

### Cross-Platform Development
- **Windows** (MSVC, MinGW)
- **Linux** (GCC, Clang)
- **macOS** (Clang, Apple Clang)
- **Cross-compilation** strategies

## 📚 Documentation Structure

Each example includes comprehensive documentation:

- **README.md** - Overview, features, and quick start
- **Building Guide** - Detailed build instructions
- **Usage Examples** - Command-line and API usage
- **Architecture Documentation** - Design decisions and patterns
- **Testing Guide** - Test execution and coverage
- **Deployment Guide** - Distribution and packaging
- **API Reference** - Generated documentation (where applicable)

## 🔄 Continuous Integration

All examples include CI/CD configurations:

- **Multi-platform builds** (Windows, Linux, macOS)
- **Multiple compilers** (GCC, Clang, MSVC)
- **Dependency management** automation
- **Testing** execution and reporting
- **Code quality** checks (formatting, static analysis)
- **Documentation** generation and deployment
- **Release automation** with semantic versioning

## 🤝 Contributing

These examples are designed to be:

- **Educational** - Clear, well-documented code
- **Practical** - Real-world applicable patterns
- **Maintainable** - Modern best practices
- **Extensible** - Easy to modify and expand

### Adding New Examples

When adding new examples, ensure they:

1. **Follow the established structure** and documentation patterns
2. **Include comprehensive tests** with good coverage
3. **Demonstrate specific features** or use cases
4. **Provide clear build instructions** for all platforms
5. **Include CI/CD configuration** for automated testing

## 📈 Performance Considerations

Examples demonstrate various performance optimization techniques:

- **Compiler optimizations** and build configurations
- **Memory management** and resource optimization
- **Parallel processing** with threads and async operations
- **Caching strategies** for frequently accessed data
- **Profiling integration** for performance analysis

## 🔒 Security Best Practices

Security considerations demonstrated across examples:

- **Input validation** and sanitization
- **Secure coding practices** to prevent vulnerabilities
- **Authentication and authorization** patterns
- **Encryption** for data protection
- **Dependency security** scanning and updates

## 🌟 Next Steps

After exploring these examples, consider:

1. **Customizing examples** for your specific needs
2. **Combining patterns** from different examples
3. **Contributing improvements** back to the community
4. **Creating new examples** for additional use cases
5. **Sharing your experiences** with the CPP-Scaffold community

## 📞 Support and Resources

- **Documentation**: [CPP-Scaffold Docs](../../docs/)
- **Issues**: [GitHub Issues](https://github.com/cpp-scaffold/cpp-scaffold/issues)
- **Discussions**: [GitHub Discussions](https://github.com/cpp-scaffold/cpp-scaffold/discussions)
- **Examples**: This directory and subdirectories

---

**Happy coding with CPP-Scaffold!** 🚀

These examples represent modern C++ development practices and demonstrate the power of CPP-Scaffold for rapid, high-quality project creation.

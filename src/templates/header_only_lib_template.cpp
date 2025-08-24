#include "header_only_lib_template.h"
#include "../utils/file_utils.h"
#include "../utils/string_utils.h"
#include "../utils/terminal_utils.h"
#include <fmt/format.h>
#include <spdlog/spdlog.h>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <sstream>

using namespace utils;

HeaderOnlyLibTemplate::HeaderOnlyLibTemplate(const CliOptions &options)
    : TemplateBase(options) {}

bool HeaderOnlyLibTemplate::create() {
  spdlog::info("Creating header-only library project: {}", options_.projectName);

  std::string projectPath = options_.projectName;

  // Create project structure
  if (!createProjectStructure()) {
    spdlog::error("Failed to create project structure");
    return false;
  }

  // Create build system files
  if (!createBuildSystem()) {
    spdlog::error("Failed to create build system files");
    return false;
  }

  // Setup package manager
  if (!setupPackageManager()) {
    spdlog::error("Failed to setup package manager");
    return false;
  }

  // Setup test framework
  if (options_.includeTests && !setupTestFramework()) {
    spdlog::error("Failed to setup test framework");
    return false;
  }

  // Setup documentation
  if (options_.includeDocumentation && !setupDocumentation()) {
    spdlog::error("Failed to setup documentation");
    return false;
  }

  // Setup code formatting
  if (options_.includeCodeStyleTools && !setupCodeFormatting()) {
    spdlog::error("Failed to setup code formatting");
    return false;
  }

  // Setup CI/CD
  if (!options_.ciOptions.empty() && !setupContinuousIntegration()) {
    spdlog::error("Failed to setup CI/CD");
    return false;
  }

  // Create examples
  if (!createExamples()) {
    spdlog::error("Failed to create examples");
    return false;
  }

  // Create single header version
  if (!createSingleHeaderVersion()) {
    spdlog::error("Failed to create single header version");
    return false;
  }

  // Initialize Git
  if (options_.initGit) {
    if (!initializeGit(projectPath)) {
      spdlog::error("Failed to initialize Git repository");
      return false;
    }
    spdlog::info("✅ Git repository initialized");
  }

  spdlog::info("\nYour header-only library project is ready!\n");

  // Print usage instructions
  TerminalUtils::showSuccess("Project created successfully!");
  TerminalUtils::showInfo("Next steps:");
  std::cout << "  1. cd " << options_.projectName << "\n";
  std::cout << "  2. Review the generated headers in include/" << options_.projectName << "/\n";
  std::cout << "  3. Customize the library implementation\n";

  if (options_.includeTests) {
    std::cout << "  4. Build and run tests:\n";
    if (enums::to_string(options_.buildSystem) == "cmake") {
      std::cout << "     mkdir build && cd build\n";
      std::cout << "     cmake .. && make && ctest\n";
    }
  }

  std::cout << "  5. Use the single-header version from single_header/ directory\n";
  std::cout << "  6. Check examples/ directory for usage examples\n";

  return true;
}

bool HeaderOnlyLibTemplate::createProjectStructure() {
  std::string projectPath = options_.projectName;

  // Create main directories
  std::vector<std::string> directories = {
      projectPath,
      FileUtils::combinePath(projectPath, "include"),
      FileUtils::combinePath(FileUtils::combinePath(projectPath, "include"), options_.projectName),
      FileUtils::combinePath(FileUtils::combinePath(FileUtils::combinePath(projectPath, "include"), options_.projectName), "detail"),
      FileUtils::combinePath(projectPath, "examples"),
      FileUtils::combinePath(projectPath, "single_header"),
      FileUtils::combinePath(projectPath, "scripts"),
      FileUtils::combinePath(projectPath, "docs")
  };

  if (options_.includeTests) {
    directories.push_back(FileUtils::combinePath(projectPath, "tests"));
  }

  for (const auto &dir : directories) {
    if (!FileUtils::createDirectory(dir)) {
      spdlog::error("Failed to create directory: {}", dir);
      return false;
    }
  }

  // Create main header file
  std::string mainHeaderPath = FileUtils::combinePath(
      FileUtils::combinePath(FileUtils::combinePath(projectPath, "include"), options_.projectName),
      options_.projectName + ".h");
  if (!FileUtils::writeToFile(mainHeaderPath, getMainHeaderContent())) {
    spdlog::error("Failed to create main header file");
    return false;
  }

  // Create detail header
  std::string detailHeaderPath = FileUtils::combinePath(
      FileUtils::combinePath(FileUtils::combinePath(FileUtils::combinePath(projectPath, "include"), options_.projectName), "detail"),
      "impl.h");
  if (!FileUtils::writeToFile(detailHeaderPath, getDetailHeaderContent())) {
    spdlog::error("Failed to create detail header file");
    return false;
  }

  // Create version header
  std::string versionHeaderPath = FileUtils::combinePath(
      FileUtils::combinePath(FileUtils::combinePath(projectPath, "include"), options_.projectName),
      "version.h");
  if (!FileUtils::writeToFile(versionHeaderPath, getVersionHeaderContent())) {
    spdlog::error("Failed to create version header file");
    return false;
  }

  // Create config header
  std::string configHeaderPath = FileUtils::combinePath(
      FileUtils::combinePath(FileUtils::combinePath(projectPath, "include"), options_.projectName),
      "config.h");
  if (!FileUtils::writeToFile(configHeaderPath, getConfigHeaderContent())) {
    spdlog::error("Failed to create config header file");
    return false;
  }

  // Create README
  std::string readmePath = FileUtils::combinePath(projectPath, "README.md");
  if (!FileUtils::writeToFile(readmePath, getReadmeContent())) {
    spdlog::error("Failed to create README file");
    return false;
  }

  // Create example usage
  std::string examplePath = FileUtils::combinePath(
      FileUtils::combinePath(projectPath, "examples"), "basic_usage.cpp");
  if (!FileUtils::writeToFile(examplePath, getExampleUsageContent())) {
    spdlog::error("Failed to create example file");
    return false;
  }

  // Create single header generation script
  std::string scriptPath = FileUtils::combinePath(
      FileUtils::combinePath(projectPath, "scripts"), "generate_single_header.py");
  if (!FileUtils::writeToFile(scriptPath, getSingleHeaderScript())) {
    spdlog::error("Failed to create single header script");
    return false;
  }

  return true;
}

bool HeaderOnlyLibTemplate::createBuildSystem() {
  std::string projectPath = options_.projectName;
  std::string buildSystem = std::string(enums::to_string(options_.buildSystem));

  if (buildSystem == "cmake") {
    std::string cmakePath = FileUtils::combinePath(projectPath, "CMakeLists.txt");
    if (!FileUtils::writeToFile(cmakePath, getCMakeContent())) {
      spdlog::error("Failed to create CMakeLists.txt");
      return false;
    }
  } else if (buildSystem == "meson") {
    std::string mesonPath = FileUtils::combinePath(projectPath, "meson.build");
    if (!FileUtils::writeToFile(mesonPath, getMesonContent())) {
      spdlog::error("Failed to create meson.build");
      return false;
    }
  } else if (buildSystem == "bazel") {
    std::string bazelPath = FileUtils::combinePath(projectPath, "BUILD");
    if (!FileUtils::writeToFile(bazelPath, getBazelContent())) {
      spdlog::error("Failed to create BUILD file");
      return false;
    }

    std::string workspacePath = FileUtils::combinePath(projectPath, "WORKSPACE");
    if (!FileUtils::writeToFile(workspacePath, "workspace(name = \"" + options_.projectName + "\")\n")) {
      spdlog::error("Failed to create WORKSPACE file");
      return false;
    }
  }

  return true;
}

bool HeaderOnlyLibTemplate::setupPackageManager() {
  std::string projectPath = options_.projectName;
  std::string packageManager = std::string(enums::to_string(options_.packageManager));

  if (packageManager == "vcpkg") {
    std::string vcpkgPath = FileUtils::combinePath(projectPath, "vcpkg.json");
    if (!FileUtils::writeToFile(vcpkgPath, getVcpkgJsonContent())) {
      spdlog::error("Failed to create vcpkg.json");
      return false;
    }
  } else if (packageManager == "conan") {
    std::string conanPath = FileUtils::combinePath(projectPath, "conanfile.txt");
    if (!FileUtils::writeToFile(conanPath, getConanfileContent())) {
      spdlog::error("Failed to create conanfile.txt");
      return false;
    }
  }

  return true;
}

bool HeaderOnlyLibTemplate::setupTestFramework() {
  if (!options_.includeTests) {
    return true;
  }

  std::string projectPath = options_.projectName;
  std::string testFramework = std::string(enums::to_string(options_.testFramework));

  // Create test CMakeLists.txt if using CMake
  if (enums::to_string(options_.buildSystem) == "cmake") {
    std::string testCMakePath = FileUtils::combinePath(
        FileUtils::combinePath(projectPath, "tests"), "CMakeLists.txt");
    std::string testCMakeContent;

    if (testFramework == "gtest") {
      testCMakeContent = getGTestContent();
    } else if (testFramework == "catch2") {
      testCMakeContent = getCatch2Content();
    } else if (testFramework == "doctest") {
      testCMakeContent = getDocTestContent();
    }

    if (!FileUtils::writeToFile(testCMakePath, testCMakeContent)) {
      spdlog::error("Failed to create test CMakeLists.txt");
      return false;
    }
  }

  // Create basic test file
  std::string testFilePath = FileUtils::combinePath(
      FileUtils::combinePath(projectPath, "tests"), "test_" + options_.projectName + ".cpp");
  std::string testContent = fmt::format(R"(#include <{0}/{0}.h>

#ifdef USING_GTEST
#include <gtest/gtest.h>

TEST({1}Test, BasicFunctionality) {{
    {0}::Example example(42);
    EXPECT_EQ(42, example.getValue());
    EXPECT_EQ(7, {0}::add(3, 4));
}}

int main(int argc, char** argv) {{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}}

#elif defined(USING_CATCH2)
#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

TEST_CASE("{0} basic functionality", "[{0}]") {{
    {0}::Example example(42);
    REQUIRE(example.getValue() == 42);
    REQUIRE({0}::add(3, 4) == 7);
}}

#elif defined(USING_DOCTEST)
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

TEST_CASE("testing {0}") {{
    {0}::Example example(42);
    CHECK(example.getValue() == 42);
    CHECK({0}::add(3, 4) == 7);
}}

#else
#include <iostream>
#include <cassert>

int main() {{
    {0}::Example example(42);
    assert(example.getValue() == 42);
    assert({0}::add(3, 4) == 7);
    std::cout << "All tests passed!" << std::endl;
    return 0;
}}
#endif
)", options_.projectName, StringUtils::toUpper(options_.projectName));

  if (!FileUtils::writeToFile(testFilePath, testContent)) {
    spdlog::error("Failed to create test file");
    return false;
  }

  return true;
}

bool HeaderOnlyLibTemplate::setupDocumentation() {
  std::string projectPath = options_.projectName;
  std::string docsPath = FileUtils::combinePath(projectPath, "docs");

  // Create documentation directory
  if (!FileUtils::createDirectory(docsPath)) {
    spdlog::error("Failed to create documentation directory");
    return false;
  }

  // Create Doxygen configuration
  std::string doxyfilePath = FileUtils::combinePath(projectPath, "Doxyfile");
  if (!FileUtils::writeToFile(doxyfilePath, getDoxygenContent())) {
    spdlog::error("Failed to create Doxygen configuration");
    return false;
  }

  // Create README for documentation
  std::string docReadmePath = FileUtils::combinePath(docsPath, "README.md");
  if (!FileUtils::writeToFile(docReadmePath, getDocumentationReadme())) {
    spdlog::error("Failed to create documentation README");
    return false;
  }

  // Create API documentation template
  std::string apiDocsPath = FileUtils::combinePath(docsPath, "api");
  if (!FileUtils::createDirectory(apiDocsPath)) {
    spdlog::error("Failed to create API documentation directory");
    return false;
  }

  std::string apiIndexPath = FileUtils::combinePath(apiDocsPath, "index.md");
  if (!FileUtils::writeToFile(apiIndexPath, getAPIDocumentationTemplate())) {
    spdlog::error("Failed to create API documentation template");
    return false;
  }

  // Create examples documentation
  std::string examplesDocsPath = FileUtils::combinePath(docsPath, "examples");
  if (!FileUtils::createDirectory(examplesDocsPath)) {
    spdlog::error("Failed to create examples documentation directory");
    return false;
  }

  std::string examplesIndexPath = FileUtils::combinePath(examplesDocsPath, "index.md");
  if (!FileUtils::writeToFile(examplesIndexPath, getExamplesDocumentationTemplate())) {
    spdlog::error("Failed to create examples documentation template");
    return false;
  }

  // Create documentation generation script
  std::string scriptName = "generate_docs";
#ifdef _WIN32
  scriptName += ".bat";
#else
  scriptName += ".sh";
#endif

  std::string scriptPath = FileUtils::combinePath(docsPath, scriptName);
  if (!FileUtils::writeToFile(scriptPath, getDocumentationScript())) {
    spdlog::error("Failed to create documentation generation script");
    return false;
  }

  // Make script executable on Unix-like systems
#ifndef _WIN32
  system(("chmod +x \"" + scriptPath + "\"").c_str());
#endif

  spdlog::info("Documentation setup completed successfully");
  return true;
}

bool HeaderOnlyLibTemplate::setupContinuousIntegration() {
  // Implementation will be added in next chunk
  return true;
}

bool HeaderOnlyLibTemplate::setupCodeFormatting() {
  // Implementation will be added in next chunk
  return true;
}

bool HeaderOnlyLibTemplate::setupBenchmarking() {
  // Implementation will be added in next chunk
  return true;
}

bool HeaderOnlyLibTemplate::setupVersionControl() {
  // Implementation will be added in next chunk
  return true;
}

bool HeaderOnlyLibTemplate::createSingleHeaderVersion() {
  // Implementation will be added in next chunk
  return true;
}

bool HeaderOnlyLibTemplate::createExamples() {
  // Implementation will be added in next chunk
  return true;
}

bool HeaderOnlyLibTemplate::createBenchmarks() {
  // Implementation will be added in next chunk
  return true;
}

bool HeaderOnlyLibTemplate::createDocumentation() {
  // Implementation will be added in next chunk
  return true;
}

std::string HeaderOnlyLibTemplate::getMainHeaderContent() {
  std::string projectNameUpper = getProjectNameUpper();
  std::string includeGuard = getIncludeGuard(options_.projectName + ".h");

  return fmt::format(R"({0}
{0}

#include <string>
#include <memory>
#include <stdexcept>

// Include version information
#include "{1}/version.h"
#include "{1}/config.h"

namespace {1} {{

/// @brief Example class demonstrating the library functionality
/// @details This is a header-only implementation that provides
/// basic functionality for demonstration purposes.
class Example {{
public:
    /// @brief Constructor with initial value
    /// @param value Initial value to store
    explicit Example(int value = 0) : value_(value) {{
        if (value < 0) {{
            throw std::invalid_argument("Value must be non-negative");
        }}
    }}

    /// @brief Get the stored value
    /// @return The current value
    int getValue() const noexcept {{ return value_; }}

    /// @brief Set a new value
    /// @param value New value to store
    /// @throws std::invalid_argument if value is negative
    void setValue(int value) {{
        if (value < 0) {{
            throw std::invalid_argument("Value must be non-negative");
        }}
        value_ = value;
    }}

    /// @brief Double the stored value
    /// @return Reference to this object for chaining
    Example& doubleValue() noexcept {{
        value_ *= 2;
        return *this;
    }}

private:
    int value_;
}};

/// @brief Add two integers
/// @param a First integer
/// @param b Second integer
/// @return Sum of a and b
constexpr int add(int a, int b) noexcept {{
    return a + b;
}}

/// @brief Multiply two integers
/// @param a First integer
/// @param b Second integer
/// @return Product of a and b
constexpr int multiply(int a, int b) noexcept {{
    return a * b;
}}

/// @brief Get library version string
/// @return Version string in format "major.minor.patch"
inline std::string getVersion() {{
    return VERSION_STR;
}}

/// @brief Check if library was compiled with debug support
/// @return true if debug support is enabled
constexpr bool isDebugBuild() noexcept {{
#ifdef {2}_DEBUG
    return true;
#else
    return false;
#endif
}}

}} // namespace {1}

// Include implementation details
#include "{1}/detail/impl.h"

#endif // {0}
)", includeGuard, options_.projectName, projectNameUpper);
}

std::string HeaderOnlyLibTemplate::getDetailHeaderContent() {
  std::string includeGuard = getIncludeGuard("detail/impl.h");

  return fmt::format(R"({0}
{0}

// This file contains implementation details that are not part of the public API
// Users should not include this file directly

namespace {1} {{
namespace detail {{

/// @brief Internal helper function for validation
/// @param value Value to validate
/// @return true if value is valid
inline bool isValidValue(int value) noexcept {{
    return value >= 0 && value <= 1000000; // Arbitrary upper limit
}}

/// @brief Internal utility class for advanced operations
class AdvancedOperations {{
public:
    /// @brief Calculate factorial (internal use only)
    /// @param n Input number
    /// @return Factorial of n
    static constexpr long long factorial(int n) noexcept {{
        return (n <= 1) ? 1 : n * factorial(n - 1);
    }}

    /// @brief Check if number is prime (internal use only)
    /// @param n Number to check
    /// @return true if n is prime
    static bool isPrime(int n) noexcept {{
        if (n < 2) return false;
        if (n == 2) return true;
        if (n % 2 == 0) return false;

        for (int i = 3; i * i <= n; i += 2) {{
            if (n % i == 0) return false;
        }}
        return true;
    }}
}};

}} // namespace detail
}} // namespace {1}

#endif // {0}
)", includeGuard, options_.projectName);
}

std::string HeaderOnlyLibTemplate::getVersionHeaderContent() {
  std::string projectNameUpper = getProjectNameUpper();
  std::string includeGuard = getIncludeGuard("version.h");

  return fmt::format(R"({0}
{0}

#define {1}_VERSION_MAJOR 1
#define {1}_VERSION_MINOR 0
#define {1}_VERSION_PATCH 0

#define {1}_VERSION_STR "1.0.0"
#define VERSION_STR {1}_VERSION_STR

// Semantic version as integer for easy comparison
#define {1}_VERSION_INT ((({1}_VERSION_MAJOR) << 16) | (({1}_VERSION_MINOR) << 8) | ({1}_VERSION_PATCH))

// Compatibility macros
#define {1}_VERSION_AT_LEAST(major, minor, patch) \
    ({1}_VERSION_INT >= (((major) << 16) | ((minor) << 8) | (patch)))

#endif // {0}
)", includeGuard, projectNameUpper);
}

std::string HeaderOnlyLibTemplate::getConfigHeaderContent() {
  std::string projectNameUpper = getProjectNameUpper();
  std::string includeGuard = getIncludeGuard("config.h");

  return fmt::format(R"({0}
{0}

// Configuration macros for {1}

// Compiler detection
#if defined(_MSC_VER)
    #define {2}_COMPILER_MSVC
#elif defined(__GNUC__)
    #define {2}_COMPILER_GCC
#elif defined(__clang__)
    #define {2}_COMPILER_CLANG
#endif

// Platform detection
#if defined(_WIN32) || defined(_WIN64)
    #define {2}_PLATFORM_WINDOWS
#elif defined(__APPLE__)
    #define {2}_PLATFORM_MACOS
#elif defined(__linux__)
    #define {2}_PLATFORM_LINUX
#endif

// C++ standard detection
#if __cplusplus >= 202002L
    #define {2}_CPP20_OR_LATER
#elif __cplusplus >= 201703L
    #define {2}_CPP17_OR_LATER
#elif __cplusplus >= 201402L
    #define {2}_CPP14_OR_LATER
#elif __cplusplus >= 201103L
    #define {2}_CPP11_OR_LATER
#endif

// Feature detection
#ifdef {2}_CPP17_OR_LATER
    #define {2}_HAS_STRING_VIEW
    #define {2}_HAS_OPTIONAL
#endif

#ifdef {2}_CPP20_OR_LATER
    #define {2}_HAS_CONCEPTS
    #define {2}_HAS_RANGES
#endif

// Debug mode detection
#if defined(DEBUG) || defined(_DEBUG) || !defined(NDEBUG)
    #define {2}_DEBUG
#endif

// API macros
#ifdef {2}_PLATFORM_WINDOWS
    #ifdef {2}_BUILDING_DLL
        #define {2}_API __declspec(dllexport)
    #elif defined({2}_USING_DLL)
        #define {2}_API __declspec(dllimport)
    #else
        #define {2}_API
    #endif
#else
    #define {2}_API
#endif

// Inline macros
#ifdef {2}_COMPILER_MSVC
    #define {2}_FORCE_INLINE __forceinline
#elif defined({2}_COMPILER_GCC) || defined({2}_COMPILER_CLANG)
    #define {2}_FORCE_INLINE __attribute__((always_inline)) inline
#else
    #define {2}_FORCE_INLINE inline
#endif

#endif // {0}
)", includeGuard, options_.projectName, projectNameUpper);
}

std::string HeaderOnlyLibTemplate::getExampleUsageContent() {
  return fmt::format(R"(#include <iostream>
#include <{0}/{0}.h>

int main() {{
    try {{
        // Create an example object
        {0}::Example example(42);
        std::cout << "Initial value: " << example.getValue() << std::endl;

        // Modify the value
        example.setValue(10);
        std::cout << "After setValue(10): " << example.getValue() << std::endl;

        // Chain operations
        example.doubleValue().doubleValue();
        std::cout << "After doubling twice: " << example.getValue() << std::endl;

        // Use free functions
        int sum = {0}::add(3, 4);
        int product = {0}::multiply(5, 6);
        std::cout << "3 + 4 = " << sum << std::endl;
        std::cout << "5 * 6 = " << product << std::endl;

        // Get version information
        std::cout << "Library version: " << {0}::getVersion() << std::endl;
        std::cout << "Debug build: " << ({0}::isDebugBuild() ? "Yes" : "No") << std::endl;

    }} catch (const std::exception& e) {{
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }}

    return 0;
}}
)", options_.projectName);
}

std::string HeaderOnlyLibTemplate::getReadmeContent() {
  std::string buildInstructions;
  std::string packageManagerInfo;
  std::string testInstructions;

  if (enums::to_string(options_.buildSystem) == "cmake") {
    buildInstructions = R"(```bash
mkdir build && cd build
cmake ..
make
```";
  } else if (enums::to_string(options_.buildSystem) == "meson") {
    buildInstructions = R"(```bash
meson setup build
cd build
meson compile
```";
  } else if (enums::to_string(options_.buildSystem) == "bazel") {
    buildInstructions = R"(```bash
bazel build //...
```";
  }

  if (enums::to_string(options_.packageManager) == "vcpkg") {
    packageManagerInfo = "- vcpkg package manager";
  } else if (enums::to_string(options_.packageManager) == "conan") {
    packageManagerInfo = "- Conan package manager";
  }

  if (options_.includeTests) {
    testInstructions = R"(
## Testing

Run the tests:

```bash
cd build
ctest
```

Or run the test executable directly:

```bash
./tests/test_)" + options_.projectName + R"(
```
)";
  }

  return fmt::format(R"(# {0}

A modern C++ header-only library created with CPP-Scaffold.

## Features

- **Header-only**: No compilation required, just include the headers
- **Modern C++**: Uses C++17 features with C++20 compatibility
- **Cross-platform**: Works on Windows, macOS, and Linux
- **Well-documented**: Comprehensive API documentation
- **Single-header version**: Available for easy integration
- **Extensive testing**: Comprehensive test suite
{1}

## Quick Start

### Installation

Since this is a header-only library, you can simply copy the `include/{0}/` directory to your project or use a package manager.

#### Using the single-header version

Copy `single_header/{0}.hpp` to your project and include it:

```cpp
#include "{0}.hpp"
```

#### Using the modular version

Add the `include` directory to your include path and:

```cpp
#include <{0}/{0}.h>
```

### Basic Usage

```cpp
#include <iostream>
#include <{0}/{0}.h>

int main() {{
    // Create an example object
    {0}::Example example(42);
    std::cout << "Value: " << example.getValue() << std::endl;

    // Use library functions
    int result = {0}::add(3, 4);
    std::cout << "3 + 4 = " << result << std::endl;

    // Get version
    std::cout << "Version: " << {0}::getVersion() << std::endl;

    return 0;
}}
```

## Building Examples and Tests

{2}
{3}
## Documentation

- API documentation is available in the `docs/` directory
- Examples can be found in the `examples/` directory
- See `CONTRIBUTING.md` for development guidelines

## Requirements

- C++17 compatible compiler
- CMake 3.15+ (for building examples and tests)

## License

This project is licensed under the MIT License - see the LICENSE file for details.

## Contributing

Contributions are welcome! Please read `CONTRIBUTING.md` for guidelines.
)", options_.projectName, packageManagerInfo, buildInstructions, testInstructions);
}

// Helper methods
std::string HeaderOnlyLibTemplate::getProjectNameUpper() const {
  return StringUtils::toUpper(options_.projectName);
}

std::string HeaderOnlyLibTemplate::getProjectNamespace() const {
  return options_.projectName;
}

std::string HeaderOnlyLibTemplate::getIncludeGuard(const std::string& filename) const {
  std::string guard = getProjectNameUpper() + "_" + StringUtils::toUpper(filename);
  // Replace non-alphanumeric characters with underscores
  for (char& c : guard) {
    if (!std::isalnum(c)) {
      c = '_';
    }
  }
  return "#ifndef " + guard + "\n#define " + guard;
}

std::string HeaderOnlyLibTemplate::getCurrentYear() const {
  auto now = std::chrono::system_clock::now();
  auto time_t = std::chrono::system_clock::to_time_t(now);
  std::stringstream ss;
  ss << std::put_time(std::localtime(&time_t), "%Y");
  return ss.str();
}

std::string HeaderOnlyLibTemplate::getCMakeContent() {
  std::string testConfig;
  if (options_.includeTests) {
    testConfig = R"(
# Testing
enable_testing()
add_subdirectory(tests)
)";
  }

  return fmt::format(R"(cmake_minimum_required(VERSION 3.15)
project({0} VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Header-only library
add_library({0} INTERFACE)
add_library({0}::{0} ALIAS {0})

# Include directories
target_include_directories({0} INTERFACE
    $<BUILD_INTERFACE:${{CMAKE_CURRENT_SOURCE_DIR}}/include>
    $<INSTALL_INTERFACE:include>
)

# Compiler features
target_compile_features({0} INTERFACE cxx_std_17)

# Installation
include(GNUInstallDirs)
install(TARGETS {0}
    EXPORT {0}Targets
    ARCHIVE DESTINATION ${{CMAKE_INSTALL_LIBDIR}}
    LIBRARY DESTINATION ${{CMAKE_INSTALL_LIBDIR}}
    RUNTIME DESTINATION ${{CMAKE_INSTALL_BINDIR}}
)

install(DIRECTORY include/ DESTINATION ${{CMAKE_INSTALL_INCLUDEDIR}})

install(EXPORT {0}Targets
    FILE {0}Targets.cmake
    NAMESPACE {0}::
    DESTINATION ${{CMAKE_INSTALL_LIBDIR}}/cmake/{0}
)

# Create config file
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    {0}ConfigVersion.cmake
    VERSION ${{PROJECT_VERSION}}
    COMPATIBILITY SameMajorVersion
)

configure_package_config_file(
    cmake/{0}Config.cmake.in
    {0}Config.cmake
    INSTALL_DESTINATION ${{CMAKE_INSTALL_LIBDIR}}/cmake/{0}
)

install(FILES
    ${{CMAKE_CURRENT_BINARY_DIR}}/{0}Config.cmake
    ${{CMAKE_CURRENT_BINARY_DIR}}/{0}ConfigVersion.cmake
    DESTINATION ${{CMAKE_INSTALL_LIBDIR}}/cmake/{0}
)

# Examples
option(BUILD_EXAMPLES "Build examples" ON)
if(BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()
{1}
)", options_.projectName, testConfig);
}

std::string HeaderOnlyLibTemplate::getMesonContent() {
  return fmt::format(R"(project('{0}', 'cpp',
  version : '1.0.0',
  default_options : ['warning_level=3', 'cpp_std=c++17'])

# Header-only library
{0}_inc = include_directories('include')

{0}_dep = declare_dependency(
  include_directories : {0}_inc,
  version : meson.project_version()
)

# Install headers
install_subdir('include/{0}', install_dir : get_option('includedir'))

# Examples
if get_option('build_examples')
  subdir('examples')
endif

# Tests
if get_option('build_tests')
  subdir('tests')
endif

# pkg-config file
pkg = import('pkgconfig')
pkg.generate(
  name : '{0}',
  description : 'A header-only C++ library',
  version : meson.project_version(),
  subdirs : '{0}'
)
)", options_.projectName);
}

std::string HeaderOnlyLibTemplate::getBazelContent() {
  return fmt::format(R"(load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "{0}",
    hdrs = glob(["include/{0}/**/*.h"]),
    includes = ["include"],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "{0}_test",
    testonly = True,
    hdrs = glob(["tests/**/*.h"]),
    srcs = glob(["tests/**/*.cpp"]),
    deps = [
        ":{0}",
        "@googletest//:gtest_main",
    ],
)
)", options_.projectName);
}

std::string HeaderOnlyLibTemplate::getVcpkgJsonContent() {
  return fmt::format(R"({{
  "name": "{0}",
  "version": "1.0.0",
  "description": "A header-only C++ library",
  "homepage": "https://github.com/yourname/{0}",
  "dependencies": [
  ],
  "features": {{
    "tests": {{
      "description": "Build tests",
      "dependencies": [
        "gtest"
      ]
    }}
  }}
}}
)", options_.projectName);
}

std::string HeaderOnlyLibTemplate::getConanfileContent() {
  return fmt::format(R"([requires]

[generators]
CMakeDeps
CMakeToolchain

[options]
shared=False
fPIC=True

[settings]
os
compiler
build_type
arch

[build_requires]
cmake/[>=3.15]
)", options_.projectName);
}

std::string HeaderOnlyLibTemplate::getGTestContent() {
  return fmt::format(R"(find_package(GTest REQUIRED)

add_executable(test_{0}
    test_{0}.cpp
)

target_link_libraries(test_{0}
    PRIVATE
    {0}::{0}
    GTest::gtest_main
)

target_compile_definitions(test_{0} PRIVATE USING_GTEST)

include(GoogleTest)
gtest_discover_tests(test_{0})
)", options_.projectName);
}

std::string HeaderOnlyLibTemplate::getCatch2Content() {
  return fmt::format(R"(find_package(Catch2 REQUIRED)

add_executable(test_{0}
    test_{0}.cpp
)

target_link_libraries(test_{0}
    PRIVATE
    {0}::{0}
    Catch2::Catch2WithMain
)

target_compile_definitions(test_{0} PRIVATE USING_CATCH2)

include(CTest)
include(Catch)
catch_discover_tests(test_{0})
)", options_.projectName);
}

std::string HeaderOnlyLibTemplate::getDocTestContent() {
  return fmt::format(R"(find_package(doctest REQUIRED)

add_executable(test_{0}
    test_{0}.cpp
)

target_link_libraries(test_{0}
    PRIVATE
    {0}::{0}
    doctest::doctest
)

target_compile_definitions(test_{0} PRIVATE USING_DOCTEST)

include(CTest)
include(doctest)
doctest_discover_tests(test_{0})
)", options_.projectName);
}

std::string HeaderOnlyLibTemplate::getSingleHeaderScript() {
  return fmt::format(R"(#!/usr/bin/env python3
"""
Script to generate a single-header version of {0}
"""

import os
import re
import sys
from pathlib import Path

def process_includes(content, processed_files, base_path):
    """Process #include directives and inline local headers"""
    lines = content.split('\n')
    result = []

    for line in lines:
        # Check for local includes
        match = re.match(r'#include\s*["\<]({0}/.*?)["\>]', line)
        if match:
            include_path = match.group(1)
            full_path = base_path / "include" / include_path

            if full_path.exists() and str(full_path) not in processed_files:
                processed_files.add(str(full_path))
                with open(full_path, 'r') as f:
                    include_content = f.read()

                # Remove include guards
                include_content = re.sub(r'#ifndef\s+\w+\s*\n#define\s+\w+\s*\n', '', include_content)
                include_content = re.sub(r'#endif\s*//.*?$', '', include_content, flags=re.MULTILINE)

                # Recursively process includes
                processed_content = process_includes(include_content, processed_files, base_path)
                result.append(f"// Begin {include_path}")
                result.append(processed_content)
                result.append(f"// End {include_path}")
            else:
                result.append(line)
        else:
            result.append(line)

    return '\n'.join(result)

def main():
    base_path = Path(__file__).parent.parent
    main_header = base_path / "include" / "{0}" / "{0}.h"
    output_dir = base_path / "single_header"
    output_file = output_dir / "{0}.hpp"

    if not main_header.exists():
        print(f"Error: Main header {main_header} not found")
        sys.exit(1)

    output_dir.mkdir(exist_ok=True)

    with open(main_header, 'r') as f:
        content = f.read()

    processed_files = set()
    single_header_content = process_includes(content, processed_files, base_path)

    # Add header comment
    header_comment = f"""/*
 * {0} - Single Header Version
 *
 * This is an automatically generated single-header version of {0}.
 *
 * Original project: https://github.com/yourname/{0}
 * Generated on: {{}}
 */

""".format(content)

    final_content = header_comment + single_header_content

    with open(output_file, 'w') as f:
        f.write(final_content)

    print(f"Single header generated: {output_file}")
    print(f"Size: {len(final_content)} characters")

if __name__ == "__main__":
    main()
)", options_.projectName);
}

std::string HeaderOnlyLibTemplate::getDocumentationReadme() {
  return fmt::format(R"(# {} Documentation

This directory contains the documentation for the {} header-only library.

## Documentation Structure

- `api/` - API reference documentation
- `examples/` - Usage examples and tutorials
- `Doxyfile` - Doxygen configuration file (in project root)

## Building Documentation

### Using Doxygen

To generate HTML documentation using Doxygen:

```bash
# From project root
doxygen Doxyfile
```

The generated documentation will be available in `docs/html/index.html`.

### Using the Generation Script

You can also use the provided script:

```bash
# On Unix-like systems
./docs/generate_docs.sh

# On Windows
docs\generate_docs.bat
```

## Documentation Guidelines

When documenting your code:

1. Use Doxygen-style comments for all public APIs
2. Include usage examples in your documentation
3. Document parameters, return values, and exceptions
4. Use `@brief` for short descriptions
5. Use `@param` for parameter documentation
6. Use `@return` for return value documentation
7. Use `@throws` or `@exception` for exception documentation

## Example Documentation

```cpp
/**
 * @brief Adds two integers together
 * @param a First integer
 * @param b Second integer
 * @return Sum of a and b
 * @throws std::overflow_error if the result would overflow
 */
int add(int a, int b);
```

## Online Documentation

If you're hosting your documentation online, consider using:

- GitHub Pages (free for public repositories)
- Read the Docs (free for open source projects)
- GitLab Pages (free with GitLab)

## Contributing to Documentation

Please ensure all new features and public APIs are properly documented before submitting pull requests.
)", options_.projectName, options_.projectName);
}

std::string HeaderOnlyLibTemplate::getAPIDocumentationTemplate() {
  return fmt::format(R"(# {} API Reference

This document provides a comprehensive reference for the {} library API.

## Core Classes

### Example Class

The main class for demonstrating library functionality.

```cpp
#include <{}.h>

{}::Example example(42);
int value = example.getValue();
```

#### Constructor

```cpp
Example(int value)
```

Creates a new Example instance with the specified value.

**Parameters:**
- `value`: Initial value for the example

#### Methods

##### getValue()

```cpp
int getValue() const noexcept
```

Returns the current value stored in the Example instance.

**Returns:** The current value as an integer

##### setValue()

```cpp
void setValue(int newValue)
```

Sets a new value for the Example instance.

**Parameters:**
- `newValue`: The new value to set

**Throws:** `std::invalid_argument` if newValue is negative

## Utility Functions

### add()

```cpp
int add(int a, int b)
```

Adds two integers together.

**Parameters:**
- `a`: First integer
- `b`: Second integer

**Returns:** Sum of a and b

**Example:**
```cpp
int result = {}::add(3, 4); // result = 7
```

## Constants

### VERSION

```cpp
constexpr const char* VERSION
```

The version string of the library.

## Error Handling

The library uses standard C++ exceptions for error handling:

- `std::invalid_argument`: For invalid input parameters
- `std::runtime_error`: For runtime errors
- `std::logic_error`: For logic errors in usage

## Thread Safety

All functions in this library are thread-safe unless otherwise noted.

## Memory Management

This is a header-only library with minimal memory allocation. All classes use RAII principles.
)", options_.projectName, options_.projectName, options_.projectName, options_.projectName, options_.projectName);
}

std::string HeaderOnlyLibTemplate::getExamplesDocumentationTemplate() {
  return fmt::format(R"(# {} Examples

This document provides practical examples of using the {} library.

## Basic Usage

### Simple Example

```cpp
#include <{}.h>
#include <iostream>

int main() {{
    // Create an example instance
    {}::Example example(42);

    // Get the value
    std::cout << "Value: " << example.getValue() << std::endl;

    // Use utility function
    int sum = {}::add(10, 20);
    std::cout << "Sum: " << sum << std::endl;

    return 0;
}}
```

### Error Handling

```cpp
#include <{}.h>
#include <iostream>
#include <stdexcept>

int main() {{
    try {{
        {}::Example example(42);
        example.setValue(-1); // This will throw
    }} catch (const std::invalid_argument& e) {{
        std::cerr << "Error: " << e.what() << std::endl;
    }}

    return 0;
}}
```

## Advanced Usage

### Custom Configuration

```cpp
#include <{}.h>

int main() {{
    // Use library configuration
    if ({}::config::ENABLE_VALIDATION) {{
        std::cout << "Validation is enabled" << std::endl;
    }}

    // Check version
    std::cout << "Library version: " << {}::VERSION << std::endl;

    return 0;
}}
```

### Integration with STL

```cpp
#include <{}.h>
#include <vector>
#include <algorithm>

int main() {{
    std::vector<{}::Example> examples;

    // Create multiple examples
    for (int i = 0; i < 5; ++i) {{
        examples.emplace_back(i * 10);
    }}

    // Use with STL algorithms
    auto it = std::find_if(examples.begin(), examples.end(),
        [](const {}::Example& ex) {{
            return ex.getValue() > 20;
        }});

    if (it != examples.end()) {{
        std::cout << "Found example with value: " << it->getValue() << std::endl;
    }}

    return 0;
}}
```

## Building Examples

### Using CMake

```bash
mkdir build && cd build
cmake ..
make examples
```

### Manual Compilation

```bash
# Simple compilation
g++ -std=c++17 -I../include example.cpp -o example

# With optimizations
g++ -std=c++17 -O3 -I../include example.cpp -o example
```

## Performance Examples

### Benchmarking

```cpp
#include <{}.h>
#include <chrono>
#include <iostream>

int main() {{
    const int iterations = 1000000;

    auto start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < iterations; ++i) {{
        {}::add(i, i + 1);
    }}

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

    std::cout << "Time for " << iterations << " operations: "
              << duration.count() << " microseconds" << std::endl;

    return 0;
}}
```

## Common Patterns

### RAII Usage

```cpp
#include <{}.h>

class ResourceManager {{
private:
    {}::Example resource_;

public:
    ResourceManager(int value) : resource_(value) {{
        // Resource acquired in constructor
    }}

    ~ResourceManager() {{
        // Resource automatically cleaned up
    }}

    int getValue() const {{
        return resource_.getValue();
    }}
}};
```

## Troubleshooting

### Common Issues

1. **Compilation Errors**: Ensure you're using C++17 or later
2. **Linking Issues**: This is a header-only library, no linking required
3. **Include Path**: Make sure the include directory is in your compiler's search path

### Debug Mode

```cpp
#define {}_DEBUG
#include <{}.h>

// Debug information will be available
```
)",
    options_.projectName, options_.projectName, options_.projectName, options_.projectName,
    options_.projectName, options_.projectName, options_.projectName, options_.projectName,
    options_.projectName, options_.projectName, options_.projectName, options_.projectName,
    options_.projectName, options_.projectName, options_.projectName, options_.projectName,
    options_.projectName, StringUtils::toUpper(options_.projectName), options_.projectName);
}

std::string HeaderOnlyLibTemplate::getDoxygenContent() {
  return fmt::format(R"(PROJECT_NAME           = {0}
PROJECT_NUMBER         = 1.0.0
PROJECT_BRIEF         = "A header-only C++ library created with CPP-Scaffold"
OUTPUT_DIRECTORY      = docs/doxygen
CREATE_SUBDIRS        = NO
BRIEF_MEMBER_DESC     = YES
REPEAT_BRIEF         = YES
ALWAYS_DETAILED_SEC  = NO
INLINE_INHERITED_MEMB = NO
FULL_PATH_NAMES      = YES
STRIP_FROM_PATH      =
STRIP_FROM_INC_PATH  =
TAB_SIZE             = 4
MARKDOWN_SUPPORT     = YES
TOC_INCLUDE_HEADINGS = 5
AUTOLINK_SUPPORT     = YES
BUILTIN_STL_SUPPORT = YES
EXTRACT_ALL          = YES
EXTRACT_PRIVATE      = NO
EXTRACT_STATIC       = YES
EXTRACT_LOCAL_CLASSES = YES
HIDE_UNDOC_MEMBERS   = NO
HIDE_UNDOC_CLASSES   = NO
HIDE_IN_BODY_DOCS    = NO
CASE_SENSE_NAMES     = YES
HIDE_SCOPE_NAMES     = NO
HIDE_COMPOUND_REFERENCE = NO
SHOW_INCLUDE_FILES   = YES
SHOW_GROUPED_MEMB_INC = NO
FORCE_LOCAL_INCLUDES = NO
INLINE_INFO          = YES
SORT_MEMBER_DOCS     = YES
SORT_BRIEF_DOCS      = NO
SORT_MEMBERS_CTORS_1ST = NO
SORT_GROUP_NAMES     = NO
SORT_BY_SCOPE_NAME   = NO
STRICT_PROTO_MATCHING = NO
GENERATE_TODOLIST    = YES
GENERATE_TESTLIST    = YES
GENERATE_BUGLIST     = YES
GENERATE_DEPRECATEDLIST = YES
ENABLED_SECTIONS     =
MAX_INITIALIZER_LINES = 30
SHOW_USED_FILES      = YES
SHOW_FILES           = YES
SHOW_NAMESPACES      = YES
FILE_VERSION_FILTER  =
LAYOUT_FILE          =
CITE_BIB_FILES       =
QUIET                = NO
WARNINGS             = YES
WARN_IF_UNDOCUMENTED = YES
WARN_IF_DOC_ERROR    = YES
WARN_NO_PARAMDOC     = NO
WARN_AS_ERROR        = NO
WARN_FORMAT          = "$file:$line: $text"
WARN_LOGFILE         =
INPUT                = include/ README.md
INPUT_ENCODING       = UTF-8
FILE_PATTERNS        = *.h *.hpp *.hxx *.md
RECURSIVE            = YES
EXCLUDE              =
EXCLUDE_SYMLINKS     = NO
EXCLUDE_PATTERNS     = */build/* */.*
EXCLUDE_SYMBOLS      =
EXAMPLE_PATH         = examples/
EXAMPLE_PATTERNS     = *.cpp *.h
EXAMPLE_RECURSIVE    = NO
IMAGE_PATH           =
INPUT_FILTER         =
FILTER_PATTERNS      =
FILTER_SOURCE_FILES  = NO
FILTER_SOURCE_PATTERNS =
USE_MDFILE_AS_MAINPAGE = README.md
SOURCE_BROWSER       = NO
INLINE_SOURCES       = NO
STRIP_CODE_COMMENTS  = YES
REFERENCED_BY_RELATION = NO
REFERENCES_RELATION  = NO
REFERENCES_LINK_SOURCE = YES
SOURCE_TOOLTIPS      = YES
USE_HTAGS            = NO
VERBATIM_HEADERS     = YES
CLANG_ASSISTED_PARSING = NO
CLANG_OPTIONS        =
CLANG_DATABASE_PATH  =
ALPHABETICAL_INDEX   = YES
COLS_IN_ALPHA_INDEX  = 5
IGNORE_PREFIX        =
GENERATE_HTML        = YES
HTML_OUTPUT          = html
HTML_FILE_EXTENSION  = .html
HTML_HEADER          =
HTML_FOOTER          =
HTML_STYLESHEET      =
HTML_EXTRA_STYLESHEET =
HTML_EXTRA_FILES     =
HTML_COLORSTYLE_HUE  = 220
HTML_COLORSTYLE_SAT  = 100
HTML_COLORSTYLE_GAMMA = 80
HTML_TIMESTAMP       = NO
HTML_DYNAMIC_MENUS   = YES
HTML_DYNAMIC_SECTIONS = NO
HTML_INDEX_NUM_ENTRIES = 100
GENERATE_DOCSET      = NO
DOCSET_FEEDNAME      = "Doxygen generated docs"
DOCSET_BUNDLE_ID     = org.doxygen.Project
DOCSET_PUBLISHER_ID  = org.doxygen.Publisher
DOCSET_PUBLISHER_NAME = Publisher
GENERATE_HTMLHELP    = NO
CHM_FILE             =
HHC_LOCATION         =
GENERATE_CHI         = NO
CHM_INDEX_ENCODING   =
BINARY_TOC           = NO
TOC_EXPAND           = NO
GENERATE_QHP         = NO
QCH_FILE             =
QHP_NAMESPACE        = org.doxygen.Project
QHP_VIRTUAL_FOLDER   = doc
QHP_CUST_FILTER_NAME =
QHP_CUST_FILTER_ATTRS =
QHP_SECT_FILTER_ATTRS =
QHG_LOCATION         =
GENERATE_ECLIPSEHELP = NO
ECLIPSE_DOC_ID       = org.doxygen.Project
DISABLE_INDEX        = NO
GENERATE_TREEVIEW    = NO
ENUM_VALUES_PER_LINE = 4
TREEVIEW_WIDTH       = 250
EXT_LINKS_IN_WINDOW  = NO
FORMULA_FONTSIZE     = 10
FORMULA_TRANSPARENT  = YES
USE_MATHJAX          = NO
MATHJAX_FORMAT       = HTML-CSS
MATHJAX_RELPATH      = https://cdn.jsdelivr.net/npm/mathjax@2
MATHJAX_EXTENSIONS   =
MATHJAX_CODEFILE     =
SEARCHENGINE         = YES
SERVER_BASED_SEARCH  = NO
EXTERNAL_SEARCH      = NO
SEARCHENGINE_URL     =
SEARCHDATA_FILE      = searchdata.xml
EXTERNAL_SEARCH_ID   =
EXTRA_SEARCH_MAPPINGS =
GENERATE_LATEX       = NO
LATEX_OUTPUT         = latex
LATEX_CMD_NAME       = latex
MAKEINDEX_CMD_NAME   = makeindex
COMPACT_LATEX        = NO
PAPER_TYPE           = a4
EXTRA_PACKAGES       =
LATEX_HEADER         =
LATEX_FOOTER         =
LATEX_EXTRA_STYLESHEET =
LATEX_EXTRA_FILES    =
PDF_HYPERLINKS       = YES
USE_PDFLATEX         = YES
LATEX_BATCHMODE      = NO
LATEX_HIDE_INDICES   = NO
LATEX_SOURCE_CODE    = NO
LATEX_BIB_STYLE      = plain
LATEX_TIMESTAMP      = NO
GENERATE_RTF         = NO
RTF_OUTPUT           = rtf
COMPACT_RTF          = NO
RTF_HYPERLINKS       = NO
RTF_STYLESHEET_FILE  =
RTF_EXTENSIONS_FILE  =
RTF_SOURCE_CODE      = NO
GENERATE_MAN         = NO
MAN_OUTPUT           = man
MAN_EXTENSION        = .3
MAN_SUBDIR           =
MAN_LINKS            = NO
GENERATE_XML         = NO
XML_OUTPUT           = xml
XML_PROGRAMLISTING   = YES
XML_NS_MEMB_FILE_SCOPE = NO
GENERATE_DOCBOOK     = NO
DOCBOOK_OUTPUT       = docbook
DOCBOOK_PROGRAMLISTING = NO
GENERATE_AUTOGEN_DEF = NO
GENERATE_PERLMOD     = NO
PERLMOD_LATEX        = NO
PERLMOD_PRETTY       = YES
PERLMOD_MAKEVAR_PREFIX =
ENABLE_PREPROCESSING = YES
MACRO_EXPANSION      = NO
EXPAND_ONLY_PREDEF   = NO
SEARCH_INCLUDES      = YES
INCLUDE_PATH         =
INCLUDE_FILE_PATTERNS =
PREDEFINED           =
EXPAND_AS_DEFINED    =
SKIP_FUNCTION_MACROS = YES
TAGFILES             =
GENERATE_TAGFILE     =
ALLEXTERNALS         = NO
EXTERNAL_GROUPS      = YES
EXTERNAL_PAGES       = YES
PERL_PATH            = /usr/bin/perl
CLASS_DIAGRAMS       = YES
MSCGEN_PATH          =
DIA_PATH             =
HIDE_UNDOC_RELATIONS = YES
HAVE_DOT             = NO
DOT_NUM_THREADS      = 0
DOT_FONTNAME         = Helvetica
DOT_FONTSIZE         = 10
DOT_FONTPATH         =
CLASS_GRAPH          = YES
COLLABORATION_GRAPH  = YES
GROUP_GRAPHS         = YES
UML_LOOK             = NO
UML_LIMIT_NUM_FIELDS = 10
TEMPLATE_RELATIONS   = NO
INCLUDE_GRAPH        = YES
INCLUDED_BY_GRAPH    = YES
CALL_GRAPH           = NO
CALLER_GRAPH         = NO
GRAPHICAL_HIERARCHY  = YES
DIRECTORY_GRAPH      = YES
DOT_IMAGE_FORMAT     = png
INTERACTIVE_SVG      = NO
DOT_PATH             =
DOTFILE_DIRS         =
MSCFILE_DIRS         =
DIAFILE_DIRS         =
PLANTUML_JAR_PATH    =
PLANTUML_CFG_FILE    =
PLANTUML_INCLUDE_PATH =
DOT_GRAPH_MAX_NODES  = 50
MAX_DOT_GRAPH_DEPTH  = 0
DOT_TRANSPARENT      = NO
DOT_MULTI_TARGETS    = NO
GENERATE_LEGEND      = YES
DOT_CLEANUP          = YES
)", options_.projectName);
}

std::string HeaderOnlyLibTemplate::getDocumentationScript() {
#ifdef _WIN32
  return fmt::format(R"(@echo off
REM Documentation generation script for {}

echo Generating documentation for {}...

REM Check if Doxygen is installed
where doxygen >nul 2>&1
if %ERRORLEVEL% neq 0 (
    echo Error: Doxygen is not installed or not in PATH
    echo Please install Doxygen from: https://www.doxygen.nl/download.html
    exit /b 1
)

REM Change to project root directory
cd %~dp0..

REM Generate documentation
echo Running Doxygen...
doxygen Doxyfile

if %ERRORLEVEL% neq 0 (
    echo Error: Documentation generation failed!
    exit /b 1
)

echo.
echo Documentation generated successfully!
echo Open docs\html\index.html in your browser to view the documentation.
echo.
)", options_.projectName, options_.projectName);
#else
  return fmt::format(R"(#!/bin/bash
# Documentation generation script for {}

echo "Generating documentation for {}..."

# Change to project root directory
cd "$(dirname "$0")/.."

# Check if Doxygen is installed
if ! command -v doxygen &> /dev/null; then
    echo "Error: Doxygen is not installed or not in PATH"
    echo "Please install Doxygen:"
    echo "  Ubuntu/Debian: sudo apt-get install doxygen"
    echo "  macOS: brew install doxygen"
    echo "  Or download from: https://www.doxygen.nl/download.html"
    exit 1
fi

# Generate documentation
echo "Running Doxygen..."
doxygen Doxyfile

if [ $? -ne 0 ]; then
    echo "Error: Documentation generation failed!"
    exit 1
fi

echo
echo "Documentation generated successfully!"
echo "Open docs/html/index.html in your browser to view the documentation."
echo
)", options_.projectName, options_.projectName);
#endif
}

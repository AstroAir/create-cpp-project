#include "modules_template.h"
#include "../utils/file_utils.h"
#include "../utils/terminal_utils.h"
#include "../config/ci_config.h"
#include <fmt/format.h>
#include <spdlog/spdlog.h>
#include <filesystem>

ModulesTemplate::ModulesTemplate(const CliOptions &options) : TemplateBase(options) {
  spdlog::info("Creating C++20/C++23 modules project: {}", options.projectName);
}

bool ModulesTemplate::create() {
  try {
    spdlog::info("Creating modules-based project structure");

    if (!createProjectStructure()) {
      spdlog::error("Failed to create project structure");
      return false;
    }

    if (!createBuildSystem()) {
      spdlog::error("Failed to create build system");
      return false;
    }

    if (!setupPackageManager()) {
      spdlog::error("Failed to setup package manager");
      return false;
    }

    if (!setupTestFramework()) {
      spdlog::error("Failed to setup test framework");
      return false;
    }

    if (!createModuleFiles()) {
      spdlog::error("Failed to create module files");
      return false;
    }

    if (options_.includeTests && !createTestModules()) {
      spdlog::error("Failed to create test modules");
      return false;
    }

    // Setup additional configurations
    setupCICD(options_.projectName);
    setupEditorConfig(options_.projectName);
    setupCodeStyleConfig(options_.projectName);
    setupDocConfig(options_.projectName);

    if (options_.initGit) {
      initializeGit(options_.projectName);
    }

    executePostCreationActions();
    printUsageGuide();

    spdlog::info("Successfully created C++20/C++23 modules project: {}", options_.projectName);
    return true;

  } catch (const std::exception &e) {
    spdlog::error("Error creating modules project: {}", e.what());
    return false;
  }
}

bool ModulesTemplate::createProjectStructure() {
  try {
    std::filesystem::create_directories(options_.projectName);
    std::filesystem::create_directories(options_.projectName + "/src");
    std::filesystem::create_directories(options_.projectName + "/modules");
    std::filesystem::create_directories(options_.projectName + "/include");

    if (options_.includeTests) {
      std::filesystem::create_directories(options_.projectName + "/tests");
    }

    if (options_.includeDocumentation) {
      std::filesystem::create_directories(options_.projectName + "/docs");
    }

    if (options_.includeCodeExamples) {
      std::filesystem::create_directories(options_.projectName + "/examples");
    }

    // Create README
    utils::FileUtils::writeToFile(options_.projectName + "/README.md", getReadmeContent());

    // Create main.cpp
    utils::FileUtils::writeToFile(options_.projectName + "/src/main.cpp", getMainCppContent());

    return true;
  } catch (const std::exception &e) {
    spdlog::error("Failed to create project structure: {}", e.what());
    return false;
  }
}

bool ModulesTemplate::createBuildSystem() {
  try {
    std::string buildContent;
    std::string filename;

    if (to_string(options_.buildSystem) == "cmake") {
      buildContent = getCMakeModulesContent();
      filename = "CMakeLists.txt";
    } else if (to_string(options_.buildSystem) == "meson") {
      buildContent = getMesonModulesContent();
      filename = "meson.build";
    } else if (to_string(options_.buildSystem) == "bazel") {
      buildContent = getBazelModulesContent();
      filename = "BUILD.bazel";
    } else {
      spdlog::warn("Build system {} not fully supported for modules, using CMake",
                   to_string(options_.buildSystem));
      buildContent = getCMakeModulesContent();
      filename = "CMakeLists.txt";
    }

    utils::FileUtils::writeToFile(options_.projectName + "/" + filename, buildContent);
    return true;
  } catch (const std::exception &e) {
    spdlog::error("Failed to create build system: {}", e.what());
    return false;
  }
}

bool ModulesTemplate::setupPackageManager() {
  try {
    if (to_string(options_.packageManager) == "vcpkg") {
      utils::FileUtils::writeToFile(options_.projectName + "/vcpkg.json", getVcpkgJsonContent());
    } else if (to_string(options_.packageManager) == "conan") {
      utils::FileUtils::writeToFile(options_.projectName + "/conanfile.txt", getConanfileContent());
    }
    return true;
  } catch (const std::exception &e) {
    spdlog::error("Failed to setup package manager: {}", e.what());
    return false;
  }
}

bool ModulesTemplate::setupTestFramework() {
  if (!options_.includeTests) {
    return true;
  }

  try {
    std::string testContent;

    if (to_string(options_.testFramework) == "gtest") {
      testContent = getGTestModulesContent();
    } else if (to_string(options_.testFramework) == "catch2") {
      testContent = getCatch2ModulesContent();
    } else if (to_string(options_.testFramework) == "doctest") {
      testContent = getDocTestModulesContent();
    }

    if (!testContent.empty()) {
      utils::FileUtils::writeToFile(options_.projectName + "/tests/test_" + getModuleName() + ".cpp", testContent);
    }

    return true;
  } catch (const std::exception &e) {
    spdlog::error("Failed to setup test framework: {}", e.what());
    return false;
  }
}

std::string ModulesTemplate::getModuleName() const {
  return options_.projectName;
}

std::string ModulesTemplate::getCppStandardFlag() const {
  // C++20 minimum for modules support
  using namespace cli_enums;
  if (options_.cppStandard == CppStandard::Cpp23) {
    return "23";
  } else if (options_.cppStandard == CppStandard::Cpp20) {
    return "20";
  } else {
    // Force C++20 minimum for modules
    spdlog::warn("C++ modules require C++20 or later, using C++20");
    return "20";
  }
}

bool ModulesTemplate::createModuleFiles() {
  try {
    // Create module interface file
    std::string moduleInterface = getModuleInterfaceContent();
    utils::FileUtils::writeToFile(options_.projectName + "/modules/" + getModuleName() + ".cppm", moduleInterface);

    // Create module implementation file
    std::string moduleImpl = getModuleImplementationContent();
    utils::FileUtils::writeToFile(options_.projectName + "/src/" + getModuleName() + ".cpp", moduleImpl);

    return true;
  } catch (const std::exception &e) {
    spdlog::error("Failed to create module files: {}", e.what());
    return false;
  }
}

bool ModulesTemplate::createTestModules() {
  // Test modules will be created by setupTestFramework
  return true;
}

std::string ModulesTemplate::getModuleInterfaceContent() {
  return fmt::format(R"(// {0} - C++20 Module Interface
// Generated by CPP-Scaffold

export module {0};

import std;

export namespace {0} {{
    // Example class
    class Example {{
    public:
        explicit Example(int value);
        int getValue() const;
        void setValue(int value);

    private:
        int value_;
    }};

    // Example function
    int add(int a, int b);

    // Version information
    std::string getVersion();
}}
)", getModuleName());
}

std::string ModulesTemplate::getModuleImplementationContent() {
  return fmt::format(R"(// {0} - C++20 Module Implementation
// Generated by CPP-Scaffold

module {0};

namespace {0} {{
    Example::Example(int value) : value_(value) {{}}

    int Example::getValue() const {{
        return value_;
    }}

    void Example::setValue(int value) {{
        value_ = value;
    }}

    int add(int a, int b) {{
        return a + b;
    }}

    std::string getVersion() {{
        return "1.0.0";
    }}
}}
)", getModuleName());
}

std::string ModulesTemplate::getMainCppContent() {
  return fmt::format(R"(// {0} - Main Application
// Generated by CPP-Scaffold

import {0};
import std;

int main() {{
    std::println("Welcome to {{}}!", "{0}");

    // Create an Example object
    {0}::Example example(42);

    // Use the example object
    int value = example.getValue();
    std::println("Example value: {{}}", value);

    // Use a free function
    int sum = {0}::add(3, 4);
    std::println("3 + 4 = {{}}", sum);

    // Get version
    std::string version = {0}::getVersion();
    std::println("Version: {{}}", version);

    return 0;
}}
)", getModuleName());
}

std::string ModulesTemplate::getReadmeContent() {
  return fmt::format(R"(# {0}

A C++20/C++23 modules-based project created with CPP-Scaffold.

## Features

- Modern C++20/C++23 modules
- {1} build system
- {2} package manager
{3}

## Prerequisites

- C++ compiler with C++20 modules support:
  - GCC 11+ (partial support)
  - Clang 15+ (good support)
  - MSVC 19.28+ (Visual Studio 2019 16.8+)
- {1} build system
{4}

## Building

```bash
{5}
```

## Usage

```cpp
import {0};
import std;

int main() {{
    {0}::Example example(42);
    int value = example.getValue();
    int sum = {0}::add(3, 4);
    std::string version = {0}::getVersion();
    return 0;
}}
```

{6}

## License

This project is licensed under the MIT License - see the LICENSE file for details.
)",
    options_.projectName,
    to_string(options_.buildSystem),
    to_string(options_.packageManager),
    options_.includeTests ? std::string("- Testing with ") + std::string(to_string(options_.testFramework)) : "",
    to_string(options_.packageManager) != "none" ? std::string("- ") + std::string(to_string(options_.packageManager)) + " package manager" : "",
    getBuildInstructions(),
    options_.includeTests ? getTestInstructions() : "");
}

std::string ModulesTemplate::getBuildInstructions() {
  if (to_string(options_.buildSystem) == "cmake") {
    return R"(mkdir build && cd build
cmake .. -DCMAKE_CXX_STANDARD=20
cmake --build .)";
  } else if (to_string(options_.buildSystem) == "meson") {
    return R"(meson setup build
cd build && meson compile)";
  } else if (to_string(options_.buildSystem) == "bazel") {
    return "bazel build //...";
  }
  return std::string("# Build instructions for ") + std::string(to_string(options_.buildSystem));
}

std::string ModulesTemplate::getTestInstructions() {
  if (!options_.includeTests) return "";

  return fmt::format(R"(
## Testing

Run tests with:

```bash
{0}
```
)", getTestRunCommand());
}

std::string ModulesTemplate::getTestRunCommand() {
  if (to_string(options_.buildSystem) == "cmake") {
    return "cd build && ctest";
  } else if (to_string(options_.buildSystem) == "meson") {
    return "cd build && meson test";
  } else if (to_string(options_.buildSystem) == "bazel") {
    return "bazel test //...";
  }
  return std::string("# Test command for ") + std::string(to_string(options_.buildSystem));
}

std::string ModulesTemplate::getCMakeModulesContent() {
  std::string testSection;
  if (options_.includeTests) {
    std::string testFramework = std::string(to_string(options_.testFramework));
    testSection = fmt::format(R"(
# Testing
enable_testing()
find_package({0} REQUIRED)

add_executable(test_{1}
    tests/test_{1}.cpp
)

target_link_libraries(test_{1}
    PRIVATE
    {1}_module
    {0}::{0}
)

add_test(NAME {1}_tests COMMAND test_{1})
)", testFramework == "gtest" ? "GTest" :
   testFramework == "catch2" ? "Catch2" : "doctest", getModuleName());
  }

  return fmt::format(R"(cmake_minimum_required(VERSION 3.28)
project({0} LANGUAGES CXX)

# C++{2} for modules support
set(CMAKE_CXX_STANDARD {2})
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Enable modules support
set(CMAKE_CXX_SCAN_FOR_MODULES ON)

# Create module library
add_library({0}_module)
target_sources({0}_module
    PUBLIC
        FILE_SET CXX_MODULES FILES
            modules/{0}.cppm
    PRIVATE
        src/{0}.cpp
)

# Main executable
add_executable({0}
    src/main.cpp
)

target_link_libraries({0}
    PRIVATE
    {0}_module
)

{1}
)", getModuleName(), testSection, getCppStandardFlag());
}

std::string ModulesTemplate::getMesonModulesContent() {
  std::string testSection;
  if (options_.includeTests) {
    std::string testFramework = std::string(to_string(options_.testFramework));
    testSection = fmt::format(R"(
# Testing
{0}_dep = dependency('{0}')

test_exe = executable('test_{1}',
    'tests/test_{1}.cpp',
    dependencies: [{0}_dep],
    link_with: {1}_lib
)

test('{1}_tests', test_exe)
)", testFramework, getModuleName());
  }

  return fmt::format(R"(project('{0}', 'cpp',
    version: '1.0.0',
    default_options: [
        'cpp_std=c++20',
        'warning_level=3',
        'buildtype=release'
    ]
)

# Module library
{0}_lib = library('{0}',
    'modules/{0}.cppm',
    'src/{0}.cpp',
    install: true
)

# Main executable
executable('{0}',
    'src/main.cpp',
    link_with: {0}_lib,
    install: true
)

{1}
)", getModuleName(), testSection);
}

std::string ModulesTemplate::getBazelModulesContent() {
  std::string testSection;
  if (options_.includeTests) {
    std::string testFramework = std::string(to_string(options_.testFramework));
    testSection = fmt::format(R"(
cc_test(
    name = "{0}_test",
    srcs = ["tests/test_{0}.cpp"],
    deps = [
        ":{0}_module",
        "@{1}//:main",
    ],
)
)", getModuleName(), testFramework);
  }

  return fmt::format(R"(load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library", "cc_test")

cc_library(
    name = "{0}_module",
    srcs = [
        "modules/{0}.cppm",
        "src/{0}.cpp",
    ],
    copts = ["-std=c++20"],
    visibility = ["//visibility:public"],
)

cc_binary(
    name = "{0}",
    srcs = ["src/main.cpp"],
    deps = [":{0}_module"],
    copts = ["-std=c++20"],
)

{1}
)", getModuleName(), testSection);
}

std::string ModulesTemplate::getVcpkgJsonContent() {
  return fmt::format(R"({{
  "name": "{0}",
  "version": "1.0.0",
  "description": "A C++20 modules project",
  "dependencies": [
    "fmt"
  ]
}}
)", getModuleName());
}

std::string ModulesTemplate::getConanfileContent() {
  return R"([requires]
fmt/[>=8.0.0]

[generators]
CMakeDeps
CMakeToolchain

[options]

[imports]
)";
}

std::string ModulesTemplate::getGTestModulesContent() {
  return fmt::format(R"(// Test file for {0} module
// Generated by CPP-Scaffold

import {0};
import std;
#include <gtest/gtest.h>

class {0}Test : public ::testing::Test {{
protected:
    void SetUp() override {{
        // Setup code here
    }}

    void TearDown() override {{
        // Cleanup code here
    }}
}};

TEST_F({0}Test, ExampleConstructor) {{
    {0}::Example example(42);
    EXPECT_EQ(example.getValue(), 42);
}}

TEST_F({0}Test, ExampleSetValue) {{
    {0}::Example example(0);
    example.setValue(100);
    EXPECT_EQ(example.getValue(), 100);
}}

TEST_F({0}Test, AddFunction) {{
    int result = {0}::add(3, 4);
    EXPECT_EQ(result, 7);
}}

TEST_F({0}Test, GetVersion) {{
    std::string version = {0}::getVersion();
    EXPECT_FALSE(version.empty());
    EXPECT_EQ(version, "1.0.0");
}}

int main(int argc, char **argv) {{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}}
)", getModuleName());
}

std::string ModulesTemplate::getCatch2ModulesContent() {
  return fmt::format(R"(// Test file for {0} module
// Generated by CPP-Scaffold

import {0};
import std;
#include <catch2/catch_test_macros.hpp>

TEST_CASE("{0} Example class", "[example]") {{
    SECTION("Constructor sets value correctly") {{
        {0}::Example example(42);
        REQUIRE(example.getValue() == 42);
    }}

    SECTION("setValue changes value") {{
        {0}::Example example(0);
        example.setValue(100);
        REQUIRE(example.getValue() == 100);
    }}
}}

TEST_CASE("{0} utility functions", "[utilities]") {{
    SECTION("add function works correctly") {{
        REQUIRE({0}::add(3, 4) == 7);
        REQUIRE({0}::add(-1, 1) == 0);
        REQUIRE({0}::add(0, 0) == 0);
    }}

    SECTION("getVersion returns valid version") {{
        std::string version = {0}::getVersion();
        REQUIRE_FALSE(version.empty());
        REQUIRE(version == "1.0.0");
    }}
}}
)", getModuleName());
}

std::string ModulesTemplate::getDocTestModulesContent() {
  return fmt::format(R"(// Test file for {0} module
// Generated by CPP-Scaffold

import {0};
import std;
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

TEST_CASE("testing {0}::Example class") {{
    {0}::Example example(42);

    SUBCASE("constructor sets value") {{
        CHECK(example.getValue() == 42);
    }}

    SUBCASE("setValue changes value") {{
        example.setValue(100);
        CHECK(example.getValue() == 100);
    }}
}}

TEST_CASE("testing {0} utility functions") {{
    SUBCASE("add function") {{
        CHECK({0}::add(3, 4) == 7);
        CHECK({0}::add(-1, 1) == 0);
        CHECK({0}::add(0, 0) == 0);
    }}

    SUBCASE("getVersion function") {{
        std::string version = {0}::getVersion();
        CHECK_FALSE(version.empty());
        CHECK(version == "1.0.0");
    }}
}}
)", getModuleName());
}

#include "library_template.h"
#include "../utils/file_utils.h"
#include "../utils/string_utils.h"
#include <fmt/core.h>
#include <iostream>
#include <spdlog/spdlog.h>

using namespace utils;

LibraryTemplate::LibraryTemplate(const CliOptions &options)
    : TemplateBase(options) {}

bool LibraryTemplate::create() {
  std::string projectPath = options_.projectName;

  // Check if project directory already exists
  if (FileUtils::directoryExists(projectPath)) {
    spdlog::error("Directory '{}' already exists.", projectPath);
    return false;
  }

  spdlog::info("🚀 Creating library project...");

  // Create basic structure
  if (!createProjectStructure()) {
    spdlog::error("Failed to create project structure");
    return false;
  }
  spdlog::info("✅ Project structure created");

  // Create build system
  if (!createBuildSystem()) {
    spdlog::error("Failed to configure build system");
    return false;
  }
  spdlog::info("✅ Build system configured");

  // Setup package manager
  if (!setupPackageManager()) {
    spdlog::error("Failed to setup package manager");
    return false;
  }
  spdlog::info("✅ Package manager setup");

  // Setup test framework
  if (options_.includeTests) {
    if (!setupTestFramework()) {
      spdlog::error("Failed to setup test framework");
      return false;
    }
    spdlog::info("✅ Test framework configured");
  }

  // Initialize Git
  if (options_.initGit) {
    if (!initializeGit(projectPath)) {
      spdlog::error("Failed to initialize Git repository");
      return false;
    }
    spdlog::info("✅ Git repository initialized");
  }

  spdlog::info("\nYour library project is ready!\n");

  // Print usage instructions
  std::cout << fmt::format("cd {}\n", options_.projectName);

  if (options_.buildSystem == "cmake") {
    std::cout << "mkdir build && cd build\n";
    std::cout << "cmake ..\n";
    std::cout << "make\n";
  } else if (options_.buildSystem == "meson") {
    std::cout << "meson setup build\n";
    std::cout << "cd build\n";
    std::cout << "meson compile\n";
  } else if (options_.buildSystem == "bazel") {
    std::cout << "bazel build //...\n";
  }

  std::cout << "\nHappy coding! 🎉\n";

  return true;
}

bool LibraryTemplate::createProjectStructure() {
  std::string projectPath = options_.projectName;

  // Create main directory
  if (!FileUtils::createDirectory(projectPath)) {
    return false;
  }

  // Create src directory
  std::string srcPath = FileUtils::combinePath(projectPath, "src");
  if (!FileUtils::createDirectory(srcPath)) {
    return false;
  }

  // Create include directory
  std::string includePath = FileUtils::combinePath(projectPath, "include");
  if (!FileUtils::createDirectory(includePath)) {
    return false;
  }

  // Create include/project directory
  std::string includeProjectPath =
      FileUtils::combinePath(includePath, options_.projectName);
  if (!FileUtils::createDirectory(includeProjectPath)) {
    return false;
  }

  // Create example directory
  std::string examplePath = FileUtils::combinePath(projectPath, "example");
  if (!FileUtils::createDirectory(examplePath)) {
    return false;
  }

  // Write source file
  if (!FileUtils::writeToFile(
          FileUtils::combinePath(srcPath,
                                 fmt::format("{}.cpp", options_.projectName)),
          getLibraryCppContent())) {
    return false;
  }

  // Write header file
  if (!FileUtils::writeToFile(
          FileUtils::combinePath(includeProjectPath,
                                 fmt::format("{}.h", options_.projectName)),
          getLibraryHeaderContent())) {
    return false;
  }

  // Write example file
  if (!FileUtils::writeToFile(
          FileUtils::combinePath(examplePath, "example.cpp"),
          getExampleContent())) {
    return false;
  }

  // Create README.md
  if (!FileUtils::writeToFile(FileUtils::combinePath(projectPath, "README.md"),
                              getReadmeContent())) {
    return false;
  }

  return true;
}

bool LibraryTemplate::createBuildSystem() {
  std::string projectPath = options_.projectName;

  if (options_.buildSystem == "cmake") {
    // Create CMakeLists.txt
    if (!FileUtils::writeToFile(
            FileUtils::combinePath(projectPath, "CMakeLists.txt"),
            getCMakeContent())) {
      return false;
    }

    // Create cmake directory for config files
    std::string cmakePath = FileUtils::combinePath(projectPath, "cmake");
    if (!FileUtils::createDirectory(cmakePath)) {
      return false;
    }

    // Create config template
    std::string configTemplate =
        fmt::format(R"(
@PACKAGE_INIT@

include("${{CMAKE_CURRENT_LIST_DIR}}/{}Targets.cmake")

check_required_components({})
)",
                    options_.projectName, options_.projectName);

    if (!FileUtils::writeToFile(
            FileUtils::combinePath(
                cmakePath,
                fmt::format("{}Config.cmake.in", options_.projectName)),
            configTemplate)) {
      return false;
    }

  } else if (options_.buildSystem == "meson") {
    // Create meson.build
    if (!FileUtils::writeToFile(
            FileUtils::combinePath(projectPath, "meson.build"),
            getMesonContent())) {
      return false;
    }
  } else if (options_.buildSystem == "bazel") {
    // Create WORKSPACE and BUILD files
    if (!FileUtils::writeToFile(
            FileUtils::combinePath(projectPath, "WORKSPACE"),
            fmt::format("workspace(name = \"{}\")\n", options_.projectName))) {
      return false;
    }

    if (!FileUtils::writeToFile(FileUtils::combinePath(projectPath, "BUILD"),
                                getBazelContent())) {
      return false;
    }
  }

  return true;
}

bool LibraryTemplate::setupPackageManager() {
  std::string projectPath = options_.projectName;

  if (options_.packageManager == "vcpkg") {
    // Create vcpkg.json
    if (!FileUtils::writeToFile(
            FileUtils::combinePath(projectPath, "vcpkg.json"),
            getVcpkgJsonContent())) {
      return false;
    }
  } else if (options_.packageManager == "conan") {
    // Create conanfile.txt
    if (!FileUtils::writeToFile(
            FileUtils::combinePath(projectPath, "conanfile.txt"),
            getConanfileContent())) {
      return false;
    }
  }

  return true;
}

bool LibraryTemplate::setupTestFramework() {
  if (!options_.includeTests) {
    return true;
  }

  std::string projectPath = options_.projectName;
  std::string testsPath = FileUtils::combinePath(projectPath, "tests");

  if (!FileUtils::createDirectory(testsPath)) {
    return false;
  }

  std::string testContent;
  if (options_.testFramework == "gtest") {
    testContent = getGTestContent();
  } else if (options_.testFramework == "catch2") {
    testContent = getCatch2Content();
  } else if (options_.testFramework == "doctest") {
    testContent = getDocTestContent();
  }

  std::string testFileName = fmt::format("test_{}.cpp", options_.projectName);
  if (!FileUtils::writeToFile(FileUtils::combinePath(testsPath, testFileName),
                              testContent)) {
    return false;
  }

  // Update build system to include tests
  if (options_.buildSystem == "cmake") {
    std::string testCmakeContent;
    if (options_.testFramework == "gtest") {
      testCmakeContent = fmt::format(R"(
find_package(GTest REQUIRED)
add_executable(${{PROJECT_NAME}}_tests {})
target_link_libraries(${{PROJECT_NAME}}_tests PRIVATE 
    ${{PROJECT_NAME}}
    GTest::GTest 
    GTest::Main
)
add_test(NAME ${{PROJECT_NAME}}_tests COMMAND ${{PROJECT_NAME}}_tests)
)",
                                     testFileName);
    } else if (options_.testFramework == "catch2") {
      testCmakeContent = fmt::format(R"(
find_package(Catch2 REQUIRED)
add_executable(${{PROJECT_NAME}}_tests {})
target_link_libraries(${{PROJECT_NAME}}_tests PRIVATE 
    ${{PROJECT_NAME}}
    Catch2::Catch2
)
add_test(NAME ${{PROJECT_NAME}}_tests COMMAND ${{PROJECT_NAME}}_tests)
)",
                                     testFileName);
    } else if (options_.testFramework == "doctest") {
      testCmakeContent = fmt::format(R"(
find_package(doctest REQUIRED)
add_executable(${{PROJECT_NAME}}_tests {})
target_link_libraries(${{PROJECT_NAME}}_tests PRIVATE 
    ${{PROJECT_NAME}}
    doctest::doctest
)
add_test(NAME ${{PROJECT_NAME}}_tests COMMAND ${{PROJECT_NAME}}_tests)
)",
                                     testFileName);
    }

    if (!FileUtils::writeToFile(
            FileUtils::combinePath(testsPath, "CMakeLists.txt"),
            testCmakeContent)) {
      return false;
    }
  }

  return true;
}

std::string LibraryTemplate::getLibraryCppContent() {
  // Get uppercase version of project name (for header guards)
  std::string projectNameUpper = StringUtils::toUpper(options_.projectName);

  return fmt::format(R"(#include "{0}/{0}.h"

namespace {0} {{

// Example class implementation
Example::Example(int value) : value_(value) {{}}

int Example::getValue() const {{
    return value_;
}}

void Example::setValue(int value) {{
    value_ = value;
}}

// Example function implementation    
int add(int a, int b) {{
    return a + b;
}}

}} // namespace {0})",
                     options_.projectName);
}

std::string LibraryTemplate::getLibraryHeaderContent() {
  // Get uppercase version of project name (for header guards)
  std::string projectNameUpper = StringUtils::toUpper(options_.projectName);

  return fmt::format(R"(#ifndef {1}_H
#define {1}_H

#include <string>

namespace {0} {{

// Example class
class Example {{
public:
    explicit Example(int value = 0);
    
    int getValue() const;
    void setValue(int value);
    
private:
    int value_;
}};

// Example function
int add(int a, int b);

}} // namespace {0}

#endif // {1}_H
)",
                     options_.projectName, projectNameUpper);
}

std::string LibraryTemplate::getReadmeContent() {
  std::string packageManagerInfo =
      options_.packageManager != "none"
          ? fmt::format("- {} package manager\n", options_.packageManager)
          : "";

  std::string buildInstructions;
  if (options_.buildSystem == "cmake") {
    buildInstructions = R"(mkdir build && cd build
cmake ..
make
make install)";
  } else if (options_.buildSystem == "meson") {
    buildInstructions = R"(meson setup build
cd build
meson compile
meson install)";
  } else {
    buildInstructions = R"(bazel build //...
bazel run //:install)";
  }

  std::string testInstructions;
  if (options_.includeTests) {
    if (options_.buildSystem == "cmake") {
      testInstructions = R"(cd build
ctest)";
    } else if (options_.buildSystem == "meson") {
      testInstructions = R"(cd build
meson test)";
    } else {
      testInstructions = R"(bazel test //...)";
    }

    testInstructions = fmt::format(R"(## Running Tests

```bash
{}
```
)",
                                   testInstructions);
  }

  return fmt::format(R"(# {0}

A C++ library created with CPP-Scaffold.

## Features

- Feature 1
- Feature 2
- Feature 3

## Installation

### Prerequisites

- C++ compiler with C++17 support
- {1} build system
{2}
### Build and Install

```bash
{3}
```

## Usage

```cpp
#include <{0}/{0}.h>

int main() {{
    // Create an Example object
    {0}::Example example(42);
    
    // Use the example object
    int value = example.getValue();
    
    // Use a free function
    int sum = {0}::add(3, 4);
    
    return 0;
}}
```

{4}
## License

This project is licensed under the MIT License - see the LICENSE file for details.
)",
                     options_.projectName, options_.buildSystem,
                     packageManagerInfo, buildInstructions, testInstructions);
}

std::string LibraryTemplate::getCMakeContent() {
  std::string testSection = options_.includeTests ? R"(
# Tests
if(BUILD_TESTING)
  enable_testing()
  add_subdirectory(tests)
endif()
)"
                                                  : "";

  std::string vcpkgSection = options_.packageManager == "vcpkg" ? R"(
# vcpkg integration
if(DEFINED ENV{VCPKG_ROOT})
  set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
endif()
)"
                                                                : "";

  return fmt::format(R"(cmake_minimum_required(VERSION 3.14)
project({0} VERSION 0.1.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Options
option(BUILD_TESTING "Build tests" {1})
option(BUILD_EXAMPLES "Build examples" ON)
option(BUILD_SHARED_LIBS "Build as shared library" OFF)

# Source files
set(SOURCES
  src/{0}.cpp
)

# Library target
add_library(${{PROJECT_NAME}} ${{SOURCES}})
target_include_directories(${{PROJECT_NAME}} 
  PUBLIC 
    $<BUILD_INTERFACE:${{CMAKE_CURRENT_SOURCE_DIR}}/include>
    $<INSTALL_INTERFACE:include>
)

# If building shared library, set export macro
if(BUILD_SHARED_LIBS)
  include(GenerateExportHeader)
  generate_export_header(${{PROJECT_NAME}}
    EXPORT_FILE_NAME include/{0}/export.h
  )
endif()

# Examples
if(BUILD_EXAMPLES)
  add_executable(example example/example.cpp)
  target_link_libraries(example PRIVATE ${{PROJECT_NAME}})
endif()

# Add compiler warnings
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  target_compile_options(${{PROJECT_NAME}} PRIVATE -Wall -Wextra -Wpedantic -Werror)
elseif(MSVC)
  target_compile_options(${{PROJECT_NAME}} PRIVATE /W4 /WX)
endif()

# Installation
include(GNUInstallDirs)
install(TARGETS ${{PROJECT_NAME}}
  EXPORT ${{PROJECT_NAME}}Targets
  LIBRARY DESTINATION ${{CMAKE_INSTALL_LIBDIR}}
  ARCHIVE DESTINATION ${{CMAKE_INSTALL_LIBDIR}}
  RUNTIME DESTINATION ${{CMAKE_INSTALL_BINDIR}}
  INCLUDES DESTINATION ${{CMAKE_INSTALL_INCLUDEDIR}}
)

install(DIRECTORY include/
  DESTINATION ${{CMAKE_INSTALL_INCLUDEDIR}}
  FILES_MATCHING PATTERN "*.h"
)

install(EXPORT ${{PROJECT_NAME}}Targets
  FILE ${{PROJECT_NAME}}Targets.cmake
  NAMESPACE ${{PROJECT_NAME}}::
  DESTINATION lib/cmake/${{PROJECT_NAME}}
)

# Generate and install package config file
include(CMakePackageConfigHelpers)
configure_package_config_file(
  ${{CMAKE_CURRENT_SOURCE_DIR}}/cmake/{0}Config.cmake.in
  ${{CMAKE_CURRENT_BINARY_DIR}}/{0}Config.cmake
  INSTALL_DESTINATION lib/cmake/${{PROJECT_NAME}}
)

write_basic_package_version_file(
  ${{CMAKE_CURRENT_BINARY_DIR}}/{0}ConfigVersion.cmake
  VERSION ${{PROJECT_VERSION}}
  COMPATIBILITY SameMajorVersion
)

install(FILES
  ${{CMAKE_CURRENT_BINARY_DIR}}/{0}Config.cmake
  ${{CMAKE_CURRENT_BINARY_DIR}}/{0}ConfigVersion.cmake
  DESTINATION lib/cmake/${{PROJECT_NAME}}
)
{2}{3})",
                     options_.projectName, options_.includeTests ? "ON" : "OFF",
                     vcpkgSection, testSection);
}

std::string LibraryTemplate::getMesonContent() {
  std::string testDeps = "";
  if (options_.includeTests) {
    if (options_.testFramework == "gtest") {
      testDeps = "gtest_dep = dependency('gtest', main : true)\n";
    } else if (options_.testFramework == "catch2") {
      testDeps = "catch2_dep = dependency('catch2')\n";
    } else {
      testDeps = "doctest_dep = dependency('doctest')\n";
    }
  }

  std::string testSection;
  if (options_.includeTests) {
    std::string testDep = options_.testFramework == "gtest"    ? "gtest_dep"
                          : options_.testFramework == "catch2" ? "catch2_dep"
                                                               : "doctest_dep";

    testSection = fmt::format(R"(
test_exe = executable('test_{0}',
  ['tests/test_{0}.cpp'],
  include_directories : inc_dirs,
  link_with : {0}_lib,
  dependencies : [{1}]
)

test('{0}_tests', test_exe)
)",
                              options_.projectName, testDep);
  } else {
    testSection = "# No tests configured";
  }

  return fmt::format(R"(project('{0}', 'cpp',
  version : '0.1.0',
  default_options : [
    'warning_level=3',
    'cpp_std=c++17',
    'default_library=static',
    'buildtype=release'
  ]
)

# Compiler setup
cpp = meson.get_compiler('cpp')
if cpp.get_id() == 'gcc' or cpp.get_id() == 'clang'
  add_project_arguments('-Wall', '-Wextra', '-Wpedantic', language : 'cpp')
elif cpp.get_id() == 'msvc'
  add_project_arguments('/W4', language : 'cpp')
endif

# Dependencies
{1}

# Include directories
inc_dirs = include_directories('include')

# Library
{0}_lib = library('{0}',
  ['src/{0}.cpp'],
  include_directories : inc_dirs,
  install : true,
  version : meson.project_version()
)

# Declare dependency for other subprojects
{0}_dep = declare_dependency(
  include_directories : inc_dirs,
  link_with : {0}_lib
)

# Install headers
install_headers('include/{0}/{0}.h',
  subdir : '{0}'
)

# Example
executable('example',
  ['example/example.cpp'],
  include_directories : inc_dirs,
  link_with : {0}_lib,
  install : false
)

# PKG config file
pkg = import('pkgconfig')
pkg.generate(
  name : '{0}',
  filebase : '{0}',
  description : 'A C++ library created with CPP-Scaffold',
  version : meson.project_version(),
  libraries : {0}_lib
)

# Tests
{2})",
                     options_.projectName, testDeps, testSection);
}

std::string LibraryTemplate::getBazelContent() {
  std::string testSection = "";
  if (options_.includeTests) {
    std::string testFrameworkDep;
    if (options_.testFramework == "gtest") {
      testFrameworkDep = "com_google_googletest//:gtest_main";
    } else if (options_.testFramework == "catch2") {
      testFrameworkDep = "catch2//:catch2";
    } else {
      testFrameworkDep = "doctest//:doctest";
    }

    testSection = fmt::format(R"(
cc_test(
    name = "{0}_test",
    srcs = ["tests/test_{0}.cpp"],
    deps = [
        ":{0}",
        "@{1}",
    ],
    copts = select({
        "@platforms//os:windows": ["/W4", "/WX"],
        "//conditions:default": ["-Wall", "-Wextra", "-Wpedantic"],
    }),
)

# Install rule
genrule(
    name = "install",
    srcs = ["include/{0}/{0}.h", ":{0}"],
    outs = ["install.log"],
    cmd = "echo 'Installation completed.' > $@",
    local = 1,
))",
                              options_.projectName, testFrameworkDep);
  } else {
    testSection = R"(
# Install rule
genrule(
    name = "install",
    srcs = ["include/{0}/{0}.h", ":{0}"],
    outs = ["install.log"],
    cmd = "echo 'Installation completed.' > $@",
    local = 1,
))";
  }

  return fmt::format(
      R"(load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library", "cc_test")

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "{0}",
    srcs = ["src/{0}.cpp"],
    hdrs = ["include/{0}/{0}.h"],
    includes = ["include"],
    visibility = ["//visibility:public"],
    copts = select({
        "@platforms//os:windows": ["/W4", "/WX"],
        "//conditions:default": ["-Wall", "-Wextra", "-Wpedantic"],
    }),
)

cc_binary(
    name = "example",
    srcs = ["example/example.cpp"],
    deps = [":{0}"],
    copts = select({
        "@platforms//os:windows": ["/W4", "/WX"],
        "//conditions:default": ["-Wall", "-Wextra", "-Wpedantic"],
    }),
){1})",
      options_.projectName, fmt::format(testSection, options_.projectName));
}

std::string LibraryTemplate::getVcpkgJsonContent() {
  std::string testDependency = "";
  if (options_.includeTests) {
    std::string testFramework = options_.testFramework == "gtest" ? "gtest"
                                : options_.testFramework == "catch2"
                                    ? "catch2"
                                    : "doctest";
    std::string features = options_.testFramework == "gtest" ? "gmock" : "";

    if (!features.empty()) {
      testDependency = fmt::format(R"(    {{
      "name": "{}",
      "features": ["{}"]
    }},)",
                                   testFramework, features);
    } else {
      testDependency = fmt::format(R"(    {{
      "name": "{}"
    }},)",
                                   testFramework);
    }
  }

  return fmt::format(R"({{
  "name": "{0}",
  "version": "0.1.0",
  "description": "A C++ library created with CPP-Scaffold",
  "homepage": "https://github.com/yourusername/{0}",
  "dependencies": [
{1}
  ]
}}
)",
                     options_.projectName, testDependency);
}

std::string LibraryTemplate::getConanfileContent() {
  std::string testRequirement = "";
  if (options_.includeTests) {
    if (options_.testFramework == "gtest") {
      testRequirement = "gtest/1.12.1";
    } else if (options_.testFramework == "catch2") {
      testRequirement = "catch2/3.1.0";
    } else {
      testRequirement = "doctest/2.4.9";
    }
  }

  std::string generator = options_.buildSystem == "cmake" ? "cmake" : "";

  return fmt::format(R"([requires]
{0}

[generators]
{1}

[options]
# Add library-specific options here

[imports]
# Add any binary imports here
)",
                     testRequirement, generator);
}

std::string LibraryTemplate::getExampleContent() {
  return fmt::format(R"(#include <iostream>
#include "{0}/{0}.h"

int main() {{
    // Create an Example object
    {0}::Example example(42);
    
    // Get value
    int value = example.getValue();
    std::cout << "Example value: " << value << std::endl;
    
    // Modify value
    example.setValue(100);
    std::cout << "New value: " << example.getValue() << std::endl;
    
    // Use free function
    int result = {0}::add(3, 4);
    std::cout << "3 + 4 = " << result << std::endl;
    
    return 0;
}}
)",
                     options_.projectName);
}

std::string LibraryTemplate::getGTestContent() {
  return fmt::format(R"(#include <gtest/gtest.h>
#include "{0}/{0}.h"

// Test Example class
TEST(ExampleTest, Constructor) {{
    {0}::Example example;
    EXPECT_EQ(example.getValue(), 0);
    
    {0}::Example example2(42);
    EXPECT_EQ(example2.getValue(), 42);
}}

TEST(ExampleTest, GetSetValue) {{
    {0}::Example example;
    example.setValue(100);
    EXPECT_EQ(example.getValue(), 100);
}}

// Test free functions
TEST(FunctionTest, Add) {{
    EXPECT_EQ({0}::add(3, 4), 7);
    EXPECT_EQ({0}::add(-1, 1), 0);
    EXPECT_EQ({0}::add(0, 0), 0);
}}

int main(int argc, char** argv) {{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}}
)",
                     options_.projectName);
}

std::string LibraryTemplate::getCatch2Content() {
  return fmt::format(R"(#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include "{0}/{0}.h"

TEST_CASE("Example class tests", "[Example]") {{
    SECTION("Constructor") {{
        {0}::Example example;
        REQUIRE(example.getValue() == 0);
        
        {0}::Example example2(42);
        REQUIRE(example2.getValue() == 42);
    }}
    
    SECTION("Get and set value") {{
        {0}::Example example;
        example.setValue(100);
        REQUIRE(example.getValue() == 100);
    }}
}}

TEST_CASE("Free function tests", "[Functions]") {{
    SECTION("add function") {{
        REQUIRE({0}::add(3, 4) == 7);
        REQUIRE({0}::add(-1, 1) == 0);
        REQUIRE({0}::add(0, 0) == 0);
    }}
}}
)",
                     options_.projectName);
}

std::string LibraryTemplate::getDocTestContent() {
  return fmt::format(R"(#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>
#include "{0}/{0}.h"

TEST_CASE("Example class tests") {{
    SUBCASE("Constructor") {{
        {0}::Example example;
        CHECK(example.getValue() == 0);
        
        {0}::Example example2(42);
        CHECK(example2.getValue() == 42);
    }}
    
    SUBCASE("Get and set value") {{
        {0}::Example example;
        example.setValue(100);
        CHECK(example.getValue() == 100);
    }}
}}

TEST_CASE("Free function tests") {{
    SUBCASE("add function") {{
        CHECK({0}::add(3, 4) == 7);
        CHECK({0}::add(-1, 1) == 0);
        CHECK({0}::add(0, 0) == 0);
    }}
}}
)",
                     options_.projectName);
}
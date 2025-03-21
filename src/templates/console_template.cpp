#include "console_template.h"

#include "../utils/file_utils.h"

#include <fmt/core.h>
#include <iostream>
#include <spdlog/spdlog.h>

using namespace utils;

ConsoleTemplate::ConsoleTemplate(const CliOptions &options)
    : TemplateBase(options) {}

bool ConsoleTemplate::create() {
  std::string projectPath = options_.projectName;

  // Check if project directory already exists
  if (FileUtils::directoryExists(projectPath)) {
    spdlog::error("Directory '{}' already exists.", projectPath);
    return false;
  }

  spdlog::info("🚀 Creating project...");

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

  spdlog::info("\nYour project is ready!\n");

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

bool ConsoleTemplate::createProjectStructure() {
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

  std::string includeProjectPath =
      FileUtils::combinePath(includePath, options_.projectName);
  if (!FileUtils::createDirectory(includeProjectPath)) {
    return false;
  }

  // Write main.cpp
  if (!FileUtils::writeToFile(FileUtils::combinePath(srcPath, "main.cpp"),
                              getMainCppContent())) {
    return false;
  }

  // Create README.md
  if (!FileUtils::writeToFile(FileUtils::combinePath(projectPath, "README.md"),
                              getReadmeContent())) {
    return false;
  }

  return true;
}

bool ConsoleTemplate::createBuildSystem() {
  std::string projectPath = options_.projectName;

  if (options_.buildSystem == "cmake") {
    // Create CMakeLists.txt
    if (!FileUtils::writeToFile(
            FileUtils::combinePath(projectPath, "CMakeLists.txt"),
            getCMakeContent())) {
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

bool ConsoleTemplate::setupPackageManager() {
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

bool ConsoleTemplate::setupTestFramework() {
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

  if (!FileUtils::writeToFile(
          FileUtils::combinePath(testsPath, "test_main.cpp"), testContent)) {
    return false;
  }

  // Update build system to include tests
  if (options_.buildSystem == "cmake") {
    std::string cmakePath =
        FileUtils::combinePath(projectPath, "CMakeLists.txt");
    std::string cmakeContent = FileUtils::readFromFile(cmakePath);

    // Add test configuration
    const char *testConfig = R"(
# Tests
if(BUILD_TESTING)
    enable_testing()
    add_subdirectory(tests)
endif()
)";
    cmakeContent = fmt::format("{}{}", cmakeContent, testConfig);

    if (!FileUtils::writeToFile(cmakePath, cmakeContent)) {
      return false;
    }

    // Create tests/CMakeLists.txt
    std::string testCmakeContent;
    if (options_.testFramework == "gtest") {
      testCmakeContent = R"(
find_package(GTest REQUIRED)
add_executable(${PROJECT_NAME}_tests test_main.cpp)
target_link_libraries(${PROJECT_NAME}_tests PRIVATE 
    ${PROJECT_NAME}_lib
    GTest::GTest 
    GTest::Main
)
add_test(NAME ${PROJECT_NAME}_tests COMMAND ${PROJECT_NAME}_tests)
)";
    } else if (options_.testFramework == "catch2") {
      testCmakeContent = R"(
find_package(Catch2 REQUIRED)
add_executable(${PROJECT_NAME}_tests test_main.cpp)
target_link_libraries(${PROJECT_NAME}_tests PRIVATE 
    ${PROJECT_NAME}_lib
    Catch2::Catch2
)
add_test(NAME ${PROJECT_NAME}_tests COMMAND ${PROJECT_NAME}_tests)
)";
    } else if (options_.testFramework == "doctest") {
      testCmakeContent = R"(
find_package(doctest REQUIRED)
add_executable(${PROJECT_NAME}_tests test_main.cpp)
target_link_libraries(${PROJECT_NAME}_tests PRIVATE 
    ${PROJECT_NAME}_lib
    doctest::doctest
)
add_test(NAME ${PROJECT_NAME}_tests COMMAND ${PROJECT_NAME}_tests)
)";
    }

    if (!FileUtils::writeToFile(
            FileUtils::combinePath(testsPath, "CMakeLists.txt"),
            testCmakeContent)) {
      return false;
    }
  }

  return true;
}

std::string ConsoleTemplate::getMainCppContent() {
  return fmt::format(R"(#include <iostream>
#include <string>

int main(int argc, char* argv[]) {{
    std::cout << "Hello from {}!" << std::endl;
    return 0;
}}
)",
                     options_.projectName);
}

std::string ConsoleTemplate::getReadmeContent() {
  std::string packageManagerInfo =
      options_.packageManager != "none"
          ? fmt::format("- {} package manager\n", options_.packageManager)
          : "";

  std::string buildInstructions;
  if (options_.buildSystem == "cmake") {
    buildInstructions = R"(mkdir build && cd build
cmake ..
make)";
  } else if (options_.buildSystem == "meson") {
    buildInstructions = R"(meson setup build
cd build
meson compile)";
  } else {
    buildInstructions = R"(bazel build //...)";
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

    testInstructions = fmt::format(R"(### Running Tests

{}
)",
                                   testInstructions);
  }

  return fmt::format(R"(# {}
A C++ console application created with CPP-Scaffold.

## Features

- Feature 1
- Feature 2
- Feature 3

## Building

### Prerequisites

- C++ compiler with C++17 support
- {} build system
{}
### Build Instructions

```bash
{}
```
{}

License
This project is licensed under the MIT License - see the LICENSE file for details.
)",
                     options_.projectName, options_.buildSystem,
                     packageManagerInfo, buildInstructions, testInstructions);
}

std::string ConsoleTemplate::getCMakeContent() {
  std::string vcpkgIntegration = options_.packageManager == "vcpkg" ? R"(
  # vcpkg integration
  if(DEFINED ENV{VCPKG_ROOT})
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
  endif()
  )"
                                                                    : "";

  std::string conanIntegration = options_.packageManager == "conan" ? R"(
  # conan integration
  if(EXISTS "${CMAKE_BINARY_DIR}/conanbuildinfo.cmake")
    include("${CMAKE_BINARY_DIR}/conanbuildinfo.cmake")
    conan_basic_setup(TARGETS)
  endif()
  )"
                                                                    : "";

  std::string testSection = options_.includeTests ? R"(
  # Tests
  option(BUILD_TESTING "Build tests" ON)
  if(BUILD_TESTING)
    enable_testing()
    add_subdirectory(tests)
  endif()
  )"
                                                  : "";

  std::string installSection = R"(
  # Installation
  include(GNUInstallDirs)
  install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  )
  
  # Generate export header if building shared libraries
  include(GenerateExportHeader)
  option(BUILD_SHARED_LIBS "Build shared libraries" OFF)
  if(BUILD_SHARED_LIBS)
    generate_export_header(${PROJECT_NAME}_lib
      EXPORT_MACRO_NAME ${PROJECT_NAME}_API
      EXPORT_FILE_NAME include/${PROJECT_NAME}/export.h
    )
  endif()
  
  # CPack configuration for package generation
  option(BUILD_PACKAGES "Configure for package generation with CPack" OFF)
  if(BUILD_PACKAGES)
    set(CPACK_PACKAGE_VENDOR "Your Organization")
    set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "${PROJECT_NAME} - A C++ application")
    set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
    set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
    set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})
    include(CPack)
  endif()
  )";

  return fmt::format(R"(cmake_minimum_required(VERSION 3.15)
  project({} VERSION 0.1.0 LANGUAGES CXX)
  
  # Set C++ standard
  set(CMAKE_CXX_STANDARD 17)
  set(CMAKE_CXX_STANDARD_REQUIRED ON)
  set(CMAKE_CXX_EXTENSIONS OFF)
  
  # Compile commands for IDE integration
  set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
  
  # Package manager integration
  {}{}
  
  # Dependencies
  find_package(fmt REQUIRED)
  
  # Source files
  set(SOURCES
    src/main.cpp
    # Add more source files here
  )
  
  # Include directories
  include_directories(
    ${PROJECT_SOURCE_DIR}/include
  )
  
  # Library target (for reuse in tests)
  add_library(${PROJECT_NAME}_lib STATIC ${SOURCES})
  target_include_directories(${PROJECT_NAME}_lib 
    PUBLIC 
      $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
      $<INSTALL_INTERFACE:include>
  )
  target_link_libraries(${PROJECT_NAME}_lib PUBLIC fmt::fmt)
  
  # Main executable
  add_executable(${PROJECT_NAME} ${SOURCES})
  target_link_libraries(${PROJECT_NAME} PRIVATE ${PROJECT_NAME}_lib)
  
  # Add compiler warnings
  include(CheckCXXCompilerFlag)
  if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic -Werror)
  elseif(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4 /WX)
  endif()
  {}{})",
                     options_.projectName, vcpkgIntegration, conanIntegration,
                     testSection, installSection);
}

std::string ConsoleTemplate::getMesonContent() {
  std::string dependencySection;
  std::vector<std::string> dependencies;

  // 添加基本依赖
  dependencies.push_back("fmt_dep = dependency('fmt')");

  // 添加测试依赖
  if (options_.includeTests) {
    if (options_.testFramework == "gtest") {
      dependencies.push_back(
          "gtest_dep = dependency('gtest', required : false)");
      dependencies.push_back("if not gtest_dep.found()");
      dependencies.push_back(
          "  gtest_dep = dependency('GTest', required : true)");
      dependencies.push_back("endif");
      dependencies.push_back(
          "gtest_main_dep = dependency('gtest_main', required : false)");
      dependencies.push_back("if not gtest_main_dep.found()");
      dependencies.push_back(
          "  gtest_main_dep = dependency('GTest::Main', required : false)");
      dependencies.push_back("endif");
    } else if (options_.testFramework == "catch2") {
      dependencies.push_back(
          "catch2_dep = dependency('catch2', required : true)");
    } else {
      dependencies.push_back(
          "doctest_dep = dependency('doctest', required : true)");
    }
  }

  // 合并依赖部分
  dependencySection = fmt::format("{}", fmt::join(dependencies, "\n"));

  // 配置测试部分
  std::string testSection;
  if (options_.includeTests) {
    std::string testDep = options_.testFramework == "gtest"
                              ? "gtest_dep, gtest_main_dep"
                          : options_.testFramework == "catch2" ? "catch2_dep"
                                                               : "doctest_dep";

    testSection = fmt::format(R"(
  # Tests setup
  test_exe = executable(
    '{}_tests',
    ['tests/test_main.cpp'],
    include_directories : inc_dirs,
    dependencies : [{}],
    link_with : {}_lib
  )
  
  test(
    '{}_unit_tests', 
    test_exe,
    protocol : 'stdout',
    timeout : 30
  )
  )",
                              options_.projectName, testDep,
                              options_.projectName, options_.projectName);
  } else {
    testSection = "# No tests configured";
  }

  // 安装配置部分
  std::string installSection = R"(
  # Installation configuration
  install_headers(
    'include/' + meson.project_name(),
    install_dir : get_option('includedir')
  )
  
  pkg_mod = import('pkgconfig')
  pkg_mod.generate(
    name : meson.project_name(),
    description : 'A C++ application',
    version : meson.project_version(),
    libraries : [proj_lib],
    subdirs : meson.project_name()
  )
  )";

  return fmt::format(R"(project(
    '{}',
    'cpp',
    version : '0.1.0',
    default_options : [
      'warning_level=3',
      'cpp_std=c++17',
      'buildtype=release',
      'default_library=static'
    ]
  )
  
  # Compiler setup
  cpp = meson.get_compiler('cpp')
  if cpp.get_id() == 'gcc' or cpp.get_id() == 'clang'
    add_project_arguments('-Werror', '-Wall', '-Wextra', '-Wpedantic', language : 'cpp')
  elif cpp.get_id() == 'msvc'
    add_project_arguments('/W4', '/WX', language : 'cpp')
  endif
  
  # Dependencies
  {}
  
  # Source files
  src_files = [
    'src/main.cpp',
    # Add more source files here
  ]
  
  # Include directories
  inc_dirs = include_directories('include')
  
  # Library target for reuse in tests
  {}_lib = library(
    '{}_lib',
    src_files,
    include_directories : inc_dirs,
    dependencies : [fmt_dep],
    install : true
  )
  
  # Main executable
  {}_exe = executable(
    '{}',
    src_files,
    include_directories : inc_dirs,
    dependencies : [fmt_dep],
    link_with : {}_lib,
    install : true
  )
  
  # Tests
  {}
  {})",
                     options_.projectName, dependencySection,
                     options_.projectName, options_.projectName,
                     options_.projectName, options_.projectName,
                     options_.projectName, testSection, installSection);
}

std::string ConsoleTemplate::getBazelContent() {
  std::string testSection;
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
      name = "{}_test",
      srcs = ["tests/test_main.cpp"],
      deps = [
          ":{}_lib",
          "@{}",
          "@fmt",
      ],
      copts = select({
          "@platforms//os:windows": ["/W4", "/WX"],
          "//conditions:default": ["-Wall", "-Wextra", "-Wpedantic", "-Werror"],
      }),
  )
  )",
                              options_.projectName, options_.projectName,
                              testFrameworkDep);
  } else {
    testSection = "";
  }

  // WORKSPACE 文件内容
  std::string workspaceContent = fmt::format(R"(workspace(name = "{}")
  
  load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")
  
  # fmt dependency
  http_archive(
      name = "fmt",
      url = "https://github.com/fmtlib/fmt/archive/8.1.1.tar.gz",
      sha256 = "3d794d3cf67633b34b2771eb9f073bde87e846e0d395d254df7b211ef1ec7346",
      strip_prefix = "fmt-8.1.1",
      build_file = "@//:third_party/fmt.BUILD",
  )
  )",
                                             options_.projectName);

  // 写入WORKSPACE文件
  FileUtils::writeToFile(
      FileUtils::combinePath(options_.projectName, "WORKSPACE"),
      workspaceContent);

  // 创建fmt的BUILD文件
  std::string fmtBuildContent = R"(cc_library(
      name = "fmt",
      hdrs = glob(["include/fmt/**/*.h"]),
      srcs = glob(["src/*.cc"]),
      includes = ["include"],
      visibility = ["//visibility:public"],
  )
  )";

  // 创建第三方依赖目录和fmt.BUILD文件
  std::string thirdPartyDir =
      FileUtils::combinePath(options_.projectName, "third_party");
  FileUtils::createDirectory(thirdPartyDir);
  FileUtils::writeToFile(FileUtils::combinePath(thirdPartyDir, "fmt.BUILD"),
                         fmtBuildContent);

  return fmt::format(
      R"(load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library", "cc_test")
  
  package(default_visibility = ["//visibility:public"])
  
  cc_library(
      name = "{}_lib",
      srcs = glob(["src/**/*.cpp"]),
      hdrs = glob(["include/**/*.h"]),
      includes = ["include"],
      deps = [
          "@fmt",
      ],
      copts = select({
          "@platforms//os:windows": ["/W4", "/WX"],
          "//conditions:default": ["-Wall", "-Wextra", "-Wpedantic", "-Werror"],
      }),
  )
  
  cc_binary(
      name = "{}",
      srcs = ["src/main.cpp"],
      deps = [":{}_lib"],
      copts = select({
          "@platforms//os:windows": ["/W4", "/WX"],
          "//conditions:default": ["-Wall", "-Wextra", "-Wpedantic", "-Werror"],
      }),
  ){})",
      options_.projectName, options_.projectName, options_.projectName,
      testSection);
}

std::string ConsoleTemplate::getVcpkgJsonContent() {
  std::string testDependency;
  if (options_.includeTests) {
    std::string testFramework = options_.testFramework == "gtest" ? "gtest"
                                : options_.testFramework == "catch2"
                                    ? "catch2"
                                    : "doctest";

    std::string features = options_.testFramework == "gtest" ? "gmock" : "";
    std::string featuresText =
        !features.empty() ? fmt::format(R"("features": ["{}"])", features) : "";

    testDependency = fmt::format(R"( {{
"name": "{}",
{}
}})",
                                 testFramework, featuresText);
  }

  return fmt::format(R"({{
"name": "{}",
"version": "0.1.0",
"dependencies": [
{}
]
}}
)",
                     options_.projectName, testDependency);
}

std::string ConsoleTemplate::getConanfileContent() {
  std::string testRequirement;
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
{}

[generators]
{})",
                     testRequirement, generator);
}

std::string ConsoleTemplate::getGTestContent() {
  return R"(#include <gtest/gtest.h>

// Simple test case example
TEST(ExampleTest, SimpleTest) {
    EXPECT_EQ(1, 1);
    EXPECT_TRUE(true);
}

// Parameterized test example
class ParameterizedTest : public ::testing::TestWithParam<int> {};

TEST_P(ParameterizedTest, IsPositive) {
    EXPECT_GT(GetParam(), 0);
}

INSTANTIATE_TEST_SUITE_P(
    PositiveValues,
    ParameterizedTest,
    ::testing::Values(1, 2, 3, 5, 8)
);

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
)";
}

std::string ConsoleTemplate::getCatch2Content() {
  return R"(#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

TEST_CASE("Basic tests", "[example]") {
    SECTION("Simple assertions") {
        REQUIRE(1 == 1);
        CHECK(true);
    }

    SECTION("Assertions with info") {
        INFO("This is information");
        REQUIRE(2 + 2 == 4);
    }
}

TEST_CASE("Parameterized tests", "[example]") {
    SECTION("Check if value is positive") {
        auto value = GENERATE(1, 2, 3, 5, 8);
        REQUIRE(value > 0);
    }
}
)";
}

std::string ConsoleTemplate::getDocTestContent() {
  return R"(#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

TEST_CASE("Basic tests") {
    CHECK(1 == 1);
    REQUIRE(true);

    SUBCASE("Assertions with message") {
        INFO("This is information");
        CHECK_MESSAGE(2 + 2 == 4, "Addition should work correctly");
    }
}

TEST_CASE("Parameterized tests") {
    int values[] = {1, 2, 3, 5, 8};

    for(int value : values) {
        CAPTURE(value); // Add value to output
        CHECK(value > 0);
    }
}
)";
}

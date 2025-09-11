#include "console_template.h"

#include <spdlog/fmt/fmt.h>
#include <spdlog/spdlog.h>

#include <iostream>

#include "../utils/file_utils.h"
#include "../utils/terminal_utils.h"

using namespace utils;
using namespace cli_enums;

ConsoleTemplate::ConsoleTemplate(const CliOptions& options) : TemplateBase(options) {}

bool ConsoleTemplate::create() {
    std::string projectPath = options_.projectName;

    spdlog::info("🚀 Creating console project: {}", projectPath);

    // Create basic structure with enhanced error handling
    startProgressStep("Project Structure", "Creating directory structure");
    if (!createProjectStructure()) {
        failProgressStep("Failed to create project structure");
        return false;
    }
    completeProgressStep("Project structure created");

    // Create build system with progress tracking
    startProgressStep("Build System", "Configuring build system");
    if (!createBuildSystem()) {
        failProgressStep("Failed to configure build system");
        return false;
    }
    completeProgressStep("Build system configured");

    // Setup package manager with validation
    startProgressStep("Package Manager", "Setting up package manager");
    if (!setupPackageManager()) {
        failProgressStep("Failed to setup package manager");
        return false;
    }
    completeProgressStep("Package manager configured");

    // Setup test framework if requested
    if (options_.includeTests) {
        startProgressStep("Test Framework", "Configuring test framework");
        if (!setupTestFramework()) {
            failProgressStep("Failed to setup test framework");
            return false;
        }
        completeProgressStep("Test framework configured");
    }

    // Initialize Git
    if (options_.initGit) {
        if (!initializeGit(projectPath)) {
            spdlog::error("Failed to initialize Git repository");
            return false;
        }
        spdlog::info("�?Git repository initialized");
    }

    // Setup code style tools if enabled
    if (options_.includeCodeStyleTools) {
        if (!setupCodeStyleConfig(projectPath)) {
            spdlog::error("Failed to setup code style tools");
            return false;
        }
        spdlog::info("�?Code style tools configured");
    }

    spdlog::info("\nYour project is ready!\n");

    // Execute post-creation actions (temporarily disabled)
    // if (!executePostCreationActions()) {
    //   std::cerr << "⚠️ Some post-creation actions failed, but the project was created
    //   successfully.\n";
    // }

    // Print usage instructions
    printUsageGuide();

    std::cout << "\nHappy coding! 🎉\n";

    return true;
}

bool ConsoleTemplate::createProjectStructure() {
    std::filesystem::path projectPath = options_.projectName;

    // Create main directory with enhanced validation
    if (!createDirectoryWithValidation(projectPath)) {
        return false;
    }

    // Create src directory
    std::filesystem::path srcPath = projectPath / "src";
    if (!createDirectoryWithValidation(srcPath)) {
        return false;
    }

    // Create include directory
    std::filesystem::path includePath = projectPath / "include";
    if (!createDirectoryWithValidation(includePath)) {
        return false;
    }

    std::filesystem::path includeProjectPath = includePath / options_.projectName;
    if (!createDirectoryWithValidation(includeProjectPath)) {
        return false;
    }

    // Write main.cpp with enhanced error handling
    if (!createFileWithValidation(srcPath / "main.cpp", getMainCppContent())) {
        return false;
    }

    // Create README.md with enhanced error handling
    if (!createFileWithValidation(projectPath / "README.md", getReadmeContent())) {
        return false;
    }

    return true;
}

bool ConsoleTemplate::createBuildSystem() {
    std::filesystem::path projectPath = options_.projectName;

    if (options_.buildSystem == BuildSystem::CMake) {
        if (!createFileWithValidation(projectPath / "CMakeLists.txt", getCMakeContent())) {
            return false;
        }
    } else if (options_.buildSystem == BuildSystem::Meson) {
        if (!createFileWithValidation(projectPath / "meson.build", getMesonContent())) {
            return false;
        }
    } else if (options_.buildSystem == BuildSystem::Bazel) {
        if (!createFileWithValidation(
                    projectPath / "WORKSPACE",
                    fmt::format("workspace(name = \"{}\")\n", options_.projectName))) {
            return false;
        }

        if (!createFileWithValidation(projectPath / "BUILD", getBazelContent())) {
            return false;
        }
    } else if (options_.buildSystem == BuildSystem::XMake) {
        if (!createFileWithValidation(projectPath / "xmake.lua", getXMakeContent())) {
            return false;
        }
    } else if (options_.buildSystem == BuildSystem::Premake) {
        if (!createFileWithValidation(projectPath / "premake5.lua", getPremakeContent())) {
            return false;
        }
    }

    return true;
}

bool ConsoleTemplate::setupPackageManager() {
    std::string projectPath = options_.projectName;

    if (options_.packageManager == PackageManager::Vcpkg) {
        if (!FileUtils::writeToFile(FileUtils::combinePath(projectPath, "vcpkg.json"),
                                    getVcpkgJsonContent())) {
            return false;
        }
    } else if (options_.packageManager == PackageManager::Conan) {
        if (!FileUtils::writeToFile(FileUtils::combinePath(projectPath, "conanfile.txt"),
                                    getConanfileContent())) {
            return false;
        }
    } else if (options_.packageManager == PackageManager::MSYS2) {
        if (!FileUtils::writeToFile(FileUtils::combinePath(projectPath, "PKGBUILD"),
                                    getMSYS2PKGBUILDContent())) {
            return false;
        }
    }

    return true;
}

bool ConsoleTemplate::setupTestFramework() {
    if (!options_.includeTests) {
        return true;
    }

    // Use the enhanced test framework manager
    auto& testManager = testing::TestFrameworkManager::getInstance();

    // Configure test settings
    testing::TestConfig testConfig;

    // Map CLI test framework to testing framework
    switch (options_.testFramework) {
        case TestFramework::GTest:
            testConfig.framework = testing::TestFramework::GoogleTest;
            break;
        case TestFramework::Catch2:
            testConfig.framework = testing::TestFramework::Catch2;
            break;
        case TestFramework::Doctest:
            testConfig.framework = testing::TestFramework::Doctest;
            break;
        default:
            testConfig.framework = testing::TestFramework::GoogleTest;
            break;
    }

    testConfig.testTypes = {testing::TestType::Unit, testing::TestType::Integration};

    testConfig.generateMocks = false;
    testConfig.generateFixtures = true;
    testConfig.enableCodeCoverage = true;
    testConfig.enableAddressSanitizer = true;

    // Setup the test framework
    std::filesystem::path projectPath(options_.projectName);

    // Convert CLI TestFramework enum to testing::TestFramework enum
    testing::TestFramework testingFramework;
    switch (options_.testFramework) {
        case TestFramework::GTest:
            testingFramework = testing::TestFramework::GoogleTest;
            break;
        case TestFramework::Catch2:
            testingFramework = testing::TestFramework::Catch2;
            break;
        case TestFramework::Doctest:
            testingFramework = testing::TestFramework::Doctest;
            break;
        case TestFramework::Boost:
            testingFramework = testing::TestFramework::Boost_Test;
            break;
        default:
            testingFramework = testing::TestFramework::GoogleTest;
            break;
    }

    testConfig.framework = testingFramework;

    if (!testManager.setupFramework(testConfig.framework, projectPath, testConfig)) {
        spdlog::warn("Failed to setup enhanced test framework, using fallback");
    }

    // Generate test for the main class
    auto testFile = projectPath / testConfig.testDirectory / "unit" / "console_app_test.cpp";
    if (!testManager.generateTestFile(testFile, "ConsoleApp", testingFramework)) {
        spdlog::warn("Failed to generate test file, using fallback");
    }

    // Always create a test_main.cpp file for compatibility with tests
    std::string testsPath = FileUtils::combinePath(options_.projectName, "tests");
    spdlog::info("Ensuring tests directory exists: {}", testsPath);

    // Check if directory already exists
    if (FileUtils::directoryExists(testsPath)) {
        spdlog::info("Tests directory already exists");
    } else {
        if (!FileUtils::createDirectory(testsPath)) {
            spdlog::error("Failed to create tests directory: {}", testsPath);
            return false;
        }
        spdlog::info("Tests directory created successfully");
    }

    std::string testContent;
    if (options_.testFramework == TestFramework::GTest) {
        testContent = getGTestContent();
        spdlog::info("Using GTest content for test_main.cpp");
    } else if (options_.testFramework == TestFramework::Catch2) {
        testContent = getCatch2Content();
        spdlog::info("Using Catch2 content for test_main.cpp");
    } else if (options_.testFramework == TestFramework::Doctest) {
        testContent = getDocTestContent();
        spdlog::info("Using Doctest content for test_main.cpp");
    } else {
        spdlog::warn("Unknown test framework: {}, using GTest as fallback",
                     static_cast<int>(options_.testFramework));
        testContent = getGTestContent();
    }

    if (testContent.empty()) {
        spdlog::error("Test content is empty!");
        return false;
    }

    std::string testMainPath = FileUtils::combinePath(testsPath, "test_main.cpp");
    spdlog::info("Writing test_main.cpp to: {}", testMainPath);

    if (!FileUtils::writeToFile(testMainPath, testContent)) {
        spdlog::error("Failed to write test_main.cpp file");
        return false;
    }

    spdlog::info("Successfully created test_main.cpp");

    // Update build system configuration
    if (options_.buildSystem == BuildSystem::CMake) {
        // Test configuration is already included in getCMakeContent(), no need to add it again

        // Create tests/CMakeLists.txt
        std::string testCmakeContent;
        if (options_.testFramework == TestFramework::GTest) {
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
        } else if (options_.testFramework == TestFramework::Catch2) {
            testCmakeContent = R"(
find_package(Catch2 REQUIRED)
add_executable(${PROJECT_NAME}_tests test_main.cpp)
target_link_libraries(${PROJECT_NAME}_tests PRIVATE
    ${PROJECT_NAME}_lib
    Catch2::Catch2
)
add_test(NAME ${PROJECT_NAME}_tests COMMAND ${PROJECT_NAME}_tests)
)";
        } else if (options_.testFramework == TestFramework::Doctest) {
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

        std::string testsPath = FileUtils::combinePath(options_.projectName, "tests");
        if (!FileUtils::writeToFile(FileUtils::combinePath(testsPath, "CMakeLists.txt"),
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
    (void)argc; // Suppress unused parameter warning
    (void)argv; // Suppress unused parameter warning

    std::cout << "Hello from {}!" << std::endl;
    return 0;
}}
)",
                       options_.projectName);
}

std::string ConsoleTemplate::getReadmeContent() {
    std::string packageManagerInfo =
            to_string(options_.packageManager) != "none"
                    ? fmt::format("- {} package manager\n", to_string(options_.packageManager))
                    : "";

    std::string buildInstructions;
    if (to_string(options_.buildSystem) == "cmake") {
        buildInstructions = R"(mkdir build && cd build
cmake ..
make)";
    } else if (to_string(options_.buildSystem) == "meson") {
        buildInstructions = R"(meson setup build
cd build
meson compile)";
    } else {
        buildInstructions = R"(bazel build //...)";
    }

    std::string testInstructions;
    if (options_.includeTests) {
        if (to_string(options_.buildSystem) == "cmake") {
            testInstructions = R"(cd build
ctest)";
        } else if (to_string(options_.buildSystem) == "meson") {
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
                       options_.projectName, options_.buildSystem, packageManagerInfo,
                       buildInstructions, testInstructions);
}

std::string ConsoleTemplate::getCMakeContent() {
    std::string vcpkgIntegration = to_string(options_.packageManager) == "vcpkg" ? R"(
  # vcpkg integration
  if(DEFINED ENV{VCPKG_ROOT})
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
  endif()
  )"
                                                                                 : "";

    std::string conanIntegration = to_string(options_.packageManager) == "conan" ? R"(
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

    // Build the CMake content by concatenating strings to avoid brace escaping issues
    std::string cmakeContent = "cmake_minimum_required(VERSION 3.15)\n";
    cmakeContent += "project(" + options_.projectName + " VERSION 0.1.0 LANGUAGES CXX)\n\n";
    cmakeContent += "# Set C++ standard\n";
    cmakeContent += "set(CMAKE_CXX_STANDARD 17)\n";
    cmakeContent += "set(CMAKE_CXX_STANDARD_REQUIRED ON)\n";
    cmakeContent += "set(CMAKE_CXX_EXTENSIONS OFF)\n\n";
    cmakeContent += "# Compile commands for IDE integration\n";
    cmakeContent += "set(CMAKE_EXPORT_COMPILE_COMMANDS ON)\n\n";
    cmakeContent += "# Package manager integration\n";
    cmakeContent += vcpkgIntegration + conanIntegration + "\n";
    cmakeContent += "# Dependencies\n";
    cmakeContent += "find_package(fmt REQUIRED)\n\n";
    cmakeContent += "# Source files\n";
    cmakeContent += "set(SOURCES\n";
    cmakeContent += "  src/main.cpp\n";
    cmakeContent += "  # Add more source files here\n";
    cmakeContent += ")\n\n";
    cmakeContent += "# Include directories\n";
    cmakeContent += "include_directories(\n";
    cmakeContent += "  ${PROJECT_SOURCE_DIR}/include\n";
    cmakeContent += ")\n\n";
    cmakeContent += "# Library target (for reuse in tests)\n";
    cmakeContent += "add_library(${PROJECT_NAME}_lib STATIC ${SOURCES})\n";
    cmakeContent += "target_include_directories(${PROJECT_NAME}_lib\n";
    cmakeContent += "  PUBLIC\n";
    cmakeContent += "    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>\n";
    cmakeContent += "    $<INSTALL_INTERFACE:include>\n";
    cmakeContent += ")\n";
    cmakeContent += "target_link_libraries(${PROJECT_NAME}_lib PUBLIC fmt::fmt)\n\n";
    cmakeContent += "# Main executable\n";
    cmakeContent += "add_executable(${PROJECT_NAME} ${SOURCES})\n";
    cmakeContent += "target_link_libraries(${PROJECT_NAME} PRIVATE ${PROJECT_NAME}_lib)\n\n";
    cmakeContent += "# Add compiler warnings\n";
    cmakeContent += "include(CheckCXXCompilerFlag)\n";
    cmakeContent += "if(CMAKE_CXX_COMPILER_ID MATCHES \"GNU|Clang\")\n";
    cmakeContent +=
            "  target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic -Werror)\n";
    cmakeContent += "elseif(MSVC)\n";
    cmakeContent += "  target_compile_options(${PROJECT_NAME} PRIVATE /W4 /WX)\n";
    cmakeContent += "endif()\n";
    cmakeContent += testSection + installSection;

    return cmakeContent;
}

std::string ConsoleTemplate::getMesonContent() {
    std::string dependencySection;
    std::vector<std::string> dependencies;

    // 添加基本依赖
    dependencies.push_back("fmt_dep = dependency('fmt')");

    // 添加测试依赖
    if (options_.includeTests) {
        if (to_string(options_.testFramework) == "gtest") {
            dependencies.push_back("gtest_dep = dependency('gtest', required : false)");
            dependencies.push_back("if not gtest_dep.found()");
            dependencies.push_back("  gtest_dep = dependency('GTest', required : true)");
            dependencies.push_back("endif");
            dependencies.push_back("gtest_main_dep = dependency('gtest_main', required : false)");
            dependencies.push_back("if not gtest_main_dep.found()");
            dependencies.push_back(
                    "  gtest_main_dep = dependency('GTest::Main', required : false)");
            dependencies.push_back("endif");
        } else if (to_string(options_.testFramework) == "catch2") {
            dependencies.push_back("catch2_dep = dependency('catch2', required : true)");
        } else {
            dependencies.push_back("doctest_dep = dependency('doctest', required : true)");
        }
    }

    // 合并依赖部分
    std::string dependencyStr;
    for (size_t i = 0; i < dependencies.size(); ++i) {
        dependencyStr += dependencies[i];
        if (i < dependencies.size() - 1) {
            dependencyStr += "\n";
        }
    }
    dependencySection = dependencyStr;

    // 配置测试部分
    std::string testSection;
    if (options_.includeTests) {
        std::string testDep = to_string(options_.testFramework) == "gtest"
                                      ? "gtest_dep, gtest_main_dep"
                              : to_string(options_.testFramework) == "catch2" ? "catch2_dep"
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
                                  options_.projectName, testDep, options_.projectName,
                                  options_.projectName);
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
                       options_.projectName, dependencySection, options_.projectName,
                       options_.projectName, options_.projectName, options_.projectName,
                       options_.projectName, testSection, installSection);
}

std::string ConsoleTemplate::getBazelContent() {
    std::string testSection;
    if (options_.includeTests) {
        std::string testFrameworkDep;
        if (to_string(options_.testFramework) == "gtest") {
            testFrameworkDep = "com_google_googletest//:gtest_main";
        } else if (to_string(options_.testFramework) == "catch2") {
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
                                  options_.projectName, options_.projectName, testFrameworkDep);
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
    FileUtils::writeToFile(FileUtils::combinePath(options_.projectName, "WORKSPACE"),
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
    std::string thirdPartyDir = FileUtils::combinePath(options_.projectName, "third_party");
    FileUtils::createDirectory(thirdPartyDir);
    FileUtils::writeToFile(FileUtils::combinePath(thirdPartyDir, "fmt.BUILD"), fmtBuildContent);

    return fmt::format(
            R"(load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library", "cc_test")

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "{0}_lib",
    srcs = glob(["src/**/*.cpp"]),
    hdrs = glob(["include/**/*.h"]),
    includes = ["include"],
    deps = [
        "@fmt",
    ],
    copts = select({{
        "@platforms//os:windows": ["/W4", "/WX"],
        "//conditions:default": ["-Wall", "-Wextra", "-Wpedantic", "-Werror"],
    }}),
)

cc_binary(
    name = "{0}",
    srcs = ["src/main.cpp"],
    deps = [":{0}_lib"],
    copts = select({{
        "@platforms//os:windows": ["/W4", "/WX"],
        "//conditions:default": ["-Wall", "-Wextra", "-Wpedantic", "-Werror"],
    }}),
){1})",
            options_.projectName, testSection);
}

std::string ConsoleTemplate::getVcpkgJsonContent() {
    std::string testDependency;
    if (options_.includeTests) {
        std::string testFramework = to_string(options_.testFramework) == "gtest"    ? "gtest"
                                    : to_string(options_.testFramework) == "catch2" ? "catch2"
                                                                                    : "doctest";

        std::string features = to_string(options_.testFramework) == "gtest" ? "gmock" : "";
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
        if (to_string(options_.testFramework) == "gtest") {
            testRequirement = "gtest/1.12.1";
        } else if (to_string(options_.testFramework) == "catch2") {
            testRequirement = "catch2/3.1.0";
        } else {
            testRequirement = "doctest/2.4.9";
        }
    }

    std::string generator = to_string(options_.buildSystem) == "cmake" ? "cmake" : "";

    return fmt::format(R"([requires]
{}

[generators]
{})",
                       testRequirement, generator);
}

std::string ConsoleTemplate::getMSYS2PKGBUILDContent() {
    std::string testDependencies;
    if (options_.includeTests) {
        if (to_string(options_.testFramework) == "gtest") {
            testDependencies = "  \"${{MINGW_PACKAGE_PREFIX}}-gtest\"";
        } else if (to_string(options_.testFramework) == "catch2") {
            testDependencies = "  \"${{MINGW_PACKAGE_PREFIX}}-catch2\"";
        }
    }

    std::string pkgbuildContent =
            "# Maintainer: Your Name <your.email@example.com>\n"
            "_realname=" +
            options_.projectName +
            "\n"
            "pkgbase=mingw-w64-${_realname}\n"
            "pkgname=\"${MINGW_PACKAGE_PREFIX}-${_realname}\"\n"
            "pkgver=1.0.0\n"
            "pkgrel=1\n"
            "pkgdesc=\"A C++ console application (mingw-w64)\"\n"
            "arch=(\"any\")\n"
            "mingw_arch=(\"mingw32\" \"mingw64\" \"ucrt64\" \"clang64\" \"clangarm64\")\n"
            "url=\"https://github.com/yourname/" +
            options_.projectName +
            "\"\n"
            "license=(\"MIT\")\n"
            "makedepends=(\n"
            "  \"${MINGW_PACKAGE_PREFIX}-cc\"\n"
            "  \"${MINGW_PACKAGE_PREFIX}-cmake\"\n"
            "  \"${MINGW_PACKAGE_PREFIX}-ninja\"\n"
            ")\n"
            "depends=(\n"
            "  \"${MINGW_PACKAGE_PREFIX}-gcc-libs\"\n" +
            testDependencies +
            "\n"
            ")\n"
            "source=(\"${_realname}-${pkgver}.tar.gz\")\n"
            "sha256sums=(\"SKIP\")\n\n"
            "build() {\n"
            "  cd \"${srcdir}/${_realname}-${pkgver}\"\n"
            "  \n"
            "  mkdir -p build && cd build\n"
            "  \n"
            "  MSYS2_ARG_CONV_EXCL=\"-DCMAKE_INSTALL_PREFIX=\" \\\n"
            "  ${MINGW_PREFIX}/bin/cmake.exe \\\n"
            "    -GNinja \\\n"
            "    -DCMAKE_INSTALL_PREFIX=${MINGW_PREFIX} \\\n"
            "    -DCMAKE_BUILD_TYPE=Release \\\n"
            "    ..\n"
            "  \n"
            "  ${MINGW_PREFIX}/bin/cmake.exe --build .\n"
            "}\n\n"
            "check() {\n"
            "  cd \"${srcdir}/${_realname}-${pkgver}/build\"\n"
            "  \n"
            "  # Run tests if available\n"
            "  if [ -f \"test_" +
            options_.projectName +
            "\" ]; then\n"
            "    ./test_" +
            options_.projectName +
            "\n"
            "  fi\n"
            "}\n\n"
            "package() {\n"
            "  cd \"${srcdir}/${_realname}-${pkgver}/build\"\n"
            "  \n"
            "  DESTDIR=\"${pkgdir}\" ${MINGW_PREFIX}/bin/cmake.exe --install .\n"
            "  \n"
            "  # Install license\n"
            "  install -Dm644 \"${srcdir}/${_realname}-${pkgver}/LICENSE\" \\\n"
            "    \"${pkgdir}${MINGW_PREFIX}/share/licenses/${_realname}/LICENSE\"\n"
            "}\n";

    return pkgbuildContent;
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

std::string ConsoleTemplate::getXMakeContent() {
    std::string testSection;
    if (options_.includeTests) {
        std::string testFramework;
        if (to_string(options_.testFramework) == "gtest") {
            testFramework = "gtest";
        } else if (to_string(options_.testFramework) == "catch2") {
            testFramework = "catch2";
        } else if (to_string(options_.testFramework) == "doctest") {
            testFramework = "doctest";
        }

        if (!testFramework.empty()) {
            testSection = fmt::format(R"(
add_requires("{0}")

target("{1}_tests")
    set_kind("binary")
    add_files("tests/test_main.cpp")
    add_packages("{2}")
    add_deps("{3}")
)",
                                      testFramework, options_.projectName, testFramework,
                                      options_.projectName);
        }
    }

    return fmt::format(R"(set_project("{0}")
set_version("1.0.0")

set_languages("c++17")

add_requires("spdlog")

target("{1}")
    set_kind("binary")
    add_files("src/main.cpp")
    add_packages("spdlog")
{2})",
                       options_.projectName, options_.projectName, testSection);
}

std::string ConsoleTemplate::getPremakeContent() {
    std::string testSection;
    if (options_.includeTests) {
        testSection = fmt::format(R"(
project "{}_tests"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"
    targetdir "bin/%{{cfg.buildcfg}}"

    files {{
        "tests/**.cpp",
        "tests/**.h"
    }}

    includedirs {{
        "src",
        "tests"
    }}

    links {{ "{}" }}
)",
                                  options_.projectName, options_.projectName);
    }

    return fmt::format(R"(workspace "{}"
    configurations {{ "Debug", "Release" }}
    platforms {{ "x64" }}

project "{}"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"
    targetdir "bin/%{{cfg.buildcfg}}"

    files {{
        "src/**.cpp",
        "src/**.h"
    }}

    includedirs {{
        "src"
    }}

    filter "configurations:Debug"
        defines {{ "DEBUG" }}
        symbols "On"

    filter "configurations:Release"
        defines {{ "NDEBUG" }}
        optimize "On"
{})",
                       options_.projectName, options_.projectName, testSection);
}

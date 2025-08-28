#include "library_template.h"

#include "../utils/file_utils.h"
#include "../utils/string_utils.h"

#include <fmt/core.h>
#include <iostream>
#include <spdlog/spdlog.h>

using namespace utils;
using namespace cli_enums;

LibraryTemplate::LibraryTemplate(const CliOptions &options)
    : TemplateBase(options) {}

bool LibraryTemplate::create() {
  std::string projectPath = options_.projectName;

  // 检查项目目录是否已存在
  if (FileUtils::directoryExists(projectPath)) {
    spdlog::error("Directory '{}' already exists.", projectPath);
    return false;
  }

  spdlog::info("🚀 Creating library project...");

  // 创建基本结构
  if (!createProjectStructure()) {
    spdlog::error("Failed to create project structure");
    return false;
  }
  spdlog::info("✅ Project structure created");

  // 创建构建系统
  if (!createBuildSystem()) {
    spdlog::error("Failed to configure build system");
    return false;
  }
  spdlog::info("✅ Build system configured");

  // 设置包管理器
  if (!setupPackageManager()) {
    spdlog::error("Failed to setup package manager");
    return false;
  }
  spdlog::info("✅ Package manager setup");

  // 设置测试框架
  if (options_.includeTests) {
    if (!setupTestFramework()) {
      spdlog::error("Failed to setup test framework");
      return false;
    }
    spdlog::info("✅ Test framework configured");
  }

  // 设置文档
  if (setupDocumentation()) {
    spdlog::info("✅ Documentation setup");
  }

  // 设置CI/CD
  if (setupContinuousIntegration()) {
    spdlog::info("✅ CI/CD configuration setup");
  }

  // 设置代码格式化
  if (setupCodeFormatting()) {
    spdlog::info("✅ Code formatting setup");
  }

  // 设置基准测试
  if (setupBenchmarking()) {
    spdlog::info("✅ Benchmarking setup");
  }

  // 设置版本控制
  if (setupVersionControl()) {
    spdlog::info("✅ Version control setup");
  }

  // 初始化Git
  if (options_.initGit) {
    if (!initializeGit(projectPath)) {
      spdlog::error("Failed to initialize Git repository");
      return false;
    }
    spdlog::info("✅ Git repository initialized");
  }

  spdlog::info("\nYour library project is ready!\n");

  // 打印使用说明
  std::cout << fmt::format("cd {}\n", options_.projectName);

  if (to_string(options_.buildSystem) == "cmake") {
    std::cout << "mkdir build && cd build\n";
    std::cout << "cmake ..\n";
    std::cout << "make\n";
  } else if (to_string(options_.buildSystem) == "meson") {
    std::cout << "meson setup build\n";
    std::cout << "cd build\n";
    std::cout << "meson compile\n";
  } else if (to_string(options_.buildSystem) == "bazel") {
    std::cout << "bazel build //...\n";
  }

  std::cout << "\nHappy coding! 🎉\n";

  return true;
}

bool LibraryTemplate::createProjectStructure() {
  std::string projectPath = options_.projectName;

  // 创建主目录
  if (!FileUtils::createDirectory(projectPath)) {
    return false;
  }

  // 创建src目录
  std::string srcPath = FileUtils::combinePath(projectPath, "src");
  if (!FileUtils::createDirectory(srcPath)) {
    return false;
  }

  // 创建include目录
  std::string includePath = FileUtils::combinePath(projectPath, "include");
  if (!FileUtils::createDirectory(includePath)) {
    return false;
  }

  // 创建include/project目录
  std::string includeProjectPath =
      FileUtils::combinePath(includePath, options_.projectName);
  if (!FileUtils::createDirectory(includeProjectPath)) {
    return false;
  }

  // 创建example目录
  std::string examplePath = FileUtils::combinePath(projectPath, "example");
  if (!FileUtils::createDirectory(examplePath)) {
    return false;
  }

  // 写入源文件
  if (!FileUtils::writeToFile(
          FileUtils::combinePath(srcPath,
                                 fmt::format("{}.cpp", options_.projectName)),
          getLibraryCppContent())) {
    return false;
  }

  // 写入版本头文件
  if (!FileUtils::writeToFile(
          FileUtils::combinePath(includeProjectPath, "version.h"),
          getVersionHeaderContent())) {
    return false;
  }

  // 写入头文件
  if (!FileUtils::writeToFile(
          FileUtils::combinePath(includeProjectPath,
                                 fmt::format("{}.h", options_.projectName)),
          getLibraryHeaderContent())) {
    return false;
  }

  // 写入示例文件
  if (!FileUtils::writeToFile(
          FileUtils::combinePath(examplePath, "example.cpp"),
          getExampleContent())) {
    return false;
  }

  // 创建README.md
  if (!FileUtils::writeToFile(FileUtils::combinePath(projectPath, "README.md"),
                              getReadmeContent())) {
    return false;
  }

  // 创建LICENSE
  if (!FileUtils::writeToFile(FileUtils::combinePath(projectPath, "LICENSE"),
                              getLicenseContent("MIT"))) {
    return false;
  }

  // 创建安装脚本
  std::string installScriptPath =
      FileUtils::combinePath(projectPath, "install.sh");
  if (!FileUtils::writeToFile(installScriptPath, getInstallScriptContent())) {
    return false;
  }

// 使安装脚本可执行
#ifndef _WIN32
  std::string chmodCmd = fmt::format("chmod +x {}", installScriptPath);
  std::system(chmodCmd.c_str());
#endif

  return true;
}

bool LibraryTemplate::createBuildSystem() {
  std::string projectPath = options_.projectName;

  if (to_string(options_.buildSystem) == "cmake") {
    // 创建CMakeLists.txt
    if (!FileUtils::writeToFile(
            FileUtils::combinePath(projectPath, "CMakeLists.txt"),
            getCMakeContent())) {
      return false;
    }

    // 创建cmake目录用于config文件
    std::string cmakePath = FileUtils::combinePath(projectPath, "cmake");
    if (!FileUtils::createDirectory(cmakePath)) {
      return false;
    }

    // 创建config模板
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

  } else if (to_string(options_.buildSystem) == "meson") {
    // 创建meson.build
    if (!FileUtils::writeToFile(
            FileUtils::combinePath(projectPath, "meson.build"),
            getMesonContent())) {
      return false;
    }
  } else if (to_string(options_.buildSystem) == "bazel") {
    // 创建WORKSPACE和BUILD文件
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

  if (to_string(options_.packageManager) == "vcpkg") {
    // 创建vcpkg.json
    if (!FileUtils::writeToFile(
            FileUtils::combinePath(projectPath, "vcpkg.json"),
            getVcpkgJsonContent())) {
      return false;
    }
  } else if (to_string(options_.packageManager) == "conan") {
    // 创建conanfile.txt
    if (!FileUtils::writeToFile(
            FileUtils::combinePath(projectPath, "conanfile.txt"),
            getConanfileContent())) {
      return false;
    }
  } else if (to_string(options_.packageManager) == "msys2") {
    // 创建PKGBUILD
    if (!FileUtils::writeToFile(
            FileUtils::combinePath(projectPath, "PKGBUILD"),
            getMSYS2PKGBUILDContent())) {
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
  if (to_string(options_.testFramework) == "gtest") {
    testContent = getGTestContent();
  } else if (to_string(options_.testFramework) == "catch2") {
    testContent = getCatch2Content();
  } else if (to_string(options_.testFramework) == "doctest") {
    testContent = getDocTestContent();
  }

  std::string testFileName = fmt::format("test_{}.cpp", options_.projectName);
  if (!FileUtils::writeToFile(FileUtils::combinePath(testsPath, testFileName),
                              testContent)) {
    return false;
  }

  // 更新构建系统以包含测试
  if (to_string(options_.buildSystem) == "cmake") {
    std::string testCmakeContent;
    if (to_string(options_.testFramework) == "gtest") {
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
    } else if (to_string(options_.testFramework) == "catch2") {
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
    } else if (to_string(options_.testFramework) == "doctest") {
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

bool LibraryTemplate::setupDocumentation() {
  std::string projectPath = options_.projectName;
  std::string docsPath = FileUtils::combinePath(projectPath, "docs");

  if (!FileUtils::createDirectory(docsPath)) {
    spdlog::warn("Failed to create documentation directory");
    return false;
  }

  // 创建Doxygen配置文件
  if (!FileUtils::writeToFile(FileUtils::combinePath(projectPath, "Doxyfile"),
                              getDoxyfileContent())) {
    spdlog::warn("Failed to create Doxyfile");
    return false;
  }

  // 创建Sphinx文档
  std::string sphinxPath = FileUtils::combinePath(docsPath, "sphinx");
  if (!FileUtils::createDirectory(sphinxPath)) {
    spdlog::warn("Failed to create Sphinx directory");
    return false;
  }

  if (!FileUtils::writeToFile(FileUtils::combinePath(sphinxPath, "conf.py"),
                              getSphinxConfigContent())) {
    spdlog::warn("Failed to create Sphinx config");
    return false;
  }

  // 创建索引文件
  std::string indexContent =
      fmt::format(R"(
{0} Documentation
{1}

Welcome to the {0} documentation!

Contents:
=========

* Getting Started
* API Reference
* Examples
* Contributing

Indices and tables
==================

* :ref:`genindex`
* :ref:`search`
)",
                  options_.projectName,
                  std::string(options_.projectName.length() + 14, '='));

  if (!FileUtils::writeToFile(FileUtils::combinePath(sphinxPath, "index.rst"),
                              indexContent)) {
    spdlog::warn("Failed to create Sphinx index file");
    return false;
  }

  // 创建MkDocs配置
  if (!FileUtils::writeToFile(FileUtils::combinePath(projectPath, "mkdocs.yml"),
                              getMkDocsContent())) {
    spdlog::warn("Failed to create MkDocs config");
    return false;
  }

  // 创建markdown文档目录
  std::string mdPath = FileUtils::combinePath(docsPath, "md");
  if (!FileUtils::createDirectory(mdPath)) {
    spdlog::warn("Failed to create Markdown docs directory");
    return false;
  }

  // 创建索引
  std::string mdIndexContent = fmt::format(R"(# {0} Documentation

Welcome to the {0} documentation!

## Getting Started

[Installation instructions](../README.md)

## API Reference

See the [API Reference](api/index.md) for detailed function and class documentation.

## Examples

Check out the [examples](examples/index.md) to see {0} in action.
)",
                                           options_.projectName);

  if (!FileUtils::writeToFile(FileUtils::combinePath(mdPath, "index.md"),
                              mdIndexContent)) {
    spdlog::warn("Failed to create Markdown index file");
    return false;
  }

  return true;
}

bool LibraryTemplate::setupContinuousIntegration() {
  std::string projectPath = options_.projectName;

  // 创建GitHub Actions工作流目录
  std::string githubDir = FileUtils::combinePath(projectPath, ".github");
  if (!FileUtils::createDirectory(githubDir)) {
    spdlog::warn("Failed to create .github directory");
    return false;
  }

  std::string workflowsDir = FileUtils::combinePath(githubDir, "workflows");
  if (!FileUtils::createDirectory(workflowsDir)) {
    spdlog::warn("Failed to create workflows directory");
    return false;
  }

  // 创建GitHub Actions工作流
  if (!FileUtils::writeToFile(FileUtils::combinePath(workflowsDir, "build.yml"),
                              getGitHubWorkflowContent(std::string(to_string(options_.buildSystem))))) {
    spdlog::warn("Failed to create GitHub workflow file");
    return false;
  }

  // 创建Travis CI配置
  if (!FileUtils::writeToFile(
          FileUtils::combinePath(projectPath, ".travis.yml"),
          getTravisCIContent(std::string(to_string(options_.buildSystem))))) {
    spdlog::warn("Failed to create Travis CI config");
    return false;
  }

  // 创建AppVeyor配置
  if (!FileUtils::writeToFile(
          FileUtils::combinePath(projectPath, "appveyor.yml"),
          getAppVeyorContent(std::string(to_string(options_.buildSystem))))) {
    spdlog::warn("Failed to create AppVeyor config");
    return false;
  }

  return true;
}

bool LibraryTemplate::setupCodeFormatting() {
  std::string projectPath = options_.projectName;

  // 创建.clang-format文件
  if (!FileUtils::writeToFile(
          FileUtils::combinePath(projectPath, ".clang-format"),
          getClangFormatContent())) {
    spdlog::warn("Failed to create .clang-format file");
    return false;
  }

  // 创建.clang-tidy文件
  if (!FileUtils::writeToFile(
          FileUtils::combinePath(projectPath, ".clang-tidy"),
          getClangTidyContent())) {
    spdlog::warn("Failed to create .clang-tidy file");
    return false;
  }

  return true;
}

bool LibraryTemplate::setupBenchmarking() {
  std::string projectPath = options_.projectName;
  std::string benchmarkPath = FileUtils::combinePath(projectPath, "benchmark");

  if (!FileUtils::createDirectory(benchmarkPath)) {
    spdlog::warn("Failed to create benchmark directory");
    return false;
  }

  // 创建基准测试源文件
  if (!FileUtils::writeToFile(
          FileUtils::combinePath(benchmarkPath, "benchmark_main.cpp"),
          getBenchmarkContent())) {
    spdlog::warn("Failed to create benchmark source file");
    return false;
  }

  // 如果使用CMake，创建相应的CMakeLists.txt
  if (to_string(options_.buildSystem) == "cmake") {
    std::string benchmarkCmakeContent = R"(
find_package(benchmark REQUIRED)

add_executable(${PROJECT_NAME}_benchmark benchmark_main.cpp)
target_link_libraries(${PROJECT_NAME}_benchmark PRIVATE
    ${PROJECT_NAME}
    benchmark::benchmark
)
)";

    if (!FileUtils::writeToFile(
            FileUtils::combinePath(benchmarkPath, "CMakeLists.txt"),
            benchmarkCmakeContent)) {
      spdlog::warn("Failed to create benchmark CMakeLists.txt");
      return false;
    }

    // 更新主CMakeLists.txt添加benchmark子目录
    std::string cmakePath =
        FileUtils::combinePath(projectPath, "CMakeLists.txt");
    std::string cmakeContent = FileUtils::readFromFile(cmakePath);

    cmakeContent += R"(
# Benchmarking
option(BUILD_BENCHMARKS "Build benchmarks" OFF)
if(BUILD_BENCHMARKS)
  add_subdirectory(benchmark)
endif()
)";

    if (!FileUtils::writeToFile(cmakePath, cmakeContent)) {
      spdlog::warn("Failed to update main CMakeLists.txt for benchmarking");
      return false;
    }
  }

  return true;
}

bool LibraryTemplate::setupVersionControl() {
  std::string projectPath = options_.projectName;

  // 创建.gitignore文件
  if (!FileUtils::writeToFile(FileUtils::combinePath(projectPath, ".gitignore"),
                              getGitignoreContent())) {
    spdlog::warn("Failed to create .gitignore file");
    return false;
  }

  // 创建.gitattributes文件
  std::string gitattributesContent =
      R"(# Auto detect text files and perform LF normalization
* text=auto

# C++ source files
*.cpp text diff=cpp
*.hpp text diff=cpp
*.h text diff=cpp
*.cc text diff=cpp
*.cxx text diff=cpp

# Build system files
CMakeLists.txt text
*.cmake text
meson.build text
BUILD text
WORKSPACE text

# Documentation
*.md text
*.txt text
LICENSE text
)";

  if (!FileUtils::writeToFile(
          FileUtils::combinePath(projectPath, ".gitattributes"),
          gitattributesContent)) {
    spdlog::warn("Failed to create .gitattributes file");
    return false;
  }

  return true;
}

std::string LibraryTemplate::getGitignoreContent() {
  return R"(# Build directories
build/
bin/
out/
lib/
install/
cmake-build-*/

# IDE and editor specific files
.vs/
.vscode/
.idea/
*.swp
*~
.DS_Store

# Compiled object files
*.o
*.obj
*.so
*.dll
*.dylib
*.a
*.lib
*.exe

# CMake generated files
CMakeCache.txt
CMakeFiles/
cmake_install.cmake
Makefile
CTestTestfile.cmake
install_manifest.txt
compile_commands.json
_deps/

# Meson generated files
meson-private/
meson-logs/
meson-info/
*wrap_db*

# Bazel generated files
bazel-*

# Package manager files
/vcpkg_installed/
/conanbuildinfo.*
/conaninfo.txt
/graph_info.json

# Generated documentation
/docs/api/*
/docs/html/
/docs/xml/
/docs/_build/
/site/
/docs/doxygen_sqlite3.db

# Test and benchmark outputs
Testing/
GoogleTest/
)";
}

std::string
LibraryTemplate::getGitHubWorkflowContent(const std::string &buildSystem) {
  if (buildSystem == "cmake") {
    return R"(name: Build and Test

on:
  push:
    branches: [ main, master ]
  pull_request:
    branches: [ main, master ]

jobs:
  build:
    name: ${{ matrix.config.name }}
    runs-on: ${{ matrix.config.os }}
    strategy:
      fail-fast: false
      matrix:
        config:
          - {
            name: "Windows Latest MSVC",
            os: windows-latest,
            build_type: "Release",
            cc: "cl",
            cxx: "cl",
            generators: "Visual Studio 17 2022"
          }
          - {
            name: "Ubuntu Latest GCC",
            os: ubuntu-latest,
            build_type: "Release",
            cc: "gcc",
            cxx: "g++",
            generators: "Ninja"
          }
          - {
            name: "macOS Latest Clang",
            os: macos-latest,
            build_type: "Release",
            cc: "clang",
            cxx: "clang++",
            generators: "Ninja"
          }

    steps:
    - uses: actions/checkout@v3

    - name: Install Ninja and CMake
      uses: lukka/get-cmake@latest
      with:
        cmakeVersion: latest
        ninjaVersion: latest

    - name: Configure
      run: |
        cmake -B build -G "${{ matrix.config.generators }}" -DCMAKE_BUILD_TYPE=${{ matrix.config.build_type }} -DBUILD_TESTING=ON

    - name: Build
      run: |
        cmake --build build --config ${{ matrix.config.build_type }}

    - name: Test
      working-directory: build
      run: |
        ctest -C ${{ matrix.config.build_type }} --output-on-failure
)";
  } else if (buildSystem == "meson") {
    return R"(name: Build and Test

on:
  push:
    branches: [ main, master ]
  pull_request:
    branches: [ main, master ]

jobs:
  build:
    name: ${{ matrix.config.name }}
    runs-on: ${{ matrix.config.os }}
    strategy:
      fail-fast: false
      matrix:
        config:
          - {
            name: "Windows Latest MSVC",
            os: windows-latest,
            cc: "cl",
            cxx: "cl"
          }
          - {
            name: "Ubuntu Latest GCC",
            os: ubuntu-latest,
            cc: "gcc",
            cxx: "g++"
          }
          - {
            name: "macOS Latest Clang",
            os: macos-latest,
            cc: "clang",
            cxx: "clang++"
          }

    steps:
    - uses: actions/checkout@v3

    - name: Set up Python
      uses: actions/setup-python@v4
      with:
        python-version: '3.x'

    - name: Install Meson and Ninja
      run: |
        python -m pip install --upgrade pip
        pip install meson ninja

    - name: Configure
      run: |
        meson setup build -Dwerror=true -Dtests=true

    - name: Build
      run: |
        meson compile -C build

    - name: Test
      run: |
        meson test -C build -v
)";
  } else { // bazel
    return R"(name: Build and Test

on:
  push:
    branches: [ main, master ]
  pull_request:
    branches: [ main, master ]

jobs:
  build:
    name: ${{ matrix.config.name }}
    runs-on: ${{ matrix.config.os }}
    strategy:
      fail-fast: false
      matrix:
        config:
          - {
            name: "Ubuntu Latest",
            os: ubuntu-latest
          }
          - {
            name: "macOS Latest",
            os: macos-latest
          }
          - {
            name: "Windows Latest",
            os: windows-latest
          }

    steps:
    - uses: actions/checkout@v3

    - name: Set up Bazel
      uses: bazelbuild/setup-bazelisk@v2

    - name: Build
      run: |
        bazel build //...

    - name: Test
      run: |
        bazel test //...
)";
  }
}

std::string
LibraryTemplate::getTravisCIContent(const std::string &buildSystem) {
  if (buildSystem == "cmake") {
    return R"(language: cpp
sudo: required

matrix:
  include:
    - os: linux
      dist: focal
      compiler: gcc
      addons:
        apt:
          sources:
            - ubuntu-toolchain-r-test
          packages:
            - g++-9
            - cmake
      env:
        - MATRIX_EVAL="CC=gcc-9 && CXX=g++-9"

    - os: linux
      dist: focal
      compiler: clang
      addons:
        apt:
          sources:
            - ubuntu-toolchain-r-test
            - llvm-toolchain-focal-12
          packages:
            - clang-12
            - cmake
      env:
        - MATRIX_EVAL="CC=clang-12 && CXX=clang++-12"

    - os: osx
      osx_image: xcode13
      compiler: clang

before_install:
  - eval "${MATRIX_EVAL}"

script:
  - mkdir build
  - cd build
  - cmake .. -DBUILD_TESTING=ON
  - cmake --build .
  - ctest --output-on-failure
)";
  } else if (buildSystem == "meson") {
    return R"(language: cpp
sudo: required

matrix:
  include:
    - os: linux
      dist: focal
      compiler: gcc
      addons:
        apt:
          sources:
            - ubuntu-toolchain-r-test
          packages:
            - g++-9
            - python3-pip
            - python3-setuptools
            - ninja-build
      env:
        - MATRIX_EVAL="CC=gcc-9 && CXX=g++-9"

    - os: linux
      dist: focal
      compiler: clang
      addons:
        apt:
          sources:
            - ubuntu-toolchain-r-test
            - llvm-toolchain-focal-12
          packages:
            - clang-12
            - python3-pip
            - python3-setuptools
            - ninja-build
      env:
        - MATRIX_EVAL="CC=clang-12 && CXX=clang++-12"

    - os: osx
      osx_image: xcode13
      compiler: clang

before_install:
  - eval "${MATRIX_EVAL}"
  - pip3 install --user meson

script:
  - meson setup build -Dtests=true
  - meson compile -C build
  - meson test -C build -v
)";
  } else { // bazel
    return R"(language: cpp
sudo: required

matrix:
  include:
    - os: linux
      dist: focal
      addons:
        apt:
          sources:
            - ubuntu-toolchain-r-test
          packages:
            - g++-9
      env:
        - MATRIX_EVAL="CC=gcc-9 && CXX=g++-9"

    - os: osx
      osx_image: xcode13

before_install:
  - eval "${MATRIX_EVAL}"
  # Install Bazelisk
  - |
    if [[ "${TRAVIS_OS_NAME}" == "linux" ]]; then
      curl -LO "https://github.com/bazelbuild/bazelisk/releases/download/v1.11.0/bazelisk-linux-amd64"
      chmod +x bazelisk-linux-amd64
      sudo mv bazelisk-linux-amd64 /usr/local/bin/bazel
    elif [[ "${TRAVIS_OS_NAME}" == "osx" ]]; then
      brew install bazelisk
    fi

script:
  - bazel build //...
  - bazel test //...
)";
  }
}

std::string
LibraryTemplate::getAppVeyorContent(const std::string &buildSystem) {
  if (buildSystem == "cmake") {
    return R"(image:
  - Visual Studio 2022
  - Ubuntu

clone_folder: c:\projects\project

configuration:
  - Release
  - Debug

platform:
  - x64

for:
  -
    matrix:
      only:
        - image: Visual Studio 2022

    environment:
      MSVC_SETUP_PATH: C:\Program Files (x86)\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat
      MSVC_SETUP_ARGS: x64

    before_build:
      - call "%MSVC_SETUP_PATH%" %MSVC_SETUP_ARGS%
      - cd c:\projects\project
      - mkdir build
      - cd build
      - cmake .. -G "Visual Studio 17 2022" -A x64

    build_script:
      - cd c:\projects\project\build
      - cmake --build . --config %CONFIGURATION%

    test_script:
      - cd c:\projects\project\build
      - ctest -C %CONFIGURATION% --output-on-failure
  -
    matrix:
      only:
        - image: Ubuntu

    install:
      - sudo apt-get update
      - sudo apt-get -y install cmake build-essential

    build_script:
      - mkdir build
      - cd build
      - cmake .. -DCMAKE_BUILD_TYPE=$CONFIGURATION
      - cmake --build .

    test_script:
      - cd build
      - ctest --output-on-failure
)";
  } else if (buildSystem == "meson") {
    return R"(image:
  - Visual Studio 2022
  - Ubuntu

clone_folder: c:\projects\project

configuration:
  - Release
  - Debug

for:
  -
    matrix:
      only:
        - image: Visual Studio 2022

    install:
      - ps: |
          choco install python3 --version=3.10.0 -y
          python -m pip install --upgrade pip
          pip install meson ninja

    build_script:
      - cd c:\projects\project
      - meson setup build --buildtype=%CONFIGURATION%
      - meson compile -C build

    test_script:
      - cd c:\projects\project
      - meson test -C build -v
  -
    matrix:
      only:
        - image: Ubuntu

    install:
      - sudo apt-get update
      - sudo apt-get -y install python3-pip ninja-build
      - pip3 install meson

    build_script:
      - meson setup build --buildtype=$CONFIGURATION
      - meson compile -C build

    test_script:
      - meson test -C build -v
)";
  } else { // bazel
    return R"(image:
  - Visual Studio 2022
  - Ubuntu

clone_folder: c:\projects\project

configuration:
  - Release

for:
  -
    matrix:
      only:
        - image: Visual Studio 2022

    install:
      - ps: |
          choco install bazelisk -y

    build_script:
      - cd c:\projects\project
      - bazel build //...

    test_script:
      - cd c:\projects\project
      - bazel test //...
  -
    matrix:
      only:
        - image: Ubuntu

    install:
      - |
        sudo curl -Lo /usr/local/bin/bazel https://github.com/bazelbuild/bazelisk/releases/download/v1.11.0/bazelisk-linux-amd64
        sudo chmod +x /usr/local/bin/bazel

    build_script:
      - bazel build //...

    test_script:
      - bazel test //...
)";
  }
}

std::string LibraryTemplate::getClangFormatContent() {
  return R"(---
Language: Cpp
BasedOnStyle: Google
AccessModifierOffset: -2
AlignAfterOpenBracket: Align
AlignConsecutiveAssignments: false
AlignConsecutiveDeclarations: false
AlignEscapedNewlines: Left
AlignOperands: true
AlignTrailingComments: true
AllowAllParametersOfDeclarationOnNextLine: true
AllowShortBlocksOnASingleLine: false
AllowShortCaseLabelsOnASingleLine: false
AllowShortFunctionsOnASingleLine: All
AllowShortIfStatementsOnASingleLine: true
AllowShortLoopsOnASingleLine: true
AlwaysBreakAfterDefinitionReturnType: None
AlwaysBreakAfterReturnType: None
AlwaysBreakBeforeMultilineStrings: true
AlwaysBreakTemplateDeclarations: Yes
BinPackArguments: true
BinPackParameters: true
BraceWrapping:
  AfterClass: false
  AfterControlStatement: false
  AfterEnum: false
  AfterFunction: false
  AfterNamespace: false
  AfterObjCDeclaration: false
  AfterStruct: false
  AfterUnion: false
  AfterExternBlock: false
  BeforeCatch: false
  BeforeElse: false
  IndentBraces: false
  SplitEmptyFunction: true
  SplitEmptyRecord: true
  SplitEmptyNamespace: true
BreakBeforeBinaryOperators: None
BreakBeforeBraces: Attach
BreakBeforeInheritanceComma: false
BreakInheritanceList: BeforeColon
BreakBeforeTernaryOperators: true
BreakConstructorInitializersBeforeComma: false
BreakConstructorInitializers: BeforeColon
BreakAfterJavaFieldAnnotations: false
BreakStringLiterals: true
ColumnLimit: 100
CommentPragmas: '^ IWYU pragma:'
CompactNamespaces: false
ConstructorInitializerAllOnOneLineOrOnePerLine: true
ConstructorInitializerIndentWidth: 4
ContinuationIndentWidth: 4
Cpp11BracedListStyle: true
DerivePointerAlignment: true
DisableFormat: false
ExperimentalAutoDetectBinPacking: false
FixNamespaceComments: true
ForEachMacros:
  - foreach
  - Q_FOREACH
  - BOOST_FOREACH
IncludeBlocks: Regroup
IncludeCategories:
  - Regex: '^<ext/.*\.h>'
    Priority: 2
  - Regex: '^<.*\.h>'
    Priority: 1
  - Regex: '^<.*'
    Priority: 2
  - Regex: '.*'
    Priority: 3
IncludeIsMainRegex: '([-_](test|unittest))?$'
IndentCaseLabels: true
IndentPPDirectives: None
IndentWidth: 2
IndentWrappedFunctionNames: false
JavaScriptQuotes: Leave
JavaScriptWrapImports: true
KeepEmptyLinesAtTheStartOfBlocks: false
MacroBlockBegin: ''
MacroBlockEnd: ''
MaxEmptyLinesToKeep: 1
NamespaceIndentation: None
ObjCBinPackProtocolList: Never
ObjCBlockIndentWidth: 2
ObjCSpaceAfterProperty: false
ObjCSpaceBeforeProtocolList: true
PenaltyBreakAssignment: 2
PenaltyBreakBeforeFirstCallParameter: 1
PenaltyBreakComment: 300
PenaltyBreakFirstLessLess: 120
PenaltyBreakString: 1000
PenaltyBreakTemplateDeclaration: 10
PenaltyExcessCharacter: 1000000
PenaltyReturnTypeOnItsOwnLine: 200
PointerAlignment: Left
RawStringFormats:
  - Language: Cpp
    Delimiters:
      - cc
      - CC
      - cpp
      - Cpp
      - CPP
      - 'c++'
      - 'C++'
    CanonicalDelimiter: ''
    BasedOnStyle: google
  - Language: TextProto
    Delimiters:
      - pb
      - PB
      - proto
      - PROTO
    EnclosingFunctions:
      - EqualsProto
      - EquivToProto
      - PARSE_PARTIAL_TEXT_PROTO
      - PARSE_TEST_PROTO
      - PARSE_TEXT_PROTO
      - ParseTextOrDie
      - ParseTextProtoOrDie
    CanonicalDelimiter: ''
    BasedOnStyle: google
ReflowComments: true
SortIncludes: true
SortUsingDeclarations: true
SpaceAfterCStyleCast: false
SpaceAfterTemplateKeyword: true
SpaceBeforeAssignmentOperators: true
SpaceBeforeCpp11BracedList: false
SpaceBeforeCtorInitializerColon: true
SpaceBeforeInheritanceColon: true
SpaceBeforeParens: ControlStatements
SpaceBeforeRangeBasedForLoopColon: true
SpaceInEmptyParentheses: false
SpacesBeforeTrailingComments: 2
SpacesInAngles: false
SpacesInContainerLiterals: true
SpacesInCStyleCastParentheses: false
SpacesInParentheses: false
SpacesInSquareBrackets: false
Standard: Auto
TabWidth: 8
UseTab: Never
)";
}

std::string LibraryTemplate::getLibraryCppContent() {
  // 获取项目名称的大写版本（用于头文件保护）
  std::string projectNameUpper = StringUtils::toUpper(options_.projectName);

  return fmt::format(R"(#include "{0}/{0}.h"
#include "{0}/version.h"

namespace {0} {{

// Example类实现
Example::Example(int value) : value_(value) {{}}

int Example::getValue() const {{
    return value_;
}}

void Example::setValue(int value) {{
    value_ = value;
}}

// 示例函数实现
int add(int a, int b) {{
    return a + b;
}}

// 获取库版本
std::string getVersion() {{
    return VERSION_STR;
}}

}} // namespace {0})",
                     options_.projectName);
}

std::string LibraryTemplate::getLibraryHeaderContent() {
  // 获取项目名称的大写版本（用于头文件保护）
  std::string projectNameUpper = StringUtils::toUpper(options_.projectName);

  return fmt::format(R"(#ifndef {1}_H
#define {1}_H

#include <string>

namespace {0} {{

// 示例类
class Example {{
public:
    explicit Example(int value = 0);

    int getValue() const;
    void setValue(int value);

private:
    int value_;
}};

// 示例函数
int add(int a, int b);

// 获取库版本
std::string getVersion();

}} // namespace {0}

#endif // {1}_H
)",
                     options_.projectName, projectNameUpper);
}

std::string LibraryTemplate::getVersionHeaderContent() {
  std::string projectNameUpper = StringUtils::toUpper(options_.projectName);

  return fmt::format(R"(#ifndef {1}_VERSION_H
#define {1}_VERSION_H

#define VERSION_MAJOR 0
#define VERSION_MINOR 1
#define VERSION_PATCH 0

#define VERSION_STR "0.1.0"

#endif // {1}_VERSION_H
)",
                     options_.projectName, projectNameUpper);
}

std::string LibraryTemplate::getReadmeContent() {
  std::string packageManagerInfo =
      to_string(options_.packageManager) != "none"
          ? fmt::format("- {} package manager\n", options_.packageManager)
          : "";

  std::string buildInstructions;
  if (to_string(options_.buildSystem) == "cmake") {
    buildInstructions = R"(mkdir build && cd build
cmake ..
make
make install)";
  } else if (to_string(options_.buildSystem) == "meson") {
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
    if (to_string(options_.buildSystem) == "cmake") {
      testInstructions = R"(cd build
ctest)";
    } else if (to_string(options_.buildSystem) == "meson") {
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

    // Get library version
    std::string version = {0}::getVersion();

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
  std::string testSection = "";
  if (options_.includeTests) {
    std::string testFrameworkDeps = "";
    if (to_string(options_.testFramework) == "gtest") {
      testFrameworkDeps = R"(
# Test framework dependencies
find_package(gtest REQUIRED)
)";
    } else if (to_string(options_.testFramework) == "catch2") {
      testFrameworkDeps = R"(
# Test framework dependencies
find_package(Catch2 REQUIRED)
)";
    } else if (to_string(options_.testFramework) == "doctest") {
      testFrameworkDeps = R"(
# Test framework dependencies
find_package(doctest REQUIRED)
)";
    }

    testSection = fmt::format(R"({}
# Tests
if(BUILD_TESTING)
  enable_testing()
  add_subdirectory(tests)
endif()
)", testFrameworkDeps);
  }

  std::string vcpkgSection = to_string(options_.packageManager) == "vcpkg" ? R"(
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
    if (to_string(options_.testFramework) == "gtest") {
      testDeps = "gtest_dep = dependency('gtest', main : true)\n";
    } else if (to_string(options_.testFramework) == "catch2") {
      testDeps = "catch2_dep = dependency('catch2')\n";
    } else {
      testDeps = "doctest_dep = dependency('doctest')\n";
    }
  }

  std::string testSection;
  if (options_.includeTests) {
    std::string testDep = to_string(options_.testFramework) == "gtest"    ? "gtest_dep"
                          : to_string(options_.testFramework) == "catch2" ? "catch2_dep"
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
    if (to_string(options_.testFramework) == "gtest") {
      testFrameworkDep = "com_google_googletest//:gtest_main";
    } else if (to_string(options_.testFramework) == "catch2") {
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
    hdrs = ["include/{0}/{0}.h", "include/{0}/version.h"],
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
    std::string testFramework = to_string(options_.testFramework) == "gtest" ? "gtest"
                                : to_string(options_.testFramework) == "catch2"
                                    ? "catch2"
                                    : "doctest";
    std::string features = to_string(options_.testFramework) == "gtest" ? "gmock" : "";

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

std::string LibraryTemplate::getMSYS2PKGBUILDContent() {
  std::string testDependencies;
  if (options_.includeTests) {
    if (to_string(options_.testFramework) == "gtest") {
      testDependencies = "  \"${MINGW_PACKAGE_PREFIX}-gtest\"";
    } else if (to_string(options_.testFramework) == "catch2") {
      testDependencies = "  \"${MINGW_PACKAGE_PREFIX}-catch2\"";
    }
  }

  std::string pkgbuildContent =
    "# Maintainer: Your Name <your.email@example.com>\n"
    "_realname=" + options_.projectName + "\n"
    "pkgbase=mingw-w64-${_realname}\n"
    "pkgname=\"${MINGW_PACKAGE_PREFIX}-${_realname}\"\n"
    "pkgver=1.0.0\n"
    "pkgrel=1\n"
    "pkgdesc=\"A C++ library (mingw-w64)\"\n"
    "arch=(\"any\")\n"
    "mingw_arch=(\"mingw32\" \"mingw64\" \"ucrt64\" \"clang64\" \"clangarm64\")\n"
    "url=\"https://github.com/yourname/" + options_.projectName + "\"\n"
    "license=(\"MIT\")\n"
    "makedepends=(\n"
    "  \"${MINGW_PACKAGE_PREFIX}-cc\"\n"
    "  \"${MINGW_PACKAGE_PREFIX}-cmake\"\n"
    "  \"${MINGW_PACKAGE_PREFIX}-ninja\"\n"
    ")\n"
    "depends=(\n"
    "  \"${MINGW_PACKAGE_PREFIX}-gcc-libs\"\n" +
    testDependencies + "\n"
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
    "    -DBUILD_SHARED_LIBS=ON \\\n"
    "    ..\n"
    "  \n"
    "  ${MINGW_PREFIX}/bin/cmake.exe --build .\n"
    "}\n\n"
    "check() {\n"
    "  cd \"${srcdir}/${_realname}-${pkgver}/build\"\n"
    "  \n"
    "  # Run tests if available\n"
    "  if [ -f \"test_" + options_.projectName + "\" ]; then\n"
    "    ./test_" + options_.projectName + "\n"
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

    // Get library version
    std::cout << "Library version: " << {0}::getVersion() << std::endl;

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

// Test version
TEST(VersionTest, VersionNotEmpty) {{
    EXPECT_FALSE({0}::getVersion().empty());
}}

int main(int argc, char argv) {{
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

TEST_CASE("Version tests", "[Version]") {{
    SECTION("version is not empty") {{
        REQUIRE_FALSE({0}::getVersion().empty());
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

TEST_CASE("Version tests") {{
    SUBCASE("version is not empty") {{
        CHECK_FALSE({0}::getVersion().empty());
    }}
}}
)",
                     options_.projectName);
}

std::string LibraryTemplate::getBenchmarkContent() {
  return fmt::format(R"(#include <benchmark/benchmark.h>
#include "{0}/{0}.h"

// 简单的add函数基准测试
static void BM_Add(benchmark::State& state) {{
  // 这段代码会被计时
  for (auto _ : state) {{
    benchmark::DoNotOptimize({0}::add(42, 24));
  }}
}}
BENCHMARK(BM_Add);

// 使用不同输入值的基准测试
static void BM_Add_WithArgs(benchmark::State& state) {{
  const int a = state.range(0);
  const int b = state.range(1);
  for (auto _ : state) {{
    benchmark::DoNotOptimize({0}::add(a, b));
  }}
}}
// 测试不同输入组合
BENCHMARK(BM_Add_WithArgs)->Args({{1, 1}})->Args({{10, 10}})->Args({{100, 100}});

// 测试Example类
static void BM_ExampleGetValue(benchmark::State& state) {{
  {0}::Example example(state.range(0));
  for (auto _ : state) {{
    benchmark::DoNotOptimize(example.getValue());
  }}
}}
BENCHMARK(BM_ExampleGetValue)->Range(8, 8<<10);

BENCHMARK_MAIN();
)",
                     options_.projectName);
}

std::string LibraryTemplate::getInstallScriptContent() {
  return fmt::format(R"(#!/bin/bash
# Installation script for {0}

set -e

# Default installation directory
INSTALL_DIR="/usr/local"

# Parse command line arguments
while [[ "$#" -gt 0 ]]; do
  case $1 in
    --prefix=*) INSTALL_DIR="${{1#*=}}"; shift ;;
    --prefix) INSTALL_DIR="$2"; shift 2 ;;
    *) echo "Unknown parameter: $1"; exit 1 ;;
  esac
done

echo "Installing {0} to $INSTALL_DIR"

# Create build directory if it doesn't exist
if [ ! -d "build" ]; then
  mkdir -p build
fi

# Configure and build
if command -v cmake &> /dev/null; then
  cd build
  cmake .. -DCMAKE_INSTALL_PREFIX="$INSTALL_DIR"
  make -j$(nproc)
  make install
else
  echo "CMake not found. Please install CMake and try again."
  exit 1
fi

echo "{0} has been successfully installed to $INSTALL_DIR"
)",
                     options_.projectName);
}

std::string LibraryTemplate::getLicenseContent(const std::string &licenseType) {
  if (licenseType == "MIT") {
    return R"(MIT License

Copyright (c) 2023 [Author Name]

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.)";
  } else if (licenseType == "Apache") {
    return R"(                                 Apache License
                           Version 2.0, January 2004
                        http://www.apache.org/licenses/

   TERMS AND CONDITIONS FOR USE, REPRODUCTION, AND DISTRIBUTION

   1. Definitions.

      "License" shall mean the terms and conditions for use, reproduction,
      and distribution as defined by Sections 1 through 9 of this document.

      "Licensor" shall mean the copyright owner or entity authorized by
      the copyright owner that is granting the License.

      "Legal Entity" shall mean the union of the acting entity and all
      other entities that control, are controlled by, or are under common
      control with that entity. For the purposes of this definition,
      "control" means (i) the power, direct or indirect, to cause the
      direction or management of such entity, whether by contract or
      otherwise, or (ii) ownership of fifty percent (50%) or more of the
      outstanding shares, or (iii) beneficial ownership of such entity
```cpp
std::string LibraryTemplate::getLicenseContent(const std::string& licenseType) {
  if (licenseType == "MIT") {
    return R"(MIT License

Copyright (c) 2023 [Author Name]

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.)";
  }

  // Default return for unknown license types
  return "";
}

std::string LibraryTemplate::getClangTidyContent() {
  return R"(Checks: '*,
        -fuchsia-*,
        -google-*,
        -zircon-*,
        -abseil-*,
        -modernize-use-trailing-return-type,
        -llvm-*'
WarningsAsErrors: '*'
HeaderFilterRegex: ''
AnalyzeTemporaryDtors: false
FormatStyle: file
CheckOptions:
  - key: readability-identifier-length.MinimumVariableNameLength
    value: 2
  - key: readability-identifier-length.MinimumParameterNameLength
    value: 2
)";
}

std::string LibraryTemplate::getDoxyfileContent() {
  return fmt::format(R"(PROJECT_NAME           = {0}
PROJECT_NUMBER         = 0.1.0
PROJECT_BRIEF         = "A C++ library created with CPP-Scaffold"
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
INTERNAL_DOCS        = NO
SHOW_INCLUDE_FILES   = YES
FORCE_LOCAL_INCLUDES = NO
INLINE_INFO          = YES
SORT_MEMBER_DOCS     = YES
REFERENCES_RELATION  = NO
REFERENCES_LINK_SOURCE = YES
SOURCE_BROWSER       = YES
VERBATIM_HEADERS    = YES
ALPHABETICAL_INDEX  = YES
GENERATE_HTML       = YES
GENERATE_LATEX      = NO
GENERATE_XML        = YES
XML_PROGRAMLISTING  = YES
HAVE_DOT            = YES
UML_LOOK           = YES
CALL_GRAPH         = YES
CALLER_GRAPH       = YES
DOT_IMAGE_FORMAT   = svg
INTERACTIVE_SVG    = YES

INPUT               = src include
FILE_PATTERNS       = *.cpp *.h
RECURSIVE          = YES
)",
                     options_.projectName);
}

std::string LibraryTemplate::getSphinxConfigContent() {
  return fmt::format(
      R"(# Configuration file for the Sphinx documentation builder.

project = '{0}'
copyright = '2023, Author Name'
author = 'Author Name'
version = '0.1'
release = '0.1.0'

extensions = [
    'sphinx.ext.autodoc',
    'sphinx.ext.coverage',
    'sphinx.ext.napoleon',
    'breathe',
    'sphinx_rtd_theme',
]

templates_path = ['_templates']
source_suffix = '.rst'
master_doc = 'index'
language = None
exclude_patterns = ['_build', 'Thumbs.db', '.DS_Store']
pygments_style = 'sphinx'

html_theme = 'sphinx_rtd_theme'
html_static_path = ['_static']

# Breathe configuration
breathe_projects = {{ "{0}": "../doxygen/xml" }}
breathe_default_project = "{0}"
breathe_default_members = ('members', 'undoc-members')
)",
      options_.projectName);
}

std::string LibraryTemplate::getMkDocsContent() {
  return fmt::format(R"(site_name: {0}
site_description: 'Documentation for {0}'
site_author: 'Author Name'
docs_dir: docs/md
site_dir: site

theme:
  name: material
  features:
    - navigation.tabs
    - navigation.sections
    - navigation.top
    - search.suggest
    - search.highlight
  palette:
    primary: 'indigo'
    accent: 'indigo'

nav:
  - Home: index.md
  - Getting Started: getting-started.md
  - API Reference: api/index.md
  - Examples: examples/index.md
  - Contributing: contributing.md

markdown_extensions:
  - admonition
  - codehilite:
      guess_lang: false
  - toc:
      permalink: true
  - pymdownx.highlight
  - pymdownx.superfences

plugins:
  - search
  - mkdocstrings
)",
                     options_.projectName);
}

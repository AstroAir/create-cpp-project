#include "ci_config.h"

#include "../utils/file_utils.h"

#include <spdlog/spdlog.h>

using namespace utils;

bool CIConfig::createCIConfig(const std::string &projectPath,
                              const std::string &ciType,
                              const CliOptions &options) {
  spdlog::info("Creating CI/CD configuration for: " + ciType);

  if (ciType == "github") {
    return createGitHubActionsConfig(projectPath, options);
  } else if (ciType == "gitlab") {
    return createGitLabCIConfig(projectPath, options);
  } else if (ciType == "travis") {
    return createTravisCIConfig(projectPath, options);
  } else if (ciType == "appveyor") {
    return createAppVeyorConfig(projectPath, options);
  } else {
    spdlog::error("Unsupported CI/CD system: " + ciType);
    return false;
  }
}

bool CIConfig::createCIConfigs(const std::string &projectPath,
                               const std::vector<CiSystem> &ciTypes,
                               const CliOptions &options) {
  bool success = true;

  for (const auto &ciType : ciTypes) {
    if (!createCIConfig(projectPath, std::string(enums::to_string(ciType)), options)) {
      success = false;
    }
  }

  return success;
}

bool CIConfig::createGitHubActionsConfig(const std::string &projectPath,
                                         const CliOptions &options) {
  // Create .github/workflows directory
  std::string workflowsDir =
      FileUtils::combinePath(projectPath, ".github/workflows");
  if (!FileUtils::createDirectory(workflowsDir)) {
    spdlog::error("Failed to create .github/workflows directory");
    return false;
  }

  // Create workflow YAML file
  std::string workflowPath = FileUtils::combinePath(workflowsDir, "build.yml");
  if (!FileUtils::writeToFile(workflowPath,
                              getGitHubActionsYamlContent(options))) {
    spdlog::error("Failed to create GitHub Actions workflow file");
    return false;
  }

  spdlog::info("GitHub Actions configuration created successfully");
  return true;
}

bool CIConfig::createGitLabCIConfig(const std::string &projectPath,
                                    const CliOptions &options) {

  // Create .gitlab-ci.yml
  std::string gitlabCIPath =
      FileUtils::combinePath(projectPath, ".gitlab-ci.yml");
  if (!FileUtils::writeToFile(gitlabCIPath, getGitLabCIYamlContent(options))) {
    spdlog::error("Failed to create GitLab CI configuration file");
    return false;
  }

  spdlog::info("GitLab CI configuration created successfully");
  return true;
}

bool CIConfig::createTravisCIConfig(const std::string &projectPath,
                                    const CliOptions &options) {

  // Create .travis.yml
  std::string travisCIPath = FileUtils::combinePath(projectPath, ".travis.yml");
  if (!FileUtils::writeToFile(travisCIPath, getTravisCIYamlContent(options))) {
    spdlog::error("Failed to create Travis CI configuration file");
    return false;
  }

  spdlog::info("Travis CI configuration created successfully");
  return true;
}

bool CIConfig::createAppVeyorConfig(const std::string &projectPath,
                                    const CliOptions &options) {

  // Create appveyor.yml
  std::string appveyorPath =
      FileUtils::combinePath(projectPath, "appveyor.yml");
  if (!FileUtils::writeToFile(appveyorPath, getAppVeyorYamlContent(options))) {
    spdlog::error("Failed to create AppVeyor configuration file");
    return false;
  }

  spdlog::info("AppVeyor configuration created successfully");
  return true;
}

std::string CIConfig::getGitHubActionsYamlContent(const CliOptions &options) {
  std::string buildCommands;
  std::string testCommands;

  if (enums::to_string(options.buildSystem) == "cmake") {
    buildCommands = fmt::format(R"(      - name: Configure
                run: cmake -B build -DCMAKE_BUILD_TYPE=Release

            - name: Build
                run: cmake --build build --config Release)");

    if (options.includeTests) {
      testCommands = fmt::format(R"(
            - name: Test
                run: cd build && ctest -C Release --output-on-failure)");
    }
  } else if (enums::to_string(options.buildSystem) == "meson") {
    buildCommands = fmt::format(R"(      - name: Configure
                run: meson setup build

            - name: Build
                run: cd build && meson compile)");

    if (options.includeTests) {
      testCommands = fmt::format(R"(
            - name: Test
                run: cd build && meson test)");
    }
  } else if (enums::to_string(options.buildSystem) == "bazel") {
    buildCommands = fmt::format(R"(      - name: Build
                run: bazel build //...)");

    if (options.includeTests) {
      testCommands = fmt::format(R"(
            - name: Test
                run: bazel test //...)");
    }
  } else if (enums::to_string(options.buildSystem) == "xmake") {
    buildCommands = fmt::format(R"(      - name: Build
                run: xmake)");

    if (options.includeTests) {
      testCommands = fmt::format(R"(
            - name: Test
                run: xmake test)");
    }
  } else if (enums::to_string(options.buildSystem) == "premake") {
    buildCommands = fmt::format(R"(      - name: Configure
                run: premake5 gmake2

            - name: Build
                run: make config=release)");

    if (options.includeTests) {
      testCommands = fmt::format(R"(
            - name: Test
                run: bin/Release/{}_tests)",
                                 options.projectName);
    }
  }

  std::string packageManagerConfig;
  if (enums::to_string(options.packageManager) == "vcpkg") {
    packageManagerConfig = fmt::format(R"(      - name: Setup vcpkg
                uses: lukka/run-vcpkg@v7
                with:
                    vcpkgGitCommitId: master

)");
  } else if (enums::to_string(options.packageManager) == "conan") {
    packageManagerConfig = fmt::format(R"(      - name: Setup Conan
                run: |
                    pip install conan
                    conan profile new default --detect
                    conan install . --build=missing

)");
  } else {
    packageManagerConfig = "";
  }

  return fmt::format(R"(name: C++ CI

on:
    push:
        branches: [ main, master ]
    pull_request:
        branches: [ main, master ]

jobs:
    build-linux:
        runs-on: ubuntu-latest

        steps:
            - uses: actions/checkout@v2

{0}{1}{2}

    build-windows:
        runs-on: windows-latest

        steps:
            - uses: actions/checkout@v2

{0}{1}{2}

    build-macos:
        runs-on: macos-latest

        steps:
            - uses: actions/checkout@v2

{0}{1}{2})",
                     packageManagerConfig, buildCommands, testCommands);
}

std::string CIConfig::getGitLabCIYamlContent(const CliOptions &options) {
  std::string buildScript;
  std::string testScript;

  if (enums::to_string(options.buildSystem) == "cmake") {
    buildScript = R"(  script:
    - cmake -B build -DCMAKE_BUILD_TYPE=Release
    - cmake --build build --config Release)";

    if (options.includeTests) {
      testScript = R"(
    - cd build && ctest -C Release --output-on-failure)";
    }
  } else if (enums::to_string(options.buildSystem) == "meson") {
    buildScript = R"(  script:
    - meson setup build
    - cd build && meson compile)";

    if (options.includeTests) {
      testScript = R"(
    - cd build && meson test)";
    }
  } else if (enums::to_string(options.buildSystem) == "bazel") {
    buildScript = R"(  script:
    - bazel build //...)";

    if (options.includeTests) {
      testScript = R"(
    - bazel test //...)";
    }
  } else if (enums::to_string(options.buildSystem) == "xmake") {
    buildScript = R"(  script:
    - xmake)";

    if (options.includeTests) {
      testScript = R"(
    - xmake test)";
    }
  } else if (enums::to_string(options.buildSystem) == "premake") {
    buildScript = R"(  script:
    - premake5 gmake2
    - make config=release)";

    if (options.includeTests) {
      testScript = R"(
    - bin/Release/)" +
                   options.projectName + R"(_tests)";
    }
  }

  return R"(stages:
  - build
  - test

build:
  stage: build
  image: gcc:latest
)" +
         (enums::to_string(options.packageManager) == "vcpkg" ? R"(  before_script:
    - apt-get update && apt-get install -y git cmake ninja-build curl zip unzip tar pkg-config
    - git clone https://github.com/Microsoft/vcpkg.git
    - ./vcpkg/bootstrap-vcpkg.sh
    - ./vcpkg/vcpkg install )" + (options.networkLibrary ? *options.networkLibrary : "") +
                                                  R"(
)"
          : enums::to_string(options.packageManager) == "conan" ? R"(  before_script:
    - apt-get update && apt-get install -y git cmake python3-pip
    - pip3 install conan
    - conan profile new default --detect
    - conan install . --build=missing
)"
                                              : R"(  before_script:
    - apt-get update && apt-get install -y git cmake
)") + buildScript +
         testScript + R"(
  artifacts:
    paths:
      - build/
)" +
         (options.includeTests
              ? R"(

test:
  stage: test
  image: gcc:latest
  dependencies:
    - build
  script:
    - cd build && )" +
                    (enums::to_string(options.buildSystem) == "cmake"
                         ? "ctest -C Release --output-on-failure"
                     : enums::to_string(options.buildSystem) == "meson" ? "meson test"
                     : enums::to_string(options.buildSystem) == "bazel" ? "bazel test //..."
                     : enums::to_string(options.buildSystem) == "xmake"
                         ? "xmake test"
                         : "bin/Release/" + options.projectName + "_tests") +
                    R"(
)"
              : "");
}

std::string CIConfig::getTravisCIYamlContent(const CliOptions &options) {
  std::string buildScript;
  std::string testScript = "";

  if (enums::to_string(options.buildSystem) == "cmake") {
    buildScript = fmt::format(R"(  - cmake -B build -DCMAKE_BUILD_TYPE=Release
    - cmake --build build --config Release)");

    if (options.includeTests) {
      testScript = fmt::format(R"(
    - cd build && ctest -C Release --output-on-failure)");
    }
  } else if (enums::to_string(options.buildSystem) == "meson") {
    buildScript = fmt::format(R"(  - meson setup build
    - cd build && meson compile)");

    if (options.includeTests) {
      testScript = fmt::format(R"(
    - cd build && meson test)");
    }
  } else if (enums::to_string(options.buildSystem) == "bazel") {
    buildScript = fmt::format(R"(  - bazel build //...)");

    if (options.includeTests) {
      testScript = fmt::format(R"(
    - bazel test //...)");
    }
  } else if (enums::to_string(options.buildSystem) == "xmake") {
    buildScript = fmt::format(R"(  - xmake)");

    if (options.includeTests) {
      testScript = fmt::format(R"(
    - xmake test)");
    }
  } else if (enums::to_string(options.buildSystem) == "premake") {
    buildScript = fmt::format(R"(  - premake5 gmake2
    - make config=release)");

    if (options.includeTests) {
      testScript = fmt::format(R"(
    - bin/Release/{}_tests)",
                               options.projectName);
    }
  }

  return fmt::format(R"(language: cpp

compiler:
    - gcc
    - clang

os:
    - linux
    - osx

dist: focal
osx_image: xcode12.5

addons:
    apt:
        update: true
        packages:
            - cmake
            - ninja-build{}){}

before_install:{}{}{}{}

script:
{}{})",
                     enums::to_string(options.buildSystem) == "bazel" ? R"(
            - bazel)"
                                                    : "",
                     enums::to_string(options.buildSystem) == "meson" ? R"(
            - python3-pip
            - python3-setuptools
            - python3-wheel)"
                                                    : "",
                     enums::to_string(options.packageManager) == "vcpkg"
                         ? fmt::format(R"(
    - git clone https://github.com/Microsoft/vcpkg.git
    - ./vcpkg/bootstrap-vcpkg.sh
    - ./vcpkg/vcpkg install {})",
                                       options.networkLibrary.value_or("Unknown"))
                     : enums::to_string(options.packageManager) == "conan" ? R"(
    - pip3 install conan
    - conan profile new default --detect
    - conan install . --build=missing)"
                                                         : "",
                     enums::to_string(options.buildSystem) == "meson" ? R"(
    - pip3 install meson)"
                                                    : "",
                     enums::to_string(options.buildSystem) == "premake" ? R"(
    - wget https://github.com/premake/premake-core/releases/download/v5.0.0-alpha16/premake-5.0.0-alpha16-linux.tar.gz
    - tar -xf premake-5.0.0-alpha16-linux.tar.gz)"
                                                      : "",
                     enums::to_string(options.buildSystem) == "xmake" ? R"(
    - bash <(curl -fsSL https://xmake.io/shget.text))"
                                                    : "",
                     buildScript, testScript);
}

std::string CIConfig::getAppVeyorYamlContent(const CliOptions &options) {
  std::string buildCommands;
  std::string testCommands = "";

  if (enums::to_string(options.buildSystem) == "cmake") {
    buildCommands = R"(build_script:
  - cmake -B build -A x64 -DCMAKE_BUILD_TYPE=Release
  - cmake --build build --config Release)";

    if (options.includeTests) {
      testCommands = R"(
test_script:
  - cd build
  - ctest -C Release --output-on-failure)";
    }
  } else if (enums::to_string(options.buildSystem) == "meson") {
    buildCommands = R"(build_script:
  - meson setup build
  - cd build
  - meson compile)";

    if (options.includeTests) {
      testCommands = R"(
test_script:
  - cd build
  - meson test)";
    }
  } else if (enums::to_string(options.buildSystem) == "bazel") {
    buildCommands = R"(build_script:
  - bazel build //...)";

    if (options.includeTests) {
      testCommands = R"(
test_script:
  - bazel test //...)";
    }
  } else if (enums::to_string(options.buildSystem) == "xmake") {
    buildCommands = R"(build_script:
  - xmake)";

    if (options.includeTests) {
      testCommands = R"(
test_script:
  - xmake test)";
    }
  } else if (enums::to_string(options.buildSystem) == "premake") {
    buildCommands = R"(build_script:
  - premake5 vs2019
  - msbuild /p:Configuration=Release )" +
                    options.projectName + R"(.sln)";

    if (options.includeTests) {
      testCommands = R"(
test_script:
  - bin\Release\)" + options.projectName +
                     R"(_tests.exe)";
    }
  }

  return R"(image: Visual Studio 2019

platform:
  - x64

configuration:
  - Release

install:)" +
         (enums::to_string(options.packageManager) == "vcpkg" ? R"(
  - git clone https://github.com/Microsoft/vcpkg.git C:\vcpkg
  - C:\vcpkg\bootstrap-vcpkg.bat
  - C:\vcpkg\vcpkg.exe install )" + (options.networkLibrary ? *options.networkLibrary : "") +
                                                  R"( --triplet x64-windows)"
          : enums::to_string(options.packageManager) == "conan" ? R"(
  - set PATH=%PATH%;C:\Python38\Scripts
  - pip install conan
  - conan profile new default --detect
  - conan install . --build=missing)"
                                              : "") +
         (enums::to_string(options.buildSystem) == "meson" ? R"(
  - pip install meson ninja)"
                                         : "") +
         (enums::to_string(options.buildSystem) == "xmake" ? R"(
  - ps: Invoke-Expression (Invoke-WebRequest 'https://xmake.io/psget.text' -UseBasicParsing).Content)"
                                         : "") +
         (enums::to_string(options.buildSystem) == "premake" ? R"(
  - ps: Start-FileDownload 'https://github.com/premake/premake-core/releases/download/v5.0.0-alpha16/premake-5.0.0-alpha16-windows.zip'
  - 7z x premake-5.0.0-alpha16-windows.zip -o"C:\premake"
  - set PATH=%PATH%;C:\premake)"
                                           : "") +
         R"(

)" + buildCommands +
         testCommands + R"(

artifacts:
  - path: build\Release\*.exe
    name: )" +
         options.projectName + R"(-binaries
)";
}

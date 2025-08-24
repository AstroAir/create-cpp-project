#include "editor_config.h"
#include "../utils/file_utils.h"

#include <spdlog/spdlog.h>

using namespace utils;

bool EditorConfigManager::createEditorConfig(const std::string &projectPath,
                                      const std::string &editorType,
                                      const CliOptions &options) {

  spdlog::info("Creating editor configuration for: " + editorType);

  if (editorType == "vscode") {
    return createVSCodeConfig(projectPath, options);
  } else if (editorType == "clion") {
    return createCLionConfig(projectPath, options);
  } else if (editorType == "vs") {
    return createVisualStudioConfig(projectPath, options);
  } else {
    spdlog::error("Unsupported editor type: " + editorType);
    return false;
  }
}

bool EditorConfigManager::createEditorConfigs(
    const std::string &projectPath, const std::vector<EditorConfig> &editorTypes,
    const CliOptions &options) {
  bool success = true;

  for (const auto &editorType : editorTypes) {
    if (!createEditorConfig(projectPath, std::string(enums::to_string(editorType)), options)) {
      success = false;
    }
  }

  return success;
}

bool EditorConfigManager::createVSCodeConfig(const std::string &projectPath,
                                      const CliOptions &options) {

  // Create .vscode directory
  std::string vscodeDir = FileUtils::combinePath(projectPath, ".vscode");
  if (!FileUtils::createDirectory(vscodeDir)) {
    spdlog::error("Failed to create .vscode directory");
    return false;
  }

  // Create launch.json
  std::string launchJsonPath = FileUtils::combinePath(vscodeDir, "launch.json");
  if (!FileUtils::writeToFile(launchJsonPath,
                              getVSCodeLaunchJsonContent(options))) {
    spdlog::error("Failed to create launch.json");
    return false;
  }

  // Create tasks.json
  std::string tasksJsonPath = FileUtils::combinePath(vscodeDir, "tasks.json");
  if (!FileUtils::writeToFile(tasksJsonPath,
                              getVSCodeTasksJsonContent(options))) {
    spdlog::error("Failed to create tasks.json");
    return false;
  }

  // Create settings.json
  std::string settingsJsonPath =
      FileUtils::combinePath(vscodeDir, "settings.json");
  if (!FileUtils::writeToFile(settingsJsonPath,
                              getVSCodeSettingsJsonContent(options))) {
    spdlog::error("Failed to create settings.json");
    return false;
  }

  // Create c_cpp_properties.json
  std::string cppPropertiesJsonPath =
      FileUtils::combinePath(vscodeDir, "c_cpp_properties.json");
  if (!FileUtils::writeToFile(cppPropertiesJsonPath,
                              getVSCodeCppPropertiesJsonContent(options))) {
    spdlog::error("Failed to create c_cpp_properties.json");
    return false;
  }

  spdlog::info("VS Code configuration files created successfully");
  return true;
}

bool EditorConfigManager::createCLionConfig(const std::string &projectPath,
                                     const CliOptions &options) {

  // For CMake projects, create CMakePresets.json
  if (options.buildSystem == BuildSystem::CMake) {
    std::string cmakePresetsJsonPath =
        FileUtils::combinePath(projectPath, "CMakePresets.json");
    if (!FileUtils::writeToFile(cmakePresetsJsonPath,
                                getCLionCMakePresetsJsonContent(options))) {
      spdlog::error("Failed to create CMakePresets.json");
      return false;
    }
  }

  // CLion primarily uses CMake, so we just need minimal configuration
  // Create .idea directory for basic settings
  std::string ideaDir = FileUtils::combinePath(projectPath, ".idea");
  if (!FileUtils::createDirectory(ideaDir)) {
    spdlog::error("Failed to create .idea directory");
    return false;
  }

  // Create .idea/.name file
  std::string nameFilePath = FileUtils::combinePath(ideaDir, ".name");
  if (!FileUtils::writeToFile(nameFilePath, options.projectName)) {
    spdlog::error("Failed to create .idea/.name file");
    return false;
  }

  spdlog::info("CLion configuration files created successfully");
  return true;
}

bool EditorConfigManager::createVisualStudioConfig(const std::string &projectPath,
                                            const CliOptions &options) {

  // Create .vsconfig
  std::string vsconfigPath = FileUtils::combinePath(projectPath, ".vsconfig");
  if (!FileUtils::writeToFile(vsconfigPath,
                              getVisualStudioConfigContent(options))) {
    spdlog::error("Failed to create .vsconfig");
    return false;
  }

  // Create Directory.Build.props
  std::string propsPath =
      FileUtils::combinePath(projectPath, "Directory.Build.props");
  if (!FileUtils::writeToFile(propsPath,
                              getVisualStudioPropsContent(options))) {
    spdlog::error("Failed to create Directory.Build.props");
    return false;
  }

  spdlog::info("Visual Studio configuration files created successfully");
  return true;
}

std::string
EditorConfigManager::getVSCodeLaunchJsonContent(const CliOptions &options) {
  std::string buildDir;
  std::string exePath;

  if (options.buildSystem == BuildSystem::CMake) {
    buildDir = "build";
    exePath = "${workspaceFolder}/build/" + options.projectName;
  } else if (options.buildSystem == BuildSystem::Meson) {
    buildDir = "build";
    exePath = "${workspaceFolder}/build/" + options.projectName;
  } else if (options.buildSystem == BuildSystem::Bazel) {
    buildDir = "bazel-bin";
    exePath = "${workspaceFolder}/bazel-bin/" + options.projectName;
  } else if (options.buildSystem == BuildSystem::XMake) {
    buildDir = "build";
    exePath = "${workspaceFolder}/build/" + options.projectName;
  } else if (options.buildSystem == BuildSystem::Premake) {
    buildDir = "bin/Debug";
    exePath = "${workspaceFolder}/bin/Debug/" + options.projectName;
  }

  return R"({
    "version": "0.2.0",
    "configurations": [
        {
            "name": "C++ Launch",
            "type": "cppdbg",
            "request": "launch",
            "program": ")" +
         exePath + R"(",
            "args": [],
            "stopAtEntry": false,
            "cwd": "${workspaceFolder}",
            "environment": [],
            "externalConsole": false,
            "MIMode": "gdb",
            "setupCommands": [
                {
                    "description": "Enable pretty-printing for gdb",
                    "text": "-enable-pretty-printing",
                    "ignoreFailures": true
                }
            ],
            "preLaunchTask": "build",
            "miDebuggerPath": "/usr/bin/gdb"
        }
    ]
}
)";
}

std::string EditorConfigManager::getVSCodeTasksJsonContent(const CliOptions &options) {
  std::string buildCommand;

  if (options.buildSystem == BuildSystem::CMake) {
    buildCommand = "cmake --build build";
  } else if (options.buildSystem == BuildSystem::Meson) {
    buildCommand = "cd build && meson compile";
  } else if (options.buildSystem == BuildSystem::Bazel) {
    buildCommand = "bazel build //...";
  } else if (options.buildSystem == BuildSystem::XMake) {
    buildCommand = "xmake";
  } else if (options.buildSystem == BuildSystem::Premake) {
    buildCommand = "make";
  }

  std::string testCommand;
  if (options.includeTests) {
    if (options.buildSystem == BuildSystem::CMake) {
      testCommand = "cd build && ctest";
    } else if (options.buildSystem == BuildSystem::Meson) {
      testCommand = "cd build && meson test";
    } else if (options.buildSystem == BuildSystem::Bazel) {
      testCommand = "bazel test //...";
    } else if (options.buildSystem == BuildSystem::XMake) {
      testCommand = "xmake test";
    } else if (options.buildSystem == BuildSystem::Premake) {
      testCommand = "bin/Debug/" + options.projectName + "_tests";
    }
  }

  std::string tasksJson = R"({
    "version": "2.0.0",
    "tasks": [
        {
            "label": "build",
            "type": "shell",
            "command": ")" +
                          buildCommand + R"(",
            "group": {
                "kind": "build",
                "isDefault": true
            },
            "presentation": {
                "reveal": "always",
                "panel": "new"
            },
            "problemMatcher": ["$gcc"]
        })";

  // Add test task if tests are included
  if (options.includeTests && !testCommand.empty()) {
    tasksJson += R"(,
        {
            "label": "test",
            "type": "shell",
            "command": ")" +
                 testCommand + R"(",
            "group": {
                "kind": "test",
                "isDefault": true
            },
            "presentation": {
                "reveal": "always",
                "panel": "new"
            }
        })";
  }

  // Add clean task
  std::string cleanCommand;
  if (options.buildSystem == BuildSystem::CMake) {
    cleanCommand = "rm -rf build";
  } else if (options.buildSystem == BuildSystem::Meson) {
    cleanCommand = "rm -rf build";
  } else if (options.buildSystem == BuildSystem::Bazel) {
    cleanCommand = "bazel clean";
  } else if (options.buildSystem == BuildSystem::XMake) {
    cleanCommand = "xmake clean";
  } else if (options.buildSystem == BuildSystem::Premake) {
    cleanCommand = "make clean";
  }

  tasksJson += R"(,
        {
            "label": "clean",
            "type": "shell",
            "command": ")" +
               cleanCommand + R"(",
            "presentation": {
                "reveal": "always",
                "panel": "new"
            }
        }
    ]
})";

  return tasksJson;
}

std::string
EditorConfigManager::getVSCodeSettingsJsonContent(const CliOptions &options) {
  (void)options; // TODO: Use options to customize VS Code settings
  return R"({
    "C_Cpp.default.configurationProvider": "ms-vscode.cmake-tools",
    "C_Cpp.default.includePath": [
        "${workspaceFolder}/include",
        "${workspaceFolder}/src"
    ],
    "C_Cpp.default.cppStandard": "c++17",
    "editor.formatOnSave": true,
    "editor.formatOnType": true,
    "files.associations": {
        "*.h": "cpp",
        "*.hpp": "cpp",
        "*.cpp": "cpp",
        "*.cc": "cpp"
    }
}
)";
}

std::string
EditorConfigManager::getVSCodeCppPropertiesJsonContent(const CliOptions &options) {
  (void)options; // TODO: Use options to customize C++ properties
  return R"({
    "configurations": [
        {
            "name": "Linux",
            "includePath": [
                "${workspaceFolder}/",
                "${workspaceFolder}/include"
            ],
            "defines": [],
            "compilerPath": "/usr/bin/g++",
            "cStandard": "c11",
            "cppStandard": "c++17",
            "intelliSenseMode": "gcc-x64"
        },
        {
            "name": "Mac",
            "includePath": [
                "${workspaceFolder}/",
                "${workspaceFolder}/include"
            ],
            "defines": [],
            "compilerPath": "/usr/bin/clang++",
            "cStandard": "c11",
            "cppStandard": "c++17",
            "intelliSenseMode": "clang-x64"
        },
        {
            "name": "Win32",
            "includePath": [
                "${workspaceFolder}/",
                "${workspaceFolder}/include"
            ],
            "defines": [
                "_DEBUG",
                "UNICODE",
                "_UNICODE"
            ],
            "windowsSdkVersion": "10.0.18362.0",
            "compilerPath": "C:/Program Files/LLVM/bin/clang-cl.exe",
            "cStandard": "c11",
            "cppStandard": "c++17",
            "intelliSenseMode": "msvc-x64"
        }
    ],
    "version": 4
}
)";
}

std::string
EditorConfigManager::getCLionCMakePresetsJsonContent(const CliOptions &options) {
  (void)options; // TODO: Use options to customize CMake presets
  return R"({
    "version": 3,
    "configurePresets": [
        {
            "name": "debug",
            "displayName": "Debug",
            "description": "Debug build with full symbols",
            "generator": "Ninja",
            "binaryDir": "${sourceDir}/build/debug",
            "cacheVariables": {
                "CMAKE_BUILD_TYPE": "Debug",
                "CMAKE_CXX_STANDARD": "17",
                "CMAKE_EXPORT_COMPILE_COMMANDS": "ON"
            }
        },
        {
            "name": "release",
            "displayName": "Release",
            "description": "Optimized release build",
            "generator": "Ninja",
            "binaryDir": "${sourceDir}/build/release",
            "cacheVariables": {
                "CMAKE_BUILD_TYPE": "Release",
                "CMAKE_CXX_STANDARD": "17",
                "CMAKE_EXPORT_COMPILE_COMMANDS": "ON"
            }
        },
        {
            "name": "profile",
            "displayName": "Profile",
            "description": "Build with profiling information",
            "generator": "Ninja",
            "binaryDir": "${sourceDir}/build/profile",
            "cacheVariables": {
                "CMAKE_BUILD_TYPE": "RelWithDebInfo",
                "CMAKE_CXX_STANDARD": "17",
                "CMAKE_EXPORT_COMPILE_COMMANDS": "ON"
            }
        }
    ],
    "buildPresets": [
        {
            "name": "debug",
            "configurePreset": "debug"
        },
        {
            "name": "release",
            "configurePreset": "release"
        },
        {
            "name": "profile",
            "configurePreset": "profile"
        }
    ],
    "testPresets": [
        {
            "name": "debug",
            "configurePreset": "debug",
            "output": {"outputOnFailure": true},
            "execution": {"stopOnFailure": true}
        },
        {
            "name": "release",
            "configurePreset": "release",
            "output": {"outputOnFailure": true},
            "execution": {"stopOnFailure": true}
        }
    ]
}
)";
}

std::string
EditorConfigManager::getVisualStudioConfigContent(const CliOptions &options) {
  (void)options; // TODO: Use options to customize Visual Studio configuration
  return R"({
  "version": "1.0",
  "components": [
    "Microsoft.VisualStudio.Component.CoreEditor",
    "Microsoft.VisualStudio.Workload.CoreEditor",
    "Microsoft.VisualStudio.Component.NuGet",
    "Microsoft.VisualStudio.Component.Roslyn.Compiler",
    "Microsoft.Component.MSBuild",
    "Microsoft.VisualStudio.Component.TextTemplating",
    "Microsoft.VisualStudio.Component.IntelliCode",
    "Microsoft.VisualStudio.Component.VC.CoreIde",
    "Microsoft.VisualStudio.Component.VC.Tools.x86.x64",
    "Microsoft.VisualStudio.Component.Graphics.Tools",
    "Microsoft.VisualStudio.Component.VC.DiagnosticTools",
    "Microsoft.VisualStudio.Component.Windows10SDK.19041",
    "Microsoft.VisualStudio.Component.VC.Redist.14.Latest",
    "Microsoft.VisualStudio.ComponentGroup.NativeDesktop.Core",
    "Microsoft.VisualStudio.Component.VC.ATL",
    "Microsoft.VisualStudio.Component.VC.TestAdapterForBoostTest",
    "Microsoft.VisualStudio.Component.VC.TestAdapterForGoogleTest",
    "Microsoft.VisualStudio.Workload.NativeDesktop"
  ]
}
)";
}

std::string
EditorConfigManager::getVisualStudioPropsContent(const CliOptions &options) {
  (void)options; // TODO: Use options to customize Visual Studio props
  return R"(<?xml version="1.0" encoding="utf-8"?>
<Project xmlns="http://schemas.microsoft.com/developer/msbuild/2003">
  <ItemGroup Label="ProjectConfigurations">
    <ProjectConfiguration Include="Debug|Win32">
      <Configuration>Debug</Configuration>
      <Platform>Win32</Platform>
    </ProjectConfiguration>
    <ProjectConfiguration Include="Release|Win32">
      <Configuration>Release</Configuration>
      <Platform>Win32</Platform>
    </ProjectConfiguration>
    <ProjectConfiguration Include="Debug|x64">
      <Configuration>Debug</Configuration>
      <Platform>x64</Platform>
    </ProjectConfiguration>
    <ProjectConfiguration Include="Release|x64">
      <Configuration>Release</Configuration>
      <Platform>x64</Platform>
    </ProjectConfiguration>
  </ItemGroup>
  <PropertyGroup Label="Globals">
    <VCProjectVersion>16.0</VCProjectVersion>
    <WindowsTargetPlatformVersion>10.0</WindowsTargetPlatformVersion>
  </PropertyGroup>
  <PropertyGroup>
    <OutDir>$(SolutionDir)bin\$(Configuration)\</OutDir>
    <IntDir>$(SolutionDir)obj\$(Configuration)\</IntDir>
  </PropertyGroup>
  <ItemDefinitionGroup>
    <ClCompile>
      <WarningLevel>Level3</WarningLevel>
      <SDLCheck>true</SDLCheck>
      <ConformanceMode>true</ConformanceMode>
      <LanguageStandard>stdcpp17</LanguageStandard>
      <AdditionalIncludeDirectories>$(ProjectDir)include;%(AdditionalIncludeDirectories)</AdditionalIncludeDirectories>
    </ClCompile>
    <Link>
      <SubSystem>Console</SubSystem>
      <GenerateDebugInformation>true</GenerateDebugInformation>
    </Link>
  </ItemDefinitionGroup>
  <ItemDefinitionGroup Condition="'$(Configuration)'=='Debug'">
    <ClCompile>
      <PreprocessorDefinitions>_DEBUG;%(PreprocessorDefinitions)</PreprocessorDefinitions>
      <Optimization>Disabled</Optimization>
    </ClCompile>
  </ItemDefinitionGroup>
  <ItemDefinitionGroup Condition="'$(Configuration)'=='Release'">
    <ClCompile>
      <PreprocessorDefinitions>NDEBUG;%(PreprocessorDefinitions)</PreprocessorDefinitions>
      <Optimization>MaxSpeed</Optimization>
      <FunctionLevelLinking>true</FunctionLevelLinking>
      <IntrinsicFunctions>true</IntrinsicFunctions>
    </ClCompile>
    <Link>
      <EnableCOMDATFolding>true</EnableCOMDATFolding>
      <OptimizeReferences>true</OptimizeReferences>
    </Link>
  </ItemDefinitionGroup>
</Project>
)";
}
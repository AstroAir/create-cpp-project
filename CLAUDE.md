# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

CPP-Scaffold is a C++ project scaffolding tool that creates project templates with various configurations. The executable is named `cpp-scaffold` and helps developers quickly set up C++ projects with different architectures, build systems, package managers, and development tools.

## Build Commands

### Standard Build

```bash
# Create build directory and configure
mkdir build && cd build
cmake .. -G Ninja

# Build the project
ninja

# Or use the provided build script on Windows
./build.bat
```

### Testing

```bash
# Run all tests
cd build
ctest

# Run specific test
./test_file_utils
./test_template_manager
./all_tests

# Build with testing enabled (default)
cmake .. -DBUILD_TESTING=ON
```

### Development Build with Debug Info

```bash
mkdir build && cd build
cmake .. -G Ninja -DCMAKE_BUILD_TYPE=Debug
ninja
```

## Core Architecture

### Template System

The project uses a factory pattern for templates:

- `TemplateBase` (src/templates/template_base.h) - Abstract base class with common functionality
- `TemplateManager` (src/templates/template_manager.h) - Factory that creates appropriate template instances
- Multiple concrete templates: ConsoleTemplate, LibraryTemplate, GuiTemplate, NetworkTemplate, etc.

Each template implements:

- `createProjectStructure()` - Creates directory layout
- `createBuildSystem()` - Generates CMakeLists.txt, Meson, etc.
- `setupPackageManager()` - Configures vcpkg, Conan
- `setupTestFramework()` - Integrates GTest, Catch2, etc.

### CLI Architecture  

- `CliParser` (src/cli/cli_parser.h) - Command line argument parsing with rich enum types
- `ProjectWizard` (src/cli/project_wizard.cpp) - Interactive project creation wizard
- `UserInput` class - Type-safe input validation and styled prompts

### Configuration System

Located in src/config/:

- `ConfigManager` - Persistent settings and profiles
- `EditorConfig` - VSCode, CLion, etc. integration
- `CiConfig` - GitHub Actions, GitLab CI setup
- `CodeStyleConfig` - clang-format, clang-tidy configuration

### Utility Libraries

- `FileUtils` - Cross-platform file operations
- `TerminalUtils` - Colored output, progress indicators, table formatting
- `GitUtils` - Git repository initialization
- `FrameworkIntegration` - Support for Qt, SFML, Boost frameworks

## Dependencies

The project uses FetchContent with a custom caching system (deps_cache/) to avoid re-downloading dependencies:

- spdlog v1.12.0 - Logging framework
- nlohmann/json v3.11.2 - JSON parsing
- Google Test v1.14.0 - Testing framework (when BUILD_TESTING=ON)

## Project Structure

```txt
src/
├── cli/                    # Command-line interface
├── templates/              # Project template implementations  
├── config/                 # Configuration management
├── utils/                  # Utility libraries
├── testing/                # Test framework integration
└── documentation/          # Documentation generation

tests/                      # Unit tests using Google Test
deps_cache/                 # Cached dependencies
```

## Key Features

- Multi-language support with `Localization` class
- Template-based project generation for various C++ project types
- Interactive wizard mode with styled terminal output
- Persistent configuration profiles
- Integrated CI/CD setup for multiple platforms
- Framework integration (Qt, SFML, Boost, game engines)
- Cross-platform support (Windows, Linux, macOS)

## Development Notes

- C++17 standard required
- Uses modern CMake (3.14+) with FetchContent
- Extensive use of enums with string conversion utilities
- Logging configured in main.cpp with both console and file output
- Template factory pattern allows easy addition of new project types
- Windows builds require shlwapi library linkage

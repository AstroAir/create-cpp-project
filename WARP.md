# WARP.md

This file provides guidance to WARP (warp.dev) when working with code in this repository.

## Project Overview

CPP-Scaffold is a modern C++ project scaffolding tool that creates well-structured C++ projects with various configurations. The main executable is `cpp-scaffold` and helps developers quickly set up C++ projects with different architectures, build systems, package managers, and development tools.

## Essential Commands

### Building
```bash
# Quick build (Windows)
build.bat

# Build with CMake presets (recommended)
cmake --preset default && cmake --build --preset default

# Using the build script (Linux/macOS)
./scripts/build.sh --config release --tests

# Debug build with sanitizers  
cmake --preset debug-sanitized && cmake --build --preset debug-sanitized

# Fast build for development
cmake --preset fast-build && cmake --build --preset fast-build
```

### Testing
```bash
# Run all tests
ctest --preset default

# Run tests in parallel
ctest --preset parallel

# Run with sanitizers
ctest --preset sanitized

# Run specific test
./build/test_file_utils
./build/all_tests
```

### Development
```bash
# Lint/format code (if available)
cmake --build --preset default --target clang-format
cmake --build --preset default --target clang-tidy

# Install locally
cmake --install build --prefix /usr/local
```

## Architecture Overview

### Core Components

**Template System (Factory Pattern)**
- `TemplateBase` - Abstract base class for all project templates
- `TemplateManager` - Factory that creates appropriate template instances
- Concrete templates: `ConsoleTemplate`, `LibraryTemplate`, `GuiTemplate`, `NetworkTemplate`, `EmbeddedTemplate`, etc.
- Each template implements: `createProjectStructure()`, `createBuildSystem()`, `setupPackageManager()`, `setupTestFramework()`

**CLI Architecture**
- `CliParser` - Command line parsing with rich enum types
- `ProjectWizard` - Interactive project creation wizard with styled output
- `UserInput` - Type-safe input validation and prompts

**Configuration System** 
- `ConfigManager` - Persistent settings and user profiles
- `EditorConfig` - IDE integration (VSCode, CLion, etc.)
- `CiConfig` - CI/CD setup (GitHub Actions, GitLab CI, etc.)
- `CodeStyleConfig` - clang-format/clang-tidy configurations

**Utility Libraries**
- `FileUtils` - Cross-platform file operations
- `TerminalUtils` - Colored output, progress indicators, table formatting
- `GitUtils` - Git repository initialization
- `FrameworkIntegration` - Qt, SFML, Boost framework support

### Project Structure
```
src/
├── cli/                    # Command-line interface components
├── templates/              # Project template implementations
├── config/                 # Configuration management
├── utils/                  # Cross-platform utility libraries
├── testing/                # Test framework integration
└── documentation/          # Documentation generation

tests/                      # Unit tests using Google Test
├── common/                 # Test fixtures and helpers
├── integration/            # End-to-end integration tests
└── platform/              # Platform-specific tests
```

## Build System Details

**CMake Presets Available:**
- `default` - Standard release build with optimizations
- `debug` - Debug with all warnings and development tools
- `fast-build` - Maximum build speed (tests disabled)
- `minimal` - Basic build without optimizations
- `debug-sanitized` - Debug with AddressSanitizer and UBSan
- `profiling` - Build-time profiling enabled

**Dependencies (FetchContent with caching):**
- spdlog v1.12.0 - Structured logging
- nlohmann/json v3.11.3 - JSON parsing
- Google Test v1.14.0 - Unit testing framework

**Component Libraries:**
The project is built as separate component libraries linked to the main executable:
- `cpp_scaffold_cli` - CLI parsing and user interaction
- `cpp_scaffold_templates` - Template system
- `cpp_scaffold_utils` - Utility functions
- `cpp_scaffold_config` - Configuration management
- `cpp_scaffold_testing` - Test framework integration
- `cpp_scaffold_documentation` - Documentation generation

## Development Notes

**Language Standard:** C++17 minimum, supports C++20 features
**Build Requirements:** CMake 3.20+, requires Ninja on Windows
**Key Patterns:**
- Extensive use of enums with string conversion utilities
- Template factory pattern for extensible project types
- Component-based architecture with separate libraries
- Cross-platform support with Windows-specific shlwapi library linkage

**Testing Strategy:**
- Unit tests for individual components
- Integration tests for end-to-end functionality  
- Platform-specific tests (including MSYS2 integration)
- Sanitizer builds for memory safety validation

## Code Quality Tools

The project includes comprehensive tooling:
- clang-format and clang-tidy configuration
- Multiple sanitizer builds (AddressSanitizer, UBSanitizer)
- Build-time profiling capability
- Extensive CMake preset system for different development scenarios

## Important Files to Know

- `src/main.cpp` - Application entry point with logging setup
- `src/templates/template_manager.h` - Core template factory
- `src/cli/cli_parser.h` - Command line interface
- `CMakePresets.json` - Build configuration presets
- `dependencies.json` - Dependency version specifications
- `CLAUDE.md` - Additional architectural details

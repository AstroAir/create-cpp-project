# Modern Modular CMake Build System for cpp-scaffold
cmake_minimum_required(VERSION 3.20)
project(cpp-scaffold VERSION 1.0.0 LANGUAGES CXX)

# Add cmake modules to path
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules")

# Include modular CMake components
include(CompilerSettings)
include(BuildTypes)
include(BuildOptimizations)
include(Dependencies)
include(ProjectUtils)
include(UtilityFunctions)
include(PlatformSupport)
include(Testing)
include(TestingEnhanced)
include(DevTools)
include(Packaging)

# Configure global settings
setup_platform_support()
configure_global_cpp_standard()
configure_build_types()
configure_global_compiler_warnings()
apply_global_build_optimizations()

# Setup dependencies
setup_all_dependencies()

# Component-based library targets
# Create separate libraries for each major component

# CLI Component Library
collect_sources(CLI_SOURCES
    DIRECTORIES src/cli
    RECURSIVE
)
create_library(cpp_scaffold_cli
    SOURCES ${CLI_SOURCES}
    INCLUDE_DIRS src
    FOLDER "Components"
)
link_core_dependencies(cpp_scaffold_cli)

# Templates Component Library
collect_sources(TEMPLATES_SOURCES
    DIRECTORIES src/templates
    RECURSIVE
)
create_library(cpp_scaffold_templates
    SOURCES ${TEMPLATES_SOURCES}
    INCLUDE_DIRS src
    LINK_LIBRARIES cpp_scaffold_cli cpp_scaffold_utils
    FOLDER "Components"
)
link_core_dependencies(cpp_scaffold_templates)

# Utils Component Library
collect_sources(UTILS_SOURCES
    DIRECTORIES src/utils
    RECURSIVE
)
create_library(cpp_scaffold_utils
    SOURCES ${UTILS_SOURCES}
    INCLUDE_DIRS src
    FOLDER "Components"
)
link_core_dependencies(cpp_scaffold_utils)

# Config Component Library
collect_sources(CONFIG_SOURCES
    DIRECTORIES src/config
    RECURSIVE
)
create_library(cpp_scaffold_config
    SOURCES ${CONFIG_SOURCES}
    INCLUDE_DIRS src
    LINK_LIBRARIES cpp_scaffold_utils
    FOLDER "Components"
)
link_core_dependencies(cpp_scaffold_config)

# Testing Component Library
collect_sources(TESTING_SOURCES
    DIRECTORIES src/testing
    RECURSIVE
)
create_library(cpp_scaffold_testing
    SOURCES ${TESTING_SOURCES}
    INCLUDE_DIRS src
    LINK_LIBRARIES cpp_scaffold_utils
    FOLDER "Components"
)
link_core_dependencies(cpp_scaffold_testing)

# Documentation Component Library
collect_sources(DOCUMENTATION_SOURCES
    DIRECTORIES src/documentation
    RECURSIVE
)
create_library(cpp_scaffold_documentation
    SOURCES ${DOCUMENTATION_SOURCES}
    INCLUDE_DIRS src
    LINK_LIBRARIES cpp_scaffold_utils
    FOLDER "Components"
)
link_core_dependencies(cpp_scaffold_documentation)

# Main executable target (simplified for build test)
create_executable(cpp-scaffold
    SOURCES src/main_simple.cpp
    OUTPUT_NAME "cpp-scaffold"
    INCLUDE_DIRS src
    LINK_LIBRARIES
        # Only link spdlog for the simple version
    FOLDER "Executables"
)

# Link core dependencies to main executable
link_core_dependencies(cpp-scaffold)

# Installation and Packaging
setup_complete_packaging(cpp-scaffold ${PROJECT_VERSION})

# Testing Configuration
add_subdirectory(tests)

# Developer Tools
setup_dev_tools(MAIN_TARGET cpp-scaffold)

# Print build configuration summary
print_build_summary()

# Backup of original CMakeLists.txt created for modernization
# Original minimum version was 3.14, updated for modern features
cmake_minimum_required(VERSION 3.20)
project(cpp-scaffold VERSION 1.0.0 LANGUAGES CXX)

# Modern C++ standard configuration with compatibility focus
# Use C++17 for better compatibility with current dependencies
# C++20 can be enabled via option for testing
option(ENABLE_CPP20 "Enable C++20 standard (experimental)" OFF)
option(ENABLE_STRICT_WARNINGS "Enable strict warning mode" OFF)

if(ENABLE_CPP20)
    if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC" AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 19.29)
        set(CMAKE_CXX_STANDARD 20)
        message(STATUS "Using C++20 standard")
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 11.0)
        set(CMAKE_CXX_STANDARD 20)
        message(STATUS "Using C++20 standard")
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang" AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 13.0)
        set(CMAKE_CXX_STANDARD 20)
        message(STATUS "Using C++20 standard")
    else()
        set(CMAKE_CXX_STANDARD 17)
        message(STATUS "C++20 requested but not supported, falling back to C++17")
    endif()
else()
    set(CMAKE_CXX_STANDARD 17)
    message(STATUS "Using C++17 standard")
endif()

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Enable IDE integration and modern CMake features
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Modern compiler configuration with enhanced warnings and optimizations
# Build type specific configurations
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build" FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

# Enhanced compiler warnings and flags
if(MSVC)
    # MSVC specific flags
    add_compile_options(
        /W4                     # High warning level
        /permissive-           # Disable non-conforming code
        /Zc:__cplusplus        # Enable correct __cplusplus macro
        /utf-8                 # UTF-8 source and execution character sets
    )
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        add_compile_options(/WX)  # Treat warnings as errors in debug
    endif()
    # Windows Unicode support
    add_compile_definitions(UNICODE _UNICODE WIN32_LEAN_AND_MEAN NOMINMAX)
else()
    # GCC/Clang flags - balanced approach for existing codebase
    add_compile_options(
        -Wall -Wextra -Wpedantic
        # Temporarily disable strict conversion warnings for existing code
        # -Wconversion -Wsign-conversion
        -Wcast-qual -Wcast-align
        -Wunused -Wunreachable-code
    )
    # Only treat warnings as errors in strict mode
    if(CMAKE_BUILD_TYPE STREQUAL "Debug" AND ENABLE_STRICT_WARNINGS)
        add_compile_options(-Werror)
    endif()
    if(WIN32)
        add_compile_definitions(UNICODE _UNICODE)
    endif()
endif()

# Performance and build optimization options
option(ENABLE_UNITY_BUILD "Enable Unity builds for faster compilation" OFF)
option(ENABLE_PRECOMPILED_HEADERS "Enable precompiled headers" ON)
option(ENABLE_CCACHE "Enable ccache for faster rebuilds" ON)
option(ENABLE_LTO "Enable Link Time Optimization" OFF)
option(ENABLE_BUILD_PROFILING "Enable build time profiling" OFF)
option(ENABLE_SANITIZERS "Enable sanitizers in debug builds" OFF)

# Advanced build optimizations
if(ENABLE_CCACHE)
    find_program(CCACHE_PROGRAM ccache)
    if(CCACHE_PROGRAM)
        message(STATUS "Found ccache: ${CCACHE_PROGRAM}")
        set(CMAKE_CXX_COMPILER_LAUNCHER "${CCACHE_PROGRAM}")
        set(CMAKE_C_COMPILER_LAUNCHER "${CCACHE_PROGRAM}")
    else()
        message(STATUS "ccache not found, continuing without it")
    endif()
endif()

# Link Time Optimization (LTO)
if(ENABLE_LTO)
    include(CheckIPOSupported)
    check_ipo_supported(RESULT LTO_SUPPORTED OUTPUT LTO_ERROR)
    if(LTO_SUPPORTED)
        message(STATUS "Link Time Optimization enabled")
        set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
    else()
        message(WARNING "LTO not supported: ${LTO_ERROR}")
    endif()
endif()

# Build time profiling
if(ENABLE_BUILD_PROFILING)
    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        add_compile_options(-ftime-trace)
        message(STATUS "Clang build time profiling enabled")
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
        add_compile_options(-ftime-report)
        message(STATUS "GCC build time profiling enabled")
    endif()
endif()

# Sanitizers for debug builds
if(ENABLE_SANITIZERS AND CMAKE_BUILD_TYPE STREQUAL "Debug")
    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID MATCHES "GNU")
        add_compile_options(
            -fsanitize=address
            -fsanitize=undefined
            -fno-omit-frame-pointer
        )
        add_link_options(
            -fsanitize=address
            -fsanitize=undefined
        )
        message(STATUS "Sanitizers enabled for debug builds")
    endif()
endif()

# Third-party library configuration
include(FetchContent)

# Set global cache directory for dependencies
set(DEPS_CACHE_DIR "${CMAKE_SOURCE_DIR}/deps_cache" CACHE PATH "Dependency cache directory")
file(MAKE_DIRECTORY ${DEPS_CACHE_DIR})

# Configure FetchContent for better performance
set(FETCHCONTENT_QUIET OFF)
set(FETCHCONTENT_UPDATES_DISCONNECTED ON)

# 自定义函数用于缓存下载的依赖项
function(fetch_and_cache NAME GIT_REPO GIT_TAG)
    # 创建缓存目录路径，替换所有特殊字符
    string(REPLACE "/" "_" SAFE_REPO_NAME ${GIT_REPO})
    string(REPLACE ":" "_" SAFE_REPO_NAME ${SAFE_REPO_NAME})
    string(REPLACE "." "_" SAFE_REPO_NAME ${SAFE_REPO_NAME})
    set(CACHE_DIR "${DEPS_CACHE_DIR}/${NAME}_${SAFE_REPO_NAME}_${GIT_TAG}")

    # 检查缓存目录是否存在
    if(EXISTS ${CACHE_DIR})
        message(STATUS "使用缓存的依赖项: ${NAME} (${GIT_TAG})")
        FetchContent_Declare(
            ${NAME}
            SOURCE_DIR ${CACHE_DIR}
        )
    else()
        message(STATUS "下载依赖项: ${NAME} (${GIT_TAG})")
        FetchContent_Declare(
            ${NAME}
            GIT_REPOSITORY ${GIT_REPO}
            GIT_TAG ${GIT_TAG}
            SOURCE_DIR ${CACHE_DIR}
        )
    endif()

    # 使依赖项可用
    FetchContent_MakeAvailable(${NAME})
endfunction()

# 检查系统是否已安装spdlog
find_package(spdlog QUIET)
if(NOT spdlog_FOUND)
    message(STATUS "System spdlog not found, will use fetched version")
    # 使用缓存功能添加spdlog
    fetch_and_cache(
        spdlog
        https://github.com/gabime/spdlog.git
        v1.12.0
    )
else()
    message(STATUS "Using system spdlog: ${spdlog_VERSION}")
endif()

# 检查系统是否已安装nlohmann_json
find_package(nlohmann_json QUIET)
if(NOT nlohmann_json_FOUND)
    message(STATUS "System nlohmann_json not found, will use fetched version")
    # 使用缓存功能添加nlohmann/json
    fetch_and_cache(
        json
        https://github.com/nlohmann/json.git
        v3.11.3
    )
else()
    message(STATUS "Using system nlohmann_json: ${nlohmann_json_VERSION}")
endif()

# 源文件目录
set(CORE_SOURCES
    src/main.cpp
    src/cli/cli_parser.cpp
    src/cli/enhanced_cli_parser.cpp
    src/cli/project_wizard.cpp
    src/templates/template_manager.cpp
    src/templates/template_base.cpp
    src/templates/console_template.cpp
    src/templates/library_template.cpp
    src/templates/header_only_lib_template.cpp
    # src/templates/multi_executable_template.cpp
    src/templates/gui_template.cpp
    src/templates/network_template.cpp
    src/templates/custom_template.cpp
    src/templates/webservice_template.cpp
    src/templates/embedded_template.cpp
    src/templates/gameengine_template.cpp
    src/templates/template_discovery.cpp
    src/utils/file_utils.cpp
    src/utils/string_utils.cpp
    src/utils/terminal_utils.cpp
    src/utils/git_utils.cpp
    src/utils/framework_integration.cpp
    src/utils/progress_indicator.cpp
    src/utils/interactive_menu.cpp
    src/utils/dependency_manager.cpp
    src/utils/build_system_config.cpp
    src/utils/project_scaffolding.cpp
    src/utils/user_experience.cpp
    # src/utils/post_creation_actions.cpp
)

# 配置工具源文件
set(CONFIG_SOURCES
    src/config/editor_config.cpp
    src/config/ci_config.cpp
    src/config/code_style_config.cpp
    src/config/doc_config.cpp
    src/config/config_manager.cpp
    src/config/enhanced_config_manager.cpp
    src/config/user_preferences.cpp
    src/config/interactive_config.cpp
    src/config/project_profiles.cpp
    src/config/config_validator.cpp
)

set(TESTING_SOURCES
    src/testing/test_framework_manager.cpp
)

set(DOCUMENTATION_SOURCES
    src/documentation/doc_generator.cpp
)

# 所有源文件
set(SOURCES ${CORE_SOURCES} ${CONFIG_SOURCES} ${TESTING_SOURCES} ${DOCUMENTATION_SOURCES})

# Create the main executable target
add_executable(cpp-scaffold)

# Configure target properties
set_target_properties(cpp-scaffold PROPERTIES
    OUTPUT_NAME "cpp-scaffold"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# Add source files to target using modern CMake approach
target_sources(cpp-scaffold PRIVATE ${SOURCES})

# Modern target-based include directories
target_include_directories(cpp-scaffold PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:include>
)

# Detect filesystem library requirements
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS 9.0)
        find_package(Filesystem REQUIRED)
        if(NOT Filesystem_FOUND)
            message(FATAL_ERROR "Filesystem library not found. Use a newer compiler or provide stdc++fs library.")
        endif()
        set(FILESYSTEM_LIB stdc++fs)
    endif()
elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS 9.0)
        set(FILESYSTEM_LIB c++fs)
    endif()
endif()

# Link required libraries using modern target-based approach
target_link_libraries(cpp-scaffold PRIVATE
    spdlog::spdlog
    nlohmann_json::nlohmann_json
)

# Link filesystem library if needed
if(DEFINED FILESYSTEM_LIB)
    target_link_libraries(cpp-scaffold PRIVATE ${FILESYSTEM_LIB})
endif()

# Platform-specific libraries
if(WIN32)
    target_link_libraries(cpp-scaffold PRIVATE shlwapi)
endif()

# Configure Unity builds if enabled
if(ENABLE_UNITY_BUILD)
    set_target_properties(cpp-scaffold PROPERTIES UNITY_BUILD ON)
    message(STATUS "Unity builds enabled for cpp-scaffold")
endif()

# Configure precompiled headers if enabled
if(ENABLE_PRECOMPILED_HEADERS)
    target_precompile_headers(cpp-scaffold PRIVATE
        <iostream>
        <string>
        <vector>
        <memory>
        <filesystem>
        <fstream>
        <sstream>
        <algorithm>
        <map>
        <unordered_map>
        <set>
        <unordered_set>
        <spdlog/spdlog.h>
        <nlohmann/json.hpp>
    )
    message(STATUS "Precompiled headers enabled for cpp-scaffold")
endif()

# Advanced compiler optimizations
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
        target_compile_options(cpp-scaffold PRIVATE
            -O3
            -march=native
            -mtune=native
            -flto=auto
            -fuse-linker-plugin
            -ffat-lto-objects
        )
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        target_compile_options(cpp-scaffold PRIVATE
            -O3
            -march=native
            -mtune=native
            -flto=thin
        )
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
        target_compile_options(cpp-scaffold PRIVATE
            /O2
            /Ob2
            /Oi
            /Ot
            /GL
        )
        target_link_options(cpp-scaffold PRIVATE /LTCG)
    endif()
endif()

# Debug optimizations
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU" OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        target_compile_options(cpp-scaffold PRIVATE
            -Og  # Optimize for debugging experience
            -g3  # Maximum debug information
        )
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
        target_compile_options(cpp-scaffold PRIVATE
            /Od  # Disable optimizations
            /Zi  # Debug information
        )
    endif()
endif()

# 处理安装
install(TARGETS cpp-scaffold RUNTIME DESTINATION bin)

# 安装文档和配置文件
install(FILES README.md LICENSE DESTINATION share/cpp-scaffold)

# 制作可分发包
set(CPACK_PACKAGE_NAME "cpp-scaffold")
set(CPACK_PACKAGE_VENDOR "CPP-Scaffold Team")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "C++ Project Scaffolding Tool")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})
set(CPACK_PACKAGE_INSTALL_DIRECTORY "CPP-Scaffold")

# 设置平台特定打包选项
if(WIN32)
    set(CPACK_GENERATOR "NSIS;ZIP")
    set(CPACK_NSIS_MODIFY_PATH ON)
elseif(APPLE)
    set(CPACK_GENERATOR "DragNDrop;TGZ")
else()
    set(CPACK_GENERATOR "DEB;RPM;TGZ")
    set(CPACK_DEBIAN_PACKAGE_MAINTAINER "CPP-Scaffold Team")
    set(CPACK_DEBIAN_PACKAGE_SECTION "devel")
    set(CPACK_RPM_PACKAGE_GROUP "Development/Tools")
endif()

include(CPack)

# Testing configuration
option(BUILD_TESTING "Build the testing tree." ON)
if(BUILD_TESTING)
    enable_testing()

    # Add Google Test with caching
    fetch_and_cache(
        googletest
        https://github.com/google/googletest.git
        v1.14.0
    )

    add_subdirectory(tests)
endif()

# Developer convenience targets
add_custom_target(format
    COMMAND find src tests -name "*.cpp" -o -name "*.h" -o -name "*.hpp" | xargs clang-format -i
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Formatting source code with clang-format"
)

add_custom_target(tidy
    COMMAND clang-tidy -p ${CMAKE_BINARY_DIR} src/**/*.cpp tests/**/*.cpp
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Running clang-tidy static analysis"
    DEPENDS cpp-scaffold
)

add_custom_target(clean-all
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}
    COMMENT "Cleaning all build artifacts"
)

# Documentation target (if Doxygen is available)
find_package(Doxygen QUIET)
if(DOXYGEN_FOUND)
    add_custom_target(docs
        COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_SOURCE_DIR}/Doxyfile
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Generating documentation with Doxygen"
    )
endif()

# Print build configuration summary
message(STATUS "")
message(STATUS "=== Build Configuration Summary ===")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ standard: C++${CMAKE_CXX_STANDARD}")
message(STATUS "Compiler: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "Generator: ${CMAKE_GENERATOR}")
message(STATUS "Unity builds: ${ENABLE_UNITY_BUILD}")
message(STATUS "Precompiled headers: ${ENABLE_PRECOMPILED_HEADERS}")
message(STATUS "ccache: ${ENABLE_CCACHE}")
message(STATUS "Testing: ${BUILD_TESTING}")
if(CCACHE_PROGRAM)
    message(STATUS "ccache found: ${CCACHE_PROGRAM}")
endif()
message(STATUS "===================================")
message(STATUS "")

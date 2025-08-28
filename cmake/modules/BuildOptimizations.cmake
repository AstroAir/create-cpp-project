# BuildOptimizations.cmake
# Performance and build optimization options

# Build Optimization Options
function(setup_build_optimization_options)
    option(ENABLE_UNITY_BUILD "Enable Unity builds for faster compilation" OFF)
    option(ENABLE_PRECOMPILED_HEADERS "Enable precompiled headers" ON)
    option(ENABLE_CCACHE "Enable ccache for faster rebuilds" ON)
    option(ENABLE_LTO "Enable Link Time Optimization" OFF)
    option(ENABLE_BUILD_PROFILING "Enable build time profiling" OFF)
    option(ENABLE_SANITIZERS "Enable sanitizers in debug builds" OFF)
    option(ENABLE_FAT_LTO_OBJECTS "Enable -ffat-lto-objects flag (GCC only, may not be supported on all targets)" OFF)
endfunction()

# ccache Configuration
function(configure_ccache)
    if(ENABLE_CCACHE)
        find_program(CCACHE_PROGRAM ccache)
        if(CCACHE_PROGRAM)
            message(STATUS "Found ccache: ${CCACHE_PROGRAM}")
            set(CMAKE_CXX_COMPILER_LAUNCHER "${CCACHE_PROGRAM}" PARENT_SCOPE)
            set(CMAKE_C_COMPILER_LAUNCHER "${CCACHE_PROGRAM}" PARENT_SCOPE)
        else()
            message(STATUS "ccache not found, continuing without it")
        endif()
    endif()
endfunction()

# Link Time Optimization (LTO)
function(configure_lto)
    if(ENABLE_LTO)
        include(CheckIPOSupported)
        check_ipo_supported(RESULT LTO_SUPPORTED OUTPUT LTO_ERROR)
        if(LTO_SUPPORTED)
            message(STATUS "Link Time Optimization enabled")
            set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE PARENT_SCOPE)
        else()
            message(WARNING "LTO not supported: ${LTO_ERROR}")
        endif()
    endif()
endfunction()

# Build Time Profiling
function(configure_build_profiling)
    if(ENABLE_BUILD_PROFILING)
        if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
            add_compile_options(-ftime-trace)
            message(STATUS "Clang build time profiling enabled")
        elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
            add_compile_options(-ftime-report)
            message(STATUS "GCC build time profiling enabled")
        endif()
    endif()
endfunction()

# Sanitizers for Debug Builds
function(configure_sanitizers)
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
endfunction()

# Unity Build Configuration
function(configure_unity_build target)
    if(ENABLE_UNITY_BUILD)
        set_target_properties(${target} PROPERTIES UNITY_BUILD ON)
        message(STATUS "Unity builds enabled for ${target}")
    endif()
endfunction()

# Precompiled Headers Configuration
function(configure_precompiled_headers target)
    if(ENABLE_PRECOMPILED_HEADERS)
        target_precompile_headers(${target} PRIVATE
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
        message(STATUS "Precompiled headers enabled for ${target}")
    endif()
endfunction()

# Test-specific Precompiled Headers
function(configure_test_precompiled_headers target)
    if(ENABLE_PRECOMPILED_HEADERS)
        target_precompile_headers(${target} PRIVATE
            <gtest/gtest.h>
            <gmock/gmock.h>
            <iostream>
            <string>
            <vector>
            <memory>
            <filesystem>
        )
        message(STATUS "Test precompiled headers enabled for ${target}")
    endif()
endfunction()

# IDE Integration Configuration
function(configure_ide_integration)
    # Enable IDE integration and modern CMake features
    set(CMAKE_EXPORT_COMPILE_COMMANDS ON PARENT_SCOPE)
    set_property(GLOBAL PROPERTY USE_FOLDERS ON)
endfunction()

# Apply All Build Optimizations
function(apply_build_optimizations target)
    configure_unity_build(${target})
    configure_precompiled_headers(${target})
    configure_release_optimizations(${target})
    configure_debug_optimizations(${target})
endfunction()

# Apply All Global Build Optimizations
function(apply_global_build_optimizations)
    setup_build_optimization_options()
    configure_ccache()
    configure_lto()
    configure_build_profiling()
    configure_sanitizers()
    configure_ide_integration()
endfunction()

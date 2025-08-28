# CompilerSettings.cmake
# Modern compiler configuration with enhanced warnings and optimizations

# C++ Standard Configuration
function(configure_cpp_standard target)
    # Modern C++ standard configuration with compatibility focus
    # Use C++17 for better compatibility with current dependencies
    # C++20 can be enabled via option for testing
    option(ENABLE_CPP20 "Enable C++20 standard (experimental)" OFF)
    option(ENABLE_STRICT_WARNINGS "Enable strict warning mode" OFF)
    option(ENABLE_MSYS2_OPTIMIZATIONS "Enable MSYS2-specific optimizations" OFF)

    if(ENABLE_CPP20)
        if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC" AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 19.29)
            target_compile_features(${target} PRIVATE cxx_std_20)
            message(STATUS "Using C++20 standard for ${target}")
        elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 11.0)
            target_compile_features(${target} PRIVATE cxx_std_20)
            message(STATUS "Using C++20 standard for ${target}")
        elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang" AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 13.0)
            target_compile_features(${target} PRIVATE cxx_std_20)
            message(STATUS "Using C++20 standard for ${target}")
        else()
            target_compile_features(${target} PRIVATE cxx_std_17)
            message(STATUS "C++20 requested but not supported for ${target}, falling back to C++17")
        endif()
    else()
        target_compile_features(${target} PRIVATE cxx_std_17)
        message(STATUS "Using C++17 standard for ${target}")
    endif()
endfunction()

# Global C++ Standard Configuration
function(configure_global_cpp_standard)
    if(ENABLE_CPP20)
        if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC" AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 19.29)
            set(CMAKE_CXX_STANDARD 20 PARENT_SCOPE)
            message(STATUS "Using C++20 standard globally")
        elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 11.0)
            set(CMAKE_CXX_STANDARD 20 PARENT_SCOPE)
            message(STATUS "Using C++20 standard globally")
        elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang" AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 13.0)
            set(CMAKE_CXX_STANDARD 20 PARENT_SCOPE)
            message(STATUS "Using C++20 standard globally")
        else()
            set(CMAKE_CXX_STANDARD 17 PARENT_SCOPE)
            message(STATUS "C++20 requested but not supported, falling back to C++17")
        endif()
    else()
        set(CMAKE_CXX_STANDARD 17 PARENT_SCOPE)
        message(STATUS "Using C++17 standard globally")
    endif()

    set(CMAKE_CXX_STANDARD_REQUIRED ON PARENT_SCOPE)
    set(CMAKE_CXX_EXTENSIONS OFF PARENT_SCOPE)
endfunction()

# Compiler Warning Configuration
function(configure_compiler_warnings target)
    if(MSVC)
        # MSVC specific flags
        target_compile_options(${target} PRIVATE
            /W4                     # High warning level
            /permissive-           # Disable non-conforming code
            /Zc:__cplusplus        # Enable correct __cplusplus macro
            /utf-8                 # UTF-8 source and execution character sets
        )
        if(CMAKE_BUILD_TYPE STREQUAL "Debug")
            target_compile_options(${target} PRIVATE /WX)  # Treat warnings as errors in debug
        endif()
        # Windows Unicode support
        target_compile_definitions(${target} PRIVATE UNICODE _UNICODE WIN32_LEAN_AND_MEAN NOMINMAX)
    else()
        # GCC/Clang flags - balanced approach for existing codebase
        target_compile_options(${target} PRIVATE
            -Wall -Wextra -Wpedantic
            # Temporarily disable strict conversion warnings for existing code
            # -Wconversion -Wsign-conversion
            -Wcast-qual -Wcast-align
            -Wunused -Wunreachable-code
        )
        # Only treat warnings as errors in strict mode
        if(CMAKE_BUILD_TYPE STREQUAL "Debug" AND ENABLE_STRICT_WARNINGS)
            target_compile_options(${target} PRIVATE -Werror)
        endif()
        if(WIN32)
            target_compile_definitions(${target} PRIVATE UNICODE _UNICODE)
        endif()
    endif()
endfunction()

# Global Compiler Warning Configuration
function(configure_global_compiler_warnings)
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
endfunction()

# Build Type Configuration (Enhanced)
function(configure_build_types)
    include(BuildTypes)
    setup_enhanced_build_types()
    print_build_type_info()
endfunction()

# Optimization Configuration for Release Builds
function(configure_release_optimizations target)
    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        # Detect actual compiler being used (not just the ID)
        execute_process(
            COMMAND ${CMAKE_CXX_COMPILER} --version
            OUTPUT_VARIABLE COMPILER_VERSION_OUTPUT
            ERROR_QUIET
        )

        # More robust compiler detection
        execute_process(
            COMMAND ${CMAKE_CXX_COMPILER} -dumpversion
            OUTPUT_VARIABLE COMPILER_VERSION_NUMBER
            ERROR_QUIET
            OUTPUT_STRIP_TRAILING_WHITESPACE
        )

        # Detect Windows GCC variants (MinGW-w64, etc.)
        execute_process(
            COMMAND ${CMAKE_CXX_COMPILER} -dumpmachine
            OUTPUT_VARIABLE COMPILER_TARGET_TRIPLET
            ERROR_QUIET
            OUTPUT_STRIP_TRAILING_WHITESPACE
        )

        # Check if compiler supports -ffat-lto-objects flag
        # Use appropriate null device for the platform
        if(WIN32)
            set(NULL_DEVICE "NUL")
        else()
            set(NULL_DEVICE "/dev/null")
        endif()

        # More robust test: try to compile a simple program with the flag
        # This catches cases where the flag is accepted during preprocessing but fails during compilation
        set(TEST_SOURCE_FILE "${CMAKE_BINARY_DIR}/test_fat_lto_objects.cpp")
        file(WRITE "${TEST_SOURCE_FILE}" "int main(){return 0;}")

        execute_process(
            COMMAND ${CMAKE_CXX_COMPILER} -ffat-lto-objects -c "${TEST_SOURCE_FILE}" -o "${CMAKE_BINARY_DIR}/test_fat_lto_objects.o"
            OUTPUT_QUIET
            ERROR_QUIET
            RESULT_VARIABLE FAT_LTO_OBJECTS_SUPPORTED
        )

        # Clean up test files
        file(REMOVE "${TEST_SOURCE_FILE}")
        file(REMOVE "${CMAKE_BINARY_DIR}/test_fat_lto_objects.o")

        if(CMAKE_CXX_COMPILER_ID MATCHES "GNU" AND NOT COMPILER_VERSION_OUTPUT MATCHES "clang")
            # True GCC compiler (including MinGW-w64)
            set(GCC_OPTIMIZATION_FLAGS
                -O3
                -march=native
                -mtune=native
                -flto=auto
                -fuse-linker-plugin
            )

            # Detect if this is a Windows GCC variant
            set(IS_WINDOWS_GCC FALSE)
            if(WIN32 OR COMPILER_TARGET_TRIPLET MATCHES "mingw|w64")
                set(IS_WINDOWS_GCC TRUE)
            endif()

            # Only add -ffat-lto-objects if enabled and supported
            # This flag is GCC-specific and may not be supported on all targets/versions
            if(ENABLE_FAT_LTO_OBJECTS AND FAT_LTO_OBJECTS_SUPPORTED EQUAL 0)
                list(APPEND GCC_OPTIMIZATION_FLAGS -ffat-lto-objects)
                if(IS_WINDOWS_GCC)
                    message(STATUS "Using Windows GCC (${COMPILER_TARGET_TRIPLET}) optimizations with LTO and fat objects for ${target}")
                else()
                    message(STATUS "Using GCC-specific optimizations with LTO and fat objects for ${target}")
                endif()
            elseif(NOT ENABLE_FAT_LTO_OBJECTS)
                if(IS_WINDOWS_GCC)
                    message(STATUS "Using Windows GCC (${COMPILER_TARGET_TRIPLET}) optimizations with LTO (fat objects disabled by option) for ${target}")
                else()
                    message(STATUS "Using GCC-specific optimizations with LTO (fat objects disabled by option) for ${target}")
                endif()
            else()
                if(IS_WINDOWS_GCC)
                    message(STATUS "Using Windows GCC (${COMPILER_TARGET_TRIPLET}) optimizations with LTO (fat objects not supported) for ${target}")
                else()
                    message(STATUS "Using GCC-specific optimizations with LTO (fat objects not supported on this compiler/target) for ${target}")
                endif()
                message(STATUS "Compiler: ${CMAKE_CXX_COMPILER}")
                message(STATUS "Note: If you're seeing errors about -ffat-lto-objects, disable with -DENABLE_FAT_LTO_OBJECTS=OFF")
            endif()

            target_compile_options(${target} PRIVATE ${GCC_OPTIMIZATION_FLAGS})

        elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR COMPILER_VERSION_OUTPUT MATCHES "clang")
            # Clang compiler (including when masquerading as GCC)
            target_compile_options(${target} PRIVATE
                -O3
                -march=native
                -mtune=native
                -flto=thin
            )
            message(STATUS "Using Clang-specific optimizations with thin LTO for ${target}")
        elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
            # MSVC compiler
            target_compile_options(${target} PRIVATE
                /O2
                /Ob2
                /Oi
                /Ot
                /GL
            )
            target_link_options(${target} PRIVATE /LTCG)
            message(STATUS "Using MSVC-specific optimizations with LTCG for ${target}")
        else()
            # Fallback for unknown compilers
            target_compile_options(${target} PRIVATE -O3)
            message(STATUS "Using generic optimizations for unknown compiler for ${target}")
        endif()
    endif()
endfunction()

# Debug Configuration
function(configure_debug_optimizations target)
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        if(CMAKE_CXX_COMPILER_ID MATCHES "GNU" OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
            target_compile_options(${target} PRIVATE
                -Og  # Optimize for debugging experience
                -g3  # Maximum debug information
            )
        elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
            target_compile_options(${target} PRIVATE
                /Od  # Disable optimizations
                /Zi  # Debug information
            )
        endif()
    endif()
endfunction()

# MSYS2-specific compiler configuration
function(configure_msys2_compiler target)
    # Detect MSYS2 environment
    if(DEFINED ENV{MSYSTEM} OR DEFINED ENV{MINGW_PREFIX})
        message(STATUS "Configuring MSYS2-specific compiler settings for ${target}")

        # MSYS2 environment variables
        set(MSYSTEM $ENV{MSYSTEM})
        set(MINGW_PREFIX $ENV{MINGW_PREFIX})

        # Set MSYS2-specific definitions
        target_compile_definitions(${target} PRIVATE
            MSYS2_BUILD=1
            MINGW_BUILD=1
        )

        # Add MSYS2 system information
        if(MSYSTEM)
            target_compile_definitions(${target} PRIVATE MSYSTEM="${MSYSTEM}")
        endif()

        # Configure MinGW-specific settings
        if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
            # GCC in MSYS2 environment
            target_compile_options(${target} PRIVATE
                -pipe                    # Use pipes instead of temporary files
                -fno-keep-inline-dllexport  # Don't keep inline dllexport functions
            )

            # Windows-specific optimizations for MinGW
            if(CMAKE_BUILD_TYPE STREQUAL "Release")
                target_compile_options(${target} PRIVATE
                    -ffunction-sections     # Place functions in separate sections
                    -fdata-sections        # Place data in separate sections
                )
                target_link_options(${target} PRIVATE
                    -Wl,--gc-sections      # Remove unused sections
                    -Wl,--strip-all        # Strip all symbols
                )
            endif()

        elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
            # Clang in MSYS2 environment
            target_compile_options(${target} PRIVATE
                -fms-extensions         # Enable Microsoft extensions
                -fms-compatibility      # Enable MS compatibility mode
            )
        endif()

        # MSYS2 library paths
        if(MINGW_PREFIX)
            target_include_directories(${target} SYSTEM PRIVATE
                "${MINGW_PREFIX}/include"
            )
            target_link_directories(${target} PRIVATE
                "${MINGW_PREFIX}/lib"
            )
        endif()

        # Windows-specific libraries for MSYS2
        target_link_libraries(${target} PRIVATE
            ws2_32      # Winsock
            shlwapi     # Shell API
            advapi32    # Advanced API
        )

        # Set proper Windows subsystem
        if(WIN32 AND NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
            set_target_properties(${target} PROPERTIES
                WIN32_EXECUTABLE TRUE
            )
        endif()

    else()
        message(STATUS "Not in MSYS2 environment, skipping MSYS2-specific configuration")
    endif()
endfunction()

# Apply MSYS2 optimizations if enabled
function(apply_msys2_optimizations target)
    if(ENABLE_MSYS2_OPTIMIZATIONS AND (DEFINED ENV{MSYSTEM} OR DEFINED ENV{MINGW_PREFIX}))
        message(STATUS "Applying MSYS2-specific optimizations to ${target}")

        # MSYS2-specific optimization flags
        if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
            target_compile_options(${target} PRIVATE
                -march=native           # Optimize for current CPU
                -mtune=native          # Tune for current CPU
                -ffast-math            # Fast math optimizations
                -funroll-loops         # Unroll loops
            )

            # Link-time optimizations for release builds
            if(CMAKE_BUILD_TYPE STREQUAL "Release")
                target_compile_options(${target} PRIVATE -flto)
                target_link_options(${target} PRIVATE -flto)
            endif()
        endif()

        # Static linking for better portability in MSYS2
        if(CMAKE_BUILD_TYPE STREQUAL "Release")
            target_link_options(${target} PRIVATE
                -static-libgcc
                -static-libstdc++
            )
        endif()
    endif()
endfunction()

# Configure MSYS2 environment variables and paths
function(setup_msys2_environment)
    if(DEFINED ENV{MSYSTEM} OR DEFINED ENV{MINGW_PREFIX})
        # Set MSYS2 environment variables
        set(MSYS2_DETECTED TRUE PARENT_SCOPE)
        set(MSYSTEM $ENV{MSYSTEM} PARENT_SCOPE)
        set(MINGW_PREFIX $ENV{MINGW_PREFIX} PARENT_SCOPE)

        # Add MSYS2 paths to CMAKE_PREFIX_PATH
        if(DEFINED ENV{MINGW_PREFIX})
            list(APPEND CMAKE_PREFIX_PATH $ENV{MINGW_PREFIX})
            set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} PARENT_SCOPE)
        endif()

        # Set MSYS2-specific install prefix
        if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
            if(DEFINED ENV{MINGW_PREFIX})
                set(CMAKE_INSTALL_PREFIX $ENV{MINGW_PREFIX} PARENT_SCOPE)
            endif()
        endif()

        message(STATUS "MSYS2 environment detected: ${MSYSTEM}")
        if(DEFINED ENV{MINGW_PREFIX})
            message(STATUS "MINGW_PREFIX: $ENV{MINGW_PREFIX}")
        endif()
    endif()
endfunction()

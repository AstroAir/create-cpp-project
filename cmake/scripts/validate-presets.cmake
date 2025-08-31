# validate-presets.cmake
# Standalone script to validate CMake presets

cmake_minimum_required(VERSION 3.20)

# Get the source directory (where this script is located)
get_filename_component(SCRIPT_DIR "${CMAKE_CURRENT_LIST_FILE}" DIRECTORY)
get_filename_component(SOURCE_DIR "${SCRIPT_DIR}/../.." ABSOLUTE)

# Add cmake modules to path
list(APPEND CMAKE_MODULE_PATH "${SOURCE_DIR}/cmake/modules")

# Include preset utilities
include(PresetUtils)

# Parse command line arguments
set(VERBOSE OFF)
set(STRICT OFF)
set(GENERATE_PLATFORM_PRESETS OFF)
set(PRESET_DIR "${SOURCE_DIR}/cmake/presets")

# Simple argument parsing
foreach(ARG ${CMAKE_ARGV})
    if(ARG STREQUAL "--verbose" OR ARG STREQUAL "-v")
        set(VERBOSE ON)
    elseif(ARG STREQUAL "--strict" OR ARG STREQUAL "-s")
        set(STRICT ON)
    elseif(ARG STREQUAL "--generate-platform" OR ARG STREQUAL "-g")
        set(GENERATE_PLATFORM_PRESETS ON)
    elseif(ARG MATCHES "^--preset-dir=(.+)$")
        set(PRESET_DIR "${CMAKE_MATCH_1}")
    endif()
endforeach()

message(STATUS "CMake Preset Validation Script")
message(STATUS "==============================")

# Print preset system information
print_preset_info(VERBOSE ${VERBOSE})

# Validate main preset file
set(MAIN_PRESET "${SOURCE_DIR}/CMakePresets.json")
if(EXISTS "${MAIN_PRESET}")
    message(STATUS "Validating main preset file...")
    validate_preset_configuration(
        PRESET_FILE "${MAIN_PRESET}"
        VERBOSE ${VERBOSE}
    )
    message(STATUS "Main preset file validation: PASSED")
else()
    if(STRICT)
        message(FATAL_ERROR "Main preset file not found: ${MAIN_PRESET}")
    else()
        message(WARNING "Main preset file not found: ${MAIN_PRESET}")
    endif()
endif()

# Validate user preset file if it exists
set(USER_PRESET "${SOURCE_DIR}/CMakeUserPresets.json")
if(EXISTS "${USER_PRESET}")
    message(STATUS "Validating user preset file...")
    validate_preset_configuration(
        PRESET_FILE "${USER_PRESET}"
        VERBOSE ${VERBOSE}
    )
    message(STATUS "User preset file validation: PASSED")
endif()

# Validate all modular preset files
if(EXISTS "${PRESET_DIR}")
    message(STATUS "Validating modular preset files...")
    validate_all_presets(
        PRESET_DIR "${PRESET_DIR}"
        VERBOSE ${VERBOSE}
        STRICT ${STRICT}
    )
    message(STATUS "Modular preset files validation: PASSED")
else()
    if(STRICT)
        message(FATAL_ERROR "Preset directory not found: ${PRESET_DIR}")
    else()
        message(WARNING "Preset directory not found: ${PRESET_DIR}")
    endif()
endif()

# Generate platform-specific presets if requested
if(GENERATE_PLATFORM_PRESETS)
    message(STATUS "Generating platform-specific presets...")
    generate_platform_presets(
        OUTPUT_DIR "${PRESET_DIR}/generated"
        VERBOSE ${VERBOSE}
        PLATFORMS "windows" "linux" "macos" "android" "ios" "wasm"
        ARCHITECTURES "x64" "x86" "arm64" "arm"
    )
    message(STATUS "Platform preset generation: COMPLETED")
endif()

# Check preset dependencies
message(STATUS "Checking preset dependencies...")
check_preset_dependencies(
    PRESET_NAME "default"
    PRESET_DIRS "${PRESET_DIR}"
    VERBOSE ${VERBOSE}
)
message(STATUS "Preset dependency check: PASSED")

message(STATUS "==============================")
message(STATUS "All preset validations completed successfully!")

# Usage information
if(VERBOSE)
    message(STATUS "")
    message(STATUS "Usage: cmake -P validate-presets.cmake [options]")
    message(STATUS "Options:")
    message(STATUS "  --verbose, -v           Enable verbose output")
    message(STATUS "  --strict, -s            Enable strict validation (fail on warnings)")
    message(STATUS "  --generate-platform, -g Generate platform-specific presets")
    message(STATUS "  --preset-dir=<path>     Specify custom preset directory")
    message(STATUS "")
    message(STATUS "Examples:")
    message(STATUS "  cmake -P validate-presets.cmake --verbose")
    message(STATUS "  cmake -P validate-presets.cmake --strict --generate-platform")
    message(STATUS "  cmake -P validate-presets.cmake --preset-dir=/custom/path")
endif()

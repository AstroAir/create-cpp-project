# PresetUtils.cmake
# Utilities for CMake preset validation and composition

# Function to validate preset configuration
function(validate_preset_configuration)
    set(options VERBOSE)
    set(oneValueArgs PRESET_FILE)
    set(multiValueArgs REQUIRED_PRESETS)
    cmake_parse_arguments(VALIDATE "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if(NOT VALIDATE_PRESET_FILE)
        message(FATAL_ERROR "PRESET_FILE is required for validate_preset_configuration")
    endif()

    if(NOT EXISTS "${VALIDATE_PRESET_FILE}")
        message(FATAL_ERROR "Preset file does not exist: ${VALIDATE_PRESET_FILE}")
    endif()

    if(VALIDATE_VERBOSE)
        message(STATUS "Validating preset file: ${VALIDATE_PRESET_FILE}")
    endif()

    # Read and parse JSON file (basic validation)
    file(READ "${VALIDATE_PRESET_FILE}" PRESET_CONTENT)
    
    # Check for required JSON structure
    string(FIND "${PRESET_CONTENT}" "\"version\"" VERSION_POS)
    if(VERSION_POS EQUAL -1)
        message(FATAL_ERROR "Preset file missing version field: ${VALIDATE_PRESET_FILE}")
    endif()

    string(FIND "${PRESET_CONTENT}" "\"configurePresets\"" CONFIGURE_POS)
    if(CONFIGURE_POS EQUAL -1)
        message(WARNING "Preset file missing configurePresets: ${VALIDATE_PRESET_FILE}")
    endif()

    if(VALIDATE_VERBOSE)
        message(STATUS "Preset file validation passed: ${VALIDATE_PRESET_FILE}")
    endif()
endfunction()

# Function to check preset dependencies
function(check_preset_dependencies)
    set(options VERBOSE)
    set(oneValueArgs PRESET_NAME)
    set(multiValueArgs PRESET_DIRS)
    cmake_parse_arguments(CHECK "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if(NOT CHECK_PRESET_NAME)
        message(FATAL_ERROR "PRESET_NAME is required for check_preset_dependencies")
    endif()

    if(NOT CHECK_PRESET_DIRS)
        set(CHECK_PRESET_DIRS "${CMAKE_SOURCE_DIR}/cmake/presets")
    endif()

    if(CHECK_VERBOSE)
        message(STATUS "Checking dependencies for preset: ${CHECK_PRESET_NAME}")
    endif()

    # This is a simplified dependency check
    # In a real implementation, you would parse JSON and check inherits relationships
    foreach(PRESET_DIR ${CHECK_PRESET_DIRS})
        if(EXISTS "${PRESET_DIR}")
            file(GLOB PRESET_FILES "${PRESET_DIR}/*.json")
            foreach(PRESET_FILE ${PRESET_FILES})
                validate_preset_configuration(PRESET_FILE "${PRESET_FILE}")
            endforeach()
        endif()
    endforeach()
endfunction()

# Function to compose presets from multiple sources
function(compose_presets)
    set(options VERBOSE)
    set(oneValueArgs OUTPUT_FILE)
    set(multiValueArgs INPUT_FILES BASE_PRESETS PLATFORM_PRESETS OPTIMIZATION_PRESETS)
    cmake_parse_arguments(COMPOSE "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if(NOT COMPOSE_OUTPUT_FILE)
        message(FATAL_ERROR "OUTPUT_FILE is required for compose_presets")
    endif()

    if(COMPOSE_VERBOSE)
        message(STATUS "Composing presets to: ${COMPOSE_OUTPUT_FILE}")
    endif()

    # Create composed preset file header
    set(COMPOSED_CONTENT "{\n")
    set(COMPOSED_CONTENT "${COMPOSED_CONTENT}    \"version\": 3,\n")
    set(COMPOSED_CONTENT "${COMPOSED_CONTENT}    \"cmakeMinimumRequired\": {\n")
    set(COMPOSED_CONTENT "${COMPOSED_CONTENT}        \"major\": 3,\n")
    set(COMPOSED_CONTENT "${COMPOSED_CONTENT}        \"minor\": 20,\n")
    set(COMPOSED_CONTENT "${COMPOSED_CONTENT}        \"patch\": 0\n")
    set(COMPOSED_CONTENT "${COMPOSED_CONTENT}    },\n")

    # Add include section if input files are specified
    if(COMPOSE_INPUT_FILES)
        set(COMPOSED_CONTENT "${COMPOSED_CONTENT}    \"include\": [\n")
        list(LENGTH COMPOSE_INPUT_FILES FILE_COUNT)
        set(CURRENT_INDEX 0)
        foreach(INPUT_FILE ${COMPOSE_INPUT_FILES})
            math(EXPR CURRENT_INDEX "${CURRENT_INDEX} + 1")
            set(COMPOSED_CONTENT "${COMPOSED_CONTENT}        \"${INPUT_FILE}\"")
            if(CURRENT_INDEX LESS FILE_COUNT)
                set(COMPOSED_CONTENT "${COMPOSED_CONTENT},")
            endif()
            set(COMPOSED_CONTENT "${COMPOSED_CONTENT}\n")
        endforeach()
        set(COMPOSED_CONTENT "${COMPOSED_CONTENT}    ],\n")
    endif()

    # Add configurePresets section
    set(COMPOSED_CONTENT "${COMPOSED_CONTENT}    \"configurePresets\": [],\n")
    set(COMPOSED_CONTENT "${COMPOSED_CONTENT}    \"buildPresets\": [],\n")
    set(COMPOSED_CONTENT "${COMPOSED_CONTENT}    \"testPresets\": []\n")
    set(COMPOSED_CONTENT "${COMPOSED_CONTENT}}\n")

    # Write composed file
    file(WRITE "${COMPOSE_OUTPUT_FILE}" "${COMPOSED_CONTENT}")

    if(COMPOSE_VERBOSE)
        message(STATUS "Preset composition completed: ${COMPOSE_OUTPUT_FILE}")
    endif()
endfunction()

# Function to generate platform-specific presets
function(generate_platform_presets)
    set(options VERBOSE)
    set(oneValueArgs OUTPUT_DIR TEMPLATE_FILE)
    set(multiValueArgs PLATFORMS ARCHITECTURES)
    cmake_parse_arguments(GEN "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if(NOT GEN_OUTPUT_DIR)
        set(GEN_OUTPUT_DIR "${CMAKE_SOURCE_DIR}/cmake/presets/generated")
    endif()

    if(NOT GEN_PLATFORMS)
        set(GEN_PLATFORMS "windows" "linux" "macos" "android" "ios")
    endif()

    if(NOT GEN_ARCHITECTURES)
        set(GEN_ARCHITECTURES "x64" "x86" "arm64" "arm")
    endif()

    file(MAKE_DIRECTORY "${GEN_OUTPUT_DIR}")

    if(GEN_VERBOSE)
        message(STATUS "Generating platform presets in: ${GEN_OUTPUT_DIR}")
    endif()

    foreach(PLATFORM ${GEN_PLATFORMS})
        foreach(ARCH ${GEN_ARCHITECTURES})
            set(PRESET_NAME "${PLATFORM}-${ARCH}")
            set(PRESET_FILE "${GEN_OUTPUT_DIR}/${PRESET_NAME}-presets.json")
            
            # Generate basic preset structure
            set(PRESET_CONTENT "{\n")
            set(PRESET_CONTENT "${PRESET_CONTENT}    \"version\": 3,\n")
            set(PRESET_CONTENT "${PRESET_CONTENT}    \"configurePresets\": [\n")
            set(PRESET_CONTENT "${PRESET_CONTENT}        {\n")
            set(PRESET_CONTENT "${PRESET_CONTENT}            \"name\": \"${PRESET_NAME}\",\n")
            set(PRESET_CONTENT "${PRESET_CONTENT}            \"displayName\": \"${PLATFORM} ${ARCH} Configuration\",\n")
            set(PRESET_CONTENT "${PRESET_CONTENT}            \"description\": \"Configuration for ${PLATFORM} on ${ARCH} architecture\",\n")
            set(PRESET_CONTENT "${PRESET_CONTENT}            \"inherits\": [\"${PLATFORM}-base\", \"base\"],\n")
            set(PRESET_CONTENT "${PRESET_CONTENT}            \"cacheVariables\": {\n")
            set(PRESET_CONTENT "${PRESET_CONTENT}                \"CMAKE_SYSTEM_PROCESSOR\": \"${ARCH}\"\n")
            set(PRESET_CONTENT "${PRESET_CONTENT}            }\n")
            set(PRESET_CONTENT "${PRESET_CONTENT}        }\n")
            set(PRESET_CONTENT "${PRESET_CONTENT}    ]\n")
            set(PRESET_CONTENT "${PRESET_CONTENT}}\n")

            file(WRITE "${PRESET_FILE}" "${PRESET_CONTENT}")

            if(GEN_VERBOSE)
                message(STATUS "Generated preset: ${PRESET_FILE}")
            endif()
        endforeach()
    endforeach()
endfunction()

# Function to validate all presets in a directory
function(validate_all_presets)
    set(options VERBOSE STRICT)
    set(oneValueArgs PRESET_DIR)
    set(multiValueArgs)
    cmake_parse_arguments(VALIDATE_ALL "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if(NOT VALIDATE_ALL_PRESET_DIR)
        set(VALIDATE_ALL_PRESET_DIR "${CMAKE_SOURCE_DIR}/cmake/presets")
    endif()

    if(NOT EXISTS "${VALIDATE_ALL_PRESET_DIR}")
        if(VALIDATE_ALL_STRICT)
            message(FATAL_ERROR "Preset directory does not exist: ${VALIDATE_ALL_PRESET_DIR}")
        else()
            message(WARNING "Preset directory does not exist: ${VALIDATE_ALL_PRESET_DIR}")
            return()
        endif()
    endif()

    if(VALIDATE_ALL_VERBOSE)
        message(STATUS "Validating all presets in: ${VALIDATE_ALL_PRESET_DIR}")
    endif()

    file(GLOB_RECURSE PRESET_FILES "${VALIDATE_ALL_PRESET_DIR}/*.json")
    
    set(VALIDATION_ERRORS 0)
    foreach(PRESET_FILE ${PRESET_FILES})
        if(VALIDATE_ALL_VERBOSE)
            message(STATUS "Validating: ${PRESET_FILE}")
        endif()
        
        # Validate each preset file
        validate_preset_configuration(
            PRESET_FILE "${PRESET_FILE}"
            VERBOSE ${VALIDATE_ALL_VERBOSE}
        )
    endforeach()

    if(VALIDATE_ALL_VERBOSE)
        list(LENGTH PRESET_FILES PRESET_COUNT)
        message(STATUS "Validated ${PRESET_COUNT} preset files successfully")
    endif()
endfunction()

# Function to print preset information
function(print_preset_info)
    set(options VERBOSE)
    set(oneValueArgs)
    set(multiValueArgs)
    cmake_parse_arguments(PRINT "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    message(STATUS "=== CMake Preset System Information ===")
    message(STATUS "CMake Version: ${CMAKE_VERSION}")
    message(STATUS "Preset Support: ${CMAKE_VERSION} >= 3.19")
    
    # Check for preset files
    set(MAIN_PRESET "${CMAKE_SOURCE_DIR}/CMakePresets.json")
    set(USER_PRESET "${CMAKE_SOURCE_DIR}/CMakeUserPresets.json")
    
    if(EXISTS "${MAIN_PRESET}")
        message(STATUS "Main preset file: Found")
    else()
        message(STATUS "Main preset file: Not found")
    endif()
    
    if(EXISTS "${USER_PRESET}")
        message(STATUS "User preset file: Found")
    else()
        message(STATUS "User preset file: Not found")
    endif()

    # Check for modular preset directory
    set(PRESET_DIR "${CMAKE_SOURCE_DIR}/cmake/presets")
    if(EXISTS "${PRESET_DIR}")
        file(GLOB PRESET_FILES "${PRESET_DIR}/*.json")
        list(LENGTH PRESET_FILES PRESET_COUNT)
        message(STATUS "Modular preset files: ${PRESET_COUNT} found")
        
        if(PRINT_VERBOSE)
            foreach(PRESET_FILE ${PRESET_FILES})
                get_filename_component(PRESET_NAME "${PRESET_FILE}" NAME)
                message(STATUS "  - ${PRESET_NAME}")
            endforeach()
        endif()
    else()
        message(STATUS "Modular preset directory: Not found")
    endif()
    
    message(STATUS "=======================================")
endfunction()

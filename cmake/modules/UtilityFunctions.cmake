# UtilityFunctions.cmake
# Additional reusable CMake utility functions and macros

# Macro to add a compiler flag if supported
macro(add_compiler_flag_if_supported flag)
    include(CheckCXXCompilerFlag)
    string(MAKE_C_IDENTIFIER "HAVE_FLAG_${flag}" flag_var)
    check_cxx_compiler_flag("${flag}" ${flag_var})
    if(${flag_var})
        add_compile_options("${flag}")
    endif()
endmacro()

# Function to add a target-specific compiler flag if supported
function(add_target_compiler_flag_if_supported target flag)
    include(CheckCXXCompilerFlag)
    string(MAKE_C_IDENTIFIER "HAVE_FLAG_${flag}" flag_var)
    check_cxx_compiler_flag("${flag}" ${flag_var})
    if(${flag_var})
        target_compile_options(${target} PRIVATE "${flag}")
    endif()
endfunction()

# Function to check if a header exists and add it to precompiled headers
function(add_pch_if_available target header)
    include(CheckIncludeFileCXX)
    string(MAKE_C_IDENTIFIER "HAVE_${header}" header_var)
    string(REPLACE "/" "_" header_var ${header_var})
    string(REPLACE "." "_" header_var ${header_var})
    check_include_file_cxx("${header}" ${header_var})
    if(${header_var})
        target_precompile_headers(${target} PRIVATE <${header}>)
    endif()
endfunction()

# Function to create a version header file
function(create_version_header target version_file)
    set(options)
    set(oneValueArgs NAMESPACE MAJOR MINOR PATCH)
    set(multiValueArgs)
    cmake_parse_arguments(VER "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    # Parse version if not provided separately
    if(NOT VER_MAJOR AND NOT VER_MINOR AND NOT VER_PATCH)
        string(REPLACE "." ";" VERSION_LIST ${PROJECT_VERSION})
        list(GET VERSION_LIST 0 VER_MAJOR)
        if(VERSION_LIST)
            list(LENGTH VERSION_LIST VERSION_LIST_LENGTH)
            if(VERSION_LIST_LENGTH GREATER 1)
                list(GET VERSION_LIST 1 VER_MINOR)
            else()
                set(VER_MINOR 0)
            endif()
            if(VERSION_LIST_LENGTH GREATER 2)
                list(GET VERSION_LIST 2 VER_PATCH)
            else()
                set(VER_PATCH 0)
            endif()
        endif()
    endif()

    # Set default namespace
    if(NOT VER_NAMESPACE)
        string(TOUPPER ${PROJECT_NAME} VER_NAMESPACE)
    endif()

    # Generate version header content
    set(VERSION_HEADER_CONTENT
"#pragma once

#define ${VER_NAMESPACE}_VERSION_MAJOR ${VER_MAJOR}
#define ${VER_NAMESPACE}_VERSION_MINOR ${VER_MINOR}
#define ${VER_NAMESPACE}_VERSION_PATCH ${VER_PATCH}
#define ${VER_NAMESPACE}_VERSION \"${VER_MAJOR}.${VER_MINOR}.${VER_PATCH}\"

namespace ${VER_NAMESPACE} {
    constexpr int VERSION_MAJOR = ${VER_MAJOR};
    constexpr int VERSION_MINOR = ${VER_MINOR};
    constexpr int VERSION_PATCH = ${VER_PATCH};
    constexpr const char* VERSION = \"${VER_MAJOR}.${VER_MINOR}.${VER_PATCH}\";
}
")

    # Write the version header
    file(WRITE ${version_file} ${VERSION_HEADER_CONTENT})
    
    # Add to target sources
    target_sources(${target} PRIVATE ${version_file})
endfunction()

# Function to setup code coverage for a target
function(setup_target_coverage target)
    if(CMAKE_BUILD_TYPE STREQUAL "Debug" AND CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(${target} PRIVATE --coverage -fprofile-arcs -ftest-coverage)
        target_link_options(${target} PRIVATE --coverage)
        
        # Find gcov
        find_program(GCOV_PATH gcov)
        if(GCOV_PATH)
            message(STATUS "Code coverage enabled for ${target}")
        endif()
    endif()
endfunction()

# Function to add resource files to a target (cross-platform)
function(add_resources target)
    set(options)
    set(oneValueArgs RESOURCE_DIR OUTPUT_DIR)
    set(multiValueArgs FILES)
    cmake_parse_arguments(RES "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if(NOT RES_OUTPUT_DIR)
        set(RES_OUTPUT_DIR "${CMAKE_BINARY_DIR}/resources")
    endif()

    foreach(resource_file ${RES_FILES})
        if(RES_RESOURCE_DIR)
            set(full_path "${RES_RESOURCE_DIR}/${resource_file}")
        else()
            set(full_path "${resource_file}")
        endif()
        
        get_filename_component(filename ${resource_file} NAME)
        set(output_file "${RES_OUTPUT_DIR}/${filename}")
        
        add_custom_command(
            OUTPUT ${output_file}
            COMMAND ${CMAKE_COMMAND} -E copy ${full_path} ${output_file}
            DEPENDS ${full_path}
            COMMENT "Copying resource: ${filename}"
        )
        
        target_sources(${target} PRIVATE ${output_file})
    endforeach()
endfunction()

# Function to create a header-only library interface
function(create_header_only_library target)
    set(options)
    set(oneValueArgs NAMESPACE)
    set(multiValueArgs HEADERS INCLUDE_DIRS LINK_LIBRARIES COMPILE_DEFINITIONS)
    cmake_parse_arguments(HOL "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    add_library(${target} INTERFACE)
    
    if(HOL_HEADERS)
        target_sources(${target} INTERFACE ${HOL_HEADERS})
    endif()
    
    if(HOL_INCLUDE_DIRS)
        target_include_directories(${target} INTERFACE ${HOL_INCLUDE_DIRS})
    endif()
    
    if(HOL_LINK_LIBRARIES)
        target_link_libraries(${target} INTERFACE ${HOL_LINK_LIBRARIES})
    endif()
    
    if(HOL_COMPILE_DEFINITIONS)
        target_compile_definitions(${target} INTERFACE ${HOL_COMPILE_DEFINITIONS})
    endif()

    # Create alias with namespace if provided
    if(HOL_NAMESPACE)
        add_library(${HOL_NAMESPACE}::${target} ALIAS ${target})
    endif()
endfunction()

# Function to setup installation for header-only library
function(install_header_only_library target)
    set(options)
    set(oneValueArgs DESTINATION NAMESPACE)
    set(multiValueArgs HEADERS INCLUDE_DIRS)
    cmake_parse_arguments(IHOL "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if(NOT IHOL_DESTINATION)
        set(IHOL_DESTINATION "include")
    endif()

    # Install headers
    if(IHOL_HEADERS)
        install(FILES ${IHOL_HEADERS} DESTINATION ${IHOL_DESTINATION})
    endif()

    # Install include directories
    if(IHOL_INCLUDE_DIRS)
        foreach(include_dir ${IHOL_INCLUDE_DIRS})
            install(DIRECTORY ${include_dir}/ DESTINATION ${IHOL_DESTINATION}
                FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp" PATTERN "*.hxx"
            )
        endforeach()
    endif()

    # Install target
    install(TARGETS ${target} EXPORT ${target}Targets)
    
    # Export targets
    if(IHOL_NAMESPACE)
        set(EXPORT_NAMESPACE ${IHOL_NAMESPACE})
    else()
        set(EXPORT_NAMESPACE ${target})
    endif()

    install(EXPORT ${target}Targets
        FILE ${target}Targets.cmake
        NAMESPACE ${EXPORT_NAMESPACE}::
        DESTINATION lib/cmake/${target}
    )
endfunction()

# Function to check system requirements
function(check_system_requirements)
    set(options)
    set(oneValueArgs MIN_CMAKE_VERSION)
    set(multiValueArgs REQUIRED_PROGRAMS REQUIRED_LIBRARIES)
    cmake_parse_arguments(SYS "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    # Check CMake version
    if(SYS_MIN_CMAKE_VERSION)
        if(CMAKE_VERSION VERSION_LESS ${SYS_MIN_CMAKE_VERSION})
            message(FATAL_ERROR "CMake ${SYS_MIN_CMAKE_VERSION} or higher is required. You are running version ${CMAKE_VERSION}")
        endif()
    endif()

    # Check required programs
    if(SYS_REQUIRED_PROGRAMS)
        foreach(program ${SYS_REQUIRED_PROGRAMS})
            find_program(${program}_EXECUTABLE ${program})
            if(NOT ${program}_EXECUTABLE)
                message(FATAL_ERROR "Required program not found: ${program}")
            else()
                message(STATUS "Found required program: ${program} (${${program}_EXECUTABLE})")
            endif()
        endforeach()
    endif()

    # Check required libraries
    if(SYS_REQUIRED_LIBRARIES)
        foreach(library ${SYS_REQUIRED_LIBRARIES})
            find_package(${library} REQUIRED)
            if(NOT ${library}_FOUND)
                message(FATAL_ERROR "Required library not found: ${library}")
            else()
                message(STATUS "Found required library: ${library}")
            endif()
        endforeach()
    endif()
endfunction()

# Function to print target information
function(print_target_info target)
    if(TARGET ${target})
        get_target_property(target_type ${target} TYPE)
        get_target_property(target_sources ${target} SOURCES)
        get_target_property(target_include_dirs ${target} INCLUDE_DIRECTORIES)
        get_target_property(target_link_libs ${target} LINK_LIBRARIES)
        
        message(STATUS "")
        message(STATUS "=== Target Information: ${target} ===")
        message(STATUS "Type: ${target_type}")
        if(target_sources)
            list(LENGTH target_sources source_count)
            message(STATUS "Sources: ${source_count} files")
        endif()
        if(target_include_dirs)
            message(STATUS "Include directories: ${target_include_dirs}")
        endif()
        if(target_link_libs)
            message(STATUS "Linked libraries: ${target_link_libs}")
        endif()
        message(STATUS "=====================================")
        message(STATUS "")
    else()
        message(WARNING "Target ${target} does not exist")
    endif()
endfunction()

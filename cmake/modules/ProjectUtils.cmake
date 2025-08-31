# ProjectUtils.cmake
# Reusable CMake functions and utilities

# Create a library target with standard configuration
function(create_library target_name)
    set(options STATIC SHARED INTERFACE HEADER_ONLY)
    set(oneValueArgs OUTPUT_NAME FOLDER)
    set(multiValueArgs SOURCES HEADERS PUBLIC_HEADERS INCLUDE_DIRS LINK_LIBRARIES COMPILE_DEFINITIONS)
    cmake_parse_arguments(LIB "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    # Determine library type
    if(LIB_STATIC)
        set(LIB_TYPE STATIC)
    elseif(LIB_SHARED)
        set(LIB_TYPE SHARED)
    elseif(LIB_INTERFACE OR LIB_HEADER_ONLY)
        set(LIB_TYPE INTERFACE)
    else()
        set(LIB_TYPE STATIC)  # Default to static
    endif()

    # Create the library
    if(LIB_TYPE STREQUAL "INTERFACE")
        add_library(${target_name} INTERFACE)
        if(LIB_HEADERS)
            target_sources(${target_name} INTERFACE ${LIB_HEADERS})
        endif()
    else()
        add_library(${target_name} ${LIB_TYPE})
        if(LIB_SOURCES)
            target_sources(${target_name} PRIVATE ${LIB_SOURCES})
        endif()
        if(LIB_HEADERS)
            target_sources(${target_name} PRIVATE ${LIB_HEADERS})
        endif()
    endif()

    # Set output name if provided
    if(LIB_OUTPUT_NAME)
        set_target_properties(${target_name} PROPERTIES OUTPUT_NAME ${LIB_OUTPUT_NAME})
    endif()

    # Set folder for IDE organization
    if(LIB_FOLDER)
        set_target_properties(${target_name} PROPERTIES FOLDER ${LIB_FOLDER})
    endif()

    # Configure include directories
    if(LIB_INCLUDE_DIRS)
        if(LIB_TYPE STREQUAL "INTERFACE")
            target_include_directories(${target_name} INTERFACE ${LIB_INCLUDE_DIRS})
        else()
            target_include_directories(${target_name} PUBLIC ${LIB_INCLUDE_DIRS})
        endif()
    endif()

    # Link libraries
    if(LIB_LINK_LIBRARIES)
        if(LIB_TYPE STREQUAL "INTERFACE")
            target_link_libraries(${target_name} INTERFACE ${LIB_LINK_LIBRARIES})
        else()
            target_link_libraries(${target_name} PUBLIC ${LIB_LINK_LIBRARIES})
        endif()
    endif()

    # Set compile definitions
    if(LIB_COMPILE_DEFINITIONS)
        if(LIB_TYPE STREQUAL "INTERFACE")
            target_compile_definitions(${target_name} INTERFACE ${LIB_COMPILE_DEFINITIONS})
        else()
            target_compile_definitions(${target_name} PUBLIC ${LIB_COMPILE_DEFINITIONS})
        endif()
    endif()

    # Apply standard configurations
    if(NOT LIB_TYPE STREQUAL "INTERFACE")
        configure_cpp_standard(${target_name})
        configure_compiler_warnings(${target_name})
        apply_build_optimizations(${target_name})
    endif()
endfunction()

# Create an executable target with standard configuration
function(create_executable target_name)
    set(options WIN32 MACOSX_BUNDLE)
    set(oneValueArgs OUTPUT_NAME FOLDER OUTPUT_DIRECTORY)
    set(multiValueArgs SOURCES HEADERS INCLUDE_DIRS LINK_LIBRARIES COMPILE_DEFINITIONS)
    cmake_parse_arguments(EXE "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    # Create the executable
    if(EXE_WIN32)
        add_executable(${target_name} WIN32 ${EXE_SOURCES})
    elseif(EXE_MACOSX_BUNDLE)
        add_executable(${target_name} MACOSX_BUNDLE ${EXE_SOURCES})
    else()
        add_executable(${target_name} ${EXE_SOURCES})
    endif()

    # Add headers if provided
    if(EXE_HEADERS)
        target_sources(${target_name} PRIVATE ${EXE_HEADERS})
    endif()

    # Set output name if provided
    if(EXE_OUTPUT_NAME)
        set_target_properties(${target_name} PROPERTIES OUTPUT_NAME ${EXE_OUTPUT_NAME})
    endif()

    # Set output directory
    if(EXE_OUTPUT_DIRECTORY)
        set_target_properties(${target_name} PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY ${EXE_OUTPUT_DIRECTORY}
        )
    else()
        set_target_properties(${target_name} PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
        )
    endif()

    # Set folder for IDE organization
    if(EXE_FOLDER)
        set_target_properties(${target_name} PROPERTIES FOLDER ${EXE_FOLDER})
    endif()

    # Configure include directories
    if(EXE_INCLUDE_DIRS)
        target_include_directories(${target_name} PRIVATE ${EXE_INCLUDE_DIRS})
    endif()

    # Link libraries
    if(EXE_LINK_LIBRARIES)
        target_link_libraries(${target_name} PRIVATE ${EXE_LINK_LIBRARIES})
    endif()

    # Set compile definitions
    if(EXE_COMPILE_DEFINITIONS)
        target_compile_definitions(${target_name} PRIVATE ${EXE_COMPILE_DEFINITIONS})
    endif()

    # Apply standard configurations
    configure_cpp_standard(${target_name})
    configure_compiler_warnings(${target_name})
    apply_build_optimizations(${target_name})
endfunction()

# Create a test executable with Google Test
function(create_test_executable target_name)
    set(oneValueArgs FOLDER OUTPUT_DIRECTORY WORKING_DIRECTORY DISCOVERY_TIMEOUT)
    set(multiValueArgs SOURCES HEADERS INCLUDE_DIRS LINK_LIBRARIES COMPILE_DEFINITIONS)
    cmake_parse_arguments(TEST "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    # Create the test executable
    add_executable(${target_name} ${TEST_SOURCES})

    # Add headers if provided
    if(TEST_HEADERS)
        target_sources(${target_name} PRIVATE ${TEST_HEADERS})
    endif()

    # Set output directory
    if(TEST_OUTPUT_DIRECTORY)
        set_target_properties(${target_name} PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY ${TEST_OUTPUT_DIRECTORY}
        )
    else()
        set_target_properties(${target_name} PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/bin"
        )
    endif()

    # Set folder for IDE organization
    if(TEST_FOLDER)
        set_target_properties(${target_name} PROPERTIES FOLDER ${TEST_FOLDER})
    else()
        set_target_properties(${target_name} PROPERTIES FOLDER "Tests")
    endif()

    # Configure include directories
    if(TEST_INCLUDE_DIRS)
        target_include_directories(${target_name} PRIVATE ${TEST_INCLUDE_DIRS})
    endif()

    # Link libraries (always include gtest)
    set(TEST_LINK_LIBS gtest_main gtest)
    if(TEST_LINK_LIBRARIES)
        list(APPEND TEST_LINK_LIBS ${TEST_LINK_LIBRARIES})
    endif()
    target_link_libraries(${target_name} PRIVATE ${TEST_LINK_LIBS})

    # Set compile definitions
    if(TEST_COMPILE_DEFINITIONS)
        target_compile_definitions(${target_name} PRIVATE ${TEST_COMPILE_DEFINITIONS})
    endif()

    # Apply standard configurations
    configure_cpp_standard(${target_name})
    configure_compiler_warnings(${target_name})
    configure_unity_build(${target_name})
    configure_test_precompiled_headers(${target_name})

    # Register the test with CTest
    add_test(NAME ${target_name} COMMAND ${target_name})

    # Set working directory if specified
    if(TEST_WORKING_DIRECTORY)
        set_tests_properties(${target_name} PROPERTIES
            WORKING_DIRECTORY ${TEST_WORKING_DIRECTORY}
        )
    endif()

    # Set discovery timeout if specified
    if(TEST_DISCOVERY_TIMEOUT)
        set_tests_properties(${target_name} PROPERTIES
            TIMEOUT ${TEST_DISCOVERY_TIMEOUT}
        )
    endif()

    # Configure test discovery
    include(GoogleTest)
    set(DISCOVERY_TIMEOUT_VAL 30)
    if(TEST_DISCOVERY_TIMEOUT)
        set(DISCOVERY_TIMEOUT_VAL ${TEST_DISCOVERY_TIMEOUT})
    endif()

    set(WORKING_DIR ${CMAKE_CURRENT_BINARY_DIR})
    if(TEST_WORKING_DIRECTORY)
        set(WORKING_DIR ${TEST_WORKING_DIRECTORY})
    endif()

    gtest_discover_tests(${target_name}
        WORKING_DIRECTORY ${WORKING_DIR}
        PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY ${WORKING_DIR}
        DISCOVERY_TIMEOUT ${DISCOVERY_TIMEOUT_VAL}
    )
endfunction()

# Collect source files from directories
function(collect_sources output_var)
    set(options RECURSIVE)
    set(oneValueArgs)
    set(multiValueArgs DIRECTORIES EXTENSIONS)
    cmake_parse_arguments(COLLECT "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if(NOT COLLECT_EXTENSIONS)
        set(COLLECT_EXTENSIONS "*.cpp" "*.c" "*.cxx" "*.cc")
    endif()

    set(collected_sources)
    foreach(dir ${COLLECT_DIRECTORIES})
        foreach(ext ${COLLECT_EXTENSIONS})
            if(COLLECT_RECURSIVE)
                file(GLOB_RECURSE dir_sources "${dir}/${ext}")
            else()
                file(GLOB dir_sources "${dir}/${ext}")
            endif()
            list(APPEND collected_sources ${dir_sources})
        endforeach()
    endforeach()

    set(${output_var} ${collected_sources} PARENT_SCOPE)
endfunction()

# Collect header files from directories
function(collect_headers output_var)
    set(options RECURSIVE)
    set(oneValueArgs)
    set(multiValueArgs DIRECTORIES EXTENSIONS)
    cmake_parse_arguments(COLLECT "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if(NOT COLLECT_EXTENSIONS)
        set(COLLECT_EXTENSIONS "*.h" "*.hpp" "*.hxx" "*.hh")
    endif()

    set(collected_headers)
    foreach(dir ${COLLECT_DIRECTORIES})
        foreach(ext ${COLLECT_EXTENSIONS})
            if(COLLECT_RECURSIVE)
                file(GLOB_RECURSE dir_headers "${dir}/${ext}")
            else()
                file(GLOB dir_headers "${dir}/${ext}")
            endif()
            list(APPEND collected_headers ${dir_headers})
        endforeach()
    endforeach()

    set(${output_var} ${collected_headers} PARENT_SCOPE)
endfunction()

# Print build configuration summary
function(print_build_summary)
    message(STATUS "")
    message(STATUS "=== Build Configuration Summary ===")
    message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
    message(STATUS "C++ standard: C++${CMAKE_CXX_STANDARD}")
    message(STATUS "Compiler: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
    message(STATUS "Compiler path: ${CMAKE_CXX_COMPILER}")
    message(STATUS "Generator: ${CMAKE_GENERATOR}")
    message(STATUS "Unity builds: ${ENABLE_UNITY_BUILD}")
    message(STATUS "Precompiled headers: ${ENABLE_PRECOMPILED_HEADERS}")
    message(STATUS "ccache: ${ENABLE_CCACHE}")
    message(STATUS "Testing: ${BUILD_TESTING}")
    if(CCACHE_PROGRAM)
        message(STATUS "ccache found: ${CCACHE_PROGRAM}")
    endif()
    # Additional compiler detection for troubleshooting
    execute_process(
        COMMAND ${CMAKE_CXX_COMPILER} --version
        OUTPUT_VARIABLE COMPILER_VERSION_CHECK
        ERROR_QUIET
    )
    if(COMPILER_VERSION_CHECK MATCHES "clang")
        message(STATUS "Detected: Clang-based compiler (even if ID shows GNU)")
    elseif(COMPILER_VERSION_CHECK MATCHES "gcc" OR COMPILER_VERSION_CHECK MATCHES "g\\+\\+")
        message(STATUS "Detected: True GCC compiler")
    endif()
    message(STATUS "===================================")
    message(STATUS "")
endfunction()

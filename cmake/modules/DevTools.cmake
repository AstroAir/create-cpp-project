# DevTools.cmake
# Developer tools and convenience targets

# Code Formatting with clang-format
function(add_format_target)
    set(options)
    set(oneValueArgs TARGET_NAME)
    set(multiValueArgs SOURCE_DIRS EXTENSIONS)
    cmake_parse_arguments(FORMAT "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    # Set default target name
    if(NOT FORMAT_TARGET_NAME)
        set(FORMAT_TARGET_NAME format)
    endif()

    # Set default source directories
    if(NOT FORMAT_SOURCE_DIRS)
        set(FORMAT_SOURCE_DIRS src tests)
    endif()

    # Set default extensions
    if(NOT FORMAT_EXTENSIONS)
        set(FORMAT_EXTENSIONS "*.cpp" "*.h" "*.hpp" "*.c" "*.cxx" "*.hxx")
    endif()

    # Find clang-format
    find_program(CLANG_FORMAT_EXECUTABLE clang-format)
    
    if(CLANG_FORMAT_EXECUTABLE)
        # Build find command for source files
        set(FIND_COMMAND find)
        foreach(dir ${FORMAT_SOURCE_DIRS})
            list(APPEND FIND_COMMAND ${dir})
        endforeach()
        
        # Add name patterns
        list(APPEND FIND_COMMAND "(")
        set(first_ext TRUE)
        foreach(ext ${FORMAT_EXTENSIONS})
            if(first_ext)
                list(APPEND FIND_COMMAND -name ${ext})
                set(first_ext FALSE)
            else()
                list(APPEND FIND_COMMAND -o -name ${ext})
            endif()
        endforeach()
        list(APPEND FIND_COMMAND ")")

        add_custom_target(${FORMAT_TARGET_NAME}
            COMMAND ${FIND_COMMAND} | xargs ${CLANG_FORMAT_EXECUTABLE} -i
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            COMMENT "Formatting source code with clang-format"
        )
        
        message(STATUS "Added format target: ${FORMAT_TARGET_NAME}")
    else()
        message(STATUS "clang-format not found, format target not available")
    endif()
endfunction()

# Static Analysis with clang-tidy
function(add_tidy_target)
    set(options)
    set(oneValueArgs TARGET_NAME DEPENDS_ON)
    set(multiValueArgs SOURCE_DIRS EXTENSIONS)
    cmake_parse_arguments(TIDY "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    # Set default target name
    if(NOT TIDY_TARGET_NAME)
        set(TIDY_TARGET_NAME tidy)
    endif()

    # Set default source directories
    if(NOT TIDY_SOURCE_DIRS)
        set(TIDY_SOURCE_DIRS src tests)
    endif()

    # Set default extensions
    if(NOT TIDY_EXTENSIONS)
        set(TIDY_EXTENSIONS "*.cpp" "*.c" "*.cxx")
    endif()

    # Find clang-tidy
    find_program(CLANG_TIDY_EXECUTABLE clang-tidy)
    
    if(CLANG_TIDY_EXECUTABLE)
        # Build file list
        set(source_files)
        foreach(dir ${TIDY_SOURCE_DIRS})
            foreach(ext ${TIDY_EXTENSIONS})
                file(GLOB_RECURSE dir_files "${dir}/${ext}")
                list(APPEND source_files ${dir_files})
            endforeach()
        endforeach()

        add_custom_target(${TIDY_TARGET_NAME}
            COMMAND ${CLANG_TIDY_EXECUTABLE} -p ${CMAKE_BINARY_DIR} ${source_files}
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            COMMENT "Running clang-tidy static analysis"
        )

        if(TIDY_DEPENDS_ON)
            add_dependencies(${TIDY_TARGET_NAME} ${TIDY_DEPENDS_ON})
        endif()
        
        message(STATUS "Added tidy target: ${TIDY_TARGET_NAME}")
    else()
        message(STATUS "clang-tidy not found, tidy target not available")
    endif()
endfunction()

# Documentation Generation with Doxygen
function(add_docs_target)
    set(options)
    set(oneValueArgs TARGET_NAME CONFIG_FILE OUTPUT_DIR)
    set(multiValueArgs)
    cmake_parse_arguments(DOCS "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    # Set default target name
    if(NOT DOCS_TARGET_NAME)
        set(DOCS_TARGET_NAME docs)
    endif()

    # Set default config file
    if(NOT DOCS_CONFIG_FILE)
        set(DOCS_CONFIG_FILE "${CMAKE_SOURCE_DIR}/Doxyfile")
    endif()

    # Find Doxygen
    find_package(Doxygen QUIET)
    
    if(DOXYGEN_FOUND)
        if(EXISTS ${DOCS_CONFIG_FILE})
            add_custom_target(${DOCS_TARGET_NAME}
                COMMAND ${DOXYGEN_EXECUTABLE} ${DOCS_CONFIG_FILE}
                WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
                COMMENT "Generating documentation with Doxygen"
            )
            message(STATUS "Added docs target: ${DOCS_TARGET_NAME}")
        else()
            message(STATUS "Doxygen config file not found: ${DOCS_CONFIG_FILE}")
        endif()
    else()
        message(STATUS "Doxygen not found, docs target not available")
    endif()
endfunction()

# Clean All Build Artifacts
function(add_clean_all_target)
    set(options)
    set(oneValueArgs TARGET_NAME)
    set(multiValueArgs ADDITIONAL_DIRS)
    cmake_parse_arguments(CLEAN "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    # Set default target name
    if(NOT CLEAN_TARGET_NAME)
        set(CLEAN_TARGET_NAME clean-all)
    endif()

    # Build list of directories to clean
    set(DIRS_TO_CLEAN ${CMAKE_BINARY_DIR})
    if(CLEAN_ADDITIONAL_DIRS)
        list(APPEND DIRS_TO_CLEAN ${CLEAN_ADDITIONAL_DIRS})
    endif()

    # Create clean commands
    set(CLEAN_COMMANDS)
    foreach(dir ${DIRS_TO_CLEAN})
        list(APPEND CLEAN_COMMANDS COMMAND ${CMAKE_COMMAND} -E remove_directory ${dir})
        list(APPEND CLEAN_COMMANDS COMMAND ${CMAKE_COMMAND} -E make_directory ${dir})
    endforeach()

    add_custom_target(${CLEAN_TARGET_NAME}
        ${CLEAN_COMMANDS}
        COMMENT "Cleaning all build artifacts"
    )
    
    message(STATUS "Added clean-all target: ${CLEAN_TARGET_NAME}")
endfunction()

# Code Coverage with gcov/lcov
function(add_coverage_target)
    set(options)
    set(oneValueArgs TARGET_NAME TEST_TARGET OUTPUT_DIR)
    set(multiValueArgs)
    cmake_parse_arguments(COV "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    # Set default target name
    if(NOT COV_TARGET_NAME)
        set(COV_TARGET_NAME coverage)
    endif()

    # Set default output directory
    if(NOT COV_OUTPUT_DIR)
        set(COV_OUTPUT_DIR "${CMAKE_BINARY_DIR}/coverage")
    endif()

    # Find required tools
    find_program(GCOV_EXECUTABLE gcov)
    find_program(LCOV_EXECUTABLE lcov)
    find_program(GENHTML_EXECUTABLE genhtml)
    
    if(GCOV_EXECUTABLE AND LCOV_EXECUTABLE AND GENHTML_EXECUTABLE)
        # Add coverage flags for debug builds
        if(CMAKE_BUILD_TYPE STREQUAL "Debug")
            add_compile_options(--coverage -fprofile-arcs -ftest-coverage)
            add_link_options(--coverage)
        endif()

        add_custom_target(${COV_TARGET_NAME}
            # Clean previous coverage data
            COMMAND ${LCOV_EXECUTABLE} --directory . --zerocounters
            # Run tests
            COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
            # Capture coverage data
            COMMAND ${LCOV_EXECUTABLE} --directory . --capture --output-file coverage.info
            # Remove system headers and test files
            COMMAND ${LCOV_EXECUTABLE} --remove coverage.info '/usr/*' '*/tests/*' --output-file coverage.info
            # Generate HTML report
            COMMAND ${GENHTML_EXECUTABLE} coverage.info --output-directory ${COV_OUTPUT_DIR}
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            COMMENT "Generating code coverage report"
        )

        if(COV_TEST_TARGET)
            add_dependencies(${COV_TARGET_NAME} ${COV_TEST_TARGET})
        endif()
        
        message(STATUS "Added coverage target: ${COV_TARGET_NAME}")
        message(STATUS "Coverage report will be generated in: ${COV_OUTPUT_DIR}")
    else()
        message(STATUS "Coverage tools not found (gcov, lcov, genhtml), coverage target not available")
    endif()
endfunction()

# Benchmark Target
function(add_benchmark_target)
    set(options)
    set(oneValueArgs TARGET_NAME BENCHMARK_EXECUTABLE)
    set(multiValueArgs)
    cmake_parse_arguments(BENCH "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    # Set default target name
    if(NOT BENCH_TARGET_NAME)
        set(BENCH_TARGET_NAME benchmark)
    endif()

    if(BENCH_BENCHMARK_EXECUTABLE)
        add_custom_target(${BENCH_TARGET_NAME}
            COMMAND ${BENCH_BENCHMARK_EXECUTABLE}
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            COMMENT "Running benchmarks"
        )
        
        add_dependencies(${BENCH_TARGET_NAME} ${BENCH_BENCHMARK_EXECUTABLE})
        message(STATUS "Added benchmark target: ${BENCH_TARGET_NAME}")
    endif()
endfunction()

# Setup All Developer Tools
function(setup_dev_tools)
    set(options)
    set(oneValueArgs MAIN_TARGET)
    set(multiValueArgs)
    cmake_parse_arguments(DEV_TOOLS "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    add_format_target()
    
    if(DEV_TOOLS_MAIN_TARGET)
        add_tidy_target(DEPENDS_ON ${DEV_TOOLS_MAIN_TARGET})
    else()
        add_tidy_target()
    endif()
    
    add_docs_target()
    add_clean_all_target()
    
    # Add coverage target for debug builds
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        add_coverage_target()
    endif()
endfunction()

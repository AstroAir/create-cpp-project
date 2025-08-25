# TestingEnhanced.cmake
# Enhanced testing integration with better organization and execution

# Configure test categories and labels
function(setup_test_categories)
    # Define test categories
    set(TEST_CATEGORIES
        "unit"          # Unit tests
        "integration"   # Integration tests
        "performance"   # Performance tests
        "regression"    # Regression tests
        "smoke"         # Smoke tests
        "acceptance"    # Acceptance tests
        PARENT_SCOPE
    )
    
    # Define test labels for filtering
    set(TEST_LABELS
        "fast"          # Fast running tests
        "slow"          # Slow running tests
        "network"       # Tests requiring network
        "filesystem"    # Tests requiring filesystem access
        "memory"        # Memory intensive tests
        "cpu"           # CPU intensive tests
        PARENT_SCOPE
    )
endfunction()

# Create a categorized test executable
function(create_categorized_test target_name)
    set(options)
    set(oneValueArgs CATEGORY TIMEOUT WORKING_DIRECTORY)
    set(multiValueArgs SOURCES LABELS LINK_LIBRARIES INCLUDE_DIRS DEPENDS)
    cmake_parse_arguments(TEST "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    # Create the test executable
    create_test_executable(${target_name}
        SOURCES ${TEST_SOURCES}
        LINK_LIBRARIES ${TEST_LINK_LIBRARIES}
        INCLUDE_DIRS ${TEST_INCLUDE_DIRS}
        WORKING_DIRECTORY ${TEST_WORKING_DIRECTORY}
        FOLDER "Tests/${TEST_CATEGORY}"
    )

    # Add dependencies
    if(TEST_DEPENDS)
        add_dependencies(${target_name} ${TEST_DEPENDS})
    endif()

    # Set test properties
    if(TEST_TIMEOUT)
        set_tests_properties(${target_name} PROPERTIES TIMEOUT ${TEST_TIMEOUT})
    endif()

    # Add labels
    if(TEST_LABELS)
        set_tests_properties(${target_name} PROPERTIES LABELS "${TEST_LABELS}")
    endif()

    # Add category label
    if(TEST_CATEGORY)
        set_property(TEST ${target_name} APPEND PROPERTY LABELS ${TEST_CATEGORY})
    endif()
endfunction()

# Setup test data and fixtures
function(setup_test_data target)
    set(options)
    set(oneValueArgs DATA_DIR OUTPUT_DIR)
    set(multiValueArgs DATA_FILES)
    cmake_parse_arguments(DATA "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if(NOT DATA_OUTPUT_DIR)
        set(DATA_OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/test_data")
    endif()

    # Create test data directory
    file(MAKE_DIRECTORY ${DATA_OUTPUT_DIR})

    # Copy test data files
    if(DATA_DATA_FILES)
        foreach(data_file ${DATA_DATA_FILES})
            if(DATA_DATA_DIR)
                set(source_file "${DATA_DATA_DIR}/${data_file}")
            else()
                set(source_file "${data_file}")
            endif()
            
            get_filename_component(filename ${data_file} NAME)
            set(dest_file "${DATA_OUTPUT_DIR}/${filename}")
            
            add_custom_command(
                OUTPUT ${dest_file}
                COMMAND ${CMAKE_COMMAND} -E copy ${source_file} ${dest_file}
                DEPENDS ${source_file}
                COMMENT "Copying test data: ${filename}"
            )
            
            # Add to target dependencies
            add_custom_target(${target}_data_${filename} DEPENDS ${dest_file})
            add_dependencies(${target} ${target}_data_${filename})
        endforeach()
    endif()
endfunction()

# Create test suites with different configurations
function(create_test_suite suite_name)
    set(options PARALLEL)
    set(oneValueArgs TIMEOUT WORKING_DIRECTORY)
    set(multiValueArgs TESTS LABELS DEPENDS)
    cmake_parse_arguments(SUITE "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    # Create a custom target for the test suite
    add_custom_target(${suite_name}
        COMMENT "Running test suite: ${suite_name}"
    )

    # Add test dependencies
    if(SUITE_DEPENDS)
        add_dependencies(${suite_name} ${SUITE_DEPENDS})
    endif()

    # Configure CTest for the suite
    if(SUITE_TESTS)
        foreach(test ${SUITE_TESTS})
            add_dependencies(${suite_name} ${test})
        endforeach()
        
        # Create a CTest script for the suite
        set(CTEST_SCRIPT "${CMAKE_CURRENT_BINARY_DIR}/${suite_name}_ctest.cmake")
        file(WRITE ${CTEST_SCRIPT}
            "set(CTEST_PROJECT_NAME \"${PROJECT_NAME}\")\n"
            "set(CTEST_BINARY_DIRECTORY \"${CMAKE_BINARY_DIR}\")\n"
            "set(CTEST_SOURCE_DIRECTORY \"${CMAKE_SOURCE_DIR}\")\n"
        )
        
        if(SUITE_PARALLEL)
            file(APPEND ${CTEST_SCRIPT} "set(CTEST_PARALLEL_LEVEL 0)\n")
        endif()
        
        if(SUITE_TIMEOUT)
            file(APPEND ${CTEST_SCRIPT} "set(CTEST_TEST_TIMEOUT ${SUITE_TIMEOUT})\n")
        endif()
        
        file(APPEND ${CTEST_SCRIPT} "ctest_start(Experimental)\n")
        file(APPEND ${CTEST_SCRIPT} "ctest_test()\n")
        
        # Add custom command to run the suite
        add_custom_command(TARGET ${suite_name}
            COMMAND ${CMAKE_CTEST_COMMAND} -S ${CTEST_SCRIPT}
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            COMMENT "Running ${suite_name} test suite"
        )
    endif()
endfunction()

# Setup performance testing
function(setup_performance_testing)
    option(ENABLE_PERFORMANCE_TESTS "Enable performance tests" OFF)
    
    if(ENABLE_PERFORMANCE_TESTS)
        # Find benchmark library
        find_package(benchmark QUIET)
        
        if(benchmark_FOUND)
            message(STATUS "Google Benchmark found, performance tests enabled")
            set(PERFORMANCE_TESTING_AVAILABLE TRUE PARENT_SCOPE)
        else()
            message(STATUS "Google Benchmark not found, performance tests disabled")
            set(PERFORMANCE_TESTING_AVAILABLE FALSE PARENT_SCOPE)
        endif()
    endif()
endfunction()

# Create a performance test
function(create_performance_test target_name)
    if(PERFORMANCE_TESTING_AVAILABLE)
        set(options)
        set(oneValueArgs)
        set(multiValueArgs SOURCES LINK_LIBRARIES INCLUDE_DIRS)
        cmake_parse_arguments(PERF "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

        add_executable(${target_name} ${PERF_SOURCES})
        
        target_link_libraries(${target_name} PRIVATE
            benchmark::benchmark
            ${PERF_LINK_LIBRARIES}
        )
        
        if(PERF_INCLUDE_DIRS)
            target_include_directories(${target_name} PRIVATE ${PERF_INCLUDE_DIRS})
        endif()
        
        set_target_properties(${target_name} PROPERTIES
            FOLDER "Tests/Performance"
        )
        
        # Add as a test with performance label
        add_test(NAME ${target_name} COMMAND ${target_name})
        set_tests_properties(${target_name} PROPERTIES
            LABELS "performance;slow"
            TIMEOUT 300
        )
    endif()
endfunction()

# Setup memory testing with Valgrind
function(setup_memory_testing)
    find_program(VALGRIND_EXECUTABLE valgrind)
    
    if(VALGRIND_EXECUTABLE)
        set(VALGRIND_AVAILABLE TRUE PARENT_SCOPE)
        message(STATUS "Valgrind found: ${VALGRIND_EXECUTABLE}")
        
        # Create valgrind test target
        add_custom_target(test_memory
            COMMAND ${CMAKE_CTEST_COMMAND} -L memory -T memcheck
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            COMMENT "Running memory tests with Valgrind"
        )
    else()
        set(VALGRIND_AVAILABLE FALSE PARENT_SCOPE)
        message(STATUS "Valgrind not found, memory testing disabled")
    endif()
endfunction()

# Create test reports
function(setup_test_reporting)
    # Setup XML test output
    set(CTEST_OUTPUT_ON_FAILURE ON)
    
    # Create test report directory
    set(TEST_REPORT_DIR "${CMAKE_BINARY_DIR}/test_reports")
    file(MAKE_DIRECTORY ${TEST_REPORT_DIR})
    
    # Add test report targets
    add_custom_target(test_report
        COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure -T Test
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMENT "Generating test reports"
    )
    
    add_custom_target(test_report_xml
        COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure -T Test --output-junit ${TEST_REPORT_DIR}/test_results.xml
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMENT "Generating XML test reports"
    )
endfunction()

# Setup continuous integration testing
function(setup_ci_testing)
    # CI-specific test configurations
    if(DEFINED ENV{CI} OR DEFINED ENV{GITHUB_ACTIONS} OR DEFINED ENV{GITLAB_CI})
        message(STATUS "CI environment detected, configuring CI-specific testing")
        
        # Disable interactive tests in CI
        add_compile_definitions(CI_BUILD=1)
        
        # Set shorter timeouts for CI
        set(CTEST_TEST_TIMEOUT 120 PARENT_SCOPE)
        
        # Enable parallel testing in CI
        set(CTEST_PARALLEL_LEVEL 0 PARENT_SCOPE)
        
        # Setup test artifacts
        add_custom_target(ci_test_artifacts
            COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_BINARY_DIR}/Testing ${CMAKE_BINARY_DIR}/test_artifacts
            COMMENT "Collecting CI test artifacts"
        )
    endif()
endfunction()

# Enhanced testing setup function
function(setup_enhanced_testing)
    setup_testing_environment()
    setup_test_categories()
    setup_performance_testing()
    setup_memory_testing()
    setup_test_reporting()
    setup_ci_testing()
    
    message(STATUS "Enhanced testing framework configured")
endfunction()

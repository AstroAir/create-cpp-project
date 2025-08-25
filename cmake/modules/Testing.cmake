# Testing.cmake
# Test configuration and utilities

# Setup Testing Environment
function(setup_testing_environment)
    option(BUILD_TESTING "Build the testing tree." ON)
    
    if(BUILD_TESTING)
        enable_testing()
        
        # Enable parallel test execution
        if(CMAKE_VERSION VERSION_GREATER_EQUAL "3.17")
            list(APPEND CMAKE_CTEST_ARGUMENTS "--parallel")
        endif()
        
        # Include Google Test
        include(GoogleTest)
        
        message(STATUS "Testing enabled")
    else()
        message(STATUS "Testing disabled")
    endif()
endfunction()

# Create Test Library
function(create_test_library target_name)
    set(options)
    set(oneValueArgs FOLDER OUTPUT_DIRECTORY)
    set(multiValueArgs SOURCES HEADERS INCLUDE_DIRS LINK_LIBRARIES COMPILE_DEFINITIONS)
    cmake_parse_arguments(TEST_LIB "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    # Create test library
    add_library(${target_name} STATIC ${TEST_LIB_SOURCES})

    # Configure library properties
    set_target_properties(${target_name} PROPERTIES
        POSITION_INDEPENDENT_CODE ON
    )

    if(TEST_LIB_OUTPUT_DIRECTORY)
        set_target_properties(${target_name} PROPERTIES
            ARCHIVE_OUTPUT_DIRECTORY ${TEST_LIB_OUTPUT_DIRECTORY}
        )
    else()
        set_target_properties(${target_name} PROPERTIES
            ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/lib"
        )
    endif()

    # Set folder for IDE organization
    if(TEST_LIB_FOLDER)
        set_target_properties(${target_name} PROPERTIES FOLDER ${TEST_LIB_FOLDER})
    else()
        set_target_properties(${target_name} PROPERTIES FOLDER "Tests")
    endif()

    # Configure include directories
    if(TEST_LIB_INCLUDE_DIRS)
        target_include_directories(${target_name} PUBLIC ${TEST_LIB_INCLUDE_DIRS})
    endif()

    # Link libraries
    if(TEST_LIB_LINK_LIBRARIES)
        target_link_libraries(${target_name} PUBLIC ${TEST_LIB_LINK_LIBRARIES})
    endif()

    # Set compile definitions
    if(TEST_LIB_COMPILE_DEFINITIONS)
        target_compile_definitions(${target_name} PUBLIC ${TEST_LIB_COMPILE_DEFINITIONS})
    endif()

    # Apply standard configurations
    configure_cpp_standard(${target_name})
    configure_compiler_warnings(${target_name})
    configure_unity_build(${target_name})
    configure_test_precompiled_headers(${target_name})
endfunction()

# Add Multiple Test Executables
function(add_test_executables)
    set(options)
    set(oneValueArgs TEST_LIBRARY FOLDER OUTPUT_DIRECTORY WORKING_DIRECTORY DISCOVERY_TIMEOUT)
    set(multiValueArgs TEST_FILES INCLUDE_DIRS LINK_LIBRARIES COMPILE_DEFINITIONS)
    cmake_parse_arguments(TESTS "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    foreach(TEST_FILE ${TESTS_TEST_FILES})
        get_filename_component(TEST_NAME ${TEST_FILE} NAME_WE)
        
        create_test_executable(${TEST_NAME}
            SOURCES ${TEST_FILE}
            FOLDER ${TESTS_FOLDER}
            OUTPUT_DIRECTORY ${TESTS_OUTPUT_DIRECTORY}
            WORKING_DIRECTORY ${TESTS_WORKING_DIRECTORY}
            DISCOVERY_TIMEOUT ${TESTS_DISCOVERY_TIMEOUT}
            INCLUDE_DIRS ${TESTS_INCLUDE_DIRS}
            LINK_LIBRARIES ${TESTS_TEST_LIBRARY} ${TESTS_LINK_LIBRARIES}
            COMPILE_DEFINITIONS ${TESTS_COMPILE_DEFINITIONS}
        )
    endforeach()
endfunction()

# Create Comprehensive Test Executable
function(create_comprehensive_test target_name)
    set(options)
    set(oneValueArgs TEST_LIBRARY FOLDER OUTPUT_DIRECTORY WORKING_DIRECTORY DISCOVERY_TIMEOUT TEST_PREFIX)
    set(multiValueArgs TEST_FILES INCLUDE_DIRS LINK_LIBRARIES COMPILE_DEFINITIONS)
    cmake_parse_arguments(COMP_TEST "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    # Create comprehensive test executable
    add_executable(${target_name} ${COMP_TEST_TEST_FILES})

    # Configure test executable properties
    if(COMP_TEST_OUTPUT_DIRECTORY)
        set_target_properties(${target_name} PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY ${COMP_TEST_OUTPUT_DIRECTORY}
        )
    else()
        set_target_properties(${target_name} PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/bin"
        )
    endif()

    # Set folder for IDE organization
    if(COMP_TEST_FOLDER)
        set_target_properties(${target_name} PROPERTIES FOLDER ${COMP_TEST_FOLDER})
    else()
        set_target_properties(${target_name} PROPERTIES FOLDER "Tests")
    endif()

    # Configure include directories
    if(COMP_TEST_INCLUDE_DIRS)
        target_include_directories(${target_name} PRIVATE ${COMP_TEST_INCLUDE_DIRS})
    endif()

    # Link libraries
    set(TEST_LINK_LIBS gtest_main gtest)
    if(COMP_TEST_TEST_LIBRARY)
        list(APPEND TEST_LINK_LIBS ${COMP_TEST_TEST_LIBRARY})
    endif()
    if(COMP_TEST_LINK_LIBRARIES)
        list(APPEND TEST_LINK_LIBS ${COMP_TEST_LINK_LIBRARIES})
    endif()
    target_link_libraries(${target_name} PRIVATE ${TEST_LINK_LIBS})

    # Set compile definitions
    if(COMP_TEST_COMPILE_DEFINITIONS)
        target_compile_definitions(${target_name} PRIVATE ${COMP_TEST_COMPILE_DEFINITIONS})
    endif()

    # Apply standard configurations
    configure_cpp_standard(${target_name})
    configure_compiler_warnings(${target_name})
    configure_unity_build(${target_name})
    configure_test_precompiled_headers(${target_name})

    # Configure test discovery
    set(DISCOVERY_TIMEOUT_VAL 60)
    if(COMP_TEST_DISCOVERY_TIMEOUT)
        set(DISCOVERY_TIMEOUT_VAL ${COMP_TEST_DISCOVERY_TIMEOUT})
    endif()

    set(WORKING_DIR ${CMAKE_CURRENT_BINARY_DIR})
    if(COMP_TEST_WORKING_DIRECTORY)
        set(WORKING_DIR ${COMP_TEST_WORKING_DIRECTORY})
    endif()

    set(PREFIX "")
    if(COMP_TEST_TEST_PREFIX)
        set(PREFIX ${COMP_TEST_TEST_PREFIX})
    endif()

    gtest_discover_tests(${target_name}
        WORKING_DIRECTORY ${WORKING_DIR}
        PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY ${WORKING_DIR}
        DISCOVERY_TIMEOUT ${DISCOVERY_TIMEOUT_VAL}
        TEST_PREFIX ${PREFIX}
    )
endfunction()

# Add Custom Test Targets
function(add_custom_test_targets)
    set(options)
    set(oneValueArgs COMPREHENSIVE_TEST)
    set(multiValueArgs DEPENDENCIES)
    cmake_parse_arguments(CUSTOM_TESTS "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    # Set default dependencies
    set(TEST_DEPENDENCIES)
    if(CUSTOM_TESTS_COMPREHENSIVE_TEST)
        list(APPEND TEST_DEPENDENCIES ${CUSTOM_TESTS_COMPREHENSIVE_TEST})
    endif()
    if(CUSTOM_TESTS_DEPENDENCIES)
        list(APPEND TEST_DEPENDENCIES ${CUSTOM_TESTS_DEPENDENCIES})
    endif()

    # Add custom test targets for convenience
    add_custom_target(run_tests
        COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure --parallel
        DEPENDS ${TEST_DEPENDENCIES}
        COMMENT "Running all tests"
    )

    add_custom_target(run_tests_verbose
        COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure --verbose --parallel
        DEPENDS ${TEST_DEPENDENCIES}
        COMMENT "Running all tests with verbose output"
    )

    add_custom_target(run_tests_xml
        COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure --parallel -T Test
        DEPENDS ${TEST_DEPENDENCIES}
        COMMENT "Running all tests with XML output"
    )
endfunction()

# Setup Complete Testing Infrastructure
function(setup_complete_testing)
    set(options)
    set(oneValueArgs TEST_LIBRARY_NAME COMPREHENSIVE_TEST_NAME)
    set(multiValueArgs TEST_SOURCES TEST_FILES INCLUDE_DIRS LINK_LIBRARIES)
    cmake_parse_arguments(SETUP_TEST "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    # Setup testing environment
    setup_testing_environment()

    if(BUILD_TESTING)
        # Create test library if sources provided
        if(SETUP_TEST_TEST_SOURCES AND SETUP_TEST_TEST_LIBRARY_NAME)
            create_test_library(${SETUP_TEST_TEST_LIBRARY_NAME}
                SOURCES ${SETUP_TEST_TEST_SOURCES}
                INCLUDE_DIRS ${SETUP_TEST_INCLUDE_DIRS}
                LINK_LIBRARIES ${SETUP_TEST_LINK_LIBRARIES}
            )
        endif()

        # Add individual test executables
        if(SETUP_TEST_TEST_FILES)
            add_test_executables(
                TEST_FILES ${SETUP_TEST_TEST_FILES}
                TEST_LIBRARY ${SETUP_TEST_TEST_LIBRARY_NAME}
                INCLUDE_DIRS ${SETUP_TEST_INCLUDE_DIRS}
                LINK_LIBRARIES ${SETUP_TEST_LINK_LIBRARIES}
            )

            # Create comprehensive test executable
            if(SETUP_TEST_COMPREHENSIVE_TEST_NAME)
                create_comprehensive_test(${SETUP_TEST_COMPREHENSIVE_TEST_NAME}
                    TEST_FILES ${SETUP_TEST_TEST_FILES}
                    TEST_LIBRARY ${SETUP_TEST_TEST_LIBRARY_NAME}
                    INCLUDE_DIRS ${SETUP_TEST_INCLUDE_DIRS}
                    LINK_LIBRARIES ${SETUP_TEST_LINK_LIBRARIES}
                    TEST_PREFIX "AllTests."
                )
            endif()

            # Add custom test targets
            add_custom_test_targets(
                COMPREHENSIVE_TEST ${SETUP_TEST_COMPREHENSIVE_TEST_NAME}
            )
        endif()
    endif()
endfunction()

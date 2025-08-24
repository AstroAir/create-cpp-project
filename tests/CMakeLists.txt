# Modern test configuration for cpp-scaffold
cmake_minimum_required(VERSION 3.20)

# Include Google Test and enable parallel test discovery
include(GoogleTest)

# Enable parallel test execution
if(CMAKE_VERSION VERSION_GREATER_EQUAL "3.17")
    list(APPEND CMAKE_CTEST_ARGUMENTS "--parallel")
endif()

# Create test library with all source files except main.cpp
set(TEST_SOURCES
    ../src/cli/cli_parser.cpp
    ../src/cli/enhanced_cli_parser.cpp
    ../src/cli/project_wizard.cpp
    ../src/templates/template_manager.cpp
    ../src/templates/template_base.cpp
    ../src/templates/console_template.cpp
    ../src/templates/library_template.cpp
    ../src/templates/header_only_lib_template.cpp
    # ../src/templates/multi_executable_template.cpp
    ../src/templates/gui_template.cpp
    ../src/templates/network_template.cpp
    ../src/templates/custom_template.cpp
    ../src/templates/webservice_template.cpp
    ../src/templates/embedded_template.cpp
    ../src/templates/gameengine_template.cpp
    ../src/templates/template_discovery.cpp
    ../src/utils/file_utils.cpp
    ../src/utils/string_utils.cpp
    ../src/utils/terminal_utils.cpp
    ../src/utils/git_utils.cpp
    ../src/utils/framework_integration.cpp
    ../src/utils/progress_indicator.cpp
    ../src/utils/interactive_menu.cpp
    ../src/utils/dependency_manager.cpp
    # ../src/utils/post_creation_actions.cpp
    ../src/config/editor_config.cpp
    ../src/config/ci_config.cpp
    ../src/config/code_style_config.cpp
    ../src/config/doc_config.cpp
    ../src/config/config_manager.cpp
    ../src/config/enhanced_config_manager.cpp
    ../src/config/user_preferences.cpp
    ../src/config/interactive_config.cpp
    ../src/config/project_profiles.cpp
    ../src/config/config_validator.cpp
    ../src/testing/test_framework_manager.cpp
    ../src/documentation/doc_generator.cpp
)

# Create a modern test library with proper target configuration
add_library(cpp_scaffold_lib STATIC ${TEST_SOURCES})

# Configure library properties
set_target_properties(cpp_scaffold_lib PROPERTIES
    POSITION_INDEPENDENT_CODE ON
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/lib"
)

# Modern target-based include directories
target_include_directories(cpp_scaffold_lib PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../src>
    $<INSTALL_INTERFACE:include>
)

# Link dependencies using modern target-based approach
target_link_libraries(cpp_scaffold_lib PUBLIC
    spdlog::spdlog
    nlohmann_json::nlohmann_json
)

# Link filesystem library if needed
if(DEFINED FILESYSTEM_LIB)
    target_link_libraries(cpp_scaffold_lib PUBLIC ${FILESYSTEM_LIB})
endif()

# Platform-specific libraries
if(WIN32)
    target_link_libraries(cpp_scaffold_lib PUBLIC shlwapi)
endif()

# Enable Unity builds for test library if enabled globally
if(ENABLE_UNITY_BUILD)
    set_target_properties(cpp_scaffold_lib PROPERTIES UNITY_BUILD ON)
endif()

# Configure precompiled headers for test library
if(ENABLE_PRECOMPILED_HEADERS)
    target_precompile_headers(cpp_scaffold_lib PRIVATE
        <gtest/gtest.h>
        <gmock/gmock.h>
        <iostream>
        <string>
        <vector>
        <memory>
        <filesystem>
    )
endif()

# Test executables
set(TEST_FILES
    test_file_utils.cpp
    test_string_utils.cpp
    test_cli_parser.cpp
    test_template_manager.cpp
    test_terminal_utils.cpp
    test_integration.cpp
    test_template_edge_cases.cpp
    test_cli_edge_cases.cpp
    test_file_utils_edge_cases.cpp
)

# Create optimized test executables with better organization
foreach(TEST_FILE ${TEST_FILES})
    get_filename_component(TEST_NAME ${TEST_FILE} NAME_WE)
    add_executable(${TEST_NAME} ${TEST_FILE})

    # Configure test executable properties
    set_target_properties(${TEST_NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/bin"
        FOLDER "Tests"
    )

    # Link libraries using modern approach
    target_link_libraries(${TEST_NAME} PRIVATE
        cpp_scaffold_lib
        gtest_main
        gtest
    )

    # Discover tests with better configuration
    gtest_discover_tests(${TEST_NAME}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        DISCOVERY_TIMEOUT 30
    )

    # Enable Unity builds for individual tests if enabled
    if(ENABLE_UNITY_BUILD)
        set_target_properties(${TEST_NAME} PROPERTIES UNITY_BUILD ON)
    endif()
endforeach()

# Add a comprehensive test executable with all tests
add_executable(all_tests ${TEST_FILES})

# Configure comprehensive test properties
set_target_properties(all_tests PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/bin"
    FOLDER "Tests"
)

# Link libraries
target_link_libraries(all_tests PRIVATE
    cpp_scaffold_lib
    gtest_main
    gtest
)

# Discover all tests with optimized settings
gtest_discover_tests(all_tests
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    DISCOVERY_TIMEOUT 60
    TEST_PREFIX "AllTests."
)

# Enable Unity builds for comprehensive test if enabled
if(ENABLE_UNITY_BUILD)
    set_target_properties(all_tests PROPERTIES UNITY_BUILD ON)
endif()

# Add custom test targets for convenience
add_custom_target(run_tests
    COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure --parallel
    DEPENDS all_tests
    COMMENT "Running all tests"
)

add_custom_target(run_tests_verbose
    COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure --verbose --parallel
    DEPENDS all_tests
    COMMENT "Running all tests with verbose output"
)

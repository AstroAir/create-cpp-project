# Test configuration for cpp-scaffold
cmake_minimum_required(VERSION 3.14)

# Include Google Test
include(GoogleTest)

# Create test library with all source files except main.cpp
set(TEST_SOURCES
    ../src/cli/cli_parser.cpp
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
    ../src/utils/file_utils.cpp
    ../src/utils/string_utils.cpp
    ../src/utils/terminal_utils.cpp
    ../src/utils/git_utils.cpp
    ../src/utils/framework_integration.cpp
    ../src/utils/progress_indicator.cpp
    ../src/utils/interactive_menu.cpp
    # ../src/utils/dependency_manager.cpp
    # ../src/utils/post_creation_actions.cpp
    ../src/config/editor_config.cpp
    ../src/config/ci_config.cpp
    ../src/config/code_style_config.cpp
    ../src/config/doc_config.cpp
    ../src/config/config_manager.cpp
    ../src/config/user_preferences.cpp
    ../src/config/interactive_config.cpp
    ../src/config/project_profiles.cpp
    ../src/config/config_validator.cpp
    ../src/testing/test_framework_manager.cpp
    ../src/documentation/doc_generator.cpp
)

# Create a library for testing
add_library(cpp_scaffold_lib ${TEST_SOURCES})

# Link dependencies to the library
target_link_libraries(cpp_scaffold_lib PRIVATE
    spdlog::spdlog
    nlohmann_json::nlohmann_json
)

if(DEFINED FILESYSTEM_LIB)
    target_link_libraries(cpp_scaffold_lib PRIVATE ${FILESYSTEM_LIB})
endif()

if(WIN32)
    target_link_libraries(cpp_scaffold_lib PRIVATE shlwapi)
endif()

# Include directories for the library
target_include_directories(cpp_scaffold_lib PUBLIC ../src)

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

# Create individual test executables
foreach(TEST_FILE ${TEST_FILES})
    get_filename_component(TEST_NAME ${TEST_FILE} NAME_WE)
    add_executable(${TEST_NAME} ${TEST_FILE})

    target_link_libraries(${TEST_NAME} PRIVATE
        cpp_scaffold_lib
        gtest_main
        gtest
    )

    target_include_directories(${TEST_NAME} PRIVATE ../src)

    # Discover tests
    gtest_discover_tests(${TEST_NAME})
endforeach()

# Add a comprehensive test that runs all tests
add_executable(all_tests
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

target_link_libraries(all_tests PRIVATE
    cpp_scaffold_lib
    gtest_main
    gtest
)

target_include_directories(all_tests PRIVATE ../src)
gtest_discover_tests(all_tests)

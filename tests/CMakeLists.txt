# Modern test configuration for cpp-scaffold using modular components

# Create test library using component libraries
# Note: We don't need separate sources since we're linking to component libraries
add_library(cpp_scaffold_test_lib INTERFACE)

target_include_directories(cpp_scaffold_test_lib INTERFACE ../src)

target_link_libraries(cpp_scaffold_test_lib INTERFACE
    cpp_scaffold_cli
    cpp_scaffold_templates
    cpp_scaffold_utils
    cpp_scaffold_config
    cpp_scaffold_testing
    cpp_scaffold_documentation
)

set_target_properties(cpp_scaffold_test_lib PROPERTIES FOLDER "Tests")

# Setup complete testing infrastructure
setup_complete_testing(
    TEST_LIBRARY_NAME cpp_scaffold_test_lib
    COMPREHENSIVE_TEST_NAME all_tests
    TEST_FILES
        test_file_utils.cpp
        test_string_utils.cpp
        test_cli_parser.cpp
        test_template_manager.cpp
        test_terminal_utils.cpp
        test_integration.cpp
        test_template_edge_cases.cpp
        test_cli_edge_cases.cpp
        test_file_utils_edge_cases.cpp
        # New comprehensive test files
        test_config_manager.cpp
        test_config_validator.cpp
        test_ci_config.cpp
        test_doc_generator.cpp
        test_test_framework_manager.cpp
    INCLUDE_DIRS ../src
    LINK_LIBRARIES cpp_scaffold_test_lib
)

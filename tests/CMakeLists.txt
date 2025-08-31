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
        unit/utils/test_file_utils.cpp
        unit/utils/test_string_utils.cpp
        unit/cli/test_cli_parser.cpp
        unit/templates/test_template_manager.cpp
        unit/utils/test_terminal_utils.cpp
        integration/test_end_to_end.cpp
        unit/config/test_config_manager.cpp
        unit/config/test_config_validator.cpp
        unit/config/test_ci_config.cpp
        unit/documentation/test_doc_generator.cpp
        unit/testing/test_test_framework_manager.cpp
    INCLUDE_DIRS ../src
    LINK_LIBRARIES cpp_scaffold_test_lib
)

# CLI Tests CMakeLists.txt

# Find required packages
find_package(GTest REQUIRED)

# Create test executable for refactored CLI
add_executable(test_refactored_cli
    test_refactored_cli.cpp
    
    # Include the refactored CLI source files
    ../../src/cli/types/cli_enums.cpp
    ../../src/cli/localization/localization.cpp
    ../../src/cli/validation/cli_validator.cpp
    ../../src/cli/parsing/argument_parser.cpp
    ../../src/cli/input/user_input.cpp
    ../../src/cli/commands/command_handlers.cpp
)

# Link libraries
target_link_libraries(test_refactored_cli
    GTest::GTest
    GTest::Main
    fmt::fmt
    spdlog::spdlog
)

# Include directories
target_include_directories(test_refactored_cli PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/include
)

# Add test to CTest
add_test(NAME RefactoredCliTests COMMAND test_refactored_cli)

# Set test properties
set_tests_properties(RefactoredCliTests PROPERTIES
    TIMEOUT 30
    LABELS "cli;unit"
)

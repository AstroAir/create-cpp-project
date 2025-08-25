# Remote Sources Test Suite
# Tests for Git repository and archive-based project creation

cmake_minimum_required(VERSION 3.16)

# Find required packages
find_package(GTest REQUIRED)
find_package(GMock REQUIRED)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(${GTEST_INCLUDE_DIRS})
include_directories(${GMOCK_INCLUDE_DIRS})

# Test executable for Git Repository Template
add_executable(test_git_repository_template
    test_git_repository_template.cpp
    ${CMAKE_SOURCE_DIR}/src/templates/git_repository_template.cpp
    ${CMAKE_SOURCE_DIR}/src/utils/git_utils.cpp
    ${CMAKE_SOURCE_DIR}/src/cli/types/cli_options.cpp
    ${CMAKE_SOURCE_DIR}/src/cli/types/cli_enums.cpp
)

target_link_libraries(test_git_repository_template
    ${GTEST_LIBRARIES}
    ${GMOCK_LIBRARIES}
    pthread
    spdlog::spdlog
)

# Test executable for Archive Template
add_executable(test_archive_template
    test_archive_template.cpp
    ${CMAKE_SOURCE_DIR}/src/templates/archive_template.cpp
    ${CMAKE_SOURCE_DIR}/src/utils/archive_utils.cpp
    ${CMAKE_SOURCE_DIR}/src/utils/http_client.cpp
    ${CMAKE_SOURCE_DIR}/src/cli/types/cli_options.cpp
    ${CMAKE_SOURCE_DIR}/src/cli/types/cli_enums.cpp
)

target_link_libraries(test_archive_template
    ${GTEST_LIBRARIES}
    ${GMOCK_LIBRARIES}
    pthread
    spdlog::spdlog
)

# Test executable for HTTP Client
add_executable(test_http_client
    test_http_client.cpp
    ${CMAKE_SOURCE_DIR}/src/utils/http_client.cpp
    ${CMAKE_SOURCE_DIR}/src/utils/file_utils.cpp
)

target_link_libraries(test_http_client
    ${GTEST_LIBRARIES}
    ${GMOCK_LIBRARIES}
    pthread
    spdlog::spdlog
)

# Test executable for Archive Utils
add_executable(test_archive_utils
    test_archive_utils.cpp
    ${CMAKE_SOURCE_DIR}/src/utils/archive_utils.cpp
    ${CMAKE_SOURCE_DIR}/src/utils/file_utils.cpp
)

target_link_libraries(test_archive_utils
    ${GTEST_LIBRARIES}
    ${GMOCK_LIBRARIES}
    pthread
    spdlog::spdlog
)

# Add tests to CTest
enable_testing()

add_test(NAME GitRepositoryTemplateTests COMMAND test_git_repository_template)
add_test(NAME ArchiveTemplateTests COMMAND test_archive_template)
add_test(NAME HttpClientTests COMMAND test_http_client)
add_test(NAME ArchiveUtilsTests COMMAND test_archive_utils)

# Test categories with different requirements
add_test(NAME GitRepositoryIntegrationTests 
         COMMAND test_git_repository_template --gtest_filter="*Integration*")
set_tests_properties(GitRepositoryIntegrationTests PROPERTIES
    ENVIRONMENT "ENABLE_INTEGRATION_TESTS=1"
    LABELS "integration;network"
)

add_test(NAME ArchiveIntegrationTests 
         COMMAND test_archive_template --gtest_filter="*Integration*")
set_tests_properties(ArchiveIntegrationTests PROPERTIES
    ENVIRONMENT "ENABLE_INTEGRATION_TESTS=1"
    LABELS "integration;network"
)

add_test(NAME HttpClientIntegrationTests 
         COMMAND test_http_client --gtest_filter="*Integration*")
set_tests_properties(HttpClientIntegrationTests PROPERTIES
    ENVIRONMENT "ENABLE_INTEGRATION_TESTS=1"
    LABELS "integration;network"
)

add_test(NAME PerformanceTests 
         COMMAND test_http_client --gtest_filter="*Performance*")
set_tests_properties(PerformanceTests PROPERTIES
    ENVIRONMENT "ENABLE_PERFORMANCE_TESTS=1"
    LABELS "performance"
)

# Custom test targets
add_custom_target(test_remote_sources
    COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
    DEPENDS test_git_repository_template test_archive_template test_http_client test_archive_utils
    COMMENT "Running remote sources test suite"
)

add_custom_target(test_integration
    COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure -L integration
    DEPENDS test_git_repository_template test_archive_template test_http_client
    COMMENT "Running integration tests (requires network)"
)

add_custom_target(test_performance
    COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure -L performance
    DEPENDS test_http_client
    COMMENT "Running performance tests"
)

# Test data directory
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/test_data)

# Copy test data files if they exist
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/test_data)
    file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/test_data/
         DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/test_data/)
endif()

# Set test properties
set_tests_properties(GitRepositoryTemplateTests PROPERTIES
    TIMEOUT 30
    LABELS "unit"
)

set_tests_properties(ArchiveTemplateTests PROPERTIES
    TIMEOUT 30
    LABELS "unit"
)

set_tests_properties(HttpClientTests PROPERTIES
    TIMEOUT 30
    LABELS "unit"
)

set_tests_properties(ArchiveUtilsTests PROPERTIES
    TIMEOUT 30
    LABELS "unit"
)

# Coverage target (if gcov is available)
if(CMAKE_BUILD_TYPE STREQUAL "Debug" AND CMAKE_COMPILER_IS_GNUCXX)
    find_program(GCOV_PATH gcov)
    if(GCOV_PATH)
        add_custom_target(coverage
            COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
            COMMAND gcov ${CMAKE_CURRENT_BINARY_DIR}/*.gcno
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
            COMMENT "Generating test coverage report"
        )
    endif()
endif()

# Documentation for running tests
add_custom_target(test_help
    COMMAND ${CMAKE_COMMAND} -E echo "Available test targets:"
    COMMAND ${CMAKE_COMMAND} -E echo "  test_remote_sources  - Run all remote source tests"
    COMMAND ${CMAKE_COMMAND} -E echo "  test_integration     - Run integration tests (requires network)"
    COMMAND ${CMAKE_COMMAND} -E echo "  test_performance     - Run performance tests"
    COMMAND ${CMAKE_COMMAND} -E echo ""
    COMMAND ${CMAKE_COMMAND} -E echo "Environment variables:"
    COMMAND ${CMAKE_COMMAND} -E echo "  ENABLE_INTEGRATION_TESTS=1  - Enable network-dependent tests"
    COMMAND ${CMAKE_COMMAND} -E echo "  ENABLE_PERFORMANCE_TESTS=1  - Enable performance benchmarks"
    COMMENT "Showing test help"
)

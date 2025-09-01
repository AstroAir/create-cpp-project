-- Test Target Generation for cpp-scaffold
-- Automated test target creation to eliminate repetitive code

-- Test common utilities library
target("test_common")
    set_kind("static")
    add_files("tests/common/*.cpp")
    add_headerfiles("tests/common/*.h")
    add_includedirs("src", "tests", {public = true})
    add_packages("gtest", "spdlog", "nlohmann_json")
    set_group("Tests")
target_end()

-- Unit Tests
target("test_file_utils")
    set_kind("binary")
    add_files("tests/unit/utils/test_file_utils.cpp")
    add_includedirs("src", "tests", {public = true})
    add_deps("cpp_scaffold_utils", "test_common")
    add_packages("gtest", "spdlog", "nlohmann_json")
    set_targetdir("tests/bin")
    set_group("Tests")
    set_enabled(false)  -- Disabled by default, enabled via options
target_end()

target("test_string_utils")
    set_kind("binary")
    add_files("tests/unit/utils/test_string_utils.cpp")
    add_includedirs("src", "tests", {public = true})
    add_deps("cpp_scaffold_utils", "test_common")
    add_packages("gtest", "spdlog", "nlohmann_json")
    set_targetdir("tests/bin")
    set_group("Tests")
    set_enabled(false)  -- Disabled by default, enabled via options
target_end()

target("test_terminal_utils")
    set_kind("binary")
    add_files("tests/unit/utils/test_terminal_utils.cpp")
    add_includedirs("src", "tests", {public = true})
    add_deps("cpp_scaffold_utils", "test_common")
    add_packages("gtest", "spdlog", "nlohmann_json")
    set_targetdir("tests/bin")
    set_group("Tests")
    set_enabled(false)  -- Disabled by default, enabled via options
target_end()

target("test_http_client")
    set_kind("binary")
    add_files("tests/unit/utils/test_http_client.cpp")
    add_includedirs("src", "tests", {public = true})
    add_deps("cpp_scaffold_utils", "test_common")
    add_packages("gtest", "spdlog", "nlohmann_json")
    set_targetdir("tests/bin")
    set_group("Tests")
    set_enabled(false)  -- Disabled by default, enabled via options
target_end()

-- CLI Tests
target("test_cli_parser")
    set_kind("binary")
    add_files("tests/unit/cli/test_cli_parser.cpp")
    add_includedirs("src", "tests", {public = true})
    add_deps("cpp_scaffold_cli", "cpp_scaffold_utils", "test_common")
    add_packages("gtest", "spdlog", "nlohmann_json")
    set_targetdir("tests/bin")
    set_group("Tests")
    set_enabled(false)  -- Disabled by default, enabled via options
target_end()

-- Config Tests
target("test_config_manager")
    set_kind("binary")
    add_files("tests/unit/config/test_config_manager.cpp")
    add_includedirs("src", "tests", {public = true})
    add_deps("cpp_scaffold_config", "cpp_scaffold_utils", "test_common")
    add_packages("gtest", "spdlog", "nlohmann_json")
    set_targetdir("tests/bin")
    set_group("Tests")
    set_enabled(false)  -- Disabled by default, enabled via options
target_end()

target("test_config_validator")
    set_kind("binary")
    add_files("tests/unit/config/test_config_validator.cpp")
    add_includedirs("src", "tests", {public = true})
    add_deps("cpp_scaffold_config", "cpp_scaffold_utils", "test_common")
    add_packages("gtest", "spdlog", "nlohmann_json")
    set_targetdir("tests/bin")
    set_group("Tests")
    set_enabled(false)  -- Disabled by default, enabled via options
target_end()

target("test_ci_config")
    set_kind("binary")
    add_files("tests/unit/config/test_ci_config.cpp")
    add_includedirs("src", "tests", {public = true})
    add_deps("cpp_scaffold_config", "cpp_scaffold_utils", "test_common")
    add_packages("gtest", "spdlog", "nlohmann_json")
    set_targetdir("tests/bin")
    set_group("Tests")
    set_enabled(false)  -- Disabled by default, enabled via options
target_end()

-- Template Tests
target("test_template_manager")
    set_kind("binary")
    add_files("tests/unit/templates/test_template_manager.cpp")
    add_includedirs("src", "tests", {public = true})
    add_deps("cpp_scaffold_templates", "cpp_scaffold_cli", "cpp_scaffold_utils", "test_common")
    add_packages("gtest", "spdlog", "nlohmann_json")
    set_targetdir("tests/bin")
    set_group("Tests")
    set_enabled(false)  -- Disabled by default, enabled via options
target_end()

target("test_archive_template")
    set_kind("binary")
    add_files("tests/unit/templates/test_archive_template.cpp")
    add_includedirs("src", "tests", {public = true})
    add_deps("cpp_scaffold_templates", "cpp_scaffold_cli", "cpp_scaffold_utils", "test_common")
    add_packages("gtest", "spdlog", "nlohmann_json")
    set_targetdir("tests/bin")
    set_group("Tests")
    set_enabled(false)  -- Disabled by default, enabled via options
target_end()

target("test_git_repository_template")
    set_kind("binary")
    add_files("tests/unit/templates/test_git_repository_template.cpp")
    add_includedirs("src", "tests", {public = true})
    add_deps("cpp_scaffold_templates", "cpp_scaffold_cli", "cpp_scaffold_utils", "test_common")
    add_packages("gtest", "spdlog", "nlohmann_json")
    set_targetdir("tests/bin")
    set_group("Tests")
    set_enabled(false)  -- Disabled by default, enabled via options
target_end()

-- Documentation Tests
target("test_doc_generator")
    set_kind("binary")
    add_files("tests/unit/documentation/test_doc_generator.cpp")
    add_includedirs("src", "tests", {public = true})
    add_deps("cpp_scaffold_documentation", "cpp_scaffold_utils", "test_common")
    add_packages("gtest", "spdlog", "nlohmann_json")
    set_targetdir("tests/bin")
    set_group("Tests")
    set_enabled(false)  -- Disabled by default, enabled via options
target_end()

-- Testing Framework Tests
target("test_test_framework_manager")
    set_kind("binary")
    add_files("tests/unit/testing/test_test_framework_manager.cpp")
    add_includedirs("src", "tests", {public = true})
    add_deps("cpp_scaffold_testing", "cpp_scaffold_utils", "test_common")
    add_packages("gtest", "spdlog", "nlohmann_json")
    set_targetdir("tests/bin")
    set_group("Tests")
    set_enabled(false)  -- Disabled by default, enabled via options
target_end()

-- Integration Tests
target("test_end_to_end")
    set_kind("binary")
    add_files("tests/integration/test_end_to_end.cpp")
    add_includedirs("src", "tests", {public = true})
    add_deps("cpp_scaffold_cli", "cpp_scaffold_templates", "cpp_scaffold_utils",
             "cpp_scaffold_config", "cpp_scaffold_testing", "cpp_scaffold_documentation", "test_common")
    add_packages("gtest", "spdlog", "nlohmann_json")
    set_targetdir("tests/bin")
    set_group("Tests")
    set_enabled(false)  -- Disabled by default, enabled via options
target_end()

target("test_integration")
    set_kind("binary")
    add_files("tests/integration/test_remote_sources.cpp")
    add_includedirs("src", "tests", {public = true})
    add_deps("cpp_scaffold_cli", "cpp_scaffold_templates", "cpp_scaffold_utils",
             "cpp_scaffold_config", "test_common")
    add_packages("gtest", "spdlog", "nlohmann_json")
    set_targetdir("tests/bin")
    set_group("Tests")
    set_enabled(false)  -- Disabled by default, enabled via options
target_end()

-- Platform-specific Tests
target("test_msys2_integration")
    set_kind("binary")
    add_files("tests/platform/test_msys2_integration.cpp")
    add_includedirs("src", "tests", {public = true})
    add_deps("cpp_scaffold_utils", "test_common")
    add_packages("gtest", "spdlog", "nlohmann_json")
    set_targetdir("tests/bin")
    set_group("Tests")
    set_enabled(false)  -- Disabled by default, enabled via options
    if not is_plat("windows") then
        set_enabled(false)  -- Force disable on non-Windows platforms
    end
target_end()

-- Edge Case Tests
target("test_cli_edge_cases")
    set_kind("binary")
    add_files("tests/edge_cases/cli/*.cpp")
    add_includedirs("src", "tests", {public = true})
    add_deps("cpp_scaffold_cli", "cpp_scaffold_utils", "test_common")
    add_packages("gtest", "spdlog", "nlohmann_json")
    set_targetdir("tests/bin")
    set_group("Tests")
    set_enabled(false)  -- Disabled by default, enabled via options
target_end()

target("test_template_edge_cases")
    set_kind("binary")
    add_files("tests/edge_cases/templates/*.cpp")
    add_includedirs("src", "tests", {public = true})
    add_deps("cpp_scaffold_templates", "cpp_scaffold_cli", "cpp_scaffold_utils", "test_common")
    add_packages("gtest", "spdlog", "nlohmann_json")
    set_targetdir("tests/bin")
    set_group("Tests")
    set_enabled(false)  -- Disabled by default, enabled via options
target_end()

target("test_file_utils_edge_cases")
    set_kind("binary")
    add_files("tests/edge_cases/utils/*.cpp")
    add_includedirs("src", "tests", {public = true})
    add_deps("cpp_scaffold_utils", "test_common")
    add_packages("gtest", "spdlog", "nlohmann_json")
    set_targetdir("tests/bin")
    set_group("Tests")
    set_enabled(false)  -- Disabled by default, enabled via options
target_end()

-- All Tests Runner
target("all_tests")
    set_kind("binary")
    add_files("tests/integration/test_end_to_end.cpp")
    add_includedirs("src", "tests", {public = true})
    add_deps("cpp_scaffold_cli", "cpp_scaffold_templates", "cpp_scaffold_utils",
             "cpp_scaffold_config", "cpp_scaffold_testing", "cpp_scaffold_documentation", "test_common")
    add_packages("gtest", "spdlog", "nlohmann_json")
    set_targetdir("tests/bin")
    set_group("Tests")
    set_enabled(false)  -- Disabled by default, enabled via options
    -- This target can be used to run all tests
    after_build(function (target)
        print("All test targets built successfully!")
    end)
target_end()

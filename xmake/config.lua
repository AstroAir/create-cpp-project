-- Configuration Constants for cpp-scaffold
-- Centralized configuration management for the build system

local config = {}

-- Project Information
config.project = {
    name = "cpp-scaffold",
    version = "1.0.0",
    language = "c++17",
    min_xmake_version = "2.8.0"
}

-- Build Configuration
config.build = {
    warnings = {"all", "extra"},
    modes = {"mode.debug", "mode.release"}
}

-- Debug Mode Settings
config.debug = {
    symbols = "debug",
    optimize = "none",
    defines = {"DEBUG"}
}

-- Release Mode Settings
config.release = {
    symbols = "hidden",
    optimize = "fastest",
    defines = {"NDEBUG"}
}

-- Platform-specific Settings
config.platforms = {
    windows = {
        cxflags = {"/utf-8"},
        runtime_debug = "MTd",
        runtime_release = "MT"
    },
    unix = {
        cxflags = {"-fPIC"},
        ldflags = {"-pthread"}
    }
}

-- Directory Structure
config.directories = {
    src = "src",
    tests = "tests",
    examples = "examples",
    bin = "bin",
    test_bin = "tests/bin",
    example_bin = "examples/bin"
}

-- Package Dependencies
config.packages = {
    core = {
        {"spdlog", "1.12.0", {system = true, configs = {fmt_external = false}}},
        {"nlohmann_json", "3.11.3", {system = true}}
    },
    test = {
        {"gtest", "1.14.0", {configs = {main = true}, optional = false}}
    },
    optional = {
        {"boost", "1.82.0", {optional = true, configs = {all = true}}},
        {"qt5widgets", {optional = true}},
        {"sfml", "2.5.1", {optional = true}}
    }
}

-- Component Library Configuration
config.components = {
    {
        name = "cpp_scaffold_utils",
        description = "Utils Component Library (base dependency for other components)",
        files = {"src/utils/*.cpp"},
        headers = {"src/utils/*.h"},
        deps = {},
        packages = {"spdlog", "nlohmann_json"}
    },
    {
        name = "cpp_scaffold_config", 
        description = "Config Component Library (depends only on utils to avoid circular dependency)",
        files = {"src/config/*.cpp"},
        headers = {"src/config/*.h"},
        deps = {"cpp_scaffold_utils"},
        packages = {"spdlog", "nlohmann_json"}
    },
    {
        name = "cpp_scaffold_cli",
        description = "CLI Component Library (depends on utils and config)",
        files = {"src/cli/*.cpp", "src/cli/**/*.cpp"},
        headers = {"src/cli/*.h", "src/cli/**/*.h"},
        deps = {"cpp_scaffold_utils", "cpp_scaffold_config"},
        packages = {"spdlog", "nlohmann_json"}
    },
    {
        name = "cpp_scaffold_templates",
        description = "Templates Component Library",
        files = {"src/templates/*.cpp"},
        headers = {"src/templates/*.h"},
        deps = {"cpp_scaffold_cli", "cpp_scaffold_utils"},
        packages = {"spdlog", "nlohmann_json"}
    },
    {
        name = "cpp_scaffold_testing",
        description = "Testing Component Library",
        files = {"src/testing/*.cpp"},
        headers = {"src/testing/*.h"},
        deps = {"cpp_scaffold_utils"},
        packages = {"spdlog", "nlohmann_json"}
    },
    {
        name = "cpp_scaffold_documentation",
        description = "Documentation Component Library",
        files = {"src/documentation/*.cpp"},
        headers = {"src/documentation/*.h"},
        deps = {"cpp_scaffold_utils"},
        packages = {"spdlog", "nlohmann_json"}
    }
}

-- Main Executable Configuration
config.executable = {
    name = "cpp-scaffold",
    files = {"src/main.cpp"},
    deps = {
        "cpp_scaffold_cli", "cpp_scaffold_templates", "cpp_scaffold_utils",
        "cpp_scaffold_config", "cpp_scaffold_testing", "cpp_scaffold_documentation"
    },
    packages = {"spdlog", "nlohmann_json"},
    target_dir = "bin",
    basename = "cpp-scaffold"
}

-- Test Configuration
config.tests = {
    common = {
        name = "test_common",
        files = {"tests/common/*.cpp"},
        headers = {"tests/common/*.h"},
        packages = {"gtest", "spdlog", "nlohmann_json"}
    },
    unit_tests = {
        -- Utils tests
        {
            name = "test_file_utils",
            files = {"tests/unit/utils/test_file_utils.cpp"},
            deps = {"cpp_scaffold_utils", "test_common"}
        },
        {
            name = "test_string_utils", 
            files = {"tests/unit/utils/test_string_utils.cpp"},
            deps = {"cpp_scaffold_utils", "test_common"}
        },
        {
            name = "test_terminal_utils",
            files = {"tests/unit/utils/test_terminal_utils.cpp"},
            deps = {"cpp_scaffold_utils", "test_common"}
        },
        {
            name = "test_http_client",
            files = {"tests/unit/utils/test_http_client.cpp"},
            deps = {"cpp_scaffold_utils", "test_common"}
        },
        -- CLI tests
        {
            name = "test_cli_parser",
            files = {"tests/unit/cli/test_cli_parser.cpp"},
            deps = {"cpp_scaffold_cli", "cpp_scaffold_utils", "test_common"}
        },
        -- Config tests
        {
            name = "test_config_manager",
            files = {"tests/unit/config/test_config_manager.cpp"},
            deps = {"cpp_scaffold_config", "cpp_scaffold_utils", "test_common"}
        },
        {
            name = "test_config_validator",
            files = {"tests/unit/config/test_config_validator.cpp"},
            deps = {"cpp_scaffold_config", "cpp_scaffold_utils", "test_common"}
        },
        {
            name = "test_ci_config",
            files = {"tests/unit/config/test_ci_config.cpp"},
            deps = {"cpp_scaffold_config", "cpp_scaffold_utils", "test_common"}
        },
        -- Template tests
        {
            name = "test_template_manager",
            files = {"tests/unit/templates/test_template_manager.cpp"},
            deps = {"cpp_scaffold_templates", "cpp_scaffold_cli", "cpp_scaffold_utils", "test_common"}
        },
        {
            name = "test_archive_template",
            files = {"tests/unit/templates/test_archive_template.cpp"},
            deps = {"cpp_scaffold_templates", "cpp_scaffold_cli", "cpp_scaffold_utils", "test_common"}
        },
        {
            name = "test_git_repository_template",
            files = {"tests/unit/templates/test_git_repository_template.cpp"},
            deps = {"cpp_scaffold_templates", "cpp_scaffold_cli", "cpp_scaffold_utils", "test_common"}
        },
        -- Documentation tests
        {
            name = "test_doc_generator",
            files = {"tests/unit/documentation/test_doc_generator.cpp"},
            deps = {"cpp_scaffold_documentation", "cpp_scaffold_utils", "test_common"}
        },
        -- Testing framework tests
        {
            name = "test_test_framework_manager",
            files = {"tests/unit/testing/test_test_framework_manager.cpp"},
            deps = {"cpp_scaffold_testing", "cpp_scaffold_utils", "test_common"}
        }
    },
    integration_tests = {
        {
            name = "test_end_to_end",
            files = {"tests/integration/test_end_to_end.cpp"},
            deps = {
                "cpp_scaffold_cli", "cpp_scaffold_templates", "cpp_scaffold_utils",
                "cpp_scaffold_config", "cpp_scaffold_testing", "cpp_scaffold_documentation", "test_common"
            }
        },
        {
            name = "test_integration",
            files = {"tests/integration/test_remote_sources.cpp"},
            deps = {
                "cpp_scaffold_cli", "cpp_scaffold_templates", "cpp_scaffold_utils",
                "cpp_scaffold_config", "test_common"
            }
        }
    },
    platform_tests = {
        {
            name = "test_msys2_integration",
            files = {"tests/platform/test_msys2_integration.cpp"},
            deps = {"cpp_scaffold_utils", "test_common"},
            windows_only = true
        }
    },
    edge_case_tests = {
        {
            name = "test_cli_edge_cases",
            files = {"tests/edge_cases/cli/*.cpp"},
            deps = {"cpp_scaffold_cli", "cpp_scaffold_utils", "test_common"}
        },
        {
            name = "test_template_edge_cases",
            files = {"tests/edge_cases/templates/*.cpp"},
            deps = {"cpp_scaffold_templates", "cpp_scaffold_cli", "cpp_scaffold_utils", "test_common"}
        },
        {
            name = "test_file_utils_edge_cases",
            files = {"tests/edge_cases/utils/*.cpp"},
            deps = {"cpp_scaffold_utils", "test_common"}
        }
    }
}

return config

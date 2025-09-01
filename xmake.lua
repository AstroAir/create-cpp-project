-- Modern Modular Xmake Build System for cpp-scaffold
-- A cross-platform C++ project scaffolding tool
-- Modularized for better maintainability and organization

-- Set minimum xmake version
set_xmakever("2.8.0")

-- Project configuration
set_project("cpp-scaffold")
set_version("1.0.0")
set_languages("c++17")

-- Add build mode rules
add_rules("mode.debug", "mode.release")

-- Global compiler settings
set_warnings("all", "extra")

-- Configure optimization based on build mode
if is_mode("debug") then
    set_symbols("debug")
    set_optimize("none")
    add_defines("DEBUG")
elseif is_mode("release") then
    set_symbols("hidden")
    set_optimize("fastest")
    add_defines("NDEBUG")
end

-- MSYS2 Environment Detection and Configuration
local msystem = os.getenv("MSYSTEM")
local mingw_prefix = os.getenv("MINGW_PREFIX")
local msys2_detected = msystem ~= nil or mingw_prefix ~= nil

if msys2_detected then
    print("MSYS2 environment detected: " .. (msystem or "unknown"))

    -- Add MSYS2-specific definitions
    add_defines("MSYS2_BUILD=1", "MINGW_BUILD=1")
    if msystem then
        add_defines("MSYSTEM=\"" .. msystem .. "\"")
    end

    -- MSYS2-specific include and library paths
    if mingw_prefix then
        add_includedirs(path.join(mingw_prefix, "include"), {system = true})
        add_linkdirs(path.join(mingw_prefix, "lib"))
    end
end

-- Platform-specific configurations
if is_plat("windows") then
    if msys2_detected then
        -- MSYS2-specific configuration
        print("Applying MSYS2-specific build configurations")

        -- Windows-specific libraries for MSYS2
        add_syslinks("ws2_32", "shlwapi", "advapi32", "shell32", "user32", "kernel32")

        -- MSYS2-specific optimization flags for release builds
        if is_mode("release") then
            if msystem == "MINGW64" or msystem == "MINGW32" then
                add_cxflags("-march=native", "-mtune=native")
                add_cxflags("-ffast-math", "-funroll-loops")
                -- Link-time optimization
                add_cxflags("-flto")
                add_ldflags("-flto")
            elseif msystem == "CLANG64" or msystem == "CLANGARM64" then
                add_cxflags("-march=native", "-mtune=native")
                add_cxflags("-ffast-math")
            end

            -- Linker optimizations
            add_ldflags("-Wl,--gc-sections")  -- Remove unused sections
            add_ldflags("-Wl,--strip-all")    -- Strip symbols
            add_ldflags("-Wl,--enable-auto-import")
            add_ldflags("-Wl,--enable-runtime-pseudo-reloc")
        end

        -- UCRT runtime support
        if msystem == "UCRT64" then
            add_cxflags("-D_UCRT")
        end
    else
        -- Standard Windows (MSVC) configuration
        add_cxflags("/utf-8")
        if is_mode("release") then
            set_runtimes("MT")
        else
            set_runtimes("MTd")
        end
    end
elseif is_plat("linux", "macosx") then
    add_cxflags("-fPIC")
    add_ldflags("-pthread")
end

-- Global include directories
add_includedirs("src", {public = true})

-- Add module directories for custom modules
add_moduledirs("xmake")

-- Include modular components
includes("xmake/msys2.lua")
includes("xmake/packages.lua")
includes("xmake/components.lua")
includes("xmake/executable.lua")
includes("xmake/tests.lua")
includes("xmake/examples.lua")
includes("xmake/options.lua")
includes("xmake/tasks.lua")

-- Apply build options to targets (must be after includes)
if has_config("tests") then
    -- Enable all test targets
    for _, name in ipairs({
        "test_file_utils", "test_string_utils", "test_terminal_utils", "test_http_client",
        "test_cli_parser", "test_config_manager", "test_config_validator", "test_ci_config",
        "test_template_manager", "test_archive_template", "test_git_repository_template",
        "test_doc_generator", "test_test_framework_manager", "test_end_to_end",
        "test_integration", "test_msys2_integration", "test_cli_edge_cases",
        "test_template_edge_cases", "test_file_utils_edge_cases", "all_tests"
    }) do
        target(name)
            set_enabled(true)
        target_end()
    end
else
    -- Disable test targets by default
    for _, name in ipairs({
        "test_file_utils", "test_string_utils", "test_terminal_utils", "test_http_client",
        "test_cli_parser", "test_config_manager", "test_config_validator", "test_ci_config",
        "test_template_manager", "test_archive_template", "test_git_repository_template",
        "test_doc_generator", "test_test_framework_manager", "test_end_to_end",
        "test_integration", "test_msys2_integration", "test_cli_edge_cases",
        "test_template_edge_cases", "test_file_utils_edge_cases", "all_tests"
    }) do
        target(name)
            set_enabled(false)
        target_end()
    end
end

if has_config("examples") then
    -- Enable example targets
    for _, name in ipairs({
        "file-processor", "qt-image-viewer", "boost-network-server",
        "sfml-game-demo", "remote-source-demo"
    }) do
        target(name)
            set_enabled(true)
        target_end()
    end
else
    -- Disable example targets by default
    for _, name in ipairs({
        "file-processor", "qt-image-viewer", "boost-network-server",
        "sfml-game-demo", "remote-source-demo"
    }) do
        target(name)
            set_enabled(false)
        target_end()
    end
end

-- Code coverage support
if has_config("coverage") then
    add_cxflags("--coverage")
    add_ldflags("--coverage")
end

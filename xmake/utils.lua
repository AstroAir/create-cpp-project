-- Utility Functions for cpp-scaffold Build System
-- Reusable functions to reduce code duplication and improve maintainability

local utils = {}

-- Import configuration
local config = import("config")

-- Apply build mode configuration (debug/release)
function utils.apply_build_modes()
    -- Configure optimization based on build mode
    if is_mode("debug") then
        set_symbols(config.debug.symbols)
        set_optimize(config.debug.optimize)
        for _, define in ipairs(config.debug.defines) do
            add_defines(define)
        end
    elseif is_mode("release") then
        set_symbols(config.release.symbols)
        set_optimize(config.release.optimize)
        for _, define in ipairs(config.release.defines) do
            add_defines(define)
        end
    end
end

-- Apply platform-specific configurations
function utils.apply_platform_config()
    if is_plat("windows") then
        for _, flag in ipairs(config.platforms.windows.cxflags) do
            add_cxflags(flag)
        end
        if is_mode("release") then
            set_runtimes(config.platforms.windows.runtime_release)
        else
            set_runtimes(config.platforms.windows.runtime_debug)
        end
    elseif is_plat("linux", "macosx") then
        for _, flag in ipairs(config.platforms.unix.cxflags) do
            add_cxflags(flag)
        end
        for _, flag in ipairs(config.platforms.unix.ldflags) do
            add_ldflags(flag)
        end
    end
end

-- Create a component library target
function utils.create_component(component_config)
    target(component_config.name)
        set_kind("static")
        
        -- Add source files
        for _, file_pattern in ipairs(component_config.files) do
            add_files(file_pattern)
        end
        
        -- Add header files
        for _, header_pattern in ipairs(component_config.headers) do
            add_headerfiles(header_pattern)
        end
        
        -- Add include directories
        add_includedirs(config.directories.src, {public = true})
        
        -- Add dependencies
        if component_config.deps and #component_config.deps > 0 then
            add_deps(table.unpack(component_config.deps))
        end
        
        -- Add packages
        if component_config.packages and #component_config.packages > 0 then
            add_packages(table.unpack(component_config.packages))
        end
        
        set_group("Components")
    target_end()
end

-- Create a test target
function utils.create_test_target(test_config)
    target(test_config.name)
        set_kind("binary")
        
        -- Add source files
        for _, file_pattern in ipairs(test_config.files) do
            add_files(file_pattern)
        end
        
        -- Add include directories
        add_includedirs(config.directories.src, config.directories.tests, {public = true})
        
        -- Add dependencies
        if test_config.deps and #test_config.deps > 0 then
            add_deps(table.unpack(test_config.deps))
        end
        
        -- Add packages (always include gtest for tests)
        local packages = {"gtest", "spdlog", "nlohmann_json"}
        if test_config.packages then
            for _, pkg in ipairs(test_config.packages) do
                table.insert(packages, pkg)
            end
        end
        add_packages(table.unpack(packages))
        
        -- Set target directory
        set_targetdir(config.directories.test_bin)
        set_group("Tests")
        
        -- Platform-specific settings
        if test_config.windows_only and not is_plat("windows") then
            set_enabled(false)
        end
    target_end()
end

-- Create an example target
function utils.create_example_target(example_config)
    target(example_config.name)
        set_kind(example_config.kind or "binary")
        
        -- Add source files
        for _, file_pattern in ipairs(example_config.files) do
            add_files(file_pattern)
        end
        
        -- Add header files if specified
        if example_config.headers then
            for _, header_pattern in ipairs(example_config.headers) do
                add_headerfiles(header_pattern)
            end
        end
        
        -- Add include directories
        if example_config.include_dirs then
            for _, include_dir in ipairs(example_config.include_dirs) do
                add_includedirs(include_dir, {public = true})
            end
        end
        
        -- Add dependencies
        if example_config.deps and #example_config.deps > 0 then
            add_deps(table.unpack(example_config.deps))
        end
        
        -- Add packages
        if example_config.packages and #example_config.packages > 0 then
            add_packages(table.unpack(example_config.packages))
        end
        
        -- Set target directory
        set_targetdir(example_config.target_dir or config.directories.example_bin)
        set_group("Examples")
        
        -- Handle optional packages
        if example_config.optional_package then
            if has_package(example_config.optional_package) then
                set_enabled(true)
            else
                set_enabled(false)
            end
        end
    target_end()
end

-- Apply build options to targets
function utils.apply_build_options()
    -- Apply tests option
    if has_config("tests") then
        -- Enable all test targets
        local test_names = utils.get_all_test_names()
        for _, name in ipairs(test_names) do
            target(name)
                set_enabled(true)
            target_end()
        end
    else
        -- Disable test targets by default
        local test_names = utils.get_all_test_names()
        for _, name in ipairs(test_names) do
            target(name)
                set_enabled(false)
            target_end()
        end
    end
    
    -- Apply examples option
    if has_config("examples") then
        -- Enable example targets
        local example_names = utils.get_all_example_names()
        for _, name in ipairs(example_names) do
            target(name)
                set_enabled(true)
            target_end()
        end
    else
        -- Disable example targets by default
        local example_names = utils.get_all_example_names()
        for _, name in ipairs(example_names) do
            target(name)
                set_enabled(false)
            target_end()
        end
    end
    
    -- Apply coverage option
    if has_config("coverage") then
        add_cxflags("--coverage")
        add_ldflags("--coverage")
    end
end

-- Get all test target names
function utils.get_all_test_names()
    local names = {}
    
    -- Add common test target
    table.insert(names, config.tests.common.name)
    
    -- Add unit tests
    for _, test in ipairs(config.tests.unit_tests) do
        table.insert(names, test.name)
    end
    
    -- Add integration tests
    for _, test in ipairs(config.tests.integration_tests) do
        table.insert(names, test.name)
    end
    
    -- Add platform tests
    for _, test in ipairs(config.tests.platform_tests) do
        table.insert(names, test.name)
    end
    
    -- Add edge case tests
    for _, test in ipairs(config.tests.edge_case_tests) do
        table.insert(names, test.name)
    end
    
    -- Add special test targets
    table.insert(names, "all_tests")
    
    return names
end

-- Get all example target names
function utils.get_all_example_names()
    return {
        "file-processor", "qt-image-viewer", "boost-network-server",
        "sfml-game-demo", "remote-source-demo"
    }
end

-- Helper function to check if a table contains a value
function utils.table_contains(table, value)
    for _, v in ipairs(table) do
        if v == value then
            return true
        end
    end
    return false
end

-- Helper function to merge tables
function utils.merge_tables(...)
    local result = {}
    for _, t in ipairs({...}) do
        for _, v in ipairs(t) do
            table.insert(result, v)
        end
    end
    return result
end

return utils

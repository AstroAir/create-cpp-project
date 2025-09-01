-- Component Library Definitions for cpp-scaffold
-- Defines all component library targets with MSYS2 support

-- Detect MSYS2 environment
local msystem = os.getenv("MSYSTEM")
local mingw_prefix = os.getenv("MINGW_PREFIX")
local msys2_detected = msystem ~= nil or mingw_prefix ~= nil

-- Utils Component Library (base dependency for other components)
target("cpp_scaffold_utils")
    set_kind("static")
    add_files("src/utils/*.cpp")
    add_headerfiles("src/utils/*.h")
    add_includedirs("src", {public = true})
    add_packages("spdlog", "nlohmann_json")
    set_group("Components")

    -- MSYS2-specific configurations
    if msys2_detected then
        -- Add MSYS2-specific include and library paths
        if mingw_prefix then
            add_includedirs(path.join(mingw_prefix, "include"), {system = true})
            add_linkdirs(path.join(mingw_prefix, "lib"))
        end

        -- Add MSYS2-specific system libraries
        add_syslinks("ws2_32", "shlwapi", "advapi32")

        -- MSYS2 static linking option
        if has_config("msys2_static") then
            add_ldflags("-static-libgcc", "-static-libstdc++")
        end
    end
target_end()

-- Config Component Library (depends only on utils to avoid circular dependency)
target("cpp_scaffold_config")
    set_kind("static")
    add_files("src/config/*.cpp")
    add_headerfiles("src/config/*.h")
    add_includedirs("src", {public = true})
    add_deps("cpp_scaffold_utils")
    add_packages("spdlog", "nlohmann_json")
    set_group("Components")

    -- MSYS2-specific configurations
    if msys2_detected then
        if mingw_prefix then
            add_includedirs(path.join(mingw_prefix, "include"), {system = true})
            add_linkdirs(path.join(mingw_prefix, "lib"))
        end
        if has_config("msys2_static") then
            add_ldflags("-static-libgcc", "-static-libstdc++")
        end
    end
target_end()

-- CLI Component Library (depends on utils and config)
target("cpp_scaffold_cli")
    set_kind("static")
    add_files("src/cli/*.cpp", "src/cli/**/*.cpp")
    add_headerfiles("src/cli/*.h", "src/cli/**/*.h")
    add_includedirs("src", {public = true})
    add_deps("cpp_scaffold_utils", "cpp_scaffold_config")
    add_packages("spdlog", "nlohmann_json")
    set_group("Components")

    -- MSYS2-specific configurations
    if msys2_detected then
        if mingw_prefix then
            add_includedirs(path.join(mingw_prefix, "include"), {system = true})
            add_linkdirs(path.join(mingw_prefix, "lib"))
        end
        if has_config("msys2_static") then
            add_ldflags("-static-libgcc", "-static-libstdc++")
        end
    end
target_end()

-- Templates Component Library
target("cpp_scaffold_templates")
    set_kind("static")
    add_files("src/templates/*.cpp")
    add_headerfiles("src/templates/*.h")
    add_includedirs("src", {public = true})
    add_deps("cpp_scaffold_cli", "cpp_scaffold_utils")
    add_packages("spdlog", "nlohmann_json")
    set_group("Components")

    -- MSYS2-specific configurations
    if msys2_detected then
        if mingw_prefix then
            add_includedirs(path.join(mingw_prefix, "include"), {system = true})
            add_linkdirs(path.join(mingw_prefix, "lib"))
        end
        if has_config("msys2_static") then
            add_ldflags("-static-libgcc", "-static-libstdc++")
        end
    end
target_end()

-- Testing Component Library
target("cpp_scaffold_testing")
    set_kind("static")
    add_files("src/testing/*.cpp")
    add_headerfiles("src/testing/*.h")
    add_includedirs("src", {public = true})
    add_deps("cpp_scaffold_utils")
    add_packages("spdlog", "nlohmann_json")
    set_group("Components")

    -- MSYS2-specific configurations
    if msys2_detected then
        if mingw_prefix then
            add_includedirs(path.join(mingw_prefix, "include"), {system = true})
            add_linkdirs(path.join(mingw_prefix, "lib"))
        end
        if has_config("msys2_static") then
            add_ldflags("-static-libgcc", "-static-libstdc++")
        end
    end
target_end()

-- Documentation Component Library
target("cpp_scaffold_documentation")
    set_kind("static")
    add_files("src/documentation/*.cpp")
    add_headerfiles("src/documentation/*.h")
    add_includedirs("src", {public = true})
    add_deps("cpp_scaffold_utils")
    add_packages("spdlog", "nlohmann_json")
    set_group("Components")

    -- MSYS2-specific configurations
    if msys2_detected then
        if mingw_prefix then
            add_includedirs(path.join(mingw_prefix, "include"), {system = true})
            add_linkdirs(path.join(mingw_prefix, "lib"))
        end
        if has_config("msys2_static") then
            add_ldflags("-static-libgcc", "-static-libstdc++")
        end
    end
target_end()

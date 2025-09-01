-- Main Executable Definition for cpp-scaffold
-- Defines the main application target with MSYS2 support

-- Detect MSYS2 environment
local msystem = os.getenv("MSYSTEM")
local mingw_prefix = os.getenv("MINGW_PREFIX")
local msys2_detected = msystem ~= nil or mingw_prefix ~= nil

-- Main executable target
target("cpp-scaffold")
    set_kind("binary")
    add_files("src/main.cpp")
    add_includedirs("src", {public = true})
    add_deps("cpp_scaffold_cli", "cpp_scaffold_templates", "cpp_scaffold_utils",
             "cpp_scaffold_config", "cpp_scaffold_testing", "cpp_scaffold_documentation")
    add_packages("spdlog", "nlohmann_json")
    set_targetdir("bin")
    set_basename("cpp-scaffold")
    set_group("Executables")

    -- MSYS2-specific configurations
    if msys2_detected then
        -- Add MSYS2-specific include and library paths
        if mingw_prefix then
            add_includedirs(path.join(mingw_prefix, "include"), {system = true})
            add_linkdirs(path.join(mingw_prefix, "lib"))
        end

        -- Add Windows-specific system libraries
        add_syslinks("ws2_32", "shlwapi", "advapi32", "shell32", "user32", "kernel32")

        -- MSYS2 static linking option
        if has_config("msys2_static") then
            add_ldflags("-static-libgcc", "-static-libstdc++", "-static")
        end

        -- MSYS2 packaging configuration
        if has_config("msys2_package") then
            -- Set installation paths for MSYS2 packaging
            set_installdir("$(MINGW_PREFIX)")
            add_installfiles("bin/cpp-scaffold.exe", {prefixdir = "bin"})
            add_installfiles("README.md", {prefixdir = "share/doc/cpp-scaffold"})
            add_installfiles("LICENSE", {prefixdir = "share/licenses/cpp-scaffold"})
        end

        -- Console application (show console window)
        if is_mode("debug") then
            add_ldflags("-mconsole")
        end
    end
target_end()

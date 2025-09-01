-- Package Management for cpp-scaffold
-- Centralized package dependency management with MSYS2 support

-- Detect MSYS2 environment
local msystem = os.getenv("MSYSTEM")
local mingw_prefix = os.getenv("MINGW_PREFIX")
local msys2_detected = msystem ~= nil or mingw_prefix ~= nil

-- Configure packages based on environment
if msys2_detected then
    print("MSYS2 environment detected, using system packages")

    -- Core dependencies (prefer system packages in MSYS2)
    add_requires("spdlog", {system = true, configs = {fmt_external = false}})
    add_requires("nlohmann_json", {system = true})

    -- Test dependencies
    add_requires("gtest", {system = true, configs = {main = true}})

    -- Optional dependencies for examples
    add_requires("boost", {system = true, optional = true})
    add_requires("qt5widgets", {system = true, optional = true})
    add_requires("sfml", {system = true, optional = true})

    -- MSYS2-specific packages
    add_requires("pkgconf", {system = true, optional = true})  -- For pkg-config support

else
    -- Standard package requirements for non-MSYS2 environments
    -- Core dependencies
    add_requires("spdlog 1.12.0", {system = true, configs = {fmt_external = false}})
    add_requires("nlohmann_json 3.11.3", {system = true})

    -- Test dependencies (only for test targets)
    add_requires("gtest 1.14.0", {configs = {main = true}, optional = false})

    -- Example dependencies (optional, for examples only)
    add_requires("boost 1.82.0", {optional = true, configs = {all = true}})
    add_requires("qt5widgets", {optional = true})
    add_requires("sfml 2.5.1", {optional = true})
end

set_project("test-xmake-modules")
set_version("1.0.0")

-- Set C++20 standard for modules support
set_languages("c++20")

-- Add build modes
add_rules("mode.debug", "mode.release")

-- Module library
target("test-xmake-modules")
    set_kind("static")
    add_files("modules/test-xmake-modules.cppm", "src/test-xmake-modules.cpp")
    add_headerfiles("include/test-xmake-modules/*.h")
    add_includedirs("include", {public = true})

    -- Enable C++20 modules
    set_languages("c++20")
    add_cxxflags("/experimental:module", {tools = "cl"})
    add_cxxflags("-fmodules-ts", {tools = "gcc", "clang"})

    -- Set output directory
    set_targetdir("lib")

    -- Add compile flags
    if is_mode("debug") then
        add_defines("DEBUG")
        set_symbols("debug")
        set_optimize("none")
    elseif is_mode("release") then
        add_defines("NDEBUG")
        set_symbols("hidden")
        set_optimize("fastest")
    end

-- Main executable
target("test-xmake-modules_app")
    set_kind("binary")
    add_files("src/main.cpp")
    add_deps("test-xmake-modules")
    set_targetdir("bin")
    set_languages("c++20")
    add_cxxflags("/experimental:module", {tools = "cl"})
    add_cxxflags("-fmodules-ts", {tools = "gcc", "clang"})

add_requires("gtest")

target("test-xmake-modules_tests")
    set_kind("binary")
    add_files("tests/test_test-xmake-modules.cpp")
    add_packages("gtest")
    add_deps("test-xmake-modules")
    set_targetdir("tests/bin")
    set_languages("c++20")

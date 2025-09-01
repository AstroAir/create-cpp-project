workspace "test-premake-lib"
    configurations { "Debug", "Release" }
    platforms { "x64" }

project "test-premake-lib"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"
    targetdir "lib/%{cfg.buildcfg}"

    files {
        "src/**.cpp",
        "include/**.h"
    }

    includedirs {
        "include"
    }

    filter "configurations:Debug"
        defines { "DEBUG" }
        symbols "On"
        optimize "Off"

    filter "configurations:Release"
        defines { "NDEBUG" }
        symbols "Off"
        optimize "Speed"

project "test-premake-lib_example"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"
    targetdir "bin/%{cfg.buildcfg}"

    files {
        "examples/**.cpp"
    }

    includedirs {
        "include"
    }

    links {
        "test-premake-lib"
    }

project "test-premake-lib_tests"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"
    targetdir "bin/%{cfg.buildcfg}"

    files {
        "tests/**.cpp",
        "tests/**.h"
    }

    includedirs {
        "include",
        "tests"
    }

    links {
        "test-premake-lib"
    }

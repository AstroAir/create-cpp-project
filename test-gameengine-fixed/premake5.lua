workspace "test-gameengine-fixed"
    configurations { "Debug", "Release" }
    platforms { "x64" }

project "test-gameengine-fixed_engine"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"
    targetdir "lib/%{cfg.buildcfg}"

    files {
        "src/engine/**.cpp",
        "include/engine/**.h"
    }

    includedirs {
        "include",
        "vendor/glfw/include",
        "vendor/glm",
        "vendor/stb"
    }

    links {
        "opengl32",
        "glfw3",
        "gdi32",
        "user32",
        "kernel32"
    }

    filter "configurations:Debug"
        defines { "DEBUG", "ENGINE_DEBUG" }
        symbols "On"
        optimize "Off"

    filter "configurations:Release"
        defines { "NDEBUG", "ENGINE_RELEASE" }
        symbols "Off"
        optimize "Speed"

project "test-gameengine-fixed"
    kind "WindowedApp"
    language "C++"
    cppdialect "C++17"
    targetdir "bin/%{cfg.buildcfg}"

    files {
        "src/game/**.cpp",
        "src/main.cpp",
        "include/game/**.h"
    }

    includedirs {
        "include",
        "vendor/glfw/include",
        "vendor/glm",
        "vendor/stb"
    }

    links {
        "test-gameengine-fixed_engine",
        "opengl32",
        "glfw3",
        "gdi32",
        "user32",
        "kernel32"
    }

    filter "configurations:Debug"
        defines { "DEBUG" }
        symbols "On"
        optimize "Off"

    filter "configurations:Release"
        defines { "NDEBUG" }
        symbols "Off"
        optimize "Speed"

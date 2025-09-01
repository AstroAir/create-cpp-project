workspace "verify-premake-gameengine"
    configurations { "Debug", "Release" }
    platforms { "x64" }

project "verify-premake-gameengine_engine"
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

project "verify-premake-gameengine"
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
        "verify-premake-gameengine_engine",
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

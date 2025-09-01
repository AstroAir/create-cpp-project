-- Example Projects for cpp-scaffold
-- Defines all example project targets

-- Console File Processor Example
target("console_file_processor_core")
    set_kind("static")
    add_files("examples/console-file-processor/src/core/*.cpp")
    add_headerfiles("examples/console-file-processor/include/file-processor/*.h")
    add_includedirs("examples/console-file-processor/include", {public = true})
    add_packages("spdlog", "nlohmann_json")
    set_group("Examples")
target_end()

target("console_file_processor_utils")
    set_kind("static")
    add_files("examples/console-file-processor/src/utils/*.cpp")
    add_includedirs("examples/console-file-processor/include", {public = true})
    add_deps("console_file_processor_core")
    add_packages("spdlog", "nlohmann_json")
    set_group("Examples")
target_end()

target("console_file_processor_cli")
    set_kind("static")
    add_files("examples/console-file-processor/src/cli/*.cpp")
    add_includedirs("examples/console-file-processor/include", {public = true})
    add_deps("console_file_processor_core", "console_file_processor_utils")
    add_packages("spdlog", "nlohmann_json")
    set_group("Examples")
target_end()

target("file-processor")
    set_kind("binary")
    add_files("examples/console-file-processor/src/main.cpp")
    add_includedirs("examples/console-file-processor/include", {public = true})
    add_deps("console_file_processor_cli", "console_file_processor_core", "console_file_processor_utils")
    add_packages("spdlog", "nlohmann_json")
    set_targetdir("examples/bin")
    set_group("Examples")
target_end()

-- Qt Image Viewer Example (optional, requires Qt)
target("qt-image-viewer")
    set_kind("binary")
    add_files("examples/qt-image-viewer/src/*.cpp")
    add_includedirs("examples/qt-image-viewer/src", {public = true})
    add_packages("qt5widgets")
    set_targetdir("examples/bin")
    set_group("Examples")
    -- Only build if Qt is available
    if has_package("qt5widgets") then
        set_enabled(true)
    else
        set_enabled(false)
    end
target_end()

-- Boost Network Server Example (optional, requires Boost)
target("boost-network-server")
    set_kind("binary")
    add_files("examples/boost-network-server/src/*.cpp")
    add_includedirs("examples/boost-network-server/include", {public = true})
    add_packages("boost")
    set_targetdir("examples/bin")
    set_group("Examples")
    -- Only build if Boost is available
    if has_package("boost") then
        set_enabled(true)
    else
        set_enabled(false)
    end
target_end()

-- SFML Game Demo Example (optional, requires SFML)
target("sfml-game-demo")
    set_kind("binary")
    add_files("examples/sfml-game-demo/src/*.cpp")
    add_includedirs("examples/sfml-game-demo/include", {public = true})
    add_packages("sfml")
    set_targetdir("examples/bin")
    set_group("Examples")
    -- Only build if SFML is available
    if has_package("sfml") then
        set_enabled(true)
    else
        set_enabled(false)
    end
target_end()

-- Remote Source Demo (standalone example)
target("remote-source-demo")
    set_kind("binary")
    add_files("examples/remote_source_demo.cpp")
    add_packages("spdlog", "nlohmann_json")
    set_targetdir("examples/bin")
    set_group("Examples")
target_end()

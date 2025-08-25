#include "template_customizer.h"

namespace templates {
namespace predefined {

TemplateCustomization getConsoleTemplate() {
    TemplateCustomization config;
    config.templateName = "console";
    config.description = "Command-line application with main() function and argument parsing";

    // Directory structure
    config.structure.name = "console-standard";
    config.structure.description = "Standard console application structure";
    config.structure.directories = {"src", "include", "tests", "docs", "cmake"};
    config.structure.isDefault = true;

    // Template files
    std::vector<TemplateFile> files = {
        {
            "src/main.cpp",
            R"(#include <iostream>
#include <string>
{{#IF ENABLE_CLI11}}
#include <CLI/CLI.hpp>
{{/IF}}

int main(int argc, char* argv[]) {
{{#IF ENABLE_CLI11}}
    CLI::App app{"{{PROJECT_DESCRIPTION}}"};

    std::string name = "World";
    app.add_option("-n,--name", name, "Name to greet");

    CLI11_PARSE(app, argc, argv);

    std::cout << "Hello, " << name << "!" << std::endl;
{{/IF}}
{{#IF !ENABLE_CLI11}}
    (void)argc; // Suppress unused parameter warning
    (void)argv; // Suppress unused parameter warning

    std::cout << "Hello from {{PROJECT_NAME}}!" << std::endl;
{{/IF}}

    return 0;
})",
            true, false, "Main application entry point"
        },
        {
            "include/{{PROJECT_NAME_CPP}}/version.h",
            R"(#pragma once

#define {{PROJECT_NAME_CPP_UPPER}}_VERSION_MAJOR 1
#define {{PROJECT_NAME_CPP_UPPER}}_VERSION_MINOR 0
#define {{PROJECT_NAME_CPP_UPPER}}_VERSION_PATCH 0

#define {{PROJECT_NAME_CPP_UPPER}}_VERSION "1.0.0"

namespace {{PROJECT_NAME_CPP}} {
    const char* getVersion();
})",
            true, false, "Version information header"
        },
        {
            "src/version.cpp",
            R"(#include "{{PROJECT_NAME_CPP}}/version.h"

namespace {{PROJECT_NAME_CPP}} {
    const char* getVersion() {
        return {{PROJECT_NAME_CPP_UPPER}}_VERSION;
    }
})",
            true, false, "Version implementation"
        },
        {
            "CMakeLists.txt",
            R"(cmake_minimum_required(VERSION 3.16)
project({{PROJECT_NAME}} VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD {{CPP_STANDARD_NUM}})
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Add executable
add_executable({{PROJECT_NAME}} src/main.cpp src/version.cpp)

# Include directories
target_include_directories({{PROJECT_NAME}} PRIVATE include)

{{#IF ENABLE_CLI11}}
# Find and link CLI11
find_package(CLI11 CONFIG REQUIRED)
target_link_libraries({{PROJECT_NAME}} PRIVATE CLI11::CLI11)
target_compile_definitions({{PROJECT_NAME}} PRIVATE ENABLE_CLI11)
{{/IF}}

{{#IF ENABLE_TESTING}}
# Enable testing
enable_testing()
add_subdirectory(tests)
{{/IF}}

# Install target
install(TARGETS {{PROJECT_NAME}} DESTINATION bin))",
            true, false, "CMake build configuration", {"cmake"}
        },
        {
            "README.md",
            R"(# {{PROJECT_NAME}}

{{PROJECT_DESCRIPTION}}

## Building

```bash
mkdir build && cd build
cmake ..
make
```

## Usage

```bash
./{{PROJECT_NAME}} --help
```

## License

{{#IF ENABLE_LICENSE}}
This project is licensed under the {{LICENSE_TYPE}} License.
{{/IF}}
{{#IF !ENABLE_LICENSE}}
All rights reserved.
{{/IF}})",
            true, false, "Project documentation"
        }
    };

    config.structure.files = files;

    // Available features
    config.availableFeatures = {
        "cli11", "logging", "config-file", "testing", "documentation",
        "code-style", "ci-github", "ci-gitlab", "docker"
    };

    // Default features
    config.defaultFeatures = {"cli11", "testing", "documentation"};

    // Supported build systems
    config.supportedBuildSystems = {"cmake", "meson", "make"};

    // Recommended packages
    config.recommendedPackages = {"cli11", "fmt", "spdlog"};

    return config;
}

TemplateCustomization getLibraryTemplate() {
    TemplateCustomization config;
    config.templateName = "lib";
    config.description = "Static or shared library with public API and examples";

    // Directory structure
    config.structure.name = "library-standard";
    config.structure.description = "Standard library project structure";
    config.structure.directories = {"include", "src", "tests", "examples", "docs", "cmake"};
    config.structure.isDefault = true;

    // Template files
    std::vector<TemplateFile> files = {
        {
            "include/{{PROJECT_NAME_CPP}}/{{PROJECT_NAME_CPP}}.h",
            R"(#pragma once

#include <string>

namespace {{PROJECT_NAME_CPP}} {

    /**
     * @brief Main library class
     */
    class {{PROJECT_NAME_CPP_UPPER}} {
    public:
        /**
         * @brief Constructor
         */
        {{PROJECT_NAME_CPP_UPPER}}();

        /**
         * @brief Destructor
         */
        ~{{PROJECT_NAME_CPP_UPPER}}();

        /**
         * @brief Get library version
         * @return Version string
         */
        std::string getVersion() const;

        /**
         * @brief Example method
         * @param input Input string
         * @return Processed string
         */
        std::string process(const std::string& input) const;

    private:
        class Impl;
        std::unique_ptr<Impl> m_impl;
    };

} // namespace {{PROJECT_NAME_CPP}})",
            true, false, "Main library header"
        },
        {
            "src/{{PROJECT_NAME_CPP}}.cpp",
            R"(#include "{{PROJECT_NAME_CPP}}/{{PROJECT_NAME_CPP}}.h"
#include <memory>

namespace {{PROJECT_NAME_CPP}} {

    class {{PROJECT_NAME_CPP_UPPER}}::Impl {
    public:
        std::string version = "1.0.0";
    };

    {{PROJECT_NAME_CPP_UPPER}}::{{PROJECT_NAME_CPP_UPPER}}()
        : m_impl(std::make_unique<Impl>()) {
    }

    {{PROJECT_NAME_CPP_UPPER}}::~{{PROJECT_NAME_CPP_UPPER}}() = default;

    std::string {{PROJECT_NAME_CPP_UPPER}}::getVersion() const {
        return m_impl->version;
    }

    std::string {{PROJECT_NAME_CPP_UPPER}}::process(const std::string& input) const {
        return "Processed: " + input;
    }

} // namespace {{PROJECT_NAME_CPP}})",
            true, false, "Main library implementation"
        },
        {
            "CMakeLists.txt",
            R"(cmake_minimum_required(VERSION 3.16)
project({{PROJECT_NAME}} VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD {{CPP_STANDARD_NUM}})
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Create library
add_library({{PROJECT_NAME}} src/{{PROJECT_NAME_CPP}}.cpp)

# Include directories
target_include_directories({{PROJECT_NAME}}
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

{{#IF ENABLE_TESTING}}
# Enable testing
enable_testing()
add_subdirectory(tests)
{{/IF}}

# Examples
add_subdirectory(examples)

# Install
install(TARGETS {{PROJECT_NAME}}
    EXPORT {{PROJECT_NAME}}Targets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
)

install(DIRECTORY include/ DESTINATION include)

# Export targets
install(EXPORT {{PROJECT_NAME}}Targets
    FILE {{PROJECT_NAME}}Targets.cmake
    NAMESPACE {{PROJECT_NAME}}::
    DESTINATION lib/cmake/{{PROJECT_NAME}}
))",
            true, false, "CMake build configuration", {"cmake"}
        }
    };

    config.structure.files = files;

    // Available features
    config.availableFeatures = {
        "shared-lib", "static-lib", "examples", "benchmarks", "testing",
        "documentation", "doxygen", "code-style", "ci-github", "ci-gitlab"
    };

    // Default features
    config.defaultFeatures = {"static-lib", "examples", "testing", "documentation", "doxygen"};

    // Supported build systems
    config.supportedBuildSystems = {"cmake", "meson", "bazel"};

    // Recommended packages
    config.recommendedPackages = {"fmt", "spdlog", "benchmark"};

    return config;
}

TemplateCustomization getHeaderOnlyLibTemplate() {
    TemplateCustomization config;
    config.templateName = "header-only-lib";
    config.description = "Header-only library template with single-header generation";

    // Directory structure
    config.structure.name = "header-only-standard";
    config.structure.description = "Header-only library structure";
    config.structure.directories = {"include", "tests", "examples", "docs", "single_include", "cmake"};
    config.structure.isDefault = true;

    // Available features
    config.availableFeatures = {
        "single-header", "examples", "testing", "documentation",
        "doxygen", "benchmarks", "code-style", "ci-github"
    };

    // Default features
    config.defaultFeatures = {"single-header", "examples", "testing", "documentation"};

    // Supported build systems
    config.supportedBuildSystems = {"cmake", "meson"};

    // Recommended packages
    config.recommendedPackages = {};

    return config;
}

TemplateCustomization getGuiTemplate() {
    TemplateCustomization config;
    config.templateName = "gui";
    config.description = "Graphical user interface application with framework integration";

    // Available features
    config.availableFeatures = {
        "qt6", "imgui", "gtk", "wxwidgets", "resources", "themes",
        "internationalization", "testing", "documentation"
    };

    // Default features
    config.defaultFeatures = {"qt6", "resources", "testing"};

    return config;
}

TemplateCustomization getNetworkTemplate() {
    TemplateCustomization config;
    config.templateName = "network";
    config.description = "Network client/server application with async I/O";

    // Available features
    config.availableFeatures = {
        "asio", "boost-asio", "ssl", "websockets", "json", "protobuf",
        "client", "server", "testing", "documentation"
    };

    // Default features
    config.defaultFeatures = {"asio", "json", "client", "server", "testing"};

    return config;
}

TemplateCustomization getWebServiceTemplate() {
    TemplateCustomization config;
    config.templateName = "webservice";
    config.description = "REST API and web service with database integration";

    // Available features
    config.availableFeatures = {
        "crow", "beast", "database", "sqlite", "postgresql", "mysql",
        "authentication", "logging", "docker", "testing", "documentation"
    };

    // Default features
    config.defaultFeatures = {"crow", "sqlite", "authentication", "docker", "testing"};

    return config;
}

TemplateCustomization getEmbeddedTemplate() {
    TemplateCustomization config;
    config.templateName = "embedded";
    config.description = "Embedded systems project with hardware abstraction";

    // Available features
    config.availableFeatures = {
        "arduino", "stm32", "esp32", "rtos", "hal", "drivers",
        "testing", "documentation"
    };

    // Default features
    config.defaultFeatures = {"hal", "drivers", "testing"};

    return config;
}

TemplateCustomization getGameEngineTemplate() {
    TemplateCustomization config;
    config.templateName = "gameengine";
    config.description = "Game engine with graphics, audio, and input systems";

    // Available features
    config.availableFeatures = {
        "opengl", "vulkan", "directx", "sdl2", "glfw", "audio", "physics",
        "scripting", "assets", "shaders", "testing", "documentation"
    };

    // Default features
    config.defaultFeatures = {"opengl", "sdl2", "audio", "assets", "testing"};

    return config;
}

} // namespace predefined
} // namespace templates

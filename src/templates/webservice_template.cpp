#include "webservice_template.h"

#include <spdlog/fmt/fmt.h>

#include <iostream>

#include "../utils/core/file_utils.h"
#include "../utils/core/string_utils.h"

using namespace utils;
using namespace cli_enums;

WebServiceTemplate::WebServiceTemplate(const CliOptions& options) : TemplateBase(options) {}

bool WebServiceTemplate::create() {
    std::cout << "?? Creating Web Service project: " << options_.projectName << "\n";

    if (!createProjectStructure()) {
        std::cerr << "? Failed to create project structure\n";
        return false;
    }

    if (!createBuildSystem()) {
        std::cerr << "? Failed to create build system\n";
        return false;
    }

    if (!setupPackageManager()) {
        std::cerr << "? Failed to setup package manager\n";
        return false;
    }

    if (!setupTestFramework()) {
        std::cerr << "? Failed to setup test framework\n";
        return false;
    }

    if (!setupDockerConfiguration()) {
        std::cerr << "? Failed to setup Docker configuration\n";
        return false;
    }

    if (!setupAPIDocumentation()) {
        std::cerr << "? Failed to setup API documentation\n";
        return false;
    }

    if (!initializeGit(options_.projectName)) {
        std::cerr << "? Failed to initialize Git repository\n";
        return false;
    }

    std::cout << "? Web Service project created successfully!\n";
    printUsageGuide();
    return true;
}

bool WebServiceTemplate::createProjectStructure() {
    std::string projectPath = options_.projectName;

    // Create main directories
    std::vector<std::string> directories = {
            projectPath,
            FileUtils::combinePath(projectPath, "src"),
            FileUtils::combinePath(projectPath, "src/controllers"),
            FileUtils::combinePath(projectPath, "src/middleware"),
            FileUtils::combinePath(projectPath, "src/models"),
            FileUtils::combinePath(projectPath, "src/routes"),
            FileUtils::combinePath(projectPath, "src/utils"),
            FileUtils::combinePath(projectPath, "src/config"),
            FileUtils::combinePath(projectPath, "include"),
            FileUtils::combinePath(projectPath, "include/controllers"),
            FileUtils::combinePath(projectPath, "include/middleware"),
            FileUtils::combinePath(projectPath, "include/models"),
            FileUtils::combinePath(projectPath, "include/routes"),
            FileUtils::combinePath(projectPath, "include/utils"),
            FileUtils::combinePath(projectPath, "include/config"),
            FileUtils::combinePath(projectPath, "tests"),
            FileUtils::combinePath(projectPath, "tests/unit"),
            FileUtils::combinePath(projectPath, "tests/integration"),
            FileUtils::combinePath(projectPath, "docs"),
            FileUtils::combinePath(projectPath, "docker"),
            FileUtils::combinePath(projectPath, "scripts"),
            FileUtils::combinePath(projectPath, "config")};

    for (const auto& dir : directories) {
        if (!FileUtils::createDirectory(dir)) {
            std::cerr << "? Failed to create directory: " << dir << "\n";
            return false;
        }
    }

    // Create main application files
    if (!FileUtils::writeToFile(FileUtils::combinePath(projectPath, "src/main.cpp"),
                                getMainCppContent())) {
        return false;
    }

    if (!FileUtils::writeToFile(FileUtils::combinePath(projectPath, "include/server.h"),
                                getServerHeaderContent())) {
        return false;
    }

    if (!FileUtils::writeToFile(FileUtils::combinePath(projectPath, "src/server.cpp"),
                                getServerCppContent())) {
        return false;
    }

    if (!FileUtils::writeToFile(FileUtils::combinePath(projectPath, "README.md"),
                                getReadmeContent())) {
        return false;
    }

    std::cout << "?? Project structure created\n";
    return true;
}

bool WebServiceTemplate::createBuildSystem() {
    std::string projectPath = options_.projectName;
    std::string buildContent;

    switch (options_.buildSystem) {
        case BuildSystem::CMake:
            buildContent = getCMakeContent();
            if (!FileUtils::writeToFile(FileUtils::combinePath(projectPath, "CMakeLists.txt"),
                                        buildContent)) {
                return false;
            }
            break;
        case BuildSystem::Meson:
            buildContent = getMesonContent();
            if (!FileUtils::writeToFile(FileUtils::combinePath(projectPath, "meson.build"),
                                        buildContent)) {
                return false;
            }
            break;
        case BuildSystem::Bazel:
            buildContent = getBazelContent();
            if (!FileUtils::writeToFile(FileUtils::combinePath(projectPath, "BUILD"),
                                        buildContent)) {
                return false;
            }
            break;
        case BuildSystem::XMake:
            buildContent = getXMakeContent();
            if (!FileUtils::writeToFile(FileUtils::combinePath(projectPath, "xmake.lua"),
                                        buildContent)) {
                return false;
            }
            break;
        case BuildSystem::Premake:
            buildContent = getPremakeContent();
            if (!FileUtils::writeToFile(FileUtils::combinePath(projectPath, "premake5.lua"),
                                        buildContent)) {
                return false;
            }
            break;
        default:
            buildContent = getCMakeContent();
            if (!FileUtils::writeToFile(FileUtils::combinePath(projectPath, "CMakeLists.txt"),
                                        buildContent)) {
                return false;
            }
            break;
    }

    std::cout << "?? Build system configured\n";
    return true;
}

bool WebServiceTemplate::setupPackageManager() {
    std::string projectPath = options_.projectName;

    if (to_string(options_.packageManager) == "vcpkg") {
        std::string vcpkgContent = getVcpkgJsonContent();
        if (!FileUtils::writeToFile(FileUtils::combinePath(projectPath, "vcpkg.json"),
                                    vcpkgContent)) {
            return false;
        }
    } else if (to_string(options_.packageManager) == "conan") {
        std::string conanContent = getConanfileContent();
        if (!FileUtils::writeToFile(FileUtils::combinePath(projectPath, "conanfile.txt"),
                                    conanContent)) {
            return false;
        }
    }

    std::cout << "?? Package manager configured\n";
    return true;
}

bool WebServiceTemplate::setupTestFramework() {
    if (!options_.includeTests) {
        return true;
    }

    std::string projectPath = options_.projectName;
    std::string testsPath = FileUtils::combinePath(projectPath, "tests");

    std::string testContent;
    if (options_.testFramework == TestFramework::GTest) {
        testContent = getGTestContent();
    } else if (options_.testFramework == TestFramework::Catch2) {
        testContent = getCatch2Content();
    }

    if (!FileUtils::writeToFile(FileUtils::combinePath(testsPath, "unit/test_server.cpp"),
                                testContent)) {
        return false;
    }

    // Add integration tests
    std::string integrationTestContent = getIntegrationTestContent();
    if (!FileUtils::writeToFile(FileUtils::combinePath(testsPath, "integration/test_api.cpp"),
                                integrationTestContent)) {
        return false;
    }

    std::cout << "?? Test framework configured\n";
    return true;
}

bool WebServiceTemplate::setupDockerConfiguration() {
    std::string projectPath = options_.projectName;

    // Create Dockerfile
    std::string dockerfileContent = getDockerfileContent();
    if (!FileUtils::writeToFile(FileUtils::combinePath(projectPath, "Dockerfile"),
                                dockerfileContent)) {
        return false;
    }

    // Create docker-compose.yml
    std::string dockerComposeContent = getDockerComposeContent();
    if (!FileUtils::writeToFile(FileUtils::combinePath(projectPath, "docker-compose.yml"),
                                dockerComposeContent)) {
        return false;
    }

    // Create .dockerignore
    std::string dockerIgnoreContent = getDockerIgnoreContent();
    if (!FileUtils::writeToFile(FileUtils::combinePath(projectPath, ".dockerignore"),
                                dockerIgnoreContent)) {
        return false;
    }

    std::cout << "?? Docker configuration created\n";
    return true;
}

bool WebServiceTemplate::setupAPIDocumentation() {
    std::string projectPath = options_.projectName;
    std::string docsPath = FileUtils::combinePath(projectPath, "docs");

    // Create OpenAPI specification
    std::string openAPIContent = getOpenAPIContent();
    if (!FileUtils::writeToFile(FileUtils::combinePath(docsPath, "api.yaml"), openAPIContent)) {
        return false;
    }

    // Create Postman collection
    std::string postmanContent = getPostmanCollectionContent();
    if (!FileUtils::writeToFile(FileUtils::combinePath(docsPath, "postman_collection.json"),
                                postmanContent)) {
        return false;
    }

    std::cout << "?? API documentation created\n";
    return true;
}

std::string WebServiceTemplate::getWebFramework() const {
    // Default to a lightweight HTTP framework
    return "httplib";  // cpp-httplib is a popular choice
}

std::string WebServiceTemplate::getDatabaseType() const {
    // Default database type
    return "sqlite";  // SQLite for simplicity, can be extended
}

bool WebServiceTemplate::isRestAPI() const {
    return true;  // Default to REST API
}

bool WebServiceTemplate::isGraphQL() const {
    return false;  // GraphQL support can be added later
}

std::string WebServiceTemplate::getMainCppContent() {
    return fmt::format(R"(#include "server.h"
#include "config/config.h"
#include "utils/logger.h"
#include <iostream>
#include <signal.h>

// Global server instance for signal handling
std::unique_ptr<{}::Server> g_server;

void signalHandler(int signal) {{
    if (g_server) {{
        std::cout << "\nShutting down server gracefully...\n";
        g_server->stop();
    }}
    exit(signal);
}}

int main(int argc, char* argv[]) {{
    try {{
        // Initialize logger
        {}::Logger::initialize();

        // Load configuration
        {}::Config config;
        if (argc > 1) {{
            config.loadFromFile(argv[1]);
        }} else {{
            config.loadDefaults();
        }}

        // Setup signal handlers
        signal(SIGINT, signalHandler);
        signal(SIGTERM, signalHandler);

        // Create and start server
        g_server = std::make_unique<{}::Server>(config);

        std::cout << "Starting {} server on port " << config.getPort() << "...\n";
        g_server->start();

        return 0;
    }} catch (const std::exception& e) {{
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }}
}}
)",
                       options_.projectName, options_.projectName, options_.projectName,
                       options_.projectName, options_.projectName);
}

std::string WebServiceTemplate::getServerHeaderContent() {
    return fmt::format(R"(#pragma once

#include "config/config.h"
#include <memory>
#include <string>
#include <thread>

namespace {} {{

class Server {{
public:
    explicit Server(const Config& config);
    ~Server();

    void start();
    void stop();
    bool isRunning() const;

private:
    void setupRoutes();
    void handleRequest(const std::string& method, const std::string& path);

    Config config_;
    bool running_;
    std::unique_ptr<std::thread> serverThread_;
}};

}} // namespace {}
)",
                       options_.projectName, options_.projectName);
}

std::string WebServiceTemplate::getServerCppContent() {
    return fmt::format(R"(#include "server.h"
#include "routes/router.h"
#include "utils/logger.h"
#include <httplib.h>
#include <iostream>

namespace {} {{

Server::Server(const Config& config)
    : config_(config), running_(false) {{
}}

Server::~Server() {{
    stop();
}}

void Server::start() {{
    if (running_) {{
        return;
    }}

    running_ = true;

    httplib::Server server;

    // Setup CORS
    server.set_pre_routing_handler([](const httplib::Request& req, httplib::Response& res) {{
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
        res.set_header("Access-Control-Allow-Headers", "Content-Type, Authorization");
        return httplib::Server::HandlerResponse::Unhandled;
    }});

    // Setup routes
    setupRoutes();

    // Health check endpoint
    server.Get("/health", [](const httplib::Request&, httplib::Response& res) {{
        res.set_content("{{\\"status\\": \\"healthy\\", \\"service\\": \\"{}\\"}}", "application/json");
    }});

    // API info endpoint
    server.Get("/api/info", [](const httplib::Request&, httplib::Response& res) {{
        res.set_content("{{\\"name\\": \\"{}\\"", \\"version\\": \\"1.0.0\\", \\"description\\": \\"RESTful API service\\"}}", "application/json");
    }});

    Logger::info("Server starting on port " + std::to_string(config_.getPort()));

    if (!server.listen("0.0.0.0", config_.getPort())) {{
        Logger::error("Failed to start server on port " + std::to_string(config_.getPort()));
        running_ = false;
    }}
}}

void Server::stop() {{
    if (!running_) {{
        return;
    }}

    running_ = false;
    Logger::info("Server stopped");
}}

bool Server::isRunning() const {{
    return running_;
}}

void Server::setupRoutes() {{
    // Routes will be setup here
    // This can be extended with a proper router implementation
}}

}} // namespace {}
)",
                       options_.projectName, options_.projectName);
}

std::string WebServiceTemplate::getCMakeContent() {
    return fmt::format(R"(cmake_minimum_required(VERSION 3.15)
project({} VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Find packages
find_package(fmt REQUIRED)
find_package(spdlog REQUIRED)

# Add httplib (header-only library)
include(FetchContent)
FetchContent_Declare(
  httplib
  GIT_REPOSITORY https://github.com/yhirose/cpp-httplib.git
  GIT_TAG v0.14.0
)
FetchContent_MakeAvailable(httplib)

# Source files
set(SOURCES
  src/main.cpp
  src/server.cpp
  src/config/config.cpp
  src/utils/logger.cpp
  src/routes/router.cpp
  src/controllers/controller.cpp
)

# Create executable
add_executable({} ${{SOURCES}})

# Link libraries
target_link_libraries({} PRIVATE
  fmt::fmt
  spdlog::spdlog
  httplib::httplib
)

# Include directories
target_include_directories({} PRIVATE
  include
  ${{CMAKE_CURRENT_SOURCE_DIR}}
)

# Compiler warnings
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  target_compile_options({} PRIVATE -Wall -Wextra -Wpedantic)
elseif(MSVC)
  target_compile_options({} PRIVATE /W4)
endif()

# Install
install(TARGETS {} DESTINATION bin)
)",
                       options_.projectName, options_.projectName, options_.projectName,
                       options_.projectName, options_.projectName, options_.projectName,
                       options_.projectName);
}

std::string WebServiceTemplate::getReadmeContent() {
    return fmt::format(R"(# {}

A modern C++ web service built with cpp-httplib.

## Features

- RESTful API endpoints
- JSON request/response handling
- Logging with spdlog
- Configuration management
- Docker support
- Comprehensive testing
- API documentation

## Building

### Prerequisites

- C++17 compatible compiler
- CMake 3.15+
- vcpkg or Conan (for dependencies)

### Build Steps

```bash
mkdir build && cd build
cmake ..
make -j$(nproc)
```

### Using Docker

```bash
docker build -t {} .
docker run -p 8080:8080 {}
```

## API Endpoints

- `GET /health` - Health check
- `GET /api/info` - Service information
- `GET /api/v1/users` - Get all users
- `POST /api/v1/users` - Create user
- `GET /api/v1/users/{{id}}` - Get user by ID
- `PUT /api/v1/users/{{id}}` - Update user
- `DELETE /api/v1/users/{{id}}` - Delete user

## Configuration

The service can be configured via:
- Command line arguments
- Environment variables
- Configuration file (config.json)

## Testing

```bash
# Run unit tests
ctest

# Run integration tests
./tests/integration_tests
```

## Documentation

- API documentation: `docs/api.yaml` (OpenAPI 3.0)
- Postman collection: `docs/postman_collection.json`

## License

MIT License
)",
                       options_.projectName, options_.projectName, options_.projectName);
}

std::string WebServiceTemplate::getVcpkgJsonContent() {
    return fmt::format(R"({{
  "name": "{}",
  "version": "1.0.0",
  "description": "Modern C++ Web Service",
  "dependencies": [
    "fmt",
    "spdlog",
    "nlohmann-json",
    "sqlite3"
  ]
}}
)",
                       options_.projectName);
}

std::string WebServiceTemplate::getConanfileContent() {
    return R"([requires]
fmt/9.1.0
spdlog/1.11.0
nlohmann_json/3.11.2
sqlite3/3.41.2

[generators]
CMakeDeps
CMakeToolchain

[options]
fmt:shared=False
spdlog:shared=False
)";
}

std::string WebServiceTemplate::getDockerfileContent() {
    return fmt::format(R"(# Multi-stage build for C++ web service
FROM ubuntu:22.04 as builder

# Install build dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    pkg-config \
    && rm -rf /var/lib/apt/lists/*

# Set working directory
WORKDIR /app

# Copy source code
COPY . .

# Build the application
RUN mkdir build && cd build && \
    cmake .. && \
    make -j$(nproc)

# Production stage
FROM ubuntu:22.04

# Install runtime dependencies
RUN apt-get update && apt-get install -y \
    libstdc++6 \
    && rm -rf /var/lib/apt/lists/*

# Create app user
RUN useradd -m -u 1000 appuser

# Copy binary from builder stage
COPY --from=builder /app/build/{} /usr/local/bin/{}

# Copy configuration
COPY --from=builder /app/config /app/config

# Set ownership
RUN chown -R appuser:appuser /app

# Switch to app user
USER appuser

# Expose port
EXPOSE 8080

# Health check
HEALTHCHECK --interval=30s --timeout=3s --start-period=5s --retries=3 \
    CMD curl -f http://localhost:8080/health || exit 1

# Start the application
CMD ["/usr/local/bin/{}"]
)",
                       options_.projectName, options_.projectName, options_.projectName);
}

std::string WebServiceTemplate::getDockerComposeContent() {
    return fmt::format(R"(version: '3.8'

services:
  {}:
    build: .
    ports:
      - "8080:8080"
    environment:
      - LOG_LEVEL=info
      - DATABASE_URL=sqlite:///app/data/app.db
    volumes:
      - ./data:/app/data
      - ./config:/app/config
    restart: unless-stopped
    healthcheck:
      test: ["CMD", "curl", "-f", "http://localhost:8080/health"]
      interval: 30s
      timeout: 10s
      retries: 3
      start_period: 40s

  database:
    image: postgres:15-alpine
    environment:
      POSTGRES_DB: {}
      POSTGRES_USER: user
      POSTGRES_PASSWORD: password
    volumes:
      - postgres_data:/var/lib/postgresql/data
    ports:
      - "5432:5432"
    restart: unless-stopped

volumes:
  postgres_data:
)",
                       options_.projectName, options_.projectName);
}

std::string WebServiceTemplate::getGTestContent() {
    return fmt::format(R"(#include <gtest/gtest.h>
#include "server.h"

class ServerTest : public ::testing::Test {{
protected:
    void SetUp() override {{
        // Setup test environment
    }}

    void TearDown() override {{
        // Cleanup test environment
    }}
}};

TEST_F(ServerTest, ServerInitialization) {{
    // Test server initialization
    EXPECT_TRUE(true);
}}

TEST_F(ServerTest, HealthEndpoint) {{
    // Test health endpoint
    EXPECT_TRUE(true);
}}

int main(int argc, char **argv) {{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}}
)");
}

std::string WebServiceTemplate::getCatch2Content() {
    return R"(#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include "server.h"

TEST_CASE("Server functionality", "[server]") {
    SECTION("Server initialization") {
        REQUIRE(true);
    }

    SECTION("Health endpoint") {
        REQUIRE(true);
    }
}
)";
}

std::string WebServiceTemplate::getIntegrationTestContent() {
    return fmt::format(R"(#include <gtest/gtest.h>
#include <curl/curl.h>
#include <string>

class APIIntegrationTest : public ::testing::Test {{
protected:
    void SetUp() override {{
        curl_global_init(CURL_GLOBAL_DEFAULT);
    }}

    void TearDown() override {{
        curl_global_cleanup();
    }}

    std::string makeRequest(const std::string& url) {{
        CURL *curl;
        CURLcode res;
        std::string response;

        curl = curl_easy_init();
        if(curl) {{
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
            res = curl_easy_perform(curl);
            curl_easy_cleanup(curl);
        }}
        return response;
    }}
}};

TEST_F(APIIntegrationTest, HealthEndpoint) {{
    std::string response = makeRequest("http://localhost:8080/health");
    EXPECT_FALSE(response.empty());
}}

TEST_F(APIIntegrationTest, APIInfoEndpoint) {{
    std::string response = makeRequest("http://localhost:8080/api/info");
    EXPECT_FALSE(response.empty());
}}
)");
}

std::string WebServiceTemplate::getOpenAPIContent() {
    return fmt::format(R"(openapi: 3.0.3
info:
  title: {} API
  description: RESTful API service built with C++
  version: 1.0.0
  contact:
    name: API Support
    email: support@example.com
  license:
    name: MIT
    url: https://opensource.org/licenses/MIT

servers:
  - url: http://localhost:8080
    description: Development server
  - url: https://api.example.com
    description: Production server

paths:
  /health:
    get:
      summary: Health check endpoint
      description: Returns the health status of the service
      responses:
        '200':
          description: Service is healthy
          content:
            application/json:
              schema:
                type: object
                properties:
                  status:
                    type: string
                    example: healthy
                  service:
                    type: string
                    example: {}

  /api/info:
    get:
      summary: API information
      description: Returns information about the API
      responses:
        '200':
          description: API information
          content:
            application/json:
              schema:
                type: object
                properties:
                  name:
                    type: string
                    example: {}
                  version:
                    type: string
                    example: 1.0.0
                  description:
                    type: string
                    example: RESTful API service

components:
  schemas:
    Error:
      type: object
      properties:
        error:
          type: string
          description: Error message
        code:
          type: integer
          description: Error code
)",
                       options_.projectName, options_.projectName, options_.projectName);
}

std::string WebServiceTemplate::getPostmanCollectionContent() {
    return fmt::format(R"({{
  "info": {{
    "name": "{} API",
    "description": "Postman collection for {} API",
    "schema": "https://schema.getpostman.com/json/collection/v2.1.0/collection.json"
  }},
  "item": [
    {{
      "name": "Health Check",
      "request": {{
        "method": "GET",
        "header": [],
        "url": {{
          "raw": "{{{{base_url}}}}/health",
          "host": [
            "{{{{base_url}}}}"
          ],
          "path": [
            "health"
          ]
        }}
      }}
    }},
    {{
      "name": "API Info",
      "request": {{
        "method": "GET",
        "header": [],
        "url": {{
          "raw": "{{{{base_url}}}}/api/info",
          "host": [
            "{{{{base_url}}}}"
          ],
          "path": [
            "api",
            "info"
          ]
        }}
      }}
    }}
  ],
  "variable": [
    {{
      "key": "base_url",
      "value": "http://localhost:8080",
      "type": "string"
    }}
  ]
}}
)",
                       options_.projectName, options_.projectName);
}

std::string WebServiceTemplate::getDockerIgnoreContent() {
    return R"(.git
.gitignore
README.md
Dockerfile
.dockerignore
build/
bin/
obj/
*.log
.vscode/
.idea/
*.swp
*~
)";
}

std::string WebServiceTemplate::getMesonContent() {
    return fmt::format(R"(project('{}', 'cpp',
  version : '1.0.0',
  default_options : ['warning_level=3', 'cpp_std=c++17'])

# Dependencies
fmt_dep = dependency('fmt')
spdlog_dep = dependency('spdlog')
threads_dep = dependency('threads')

# Source files
sources = [
  'src/main.cpp',
  'src/server.cpp',
  'src/config/config.cpp',
  'src/utils/logger.cpp',
  'src/routes/router.cpp',
  'src/controllers/controller.cpp'
]

# Include directories
inc = include_directories('include')

# Executable
executable('{}',
  sources,
  include_directories : inc,
  dependencies : [fmt_dep, spdlog_dep, threads_dep],
  install : true)
)",
                       options_.projectName, options_.projectName);
}

std::string WebServiceTemplate::getBazelContent() {
    return fmt::format(R"(load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library")

cc_binary(
    name = "{}",
    srcs = [
        "src/main.cpp",
    ],
    deps = [
        ":server_lib",
        "@fmt",
        "@spdlog",
    ],
)

cc_library(
    name = "server_lib",
    srcs = [
        "src/server.cpp",
        "src/config/config.cpp",
        "src/utils/logger.cpp",
        "src/routes/router.cpp",
        "src/controllers/controller.cpp",
    ],
    hdrs = glob(["include/**/*.h"]),
    includes = ["include"],
    deps = [
        "@fmt",
        "@spdlog",
    ],
)
)",
                       options_.projectName);
}

std::string WebServiceTemplate::getXMakeContent() {
    return fmt::format(R"(set_project("{0}")
set_version("1.0.0")

-- Set C++ standard
set_languages("c++17")

-- Add build modes
add_rules("mode.debug", "mode.release")

-- Web service dependencies
add_requires("fmt", "spdlog", "nlohmann_json")

-- Main web service executable
target("{1}")
    set_kind("binary")
    add_files("src/**.cpp")
    add_headerfiles("include/**.h")
    add_includedirs("include", {{public = true}})

    -- Add packages
    add_packages("fmt", "spdlog", "nlohmann_json")

    -- Set output directory
    set_targetdir("bin")

    -- Enable C++ features
    set_languages("c++17")

    -- Add compile flags
    if is_mode("debug") then
        add_defines("DEBUG", "WEBSERVICE_DEBUG")
        set_symbols("debug")
        set_optimize("none")
    elseif is_mode("release") then
        add_defines("NDEBUG", "WEBSERVICE_RELEASE")
        set_symbols("hidden")
        set_optimize("fastest")
    end

-- Tests (if enabled)
if has_config("tests") then
    add_requires("gtest")

    target("{2}_tests")
        set_kind("binary")
        add_files("tests/**.cpp")
        add_packages("gtest", "fmt", "spdlog", "nlohmann_json")
        add_deps("{3}")
        set_targetdir("tests/bin")
        set_languages("c++17")
end
)",
                       options_.projectName, options_.projectName, options_.projectName,
                       options_.projectName);
}

std::string WebServiceTemplate::getPremakeContent() {
    return fmt::format(R"(workspace "{0}"
    configurations {{ "Debug", "Release" }}
    platforms {{ "x64" }}

project "{1}"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"
    targetdir "bin/%{{cfg.buildcfg}}"

    files {{
        "src/**.cpp",
        "include/**.h"
    }}

    includedirs {{
        "include",
        "vendor/httplib",
        "vendor/json/include",
        "vendor/fmt/include",
        "vendor/spdlog/include"
    }}

    links {{
        "ws2_32",
        "wsock32"
    }}

    filter "system:windows"
        defines {{ "WIN32", "_WIN32", "NOMINMAX" }}

    filter "system:linux"
        links {{ "pthread" }}

    filter "configurations:Debug"
        defines {{ "DEBUG", "WEBSERVICE_DEBUG" }}
        symbols "On"
        optimize "Off"

    filter "configurations:Release"
        defines {{ "NDEBUG", "WEBSERVICE_RELEASE" }}
        symbols "Off"
        optimize "Speed"

-- Tests project
project "{2}_tests"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"
    targetdir "bin/%{{cfg.buildcfg}}"

    files {{
        "tests/**.cpp"
    }}

    includedirs {{
        "include",
        "vendor/gtest/include",
        "vendor/httplib",
        "vendor/json/include",
        "vendor/fmt/include",
        "vendor/spdlog/include"
    }}

    links {{
        "gtest",
        "gtest_main",
        "ws2_32",
        "wsock32"
    }}

    filter "system:linux"
        links {{ "pthread" }}
)",
                       options_.projectName, options_.projectName, options_.projectName);
}

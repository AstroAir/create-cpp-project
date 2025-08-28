#include "multi_executable_template.h"
#include "../utils/file_utils.h"
#include "../utils/string_utils.h"
#include "../utils/terminal_utils.h"
#include "../cli/types/cli_enums.h"
#include <fmt/format.h>
#include <spdlog/spdlog.h>
#include <iostream>

using namespace utils;

MultiExecutableTemplate::MultiExecutableTemplate(const CliOptions &options)
    : TemplateBase(options) {}

bool MultiExecutableTemplate::create() {
  spdlog::info("Creating multi-executable project: {}", options_.projectName);

  std::string projectPath = options_.projectName;

  // Create project structure
  if (!createProjectStructure()) {
    spdlog::error("Failed to create project structure");
    return false;
  }

  // Setup shared library
  if (!setupSharedLibrary()) {
    spdlog::error("Failed to setup shared library");
    return false;
  }

  // Setup executables
  if (!setupExecutables()) {
    spdlog::error("Failed to setup executables");
    return false;
  }

  // Create build system files
  if (!createBuildSystem()) {
    spdlog::error("Failed to create build system files");
    return false;
  }

  // Setup package manager
  if (!setupPackageManager()) {
    spdlog::error("Failed to setup package manager");
    return false;
  }

  // Setup test framework
  if (options_.includeTests && !setupTestFramework()) {
    spdlog::error("Failed to setup test framework");
    return false;
  }

  // Initialize Git
  if (options_.initGit) {
    if (!initializeGit(projectPath)) {
      spdlog::error("Failed to initialize Git repository");
      return false;
    }
    spdlog::info("✅ Git repository initialized");
  }

  spdlog::info("\nYour multi-executable project is ready!\n");

  // Print usage instructions
  TerminalUtils::showSuccess("Project created successfully!");
  TerminalUtils::showInfo("Next steps:");
  std::cout << "  1. cd " << options_.projectName << "\n";
  std::cout << "  2. Review the generated executables in src/\n";
  std::cout << "  3. Customize the shared library in lib/\n";

  if (cli_enums::to_string(options_.buildSystem) == "cmake") {
    std::cout << "  4. Build the project:\n";
    std::cout << "     mkdir build && cd build\n";
    std::cout << "     cmake .. && make\n";
    std::cout << "  5. Run the executables:\n";
    std::cout << "     ./bin/" << options_.projectName << "_main\n";
    std::cout << "     ./bin/" << options_.projectName << "_client\n";
    std::cout << "     ./bin/" << options_.projectName << "_server\n";
    std::cout << "     ./bin/" << options_.projectName << "_tool\n";
  }

  return true;
}

bool MultiExecutableTemplate::createProjectStructure() {
  std::string projectPath = options_.projectName;

  // Create main directories
  std::vector<std::string> directories = {
      projectPath,
      FileUtils::combinePath(projectPath, "src"),
      FileUtils::combinePath(projectPath, "lib"),
      FileUtils::combinePath(FileUtils::combinePath(projectPath, "lib"), "include"),
      FileUtils::combinePath(FileUtils::combinePath(projectPath, "lib"), "src"),
      FileUtils::combinePath(projectPath, "bin"),
      FileUtils::combinePath(projectPath, "docs"),
      FileUtils::combinePath(projectPath, "scripts")
  };

  if (options_.includeTests) {
    directories.push_back(FileUtils::combinePath(projectPath, "tests"));
  }

  for (const auto &dir : directories) {
    if (!FileUtils::createDirectory(dir)) {
      spdlog::error("Failed to create directory: {}", dir);
      return false;
    }
  }

  // Create README
  std::string readmePath = FileUtils::combinePath(projectPath, "README.md");
  if (!FileUtils::writeToFile(readmePath, getReadmeContent())) {
    spdlog::error("Failed to create README file");
    return false;
  }

  return true;
}

bool MultiExecutableTemplate::setupSharedLibrary() {
  std::string projectPath = options_.projectName;

  // Create shared library header
  std::string libHeaderPath = FileUtils::combinePath(
      FileUtils::combinePath(FileUtils::combinePath(projectPath, "lib"), "include"),
      options_.projectName + "_lib.h");
  if (!FileUtils::writeToFile(libHeaderPath, getSharedLibraryHeaderContent())) {
    spdlog::error("Failed to create shared library header");
    return false;
  }

  // Create shared library source
  std::string libSourcePath = FileUtils::combinePath(
      FileUtils::combinePath(FileUtils::combinePath(projectPath, "lib"), "src"),
      options_.projectName + "_lib.cpp");
  if (!FileUtils::writeToFile(libSourcePath, getSharedLibrarySourceContent())) {
    spdlog::error("Failed to create shared library source");
    return false;
  }

  // Create utils header
  std::string utilsHeaderPath = FileUtils::combinePath(
      FileUtils::combinePath(FileUtils::combinePath(projectPath, "lib"), "include"),
      "utils.h");
  if (!FileUtils::writeToFile(utilsHeaderPath, getUtilsHeaderContent())) {
    spdlog::error("Failed to create utils header");
    return false;
  }

  // Create utils source
  std::string utilsSourcePath = FileUtils::combinePath(
      FileUtils::combinePath(FileUtils::combinePath(projectPath, "lib"), "src"),
      "utils.cpp");
  if (!FileUtils::writeToFile(utilsSourcePath, getUtilsSourceContent())) {
    spdlog::error("Failed to create utils source");
    return false;
  }

  // Create version header
  std::string versionHeaderPath = FileUtils::combinePath(
      FileUtils::combinePath(FileUtils::combinePath(projectPath, "lib"), "include"),
      "version.h");
  if (!FileUtils::writeToFile(versionHeaderPath, getVersionHeaderContent())) {
    spdlog::error("Failed to create version header");
    return false;
  }

  // Create config header
  std::string configHeaderPath = FileUtils::combinePath(
      FileUtils::combinePath(FileUtils::combinePath(projectPath, "lib"), "include"),
      "config.h");
  if (!FileUtils::writeToFile(configHeaderPath, getConfigHeaderContent())) {
    spdlog::error("Failed to create config header");
    return false;
  }

  return true;
}

bool MultiExecutableTemplate::setupExecutables() {
  std::string projectPath = options_.projectName;
  auto configs = getExecutableConfigs();

  for (const auto& config : configs) {
    std::string executablePath = FileUtils::combinePath(
        FileUtils::combinePath(projectPath, "src"), config.sourceFile);

    std::string content;
    if (config.name == "main") {
      content = getMainExecutableContent();
    } else if (config.name == "client") {
      content = getClientExecutableContent();
    } else if (config.name == "server") {
      content = getServerExecutableContent();
    } else if (config.name == "tool") {
      content = getToolExecutableContent();
    }

    if (!FileUtils::writeToFile(executablePath, content)) {
      spdlog::error("Failed to create executable: {}", config.name);
      return false;
    }
  }

  return true;
}

bool MultiExecutableTemplate::createBuildSystem() {
  std::string projectPath = options_.projectName;
  std::string buildSystem = std::string(cli_enums::to_string(options_.buildSystem));

  if (buildSystem == "cmake") {
    std::string cmakePath = FileUtils::combinePath(projectPath, "CMakeLists.txt");
    if (!FileUtils::writeToFile(cmakePath, getCMakeContent())) {
      spdlog::error("Failed to create CMakeLists.txt");
      return false;
    }
  } else if (buildSystem == "meson") {
    std::string mesonPath = FileUtils::combinePath(projectPath, "meson.build");
    if (!FileUtils::writeToFile(mesonPath, getMesonContent())) {
      spdlog::error("Failed to create meson.build");
      return false;
    }
  } else if (buildSystem == "bazel") {
    std::string bazelPath = FileUtils::combinePath(projectPath, "BUILD");
    if (!FileUtils::writeToFile(bazelPath, getBazelContent())) {
      spdlog::error("Failed to create BUILD file");
      return false;
    }

    std::string workspacePath = FileUtils::combinePath(projectPath, "WORKSPACE");
    if (!FileUtils::writeToFile(workspacePath, "workspace(name = \"" + options_.projectName + "\")\n")) {
      spdlog::error("Failed to create WORKSPACE file");
      return false;
    }
  }

  return true;
}

bool MultiExecutableTemplate::setupPackageManager() {
  std::string projectPath = options_.projectName;
  std::string packageManager = std::string(cli_enums::to_string(options_.packageManager));

  if (packageManager == "vcpkg") {
    std::string vcpkgPath = FileUtils::combinePath(projectPath, "vcpkg.json");
    if (!FileUtils::writeToFile(vcpkgPath, getVcpkgJsonContent())) {
      spdlog::error("Failed to create vcpkg.json");
      return false;
    }
  } else if (packageManager == "conan") {
    std::string conanPath = FileUtils::combinePath(projectPath, "conanfile.txt");
    if (!FileUtils::writeToFile(conanPath, getConanfileContent())) {
      spdlog::error("Failed to create conanfile.txt");
      return false;
    }
  }

  return true;
}

bool MultiExecutableTemplate::setupTestFramework() {
  if (!options_.includeTests) {
    return true;
  }

  std::string projectPath = options_.projectName;
  std::string testFramework = std::string(cli_enums::to_string(options_.testFramework));

  // Create test CMakeLists.txt if using CMake
  if (cli_enums::to_string(options_.buildSystem) == "cmake") {
    std::string testCMakePath = FileUtils::combinePath(
        FileUtils::combinePath(projectPath, "tests"), "CMakeLists.txt");
    std::string testCMakeContent;

    if (testFramework == "gtest") {
      testCMakeContent = getGTestContent();
    } else if (testFramework == "catch2") {
      testCMakeContent = getCatch2Content();
    } else if (testFramework == "doctest") {
      testCMakeContent = getDocTestContent();
    }

    if (!FileUtils::writeToFile(testCMakePath, testCMakeContent)) {
      spdlog::error("Failed to create test CMakeLists.txt");
      return false;
    }
  }

  // Create basic test file
  std::string testFilePath = FileUtils::combinePath(
      FileUtils::combinePath(projectPath, "tests"), "test_" + options_.projectName + ".cpp");

  std::string testContent = fmt::format(R"(#include <{0}_lib.h>

#ifdef USING_GTEST
#include <gtest/gtest.h>

TEST({1}Test, BasicFunctionality) {{
    {0}::Core core;
    EXPECT_TRUE(core.initialize());
    EXPECT_EQ("1.0.0", {0}::getVersion());
}}

int main(int argc, char** argv) {{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}}

#elif defined(USING_CATCH2)
#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

TEST_CASE("{0} basic functionality", "[{0}]") {{
    {0}::Core core;
    REQUIRE(core.initialize());
    REQUIRE({0}::getVersion() == "1.0.0");
}}

#elif defined(USING_DOCTEST)
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

TEST_CASE("testing {0}") {{
    {0}::Core core;
    CHECK(core.initialize());
    CHECK({0}::getVersion() == "1.0.0");
}}

#else
#include <iostream>
#include <cassert>

int main() {{
    {0}::Core core;
    assert(core.initialize());
    assert({0}::getVersion() == "1.0.0");
    std::cout << "All tests passed!" << std::endl;
    return 0;
}}
#endif
)", options_.projectName, StringUtils::toUpper(options_.projectName));

  if (!FileUtils::writeToFile(testFilePath, testContent)) {
    spdlog::error("Failed to create test file");
    return false;
  }

  return true;
}

std::vector<MultiExecutableTemplate::ExecutableConfig> MultiExecutableTemplate::getExecutableConfigs() const {
  return {
    {"main", "Main application executable", "main.cpp", true, {}},
    {"client", "Client application", "client.cpp", true, {}},
    {"server", "Server application", "server.cpp", true, {}},
    {"tool", "Command-line tool", "tool.cpp", true, {}}
  };
}

std::string MultiExecutableTemplate::getMainExecutableContent() {
  return fmt::format(R"(#include <iostream>
#include <{0}_lib.h>
#include <utils.h>
#include <version.h>

int main(int argc, char* argv[]) {{
    try {{
        std::cout << "Welcome to {0} Main Application!" << std::endl;
        std::cout << "Version: " << {0}::getVersion() << std::endl;

        // Initialize the core library
        {0}::Core core;
        if (!core.initialize()) {{
            std::cerr << "Failed to initialize core library" << std::endl;
            return 1;
        }}

        // Parse command line arguments
        if (argc > 1) {{
            std::cout << "Processing arguments:" << std::endl;
            for (int i = 1; i < argc; ++i) {{
                std::cout << "  Arg " << i << ": " << argv[i] << std::endl;
            }}
        }}

        // Main application logic
        std::cout << "Running main application logic..." << std::endl;

        // Use utility functions
        std::string timestamp = {0}::utils::getCurrentTimestamp();
        std::cout << "Current timestamp: " << timestamp << std::endl;

        // Demonstrate core functionality
        auto result = core.processData("sample data");
        std::cout << "Processing result: " << result << std::endl;

        std::cout << "Main application completed successfully!" << std::endl;
        return 0;

    }} catch (const std::exception& e) {{
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }}
}}
)", options_.projectName);
}

std::string MultiExecutableTemplate::getClientExecutableContent() {
  return fmt::format(R"DELIM(#include <iostream>
#include <string>
#include <{0}_lib.h>
#include <utils.h>
#include <version.h>

void printUsage(const char* programName) {{
    std::cout << "Usage: " << programName << " [options]" << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  -h, --help     Show this help message" << std::endl;
    std::cout << "  -v, --version  Show version information" << std::endl;
    std::cout << "  -c, --connect  Connect to server" << std::endl;
    std::cout << "  -s, --server   Server address (default: localhost)" << std::endl;
    std::cout << "  -p, --port     Server port (default: 8080)" << std::endl;
}}

int main(int argc, char* argv[]) {{
    try {{
        std::string serverAddress = "localhost";
        int port = 8080;
        bool shouldConnect = false;

        // Parse command line arguments
        for (int i = 1; i < argc; ++i) {{
            std::string arg = argv[i];

            if (arg == "-h" || arg == "--help") {{
                printUsage(argv[0]);
                return 0;
            }} else if (arg == "-v" || arg == "--version") {{
                std::cout << "{0} Client v" << {0}::getVersion() << std::endl;
                return 0;
            }} else if (arg == "-c" || arg == "--connect") {{
                shouldConnect = true;
            }} else if (arg == "-s" || arg == "--server") {{
                if (i + 1 < argc) {{
                    serverAddress = argv[++i];
                }} else {{
                    std::cerr << "Error: --server requires an argument" << std::endl;
                    return 1;
                }}
            }} else if (arg == "-p" || arg == "--port") {{
                if (i + 1 < argc) {{
                    port = std::stoi(argv[++i]);
                }} else {{
                    std::cerr << "Error: --port requires an argument" << std::endl;
                    return 1;
                }}
            }}
        }}

        std::cout << "{0} Client Application" << std::endl;
        std::cout << "Version: " << {0}::getVersion() << std::endl;

        // Initialize the core library
        {0}::Core core;
        if (!core.initialize()) {{
            std::cerr << "Failed to initialize core library" << std::endl;
            return 1;
        }}

        if (shouldConnect) {{
            std::cout << "Connecting to server at " << serverAddress << ":" << port << std::endl;

            // Simulate client connection logic
            std::cout << "Establishing connection..." << std::endl;
            std::cout << "Connected successfully!" << std::endl;

            // Client-specific operations
            std::cout << "Sending request to server..." << std::endl;
            auto response = core.processData("client request");
            std::cout << "Server response: " << response << std::endl;

            std::cout << "Disconnecting from server..." << std::endl;
        }} else {{
            std::cout << "Use -c or --connect to connect to server" << std::endl;
            std::cout << "Use -h or --help for more options" << std::endl;
        }}

        return 0;

    }} catch (const std::exception& e) {{
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }}
}}
)DELIM", options_.projectName);
}

std::string MultiExecutableTemplate::getServerExecutableContent() {
  return fmt::format(R"DELIM(#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <{0}_lib.h>
#include <utils.h>
#include <version.h>

void printUsage(const char* programName) {{
    std::cout << "Usage: " << programName << " [options]" << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  -h, --help     Show this help message" << std::endl;
    std::cout << "  -v, --version  Show version information" << std::endl;
    std::cout << "  -p, --port     Server port (default: 8080)" << std::endl;
    std::cout << "  -d, --daemon   Run as daemon" << std::endl;
}}

int main(int argc, char* argv[]) {{
    try {{
        int port = 8080;
        bool daemonMode = false;

        // Parse command line arguments
        for (int i = 1; i < argc; ++i) {{
            std::string arg = argv[i];

            if (arg == "-h" || arg == "--help") {{
                printUsage(argv[0]);
                return 0;
            }} else if (arg == "-v" || arg == "--version") {{
                std::cout << "{0} Server v" << {0}::getVersion() << std::endl;
                return 0;
            }} else if (arg == "-p" || arg == "--port") {{
                if (i + 1 < argc) {{
                    port = std::stoi(argv[++i]);
                }} else {{
                    std::cerr << "Error: --port requires an argument" << std::endl;
                    return 1;
                }}
            }} else if (arg == "-d" || arg == "--daemon") {{
                daemonMode = true;
            }}
        }}

        std::cout << "{0} Server Application" << std::endl;
        std::cout << "Version: " << {0}::getVersion() << std::endl;
        std::cout << "Starting server on port " << port << std::endl;

        // Initialize the core library
        {0}::Core core;
        if (!core.initialize()) {{
            std::cerr << "Failed to initialize core library" << std::endl;
            return 1;
        }}

        if (daemonMode) {{
            std::cout << "Running in daemon mode..." << std::endl;
        }}

        // Simulate server startup
        std::cout << "Initializing server components..." << std::endl;
        std::cout << "Server listening on port " << port << std::endl;
        std::cout << "Server ready to accept connections!" << std::endl;

        // Main server loop
        bool running = true;
        int connectionCount = 0;

        while (running) {{
            // Simulate handling connections
            std::this_thread::sleep_for(std::chrono::seconds(2));

            connectionCount++;
            std::cout << "Handled connection #" << connectionCount << std::endl;

            // Process some data
            auto result = core.processData("server data " + std::to_string(connectionCount));
            std::cout << "Processed: " << result << std::endl;

            // Simple exit condition for demo
            if (connectionCount >= 5) {{
                std::cout << "Demo complete, shutting down server..." << std::endl;
                running = false;
            }}
        }}

        std::cout << "Server shutdown complete." << std::endl;
        return 0;

    }} catch (const std::exception& e) {{
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }}
}}
)DELIM", options_.projectName);
}

std::string MultiExecutableTemplate::getToolExecutableContent() {
  return fmt::format(R"(#include <iostream>
#include <string>
#include <vector>
#include <{}_lib.h>
#include <utils.h>
#include <version.h>

void printUsage(const char* programName) {{
    std::cout << "Usage: " << programName << " [command] [options]" << std::endl;
    std::cout << "Commands:" << std::endl;
    std::cout << "  info       Show project information" << std::endl;
    std::cout << "  process    Process data" << std::endl;
    std::cout << "  validate   Validate configuration" << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  -h, --help     Show this help message" << std::endl;
    std::cout << "  -v, --version  Show version information" << std::endl;
    std::cout << "  -q, --quiet    Quiet mode" << std::endl;
}}

int main(int argc, char* argv[]) {{
    try {{
        bool quietMode = false;
        std::string command;

        // Parse command line arguments
        for (int i = 1; i < argc; ++i) {{
            std::string arg = argv[i];

            if (arg == "-h" || arg == "--help") {{
                printUsage(argv[0]);
                return 0;
            }} else if (arg == "-v" || arg == "--version") {{
                std::cout << "{} Tool v" << {}::getVersion() << std::endl;
                return 0;
            }} else if (arg == "-q" || arg == "--quiet") {{
                quietMode = true;
            }} else if (command.empty()) {{
                command = arg;
            }}
        }}

        if (!quietMode) {{
            std::cout << "{} Command Line Tool" << std::endl;
            std::cout << "Version: " << {}::getVersion() << std::endl;
        }}

        // Initialize the core library
        {}::Core core;
        if (!core.initialize()) {{
            std::cerr << "Failed to initialize core library" << std::endl;
            return 1;
        }}

        if (command.empty()) {{
            std::cerr << "Error: No command specified" << std::endl;
            printUsage(argv[0]);
            return 1;
        }}

        if (command == "info") {{
            std::cout << "Project: {}" << std::endl;
            std::cout << "Version: " << {}::getVersion() << std::endl;
            std::cout << "Build timestamp: " << {}::utils::getCurrentTimestamp() << std::endl;
        }} else if (command == "process") {{
            std::cout << "Processing data..." << std::endl;
            auto result = core.processData("tool command data");
            std::cout << "Result: " << result << std::endl;
        }} else if (command == "validate") {{
            std::cout << "Validating configuration..." << std::endl;
            // Add validation logic here
            std::cout << "Configuration is valid" << std::endl;
        }} else {{
            std::cerr << "Error: Unknown command '" << command << "'" << std::endl;
            printUsage(argv[0]);
            return 1;
        }}

        return 0;

    }} catch (const std::exception& e) {{
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }}
}}
)", options_.projectName, options_.projectName, options_.projectName, options_.projectName, options_.projectName, options_.projectName, options_.projectName, options_.projectName, options_.projectName);
}

std::string MultiExecutableTemplate::getSharedLibraryHeaderContent() {
  return fmt::format(R"(#pragma once

#include <string>
#include <memory>

namespace {} {{

class Core {{
public:
    Core();
    ~Core();

    bool initialize();
    void shutdown();

    std::string processData(const std::string& input);
    bool isInitialized() const;

private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
}};

// Utility functions
std::string getVersion();
bool isDebugBuild();

}} // namespace {}
)", options_.projectName, options_.projectName);
}

std::string MultiExecutableTemplate::getSharedLibrarySourceContent() {
  return fmt::format(R"DELIM(#include "{}_lib.h"
#include "utils.h"
#include "version.h"
#include <iostream>
#include <sstream>

namespace {} {{

class Core::Impl {{
public:
    bool initialized = false;

    bool initialize() {{
        if (initialized) {{
            return true;
        }}

        // Initialize core components
        std::cout << "Initializing {} core library..." << std::endl;
        initialized = true;
        return true;
    }}

    void shutdown() {{
        if (initialized) {{
            std::cout << "Shutting down {} core library..." << std::endl;
            initialized = false;
        }}
    }}

    std::string processData(const std::string& input) {{
        if (!initialized) {{
            return "Error: Core not initialized";
        }}

        std::ostringstream result;
        result << "Processed: " << input << " (length: " << input.length() << ")";
        return result.str();
    }}
}};

Core::Core() : pImpl(std::make_unique<Impl>()) {{}}

Core::~Core() {{
    if (pImpl) {{
        pImpl->shutdown();
    }}
}}

bool Core::initialize() {{
    return pImpl ? pImpl->initialize() : false;
}}

void Core::shutdown() {{
    if (pImpl) {{
        pImpl->shutdown();
    }}
}}

std::string Core::processData(const std::string& input) {{
    return pImpl ? pImpl->processData(input) : "Error: Invalid core instance";
}}

bool Core::isInitialized() const {{
    return pImpl ? pImpl->initialized : false;
}}

std::string getVersion() {{
    return VERSION_STRING;
}}

bool isDebugBuild() {{
#ifdef DEBUG
    return true;
#else
    return false;
#endif
}}

}} // namespace {{}}
)DELIM", options_.projectName, options_.projectName, options_.projectName, options_.projectName, options_.projectName);
}

std::string MultiExecutableTemplate::getUtilsHeaderContent() {
  return fmt::format(R"(#pragma once

#include <string>
#include <chrono>

namespace {}::utils {{

// Time utilities
std::string getCurrentTimestamp();
std::string formatTime(const std::chrono::system_clock::time_point& time);

// String utilities
std::string toUpperCase(const std::string& str);
std::string toLowerCase(const std::string& str);
bool startsWith(const std::string& str, const std::string& prefix);
bool endsWith(const std::string& str, const std::string& suffix);

// File utilities
bool fileExists(const std::string& path);
std::string getFileExtension(const std::string& path);

}} // namespace {}::utils
)", options_.projectName, options_.projectName);
}

std::string MultiExecutableTemplate::getUtilsSourceContent() {
  return fmt::format(R"(#include "utils.h"
#include <iomanip>
#include <sstream>
#include <filesystem>
#include <algorithm>
#include <cctype>

namespace {}::utils {{

std::string getCurrentTimestamp() {{
    auto now = std::chrono::system_clock::now();
    return formatTime(now);
}}

std::string formatTime(const std::chrono::system_clock::time_point& time) {{
    auto time_t = std::chrono::system_clock::to_time_t(time);
    std::ostringstream oss;
    oss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
    return oss.str();
}}

std::string toUpperCase(const std::string& str) {{
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(),
                   [](unsigned char c) {{ return std::toupper(c); }});
    return result;
}}

std::string toLowerCase(const std::string& str) {{
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(),
                   [](unsigned char c) {{ return std::tolower(c); }});
    return result;
}}

bool startsWith(const std::string& str, const std::string& prefix) {{
    return str.length() >= prefix.length() &&
           str.compare(0, prefix.length(), prefix) == 0;
}}

bool endsWith(const std::string& str, const std::string& suffix) {{
    return str.length() >= suffix.length() &&
           str.compare(str.length() - suffix.length(), suffix.length(), suffix) == 0;
}}

bool fileExists(const std::string& path) {{
    return std::filesystem::exists(path);
}}

std::string getFileExtension(const std::string& path) {{
    return std::filesystem::path(path).extension().string();
}}

}} // namespace {}::utils
)", options_.projectName, options_.projectName);
}

std::string MultiExecutableTemplate::getVersionHeaderContent() {
  return fmt::format(R"(#pragma once

#define {}_VERSION_MAJOR 1
#define {}_VERSION_MINOR 0
#define {}_VERSION_PATCH 0

#define {}_VERSION_STRING "1.0.0"

// Build information
#ifdef DEBUG
#define {}_BUILD_TYPE "Debug"
#else
#define {}_BUILD_TYPE "Release"
#endif

#define VERSION_STRING {}_VERSION_STRING
)",
    StringUtils::toUpper(options_.projectName),
    StringUtils::toUpper(options_.projectName),
    StringUtils::toUpper(options_.projectName),
    StringUtils::toUpper(options_.projectName),
    StringUtils::toUpper(options_.projectName),
    StringUtils::toUpper(options_.projectName),
    StringUtils::toUpper(options_.projectName));
}

std::string MultiExecutableTemplate::getConfigHeaderContent() {
  return fmt::format(R"(#pragma once

// Configuration settings for {}
namespace {}::config {{

// Default settings
constexpr int DEFAULT_PORT = 8080;
constexpr const char* DEFAULT_HOST = "localhost";
constexpr int DEFAULT_TIMEOUT = 30;
constexpr int DEFAULT_MAX_CONNECTIONS = 100;

// Feature flags
constexpr bool ENABLE_LOGGING = true;
constexpr bool ENABLE_METRICS = true;
constexpr bool ENABLE_DEBUG_OUTPUT =
#ifdef DEBUG
    true;
#else
    false;
#endif

}} // namespace {}::config
)", options_.projectName, options_.projectName, options_.projectName);
}

std::string MultiExecutableTemplate::getCMakeContent() {
  return fmt::format(R"(cmake_minimum_required(VERSION 3.14)
project({} VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Compiler warnings
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wall -Wextra -Wpedantic)
elseif(MSVC)
    add_compile_options(/W4)
endif()

# Shared library
add_library({}_lib SHARED
    lib/src/{}_lib.cpp
    lib/src/utils.cpp
)

target_include_directories({}_lib PUBLIC
    lib/include
)

target_compile_definitions({}_lib PRIVATE
    VERSION_STRING="${{PROJECT_VERSION}}"
)

# Executables
set(EXECUTABLES main client server tool)

foreach(EXECUTABLE ${{EXECUTABLES}})
    add_executable({}_${{EXECUTABLE}}
        src/${{EXECUTABLE}}.cpp
    )

    target_link_libraries({}_${{EXECUTABLE}} PRIVATE {}_lib)

    target_include_directories({}_${{EXECUTABLE}} PRIVATE
        lib/include
    )

    set_target_properties({}_${{EXECUTABLE}} PROPERTIES
        OUTPUT_NAME {}_${{EXECUTABLE}}
        RUNTIME_OUTPUT_DIRECTORY ${{CMAKE_BINARY_DIR}}/bin
    )
endforeach()

# Install targets
install(TARGETS {}_lib
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
)

foreach(EXECUTABLE ${{EXECUTABLES}})
    install(TARGETS {}_${{EXECUTABLE}}
        RUNTIME DESTINATION bin
    )
endforeach()

install(DIRECTORY lib/include/
    DESTINATION include
    FILES_MATCHING PATTERN "*.h"
)
)", options_.projectName, options_.projectName, options_.projectName, options_.projectName, options_.projectName, options_.projectName, options_.projectName, options_.projectName, options_.projectName, options_.projectName, options_.projectName, options_.projectName, options_.projectName, options_.projectName);
}

std::string MultiExecutableTemplate::getVcpkgJsonContent() {
  return fmt::format(R"({{
  "name": "{}",
  "version": "1.0.0",
  "description": "Multi-executable C++ project",
  "dependencies": [
    "fmt",
    "spdlog"
  ],
  "features": {{
    "tests": {{
      "description": "Build tests",
      "dependencies": [
        "gtest"
      ]
    }}
  }}
}}
)", options_.projectName);
}

std::string MultiExecutableTemplate::getConanfileContent() {
  return fmt::format(R"([requires]
fmt/9.1.0
spdlog/1.12.0

[generators]
CMakeDeps
CMakeToolchain

[options]
shared=False

[imports]
bin, *.dll -> ./bin
bin, *.dylib* -> ./bin
lib, *.dylib* -> ./bin
)");
}

std::string MultiExecutableTemplate::getReadmeContent() {
  return fmt::format(R"(# {}

A multi-executable C++ project created with CPP-Scaffold.

## Overview

This project contains multiple executables that share a common library:

- **{}_main**: Main application executable
- **{}_client**: Client application for network communication
- **{}_server**: Server application for handling requests
- **{}_tool**: Command-line utility tool

## Building

### Prerequisites

- C++17 compatible compiler
- CMake 3.14 or higher
- Package manager (vcpkg or Conan)

### Build Instructions

```bash
# Create build directory
mkdir build && cd build

# Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build all executables
cmake --build .

# Install (optional)
cmake --install . --prefix /usr/local
```

### Using vcpkg

```bash
# Install dependencies
vcpkg install fmt spdlog

# Configure with vcpkg
cmake .. -DCMAKE_TOOLCHAIN_FILE=/path/to/vcpkg/scripts/buildsystems/vcpkg.cmake
```

### Using Conan

```bash
# Install dependencies
conan install . --build=missing

# Configure with Conan
cmake .. -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake
```

## Usage

### Main Application

```bash
./bin/{}_main [arguments]
```

### Client Application

```bash
# Connect to server
./bin/{}_client --connect --server localhost --port 8080

# Show help
./bin/{}_client --help
```

### Server Application

```bash
# Start server on default port (8080)
./bin/{}_server

# Start server on custom port
./bin/{}_server --port 9090

# Run as daemon
./bin/{}_server --daemon
```

### Command Line Tool

```bash
# Show project information
./bin/{}_tool info

# Process data
./bin/{}_tool process

# Validate configuration
./bin/{}_tool validate
```

## Project Structure

```
{}/
├── CMakeLists.txt          # Main build configuration
├── lib/                    # Shared library
│   ├── include/           # Public headers
│   │   ├── {}_lib.h      # Main library header
│   │   ├── utils.h        # Utility functions
│   │   ├── version.h      # Version information
│   │   └── config.h       # Configuration constants
│   └── src/               # Library implementation
│       ├── {}_lib.cpp    # Main library source
│       └── utils.cpp      # Utility implementations
├── src/                   # Executable sources
│   ├── main.cpp          # Main application
│   ├── client.cpp        # Client application
│   ├── server.cpp        # Server application
│   └── tool.cpp          # Command-line tool
├── bin/                   # Built executables (after build)
├── docs/                  # Documentation
└── scripts/               # Build and utility scripts
```

## Development

### Adding New Executables

1. Create a new `.cpp` file in the `src/` directory
2. Add the executable to the CMakeLists.txt `EXECUTABLES` list
3. Rebuild the project

### Extending the Shared Library

1. Add new headers to `lib/include/`
2. Add implementations to `lib/src/`
3. Update the CMakeLists.txt if needed

## License

This project is licensed under the MIT License - see the LICENSE file for details.
)",
    options_.projectName,
    options_.projectName, options_.projectName, options_.projectName, options_.projectName,
    options_.projectName, options_.projectName, options_.projectName, options_.projectName,
    options_.projectName, options_.projectName, options_.projectName, options_.projectName,
    options_.projectName, options_.projectName);
}

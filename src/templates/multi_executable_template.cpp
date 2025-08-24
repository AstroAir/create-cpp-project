#include "multi_executable_template.h"
#include "../utils/file_utils.h"
#include "../utils/string_utils.h"
#include "../utils/terminal_utils.h"
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
  
  if (enums::to_string(options_.buildSystem) == "cmake") {
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
  std::string buildSystem = std::string(enums::to_string(options_.buildSystem));

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
  std::string packageManager = std::string(enums::to_string(options_.packageManager));

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
  std::string testFramework = std::string(enums::to_string(options_.testFramework));

  // Create test CMakeLists.txt if using CMake
  if (enums::to_string(options_.buildSystem) == "cmake") {
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
  return fmt::format(R"(#include <iostream>
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
)", options_.projectName);
}

std::string MultiExecutableTemplate::getServerExecutableContent() {
  return fmt::format(R"(#include <iostream>
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
)", options_.projectName);
}

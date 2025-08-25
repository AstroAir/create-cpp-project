#include "network_template.h"

#include "../utils/file_utils.h"
#include "../utils/terminal_utils.h"

#include <fmt/core.h>
#include <spdlog/spdlog.h>

using namespace utils;

NetworkTemplate::NetworkTemplate(const CliOptions &options)
    : TemplateBase(options) {}

bool NetworkTemplate::create() {
  std::string projectPath = options_.projectName;

  // Check if project directory already exists
  if (FileUtils::directoryExists(projectPath)) {
    spdlog::error("Directory '{}' already exists.", projectPath);
    return false;
  }

  spdlog::info("🚀 Creating network application project '{}'...", projectPath);

  // Create basic structure
  spdlog::info("Creating project structure...");
  if (!createProjectStructure()) {
    spdlog::error("Failed to create project structure");
    return false;
  }
  spdlog::info("✅ Project structure created");

  // Create build system
  spdlog::info("Configuring build system...");
  if (!createBuildSystem()) {
    spdlog::error("Failed to configure build system");
    return false;
  }
  spdlog::info("✅ Build system configured");

  // Setup network library
  spdlog::info("Setting up network library...");
  if (!setupNetworkLibrary()) {
    spdlog::error("Failed to setup network library");
    return false;
  }
  spdlog::info("✅ Network library setup");

  // Setup package manager
  spdlog::info("Setting up package manager...");
  if (!setupPackageManager()) {
    spdlog::error("Failed to setup package manager");
    return false;
  }
  spdlog::info("✅ Package manager setup");

  // Setup test framework
  if (options_.includeTests) {
    spdlog::info("Configuring test framework...");
    if (!setupTestFramework()) {
      spdlog::error("Failed to setup test framework");
      return false;
    }
    spdlog::info("✅ Test framework configured");
  }

  // Setup code style tools
  if (options_.includeCodeStyleTools) {
    spdlog::info("Configuring code style tools...");
    if (!setupCodeStyleTools()) {
      spdlog::error("Failed to configure code style tools");
      return false;
    }
    spdlog::info("✅ Code style tools configured");
  }

  // Setup editor integrations
  if (!options_.editorOptions.empty()) {
    spdlog::info("Configuring editor integrations...");
    if (!setupEditorIntegrations()) {
      spdlog::error("Failed to configure editor integrations");
      return false;
    }
    spdlog::info("✅ Editor integrations configured");
  }

  // Setup CI/CD integrations
  if (!options_.ciOptions.empty()) {
    spdlog::info("Configuring CI/CD integrations...");
    if (!setupCICDIntegrations()) {
      spdlog::error("Failed to configure CI/CD integrations");
      return false;
    }
    spdlog::info("✅ CI/CD integrations configured");
  }

  // Initialize Git
  if (options_.initGit) {
    spdlog::info("Initializing Git repository...");
    if (!initializeGit(projectPath)) {
      spdlog::error("Failed to initialize Git repository");
      return false;
    }
    spdlog::info("✅ Git repository initialized");
  }

  // Display project creation completion information
  std::vector<std::string> infoLines = {
      fmt::format("Network application project '{}' created successfully!", options_.projectName),
      "",
      fmt::format("Build system: {}", enums::to_string(options_.buildSystem)),
      fmt::format("Package manager: {}", enums::to_string(options_.packageManager)),
      options_.includeTests ? fmt::format("Test framework: {}", enums::to_string(options_.testFramework))
                            : "No test framework included",
      "",
      "Getting started:"
  };

  if (enums::to_string(options_.buildSystem) == "cmake") {
    infoLines.push_back(fmt::format("cd {}", options_.projectName));
    infoLines.push_back("mkdir build && cd build");
    infoLines.push_back("cmake ..");
    infoLines.push_back("make");
  } else if (enums::to_string(options_.buildSystem) == "meson") {
    infoLines.push_back(fmt::format("cd {}", options_.projectName));
    infoLines.push_back("meson setup build");
    infoLines.push_back("cd build");
    infoLines.push_back("meson compile");
  } else if (enums::to_string(options_.buildSystem) == "bazel") {
    infoLines.push_back(fmt::format("cd {}", options_.projectName));
    infoLines.push_back("bazel build //...");
  } else if (enums::to_string(options_.buildSystem) == "xmake") {
    infoLines.push_back(fmt::format("cd {}", options_.projectName));
    infoLines.push_back("xmake");
  } else if (enums::to_string(options_.buildSystem) == "premake") {
    infoLines.push_back(fmt::format("cd {}", options_.projectName));
    infoLines.push_back("premake5 gmake2");
    infoLines.push_back("make");
  }

  utils::TerminalUtils::showBox(infoLines, utils::BorderStyle::Rounded);
  spdlog::info("Network application project '{}' creation completed", projectPath);

  return true;
}

bool NetworkTemplate::createProjectStructure() {
  std::string projectPath = options_.projectName;

  // Create main directory
  if (!FileUtils::createDirectory(projectPath)) {
    spdlog::error("Failed to create project main directory: {}", projectPath);
    return false;
  }

  // Create src directory
  std::string srcPath = FileUtils::combinePath(projectPath, "src");
  if (!FileUtils::createDirectory(srcPath)) {
    spdlog::error("Failed to create src directory: {}", srcPath);
    return false;
  }

  // Create include directory
  std::string includePath = FileUtils::combinePath(projectPath, "include");
  if (!FileUtils::createDirectory(includePath)) {
    spdlog::error("Failed to create include directory: {}", includePath);
    return false;
  }

  std::string includeProjectPath =
      FileUtils::combinePath(includePath, options_.projectName);
  if (!FileUtils::createDirectory(includeProjectPath)) {
    spdlog::error("Failed to create project include directory: {}", includeProjectPath);
    return false;
  }

  // Create network application specific directories
  std::string serverPath = FileUtils::combinePath(srcPath, "server");
  if (!FileUtils::createDirectory(serverPath)) {
    spdlog::error("Failed to create server directory: {}", serverPath);
    return false;
  }

  std::string clientPath = FileUtils::combinePath(srcPath, "client");
  if (!FileUtils::createDirectory(clientPath)) {
    spdlog::error("Failed to create client directory: {}", clientPath);
    return false;
  }

  std::string serverIncludePath =
      FileUtils::combinePath(includeProjectPath, "server");
  if (!FileUtils::createDirectory(serverIncludePath)) {
    spdlog::error("Failed to create server include directory: {}", serverIncludePath);
    return false;
  }

  std::string clientIncludePath =
      FileUtils::combinePath(includeProjectPath, "client");
  if (!FileUtils::createDirectory(clientIncludePath)) {
    spdlog::error("Failed to create client include directory: {}", clientIncludePath);
    return false;
  }

  // Write main.cpp
  spdlog::info("Generating main.cpp content...");
  std::string mainContent = getMainCppContent();
  spdlog::info("Main.cpp content generated successfully");

  if (!FileUtils::writeToFile(FileUtils::combinePath(srcPath, "main.cpp"), mainContent)) {
    spdlog::error("Failed to write main.cpp file");
    return false;
  }

  // Write server-related files
  spdlog::info("Generating server header content...");
  std::string serverHeaderContent = getServerHeaderContent();
  spdlog::info("Server header content generated successfully");

  if (!FileUtils::writeToFile(
          FileUtils::combinePath(serverIncludePath, "server.h"),
          serverHeaderContent)) {
    spdlog::error("Failed to write server.h file");
    return false;
  }

  spdlog::info("Generating server implementation content...");
  std::string serverImplContent = getServerImplContent();
  spdlog::info("Server implementation content generated successfully");

  if (!FileUtils::writeToFile(FileUtils::combinePath(serverPath, "server.cpp"),
                              serverImplContent)) {
    spdlog::error("Failed to write server.cpp file");
    return false;
  }

  // Write client-related files
  spdlog::info("Generating client header content...");
  std::string clientHeaderContent = getClientHeaderContent();
  spdlog::info("Client header content generated successfully");

  if (!FileUtils::writeToFile(
          FileUtils::combinePath(clientIncludePath, "client.h"),
          clientHeaderContent)) {
    spdlog::error("Failed to write client.h file");
    return false;
  }

  spdlog::info("Generating client implementation content...");
  std::string clientImplContent = getClientImplContent();
  spdlog::info("Client implementation content generated successfully");

  if (!FileUtils::writeToFile(FileUtils::combinePath(clientPath, "client.cpp"),
                              clientImplContent)) {
    spdlog::error("Failed to write client.cpp file");
    return false;
  }

  // Create README.md
  if (!FileUtils::writeToFile(FileUtils::combinePath(projectPath, "README.md"),
                              getReadmeContent())) {
    spdlog::error("Failed to write README.md file");
    return false;
  }

  spdlog::info("Network application project directory structure created successfully");
  return true;
}

bool NetworkTemplate::createBuildSystem() {
  std::string projectPath = options_.projectName;

  if (enums::to_string(options_.buildSystem) == "cmake") {
    // Create CMakeLists.txt
    if (!FileUtils::writeToFile(
            FileUtils::combinePath(projectPath, "CMakeLists.txt"),
            getCMakeContent())) {
      spdlog::error("Failed to write CMakeLists.txt file");
      return false;
    }
  } else if (enums::to_string(options_.buildSystem) == "meson") {
    // Create meson.build
    if (!FileUtils::writeToFile(
            FileUtils::combinePath(projectPath, "meson.build"),
            getMesonContent())) {
      spdlog::error("Failed to write meson.build file");
      return false;
    }
  } else if (enums::to_string(options_.buildSystem) == "bazel") {
    // Create WORKSPACE and BUILD files
    if (!FileUtils::writeToFile(
            FileUtils::combinePath(projectPath, "WORKSPACE"),
            fmt::format("workspace(name = \"{}\")\n", options_.projectName))) {
      spdlog::error("Failed to write WORKSPACE file");
      return false;
    }

    if (!FileUtils::writeToFile(FileUtils::combinePath(projectPath, "BUILD"),
                                getBazelContent())) {
      spdlog::error("Failed to write BUILD file");
      return false;
    }
  } else if (enums::to_string(options_.buildSystem) == "xmake") {
    // Create xmake.lua
    if (!FileUtils::writeToFile(
            FileUtils::combinePath(projectPath, "xmake.lua"),
            getXMakeContent())) {
      spdlog::error("Failed to write xmake.lua file");
      return false;
    }
  } else if (enums::to_string(options_.buildSystem) == "premake") {
    // Create premake5.lua
    if (!FileUtils::writeToFile(
            FileUtils::combinePath(projectPath, "premake5.lua"),
            getPremakeContent())) {
      spdlog::error("Failed to write premake5.lua file");
      return false;
    }
  } else {
    spdlog::error("Unsupported build system: {}", options_.buildSystem);
    return false;
  }

  spdlog::info("Build system ({}) configured successfully", options_.buildSystem);
  return true;
}

bool NetworkTemplate::setupNetworkLibrary() {
  std::string projectPath = options_.projectName;

  // Add necessary dependencies based on selected network library
  if (options_.networkLibrary == "asio") {
    // Configure Asio library
    if (enums::to_string(options_.packageManager) == "vcpkg") {
      // Modify vcpkg.json to add asio dependency
      std::string vcpkgJsonPath =
          FileUtils::combinePath(projectPath, "vcpkg.json");
      std::string vcpkgJson = FileUtils::readFromFile(vcpkgJsonPath);
      // Add asio dependency
      size_t pos = vcpkgJson.find("\"dependencies\": [");
      if (pos != std::string::npos) {
        pos = vcpkgJson.find("[", pos);
        if (pos != std::string::npos) {
          std::string asioEntry = "    {\n      \"name\": \"asio\"\n    },\n";
          vcpkgJson.insert(pos + 1, asioEntry);
          if (!FileUtils::writeToFile(vcpkgJsonPath, vcpkgJson)) {
            spdlog::error("Failed to update vcpkg.json to add asio dependency");
            return false;
          }
        }
      }
    } else if (enums::to_string(options_.packageManager) == "conan") {
      // Modify conanfile.txt to add asio dependency
      std::string conanfilePath =
          FileUtils::combinePath(projectPath, "conanfile.txt");
      std::string conanfile = FileUtils::readFromFile(conanfilePath);
      // Add asio dependency
      size_t pos = conanfile.find("[requires]");
      if (pos != std::string::npos) {
        pos = conanfile.find("\n", pos);
        if (pos != std::string::npos) {
          conanfile.insert(pos, "\nasio/1.24.0");
          if (!FileUtils::writeToFile(conanfilePath, conanfile)) {
            spdlog::error("Failed to update conanfile.txt to add asio dependency");
            return false;
          }
        }
      }
    }
  } else if (options_.networkLibrary == "boost") {
    // Configure Boost library
    if (enums::to_string(options_.packageManager) == "vcpkg") {
      // Modify vcpkg.json to add boost dependency
      std::string vcpkgJsonPath =
          FileUtils::combinePath(projectPath, "vcpkg.json");
      std::string vcpkgJson = FileUtils::readFromFile(vcpkgJsonPath);
      // Add boost dependency
      size_t pos = vcpkgJson.find("\"dependencies\": [");
      if (pos != std::string::npos) {
        pos = vcpkgJson.find("[", pos);
        if (pos != std::string::npos) {
          std::string boostEntry =
              "    {\n      \"name\": \"boost\",\n      \"features\": [\"asio\"]\n    },\n";
          vcpkgJson.insert(pos + 1, boostEntry);
          if (!FileUtils::writeToFile(vcpkgJsonPath, vcpkgJson)) {
            spdlog::error("Failed to update vcpkg.json to add boost dependency");
            return false;
          }
        }
      }
    } else if (enums::to_string(options_.packageManager) == "conan") {
      // Modify conanfile.txt to add boost dependency
      std::string conanfilePath =
          FileUtils::combinePath(projectPath, "conanfile.txt");
      std::string conanfile = FileUtils::readFromFile(conanfilePath);
      // Add boost dependency
      size_t pos = conanfile.find("[requires]");
      if (pos != std::string::npos) {
        pos = conanfile.find("\n", pos);
        if (pos != std::string::npos) {
          conanfile.insert(pos, "\nboost/1.80.0");
          if (!FileUtils::writeToFile(conanfilePath, conanfile)) {
            spdlog::error("Failed to update conanfile.txt to add boost dependency");
            return false;
          }
        }
      }
    }
  } else if (options_.networkLibrary == "poco") {
    // Configure POCO library
    if (enums::to_string(options_.packageManager) == "vcpkg") {
      // Modify vcpkg.json to add poco dependency
      std::string vcpkgJsonPath =
          FileUtils::combinePath(projectPath, "vcpkg.json");
      std::string vcpkgJson = FileUtils::readFromFile(vcpkgJsonPath);
      // Add poco dependency
      size_t pos = vcpkgJson.find("\"dependencies\": [");
      if (pos != std::string::npos) {
        pos = vcpkgJson.find("[", pos);
        if (pos != std::string::npos) {
          std::string pocoEntry =
              "    {\n      \"name\": \"poco\",\n      \"features\": [\"netssl\"]\n    },\n";
          vcpkgJson.insert(pos + 1, pocoEntry);
          if (!FileUtils::writeToFile(vcpkgJsonPath, vcpkgJson)) {
            spdlog::error("Failed to update vcpkg.json to add poco dependency");
            return false;
          }
        }
      }
    } else if (enums::to_string(options_.packageManager) == "conan") {
      // Modify conanfile.txt to add poco dependency
      std::string conanfilePath =
          FileUtils::combinePath(projectPath, "conanfile.txt");
      std::string conanfile = FileUtils::readFromFile(conanfilePath);
      // Add poco dependency
      size_t pos = conanfile.find("[requires]");
      if (pos != std::string::npos) {
        pos = conanfile.find("\n", pos);
        if (pos != std::string::npos) {
          conanfile.insert(pos, "\npoco/1.12.2");
          if (!FileUtils::writeToFile(conanfilePath, conanfile)) {
            spdlog::error("Failed to update conanfile.txt to add poco dependency");
            return false;
          }
        }
      }
    }
  }

  spdlog::info("Network library ({}) configured successfully", options_.networkLibrary.value_or("Unknown"));
  return true;
}

bool NetworkTemplate::setupPackageManager() {
  std::string projectPath = options_.projectName;

  if (enums::to_string(options_.packageManager) == "vcpkg") {
    // Create base vcpkg.json file
    std::string vcpkgJson = fmt::format(R"({
  "name": "{}",
  "version": "0.1.0",
  "dependencies": [
  ]
}
)", options_.projectName);
    if (!FileUtils::writeToFile(
            FileUtils::combinePath(projectPath, "vcpkg.json"), vcpkgJson)) {
      spdlog::error("Failed to write vcpkg.json file");
      return false;
    }
  } else if (enums::to_string(options_.packageManager) == "conan") {
    // Create conanfile.txt
    std::string generator = (enums::to_string(options_.buildSystem) == "cmake" ? "cmake" : "");
    std::string conanfile = fmt::format(R"([requires]

[generators]
{})", generator);

    if (!FileUtils::writeToFile(
            FileUtils::combinePath(projectPath, "conanfile.txt"), conanfile)) {
      spdlog::error("Failed to write conanfile.txt file");
      return false;
    }
  } else if (enums::to_string(options_.packageManager) != "none") {
    spdlog::error("Unsupported package manager: {}", enums::to_string(options_.packageManager));
    return false;
  }

  spdlog::info("Package manager ({}) configured successfully", enums::to_string(options_.packageManager));
  return true;
}

bool NetworkTemplate::setupTestFramework() {
  if (!options_.includeTests) {
    return true;
  }

  std::string projectPath = options_.projectName;
  std::string testsPath = FileUtils::combinePath(projectPath, "tests");

  if (!FileUtils::createDirectory(testsPath)) {
    spdlog::error("Failed to create tests directory: {}", testsPath);
    return false;
  }

  // Write network test file
  if (!FileUtils::writeToFile(
          FileUtils::combinePath(testsPath, "network_test.cpp"),
          getNetworkTestContent())) {
    spdlog::error("Failed to write network_test.cpp file");
    return false;
  }

  // Update build system to include tests
  if (enums::to_string(options_.buildSystem) == "cmake") {
    std::string cmakePath =
        FileUtils::combinePath(projectPath, "CMakeLists.txt");
    std::string cmakeContent = FileUtils::readFromFile(cmakePath);

    // Add test configuration
    std::string testConfig = R"(
# Tests
if(BUILD_TESTING)
    enable_testing()
    add_subdirectory(tests)
endif()
)";
    cmakeContent = fmt::format("{}{}", cmakeContent, testConfig);

    if (!FileUtils::writeToFile(cmakePath, cmakeContent)) {
      spdlog::error("Failed to update CMakeLists.txt to add test configuration");
      return false;
    }

    // Create tests/CMakeLists.txt
    std::string testCmakeContent;
    if (enums::to_string(options_.testFramework) == "gtest") {
      testCmakeContent = R"(
find_package(GTest REQUIRED)
add_executable(${PROJECT_NAME}_tests network_test.cpp)
target_link_libraries(${PROJECT_NAME}_tests PRIVATE
    ${PROJECT_NAME}_lib
    GTest::GTest
    GTest::Main
)
add_test(NAME ${PROJECT_NAME}_tests COMMAND ${PROJECT_NAME}_tests)
)";
    } else if (enums::to_string(options_.testFramework) == "catch2") {
      testCmakeContent = R"(
find_package(Catch2 REQUIRED)
add_executable(${PROJECT_NAME}_tests network_test.cpp)
target_link_libraries(${PROJECT_NAME}_tests PRIVATE
    ${PROJECT_NAME}_lib
    Catch2::Catch2
)
add_test(NAME ${PROJECT_NAME}_tests COMMAND ${PROJECT_NAME}_tests)
)";
    } else if (enums::to_string(options_.testFramework) == "doctest") {
      testCmakeContent = R"(
find_package(doctest REQUIRED)
add_executable(${PROJECT_NAME}_tests network_test.cpp)
target_link_libraries(${PROJECT_NAME}_tests PRIVATE
    ${PROJECT_NAME}_lib
    doctest::doctest
)
add_test(NAME ${PROJECT_NAME}_tests COMMAND ${PROJECT_NAME}_tests)
)";
    }

    if (!FileUtils::writeToFile(
            FileUtils::combinePath(testsPath, "CMakeLists.txt"),
            testCmakeContent)) {
      spdlog::error("Failed to write tests/CMakeLists.txt file");
      return false;
    }
  }

  spdlog::info("Test framework ({}) configured successfully", enums::to_string(options_.testFramework));
  return true;
}

bool NetworkTemplate::setupCodeStyleTools() {
  std::string projectPath = options_.projectName;

  // 使用基类提供的方法设置代码风格
  bool result = TemplateBase::setupCodeStyleConfig(projectPath);

  // 如果基类实现不满足特定需求，可以在这里添加网络模板特有的代码风格配置

  return result;
}

bool NetworkTemplate::setupEditorIntegrations() {
  std::string projectPath = options_.projectName;

  // 使用基类提供的方法设置编辑器配置
  bool result = TemplateBase::setupEditorConfig(projectPath);

  // 如果基类实现不满足特定需求，可以在这里添加网络模板特有的编辑器配置

  return result;
}

bool NetworkTemplate::setupCICDIntegrations() {
  std::string projectPath = options_.projectName;

  // 使用基类提供的方法设置CI/CD配置
  bool result = TemplateBase::setupCICD(projectPath);

  // 如果基类实现不满足特定需求，可以在这里添加网络模板特有的CI/CD配置

  return result;
}

std::string NetworkTemplate::getMainCppContent() {
  std::string networkInclude;

  if (options_.networkLibrary == "asio") {
    networkInclude = R"(#include <asio.hpp>)";
  } else if (options_.networkLibrary == "boost") {
    networkInclude = R"(#include <boost/asio.hpp>)";
  } else if (options_.networkLibrary == "poco") {
    networkInclude = R"(#include <Poco/Net/SocketAddress.h>
#include <Poco/Net/ServerSocket.h>)";
  }

  return fmt::format(R"(#include <iostream>
#include <string>
#include <thread>
{}
#include "{}/server/server.h"
#include "{}/client/client.h"

int main(int argc, char* argv[]) {{
    std::cout << "Network Application: {}" << std::endl;

    if (argc < 2) {{
        std::cout << "Usage: " << argv[0] << " [server|client]" << std::endl;
        return 1;
    }}

    std::string mode = argv[1];

    try {{
        if (mode == "server") {{
            std::cout << "Starting server..." << std::endl;
            {}::Server server;
            server.start(8080);
        }} else if (mode == "client") {{
            std::cout << "Starting client..." << std::endl;
            {}::Client client;
            client.connect("localhost", 8080);
            client.sendMessage("Hello from client!");
        }} else {{
            std::cout << "Unknown mode: " << mode << std::endl;
            return 1;
        }}
    }} catch (const std::exception& e) {{
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }}

    return 0;
}}
)", networkInclude,
     options_.projectName,
     options_.projectName,
     options_.projectName,
     options_.projectName,
     options_.projectName);
}

std::string NetworkTemplate::getServerHeaderContent() {
  std::string networkLibraryIncludes;
  std::string networkLibraryMembers;

  if (options_.networkLibrary == "asio") {
    networkLibraryIncludes = R"(#include <asio.hpp>)";
    networkLibraryMembers = R"(
        asio::io_context io_context_;
        asio::ip::tcp::acceptor acceptor_;
        std::vector<std::shared_ptr<asio::ip::tcp::socket>> clients_;
        std::thread worker_thread_;
        bool running_ = false;

        void accept_connections();
        void handle_client(std::shared_ptr<asio::ip::tcp::socket> client_socket);)";
  } else if (options_.networkLibrary == "boost") {
    networkLibraryIncludes = R"(#include <boost/asio.hpp>
namespace asio = boost::asio;)";
    networkLibraryMembers = R"(
        asio::io_context io_context_;
        asio::ip::tcp::acceptor acceptor_;
        std::vector<std::shared_ptr<asio::ip::tcp::socket>> clients_;
        std::thread worker_thread_;
        bool running_ = false;

        void accept_connections();
        void handle_client(std::shared_ptr<asio::ip::tcp::socket> client_socket);)";
  } else if (options_.networkLibrary == "poco") {
    networkLibraryIncludes = R"(#include <Poco/Net/ServerSocket.h>
#include <Poco/Net/SocketReactor.h>
#include <Poco/Net/SocketAcceptor.h>
#include <Poco/Net/SocketNotification.h>)";
    networkLibraryMembers = R"(
        Poco::Net::ServerSocket server_socket_;
        Poco::Net::SocketReactor reactor_;
        std::unique_ptr<Poco::Net::SocketAcceptor<Server>> acceptor_;
        std::thread reactor_thread_;
        bool running_ = false;

        void onClientConnection(const Poco::AutoPtr<Poco::Net::ReadableNotification>& notification);
        void onClientDisconnection(const Poco::AutoPtr<Poco::Net::ShutdownNotification>& notification);)";
  }

  return fmt::format(R"(#pragma once

#include <string>
#include <vector>
#include <memory>
#include <thread>
#include <functional>
{}

namespace {} {{

class Server {{
public:
    Server();
    ~Server();

    // Start the server on the specified port
    void start(int port);

    // Stop the server
    void stop();

    // Send message to all connected clients
    void broadcast(const std::string& message);

    // Set callback for new messages
    void setMessageCallback(std::function<void(const std::string&)> callback);

private:{}

    std::function<void(const std::string&)> message_callback_;
}};

}} // namespace {})",
    networkLibraryIncludes,
    options_.projectName,
    networkLibraryMembers,
    options_.projectName);
}

std::string NetworkTemplate::getServerImplContent() {
  std::string serverImpl;

  if (options_.networkLibrary == "asio") {
    serverImpl = fmt::format(R"(#include "{}/server/server.h"
#include <iostream>
#include <spdlog/spdlog.h>

namespace {} {{

Server::Server() : io_context_(), acceptor_(io_context_) {{
    message_callback_ = [](const std::string& msg) {{
        spdlog::info("Message received: {{}}", msg);
    }};
}}

Server::~Server() {{
    stop();
}}

void Server::start(int port) {{
    try {{
        acceptor_.open(asio::ip::tcp::v4());
        acceptor_.set_option(asio::ip::tcp::acceptor::reuse_address(true));
        acceptor_.bind(asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port));
        acceptor_.listen();

        running_ = true;
        worker_thread_ = std::thread([this]() {{
            accept_connections();
            io_context_.run();
        }});

        spdlog::info("Server started on port {{}}", port);
    }} catch (const std::exception& e) {{
        spdlog::error("Server start error: {{}}", e.what());
        throw;
    }}
}}

void Server::stop() {{
    if (!running_) return;

    running_ = false;
    io_context_.stop();

    if (worker_thread_.joinable()) {{
        worker_thread_.join();
    }}

    clients_.clear();
    spdlog::info("Server stopped");
}}

void Server::broadcast(const std::string& message) {{
    for (auto& client : clients_) {{
        try {{
            asio::write(*client, asio::buffer(message + "\n"));
        }} catch (const std::exception& e) {{
            spdlog::error("Error broadcasting message: {{}}", e.what());
        }}
    }}
}}

void Server::setMessageCallback(std::function<void(const std::string&)> callback) {{
    message_callback_ = std::move(callback);
}}

void Server::accept_connections() {{
    if (!running_) return;

    auto client_socket = std::make_shared<asio::ip::tcp::socket>(io_context_);
    acceptor_.async_accept(*client_socket, [this, client_socket](const asio::error_code& error) {{
        if (!error) {{
            spdlog::info("New client connected");
            clients_.push_back(client_socket);
            handle_client(client_socket);
        }}

        // Continue accepting connections
        accept_connections();
    }});
}}

void Server::handle_client(std::shared_ptr<asio::ip::tcp::socket> client_socket) {{
    auto buffer = std::make_shared<asio::streambuf>();

    asio::async_read_until(*client_socket, *buffer, '\n',
        [this, client_socket, buffer](const asio::error_code& error, std::size_t bytes_transferred) {{
            if (!error) {{
                std::string message(asio::buffers_begin(buffer->data()),
                                   asio::buffers_begin(buffer->data()) + bytes_transferred);
                message.pop_back(); // Remove the newline

                if (message_callback_) {{
                    message_callback_(message);
                }}

                buffer->consume(bytes_transferred);

                // Continue reading from this client
                handle_client(client_socket);
            }} else {{
                // Error or client disconnected
                auto it = std::find(clients_.begin(), clients_.end(), client_socket);
                if (it != std::string::npos) {{
                    clients_.erase(it);
                    spdlog::info("Client disconnected");
                }}
            }}
        }});
}}

}} // namespace {})",
    options_.projectName,
    options_.projectName,
    options_.projectName);
  } else if (options_.networkLibrary == "boost") {
    serverImpl = fmt::format(R"(#include "{}/server/server.h"
#include <iostream>
#include <spdlog/spdlog.h>

namespace {} {{

Server::Server() : io_context_(), acceptor_(io_context_) {{
    message_callback_ = [](const std::string& msg) {{
        spdlog::info("Message received: {{}}", msg);
    }};
}}

Server::~Server() {{
    stop();
}}

void Server::start(int port) {{
    try {{
        acceptor_.open(asio::ip::tcp::v4());
        acceptor_.set_option(asio::ip::tcp::acceptor::reuse_address(true));
        acceptor_.bind(asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port));
        acceptor_.listen();

        running_ = true;
        worker_thread_ = std::thread([this]() {{
            accept_connections();
            io_context_.run();
        }});

        spdlog::info("Server started on port {{}}", port);
    }} catch (const std::exception& e) {{
        spdlog::error("Server start error: {{}}", e.what());
        throw;
    }}
}}

void Server::stop() {{
    if (!running_) return;

    running_ = false;
    io_context_.stop();

    if (worker_thread_.joinable()) {{
        worker_thread_.join();
    }}

    clients_.clear();
    spdlog::info("Server stopped");
}}

void Server::broadcast(const std::string& message) {{
    for (auto& client : clients_) {{
        try {{
            asio::write(*client, asio::buffer(message + "\n"));
        }} catch (const std::exception& e) {{
            spdlog::error("Error broadcasting message: {{}}", e.what());
        }}
    }}
}}

void Server::setMessageCallback(std::function<void(const std::string&)> callback) {{
    message_callback_ = std::move(callback);
}}

void Server::accept_connections() {{
    if (!running_) return;

    auto client_socket = std::make_shared<asio::ip::tcp::socket>(io_context_);
    acceptor_.async_accept(*client_socket, [this, client_socket](const boost::system::error_code& error) {{
        if (!error) {{
            spdlog::info("New client connected");
            clients_.push_back(client_socket);
            handle_client(client_socket);
        }}

        // Continue accepting connections
        accept_connections();
    }});
}}

void Server::handle_client(std::shared_ptr<asio::ip::tcp::socket> client_socket) {{
    auto buffer = std::make_shared<boost::asio::streambuf>();

    asio::async_read_until(*client_socket, *buffer, '\n',
        [this, client_socket, buffer](const boost::system::error_code& error, std::size_t bytes_transferred) {{
            if (!error) {{
                std::string message(boost::asio::buffers_begin(buffer->data()),
                                    boost::asio::buffers_begin(buffer->data()) + bytes_transferred);
                message.pop_back(); // Remove the newline

                if (message_callback_) {{
                    message_callback_(message);
                }}

                buffer->consume(bytes_transferred);

                // Continue reading from this client
                handle_client(client_socket);
            }} else {{
                // Error or client disconnected
                auto it = std::find(clients_.begin(), clients_.end(), client_socket);
                if (it != std::string::npos) {{
                    clients_.erase(it);
                    spdlog::info("Client disconnected");
                }}
            }}
        }});
}}

}} // namespace {})",
    options_.projectName,
    options_.projectName,
    options_.projectName);
  } else if (options_.networkLibrary == "poco") {
    serverImpl = fmt::format(R"(#include "{}/server/server.h"
#include <Poco/Net/SocketStream.h>
#include <Poco/Net/StreamSocket.h>
#include <iostream>
#include <spdlog/spdlog.h>

namespace {} {{

Server::Server() {{
    message_callback_ = [](const std::string& msg) {{
        spdlog::info("Message received: {{}}", msg);
    }};
}}

Server::~Server() {{
    stop();
}}

void Server::start(int port) {{
    try {{
        // Set up server socket
        Poco::Net::SocketAddress address("0.0.0.0", port);
        server_socket_.bind(address);
        server_socket_.listen();

        // Set up acceptor
        acceptor_ = std::make_unique<Poco::Net::SocketAcceptor<Server>>(
            *this, server_socket_, reactor_);

        running_ = true;
        reactor_thread_ = std::thread([this]() {{
            reactor_.run();
        }});

        spdlog::info("Server started on port {{}}", port);
    }} catch (const Poco::Exception& e) {{
        spdlog::error("Server start error: {{}}", e.displayText());
        throw std::runtime_error(e.displayText());
    }}
}}

void Server::stop() {{
    if (!running_) return;

    running_ = false;
    reactor_.stop();

    if (reactor_thread_.joinable()) {{
        reactor_thread_.join();
    }}

    server_socket_.close();
    spdlog::info("Server stopped");
}}

void Server::broadcast(const std::string& message) {{
    // Implementation depends on how you store client connections
    // This is a simplified example
    spdlog::info("Broadcasting: {{}}", message);
}}

void Server::setMessageCallback(std::function<void(const std::string&)> callback) {{
    message_callback_ = std::move(callback);
}}

void Server::onClientConnection(const Poco::AutoPtr<Poco::Net::ReadableNotification>& notification) {{
    Poco::Net::StreamSocket& socket = *static_cast<Poco::Net::StreamSocket*>(notification->socket());
    try {{
        Poco::Net::SocketStream stream(socket);
        std::string message;
        std::getline(stream, message);

        if (message_callback_) {{
            message_callback_(message);
        }}
    }} catch (const Poco::Exception& e) {{
        spdlog::error("Error handling client: {{}}", e.displayText());
    }}
}}

void Server::onClientDisconnection(const Poco::AutoPtr<Poco::Net::ShutdownNotification>& notification) {{
    spdlog::info("Client disconnected");
}}

}} // namespace {})",
    options_.projectName,
    options_.projectName,
    options_.projectName);
  }

  return serverImpl;
}

std::string NetworkTemplate::getClientHeaderContent() {
  std::string networkLibraryIncludes;
  std::string networkLibraryMembers;

  if (options_.networkLibrary == "asio") {
    networkLibraryIncludes = R"(#include <asio.hpp>)";
    networkLibraryMembers = R"(
        asio::io_context io_context_;
        std::shared_ptr<asio::ip::tcp::socket> socket_;
        std::thread worker_thread_;
        bool connected_ = false;

        void read_messages();)";
  } else if (options_.networkLibrary == "boost") {
    networkLibraryIncludes = R"(#include <boost/asio.hpp>
namespace asio = boost::asio;)";
    networkLibraryMembers = R"(
        asio::io_context io_context_;
        std::shared_ptr<asio::ip::tcp::socket> socket_;
        std::thread worker_thread_;
        bool connected_ = false;

        void read_messages();)";
  } else if (options_.networkLibrary == "poco") {
    networkLibraryIncludes = R"(#include <Poco/Net/StreamSocket.h>
#include <Poco/Net/SocketAddress.h>)";
    networkLibraryMembers = R"(
        Poco::Net::StreamSocket socket_;
        std::thread reader_thread_;
        bool connected_ = false;

        void read_messages();)";
  }

  return fmt::format(R"(#pragma once

#include <string>
#include <memory>
#include <thread>
#include <functional>
{}

namespace {} {{

class Client {{
public:
    Client();
    ~Client();

    // Connect to a server
    void connect(const std::string& host, int port);

    // Disconnect from the server
    void disconnect();

    // Send a message to the server
    void sendMessage(const std::string& message);

    // Set callback for received messages
    void setMessageCallback(std::function<void(const std::string&)> callback);

    // Check if connected to the server
    bool isConnected() const;

private:{}

    std::function<void(const std::string&)> message_callback_;
}};

}} // namespace {})",
    networkLibraryIncludes,
    options_.projectName,
    networkLibraryMembers,
    options_.projectName);
}

std::string NetworkTemplate::getClientImplContent() {
  std::string clientImpl;

  if (options_.networkLibrary == "asio") {
    clientImpl = fmt::format(R"(#include "{}/client/client.h"
#include <iostream>
#include <spdlog/spdlog.h>

namespace {} {{

Client::Client() : io_context_() {{
    message_callback_ = [](const std::string& msg) {{
        spdlog::info("Message from server: {{}}", msg);
    }};
}}

Client::~Client() {{
    disconnect();
}}

void Client::connect(const std::string& host, int port) {{
    try {{
        socket_ = std::make_shared<asio::ip::tcp::socket>(io_context_);
        asio::ip::tcp::resolver resolver(io_context_);
        auto endpoints = resolver.resolve(host, std::to_string(port));

        asio::connect(*socket_, endpoints);
        connected_ = true;

        // Start worker thread for handling incoming messages
        worker_thread_ = std::thread([this]() {{
            read_messages();
            io_context_.run();
        }});

        spdlog::info("Connected to server {{}}:{{}}", host, port);
    }} catch (const std::exception& e) {{
        spdlog::error("Connection error: {{}}", e.what());
        throw;
    }}
}}

void Client::disconnect() {{
    if (!connected_) return;

    connected_ = false;

    if (socket_ && socket_->is_open()) {{
        socket_->close();
    }}

    io_context_.stop();

    if (worker_thread_.joinable()) {{
        worker_thread_.join();
    }}

    spdlog::info("Disconnected from server");
}}

void Client::sendMessage(const std::string& message) {{
    if (!connected_ || !socket_) {{
        throw std::runtime_error("Not connected to server");
    }}

    try {{
        asio::write(*socket_, asio::buffer(message + "\n"));
        spdlog::info("Message sent: {{}}", message);
    }} catch (const std::exception& e) {{
        spdlog::error("Error sending message: {{}}", e.what());
        throw;
    }}
}}

void Client::setMessageCallback(std::function<void(const std::string&)> callback) {{
    message_callback_ = std::move(callback);
}}

bool Client::isConnected() const {{
    return connected_ && socket_ && socket_->is_open();
}}

void Client::read_messages() {{
    if (!connected_ || !socket_) return;

    auto buffer = std::make_shared<asio::streambuf>();

    asio::async_read_until(*socket_, *buffer, '\n',
        [this, buffer](const asio::error_code& error, std::size_t bytes_transferred) {{
            if (!error) {{
                std::string message(asio::buffers_begin(buffer->data()),
                                   asio::buffers_begin(buffer->data()) + bytes_transferred);
                message.pop_back(); // Remove the newline

                if (message_callback_) {{
                    message_callback_(message);
                }}

                buffer->consume(bytes_transferred);

                // Continue reading messages
                read_messages();
            }} else {{
                spdlog::error("Error reading from server: {{}}", error.message());
                disconnect();
            }}
        }});
}}

}} // namespace {})",
    options_.projectName,
    options_.projectName,
    options_.projectName);
  } else if (options_.networkLibrary == "boost") {
    clientImpl = fmt::format(R"(#include "{}/client/client.h"
#include <iostream>
#include <spdlog/spdlog.h>

namespace {} {{

Client::Client() : io_context_() {{
    message_callback_ = [](const std::string& msg) {{
        spdlog::info("Message from server: {{}}", msg);
    }};
}}

Client::~Client() {{
    disconnect();
}}

void Client::connect(const std::string& host, int port) {{
    try {{
        socket_ = std::make_shared<asio::ip::tcp::socket>(io_context_);
        asio::ip::tcp::resolver resolver(io_context_);
        auto endpoints = resolver.resolve(host, std::to_string(port));

        asio::connect(*socket_, endpoints);
        connected_ = true;

        // Start worker thread for handling incoming messages
        worker_thread_ = std::thread([this]() {{
            read_messages();
            io_context_.run();
        }});

        spdlog::info("Connected to server {{}}:{{}}", host, port);
    }} catch (const std::exception& e) {{
        spdlog::error("Connection error: {{}}", e.what());
        throw;
    }}
}}

void Client::disconnect() {{
    if (!connected_) return;

    connected_ = false;

    if (socket_ && socket_->is_open()) {{
        socket_->close();
    }}

    io_context_.stop();

    if (worker_thread_.joinable()) {{
        worker_thread_.join();
    }}

    spdlog::info("Disconnected from server");
}}

void Client::sendMessage(const std::string& message) {{
    if (!connected_ || !socket_) {{
        throw std::runtime_error("Not connected to server");
    }}

    try {{
        asio::write(*socket_, asio::buffer(message + "\n"));
        spdlog::info("Message sent: {{}}", message);
    }} catch (const std::exception& e) {{
        spdlog::error("Error sending message: {{}}", e.what());
        throw;
    }}
}}

void Client::setMessageCallback(std::function<void(const std::string&)> callback) {{
    message_callback_ = std::move(callback);
}}

bool Client::isConnected() const {{
    return connected_ && socket_ && socket_->is_open();
}}

void Client::read_messages() {{
    if (!connected_ || !socket_) return;

    auto buffer = std::make_shared<boost::asio::streambuf>();

    asio::async_read_until(*socket_, *buffer, '\n',
        [this, buffer](const boost::system::error_code& error, std::size_t bytes_transferred) {{
            if (!error) {{
                std::string message(boost::asio::buffers_begin(buffer->data()),
                                    boost::asio::buffers_begin(buffer->data()) + bytes_transferred);
                message.pop_back(); // Remove the newline

                if (message_callback_) {{
                    message_callback_(message);
                }}

                buffer->consume(bytes_transferred);

                // Continue reading messages
                read_messages();
            }} else {{
                spdlog::error("Error reading from server: {{}}", error.message());
                disconnect();
            }}
        }});
}}

}} // namespace {})",
    options_.projectName,
    options_.projectName,
    options_.projectName);
  } else if (options_.networkLibrary == "poco") {
    clientImpl = fmt::format(R"(#include "{}/client/client.h"
#include <Poco/Net/SocketStream.h>
#include <atomic>
#include <iostream>
#include <spdlog/spdlog.h>

namespace {} {{

Client::Client() {{
    message_callback_ = [](const std::string& msg) {{
        spdlog::info("Message from server: {{}}", msg);
    }};
}}

Client::~Client() {{
    disconnect();
}}

void Client::connect(const std::string& host, int port) {{
    try {{
        Poco::Net::SocketAddress address(host, port);
        socket_.connect(address);
        socket_.setNoDelay(true);
        connected_ = true;

        // Start reader thread
        reader_thread_ = std::thread([this]() {{
            read_messages();
        }});

        spdlog::info("Connected to server {}:{}", host, port);
    }} catch (const Poco::Exception& e) {{
        spdlog::error("Connection error: {{}}", e.displayText());
        throw std::runtime_error(e.displayText());
    }}
}}

void Client::disconnect() {{
    if (!connected_) return;

    connected_ = false;
    socket_.close();

    if (reader_thread_.joinable()) {{
        reader_thread_.join();
    }}

    spdlog::info("Disconnected from server");
}}

void Client::sendMessage(const std::string& message) {{
    if (!connected_) {{
        throw std::runtime_error("Not connected to server");
    }}

    try {{
        Poco::Net::SocketStream stream(socket_);
        stream << message << std::endl;
        stream.flush();
        spdlog::info("Message sent: {{}}", message);
    }} catch (const Poco::Exception& e) {{
        spdlog::error("Error sending message: {{}}", e.displayText());
        throw std::runtime_error(e.displayText());
    }}
}}

void Client::setMessageCallback(std::function<void(const std::string&)> callback) {{
    message_callback_ = std::move(callback);
}}

bool Client::isConnected() const {{
    return connected_;
}}

void Client::read_messages() {{
    try {{
        Poco::Net::SocketStream stream(socket_);
        std::string message;

        while (connected_ && std::getline(stream, message)) {{
            if (message_callback_) {{
                message_callback_(message);
            }}
        }}
    }} catch (const Poco::Exception& e) {{
        if (connected_) {{
            spdlog::error("Error reading from server: {{}}", e.displayText());
            disconnect();
        }}
    }}
}}

}} // namespace {})",
    options_.projectName,
    options_.projectName,
    options_.projectName);
  }

  return clientImpl;
}

std::string NetworkTemplate::getReadmeContent() {
  return fmt::format(R"(# {}

一个C++网络应用，使用CPP-Scaffold创建。

## 功能

- 基于 {} 的客户端/服务器网络通信
- 跨平台兼容性
- 简洁易用的API

## 项目结构
```
{}/
├── include/
│   └── {}/
│       ├── client/
│       │   └── client.h
│       └── server/
│           └── server.h
├── src/
│   ├── client/
│   │   └── client.cpp
│   ├── server/
│   │   └── server.cpp
│   └── main.cpp
└── CMakeLists.txt
```

## 快速开始

```sh
# 克隆仓库
git clone <repository-url>
cd {}

# 创建构建目录并生成构建文件
mkdir build
cd build
cmake ..
make

# 运行服务器
./{} server

# 运行客户端
./{} client
```

## 依赖

- C++17 或更高版本
- CMake 3.10 或更高版本
- {} 库
- {} 库

## 贡献

欢迎贡献代码！请提交Pull Request或报告问题。

## 许可证

本项目使用MIT许可证。详情请参阅LICENSE文件。
)",
    options_.projectName,
    options_.networkLibrary.value_or("Unknown"),
    options_.projectName,
    options_.projectName,
    options_.projectName,
    options_.projectName,
    options_.projectName,
    options_.networkLibrary.value_or("Unknown"),
    enums::to_string(options_.packageManager));
}

std::string NetworkTemplate::getCMakeContent() {
  std::string networkLibraryConfig;
  std::string networkLibraryLink;

  if (options_.networkLibrary == "asio") {
    networkLibraryConfig = R"(
# Find Asio
find_package(asio REQUIRED)
if(NOT asio_FOUND)
    message(STATUS "Asio not found, using FetchContent")
    FetchContent_Declare(
        asio
        GIT_REPOSITORY https://github.com/chriskohlhoff/asio.git
        GIT_TAG asio-1-24-0
    )
    FetchContent_MakeAvailable(asio)
endif()
)";
    networkLibraryLink = "asio::asio";
  } else if (options_.networkLibrary == "boost") {
    networkLibraryConfig = R"(
# Find boost
find_package(boost REQUIRED COMPONENTS system thread)
)";
    networkLibraryLink = "boost::system boost::thread";
  } else if (options_.networkLibrary == "poco") {
    networkLibraryConfig = R"(
# Find Poco
find_package(Poco REQUIRED COMPONENTS Foundation Net)
)";
    networkLibraryLink = "Poco::Foundation Poco::Net";
  }

  return fmt::format(R"(cmake_minimum_required(VERSION 3.14)
project({} VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Find required packages
find_package(spdlog REQUIRED)
{}

# Create library
add_library({}_lib STATIC
    src/server/server.cpp
    src/client/client.cpp
)

target_include_directories({}_lib PUBLIC
    $<BUILD_INTERFACE:${{CMAKE_CURRENT_SOURCE_DIR}}/include>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries({}_lib PUBLIC
    spdlog::spdlog
    {}
)

# Create executable
add_executable({} src/main.cpp)
target_link_libraries({} PRIVATE {}_lib)

# Install targets
install(TARGETS {} {}_lib
    EXPORT {}_targets
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    INCLUDES DESTINATION include
)

install(DIRECTORY include/ DESTINATION include)

install(EXPORT {}_targets
    FILE {}_targets.cmake
    NAMESPACE {}::
    DESTINATION lib/cmake/{}
)
)",
    options_.projectName,
    networkLibraryConfig,
    options_.projectName,
    options_.projectName,
    options_.projectName,
    networkLibraryLink,
    options_.projectName,
    options_.projectName,
    options_.projectName,
    options_.projectName,
    options_.projectName,
    options_.projectName,
    options_.projectName,
    options_.projectName,
    options_.projectName,
    options_.projectName);
}

std::string NetworkTemplate::getMesonContent() {
  std::string networkLibraryDep;

  if (options_.networkLibrary == "asio") {
    networkLibraryDep = "asio_dep = dependency('asio', fallback: ['asio', 'asio_dep'])";
  } else if (options_.networkLibrary == "boost") {
    networkLibraryDep = "boost_dep = dependency('boost', modules: ['system', 'thread'])";
  } else if (options_.networkLibrary == "poco") {
    networkLibraryDep = "poco_dep = dependency('poco', modules: ['foundation', 'net'])";
  }

  return fmt::format(R"(project('{}', 'cpp',
  version : '1.0.0',
  default_options : ['cpp_std=c++17'])

# Dependencies
spdlog_dep = dependency('spdlog')
{}

# Library
{}_lib = static_library('{}_lib',
  'src/server/server.cpp',
  'src/client/client.cpp',
  include_directories : include_directories('include'),
  dependencies : [spdlog_dep, {}])

# Executable
{}_exe = executable('{}',
  'src/main.cpp',
  link_with : {}_lib,
  install : true)
)",
    options_.projectName,
    networkLibraryDep,
    options_.projectName,
    options_.projectName,
    options_.networkLibrary == "asio" ? "asio_dep" :
    options_.networkLibrary == "boost" ? "boost_dep" : "poco_dep",
    options_.projectName,
    options_.projectName,
    options_.projectName);
}

std::string NetworkTemplate::getBazelContent() {
  std::string networkLibraryDeps;

  if (options_.networkLibrary == "asio") {
    networkLibraryDeps = R"(
    "@asio//:asio",)";
  } else if (options_.networkLibrary == "boost") {
    networkLibraryDeps = R"(
    "@boost//:system",
    "@boost//:thread",)";
  } else if (options_.networkLibrary == "poco") {
    networkLibraryDeps = R"(
    "@poco//:foundation",
    "@poco//:net",)";
  }

  return fmt::format(R"(load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library")

cc_library(
    name = "{}_lib",
    srcs = [
        "src/server/server.cpp",
        "src/client/client.cpp",
    ],
    hdrs = glob(["include/**/*.h"]),
    includes = ["include"],
    deps = [
        "@spdlog//:spdlog",{}
    ],
    visibility = ["//visibility:public"],
)

cc_binary(
    name = "{}",
    srcs = ["src/main.cpp"],
    deps = [":{}_lib"],
)
)",
    options_.projectName,
    networkLibraryDeps,
    options_.projectName,
    options_.projectName);
}

std::string NetworkTemplate::getXMakeContent() {
  std::string networkLibraryConfig;

  if (options_.networkLibrary == "asio") {
    networkLibraryConfig = R"(
    add_requires("asio")
    add_packages("asio"))";
  } else if (options_.networkLibrary == "boost") {
    networkLibraryConfig = R"(
    add_requires("boost")
    add_packages("boost"))";
  } else if (options_.networkLibrary == "poco") {
    networkLibraryConfig = R"(
    add_requires("poco")
    add_packages("poco"))";
  }

  return fmt::format(R"(set_project("{}")
set_version("1.0.0")

set_languages("cxx17")

add_requires("spdlog"){}

target("{}_lib")
    set_kind("static")
    add_files("src/server/server.cpp", "src/client/client.cpp")
    add_includedirs("include", {{public = true}})
    add_packages("spdlog"{})

target("{}")
    set_kind("binary")
    add_files("src/main.cpp")
    add_deps("{}_lib")
)",
    options_.projectName,
    networkLibraryConfig,
    options_.projectName,
    options_.networkLibrary == "asio" ? ", \"asio\"" :
    options_.networkLibrary == "boost" ? ", \"boost\"" :
    options_.networkLibrary == "poco" ? ", \"poco\"" : "",
    options_.projectName,
    options_.projectName);
}

std::string NetworkTemplate::getPremakeContent() {
  std::string networkLibraryConfig;

  if (options_.networkLibrary == "asio") {
    networkLibraryConfig = R"(
    includedirs { "path/to/asio/include" }
    links { "asio" })";
  } else if (options_.networkLibrary == "boost") {
    networkLibraryConfig = R"(
    includedirs { "path/to/boost/include" }
    links { "boost_system", "boost_thread" })";
  } else if (options_.networkLibrary == "poco") {
    networkLibraryConfig = R"(
    includedirs { "path/to/poco/include" }
    links { "PocoFoundation", "PocoNet" })";
  }

  return fmt::format(R"(workspace "{}"
    configurations {{ "Debug", "Release" }}
    platforms {{ "x64" }}

project "{}_lib"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"

    targetdir "bin/%{{cfg.buildcfg}}"
    objdir "bin-int/%{{cfg.buildcfg}}"

    files {{
        "src/server/server.cpp",
        "src/client/client.cpp",
        "include/**.h"
    }}

    includedirs {{
        "include",
        "path/to/spdlog/include"
    }}{}

    filter "configurations:Debug"
        defines {{ "DEBUG" }}
        symbols "On"

    filter "configurations:Release"
        defines {{ "NDEBUG" }}
        optimize "On"

project "{}"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"

    targetdir "bin/%{{cfg.buildcfg}}"
    objdir "bin-int/%{{cfg.buildcfg}}"

    files {{
        "src/main.cpp"
    }}

    links {{
        "{}_lib"
    }}

    filter "configurations:Debug"
        defines {{ "DEBUG" }}
        symbols "On"

    filter "configurations:Release"
        defines {{ "NDEBUG" }}
        optimize "On"
)",
    options_.projectName,
    options_.projectName,
    networkLibraryConfig,
    options_.projectName,
    options_.projectName);
}

std::string NetworkTemplate::getNetworkTestContent() {
  std::string testFrameworkInclude;
  std::string testFrameworkMacros;

  if (enums::to_string(options_.testFramework) == "gtest") {
    testFrameworkInclude = "#include <gtest/gtest.h>";
    testFrameworkMacros = R"(
TEST(NetworkTest, ServerCreation) {
    EXPECT_NO_THROW({
        auto server = std::make_unique<)" + options_.projectName + R"(::Server>();
    });
}

TEST(NetworkTest, ClientCreation) {
    EXPECT_NO_THROW({
        auto client = std::make_unique<)" + options_.projectName + R"(::Client>();
    });
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
})";
  } else if (enums::to_string(options_.testFramework) == "catch2") {
    testFrameworkInclude = "#include <catch2/catch_test_macros.hpp>";
    testFrameworkMacros = R"(
TEST_CASE("Server can be created", "[network]") {
    REQUIRE_NOTHROW()" + options_.projectName + R"(::Server{});
}

TEST_CASE("Client can be created", "[network]") {
    REQUIRE_NOTHROW()" + options_.projectName + R"(::Client{});
})";
  } else if (enums::to_string(options_.testFramework) == "doctest") {
    testFrameworkInclude = "#include <doctest/doctest.h>";
    testFrameworkMacros = R"(
TEST_CASE("Server creation") {
    CHECK_NOTHROW()" + options_.projectName + R"(::Server{});
}

TEST_CASE("Client creation") {
    CHECK_NOTHROW()" + options_.projectName + R"(::Client{});
})";
  }

  return fmt::format(R"({}
#include <memory>
#include "{}/server/server.h"
#include "{}/client/client.h"

{}
)",
    testFrameworkInclude,
    options_.projectName,
    options_.projectName,
    testFrameworkMacros);
}

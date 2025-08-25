#include "cli/cli_parser.h"
#include "templates/template_manager.h"
#include "utils/progress_indicator.h"
#include "utils/interactive_menu.h"
#include "config/config_manager.h"
#include "testing/test_framework_manager.h"
#include "documentation/doc_generator.h"
#include "utils/framework_integration.h"

#include <iostream>
#include <thread>
#include <chrono>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

// Initialize the logging system
void initializeLogger(bool verbose) {
  try {
    // Create a logger - one for console output and one for file output
    auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    console_sink->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] %v");

    // Create a rotating file sink, with a maximum file size of 5MB and 3
    // rotated files
    auto file_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
        "cpp-scaffold.log", 5 * 1024 * 1024, 3);
    file_sink->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%l] [%t] %v");

    // Create a logger with multiple sinks
    auto logger = std::make_shared<spdlog::logger>(
        "cpp-scaffold", spdlog::sinks_init_list{console_sink, file_sink});

    // Set logging level based on verbosity
    if (verbose) {
      logger->set_level(spdlog::level::debug);
      logger->flush_on(spdlog::level::debug);
    } else {
      logger->set_level(spdlog::level::info);
      logger->flush_on(spdlog::level::err);
    }

    // Set the default logger
    spdlog::set_default_logger(logger);

    spdlog::info("Logging system initialized");
    spdlog::debug("Verbose logging enabled");
  } catch (const spdlog::spdlog_ex &ex) {
    std::cerr << "Logging initialization failed: " << ex.what() << std::endl;
  }
}

// Demonstrate advanced features
void demonstrateAdvancedFeatures() {
  spdlog::info("Demonstrating CPP-Scaffold advanced features...");

  // 1. Progress Indicators
  {
    auto progress = utils::progress::spinner("Initializing advanced features");
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    progress->update(0.5, "Loading configuration system");
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    progress->finish("Advanced features initialized!");
  }

  // 2. Configuration Management
  {
    auto& configManager = config::ConfigManager::getInstance();
    spdlog::info("Configuration system available with {} profiles",
                 configManager.listProfiles().size());
  }

  // 3. Framework Integration
  {
    auto frameworks = utils::FrameworkIntegration::listAvailableFrameworks();
    spdlog::info("Available frameworks: {}", frameworks.size());
    for (const auto& framework : frameworks) {
      auto info = utils::FrameworkIntegration::getFrameworkInfo(framework);
      if (info) {
        spdlog::debug("Framework: {} - {}", info->name, info->description);
      }
    }
  }

  // 4. Testing Infrastructure
  {
    auto& testManager = testing::TestFrameworkManager::getInstance();
    auto testFrameworks = testManager.listSupportedFrameworks();
    spdlog::info("Supported test frameworks: {}", testFrameworks.size());
  }

  // 5. Documentation Generation
  {
    documentation::DocConfig docConfig;
    docConfig.projectName = "CPP-Scaffold";
    docConfig.projectDescription = "Advanced C++ project scaffolding tool";
    docConfig.author = "CPP-Scaffold Team";

    documentation::DocGenerator docGen(docConfig);
    spdlog::info("Documentation generator configured for: {}", docConfig.projectName);
  }

  spdlog::info("All advanced features are operational!");
}

int main(int argc, char *argv[]) {
  try {
    // Perform basic initialization first
    initializeLogger(false); // Default: non-verbose mode

    spdlog::info("CPP-Scaffold is starting...");
    spdlog::debug("Parsing command line arguments");

    // Demonstrate advanced features if in verbose mode
    if (false) { // Set to true to enable demonstration
      demonstrateAdvancedFeatures();
    }

    // Parse command line arguments
    CliOptions options = CliParser::parse(argc, argv);

    // Reconfigure logging level if verbose mode is enabled
    if (options.verbose) {
      spdlog::set_level(spdlog::level::debug);
      spdlog::debug("Verbose logging enabled");
    }

    spdlog::debug("Command line arguments parsed");

    // Show help
    if (options.showHelp) {
      spdlog::debug("User requested help information");
      CliParser::showHelp();
      return 0;
    }

    // Show version
    if (options.version) {
      spdlog::debug("User requested version information");
      CliParser::showVersion();
      return 0;
    }

    // Create project
    spdlog::info("Creating project: {}", options.projectName);
    spdlog::debug(
        "Project configuration: Type={}, Build System={}, Package Manager={}",
        enums::to_string(options.templateType), enums::to_string(options.buildSystem), enums::to_string(options.packageManager));

    TemplateManager templateManager;
    spdlog::debug("Template manager initialized");

    bool success = templateManager.createProject(options);
    if (!success) {
      spdlog::error("Project creation failed");
      return 1;
    }

    spdlog::info("Project {} created successfully!", options.projectName);
  } catch (const std::exception &e) {
    spdlog::critical("A critical error occurred: {}", e.what());
    std::cerr << "Error: " << e.what() << std::endl;
    return 1;
  } catch (...) {
    spdlog::critical("An unknown critical error occurred");
    std::cerr << "An unknown error occurred" << std::endl;
    return 1;
  }

  spdlog::info("CPP-Scaffold completed execution and exited normally");
  return 0;
}

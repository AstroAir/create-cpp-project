#include "cli/parsing/argument_parser.h"
#include "cli/commands/command_handlers.h"
#include "templates/template_manager.h"
#include "utils/progress_indicator.h"
#include "utils/interactive_menu.h"
#include "utils/enhanced_terminal.h"
#include "utils/enhanced_wizard.h"
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

    // Get enhanced terminal instance
    auto& terminal = utils::EnhancedTerminal::getInstance();

    // Parse command line arguments
    CliOptions options = argument_parser::parseArguments(argc, argv);

    // If no arguments provided, run interactive wizard
    if (argc <= 1) {
      auto& wizard = utils::EnhancedWizard::getInstance();
      options = wizard.runInteractiveWizard();
    }

    // Reconfigure logging level if verbose mode is enabled
    if (options.verbose) {
      spdlog::set_level(spdlog::level::debug);
      spdlog::debug("Verbose logging enabled");
    }

    spdlog::debug("Command line arguments parsed");

    // Show help
    if (options.showHelp) {
      spdlog::debug("User requested help information");
      cli_commands::showHelp();
      return 0;
    }

    // Show version
    if (options.version) {
      spdlog::debug("User requested version information");
      cli_commands::showVersion();
      return 0;
    }

    // Start enhanced project creation experience
    terminal.startProjectCreation(options);

    // Create project with enhanced progress indication
    TemplateManager templateManager;
    spdlog::debug("Template manager initialized");

    bool success = templateManager.createProject(options);

    // Finish with enhanced completion screen
    terminal.finishProjectCreation(options, success);

    if (!success) {
      spdlog::error("Project creation failed");
      return 1;
    }

    spdlog::info("Project {} created successfully!", options.projectName);
  } catch (const std::exception &e) {
    spdlog::critical("A critical error occurred: {}", e.what());

    // Show enhanced error screen
    auto& terminal = utils::EnhancedTerminal::getInstance();
    terminal.showErrorScreen(e.what(), "Check the log file for more details");

    return 1;
  } catch (...) {
    spdlog::critical("An unknown critical error occurred");

    // Show enhanced error screen
    auto& terminal = utils::EnhancedTerminal::getInstance();
    terminal.showErrorScreen("An unknown error occurred", "Please report this issue on GitHub");

    return 1;
  }

  spdlog::info("CPP-Scaffold completed execution and exited normally");
  return 0;
}

#pragma once

#include "types/cli_options.h"
#include "types/cli_enums.h"
#include "localization/localization.h"
#include "input/user_input.h"
#include "commands/command_handlers.h"
#include "parsing/argument_parser.h"
#include "wizard/project_wizard.h"

// Main CLI parser class using the refactored modules
class CliParser {
public:
  // Main parsing method
  static CliOptions parse(int argc, char* argv[]);

  // Interactive options configuration
  static CliOptions promptUserForOptions(const CliOptions& defaultOptions = {});

  // Validation methods
  static bool validateOptions(CliOptions& options);

  // Interactive mode
  static CliOptions runInteractiveMode();

private:
  // System detection and defaults
  static CliOptions getSystemSuggestedDefaults();

  // Configuration loading
  static CliOptions loadDefaultConfiguration();

  // Validation helpers
  static bool validateProjectName(const std::string& name);
  static bool validateProjectPath(const std::string& path);

  // Error handling
  static void handleParsingError(const std::string& error);
};

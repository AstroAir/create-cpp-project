#pragma once

#include "../types/cli_options.h"
#include "../types/cli_enums.h"
#include <string>

// Command handler functions for different CLI operations
namespace cli_commands {

// Information display commands
void showHelp(Language lang = Language::English);
void showVersion();
void showAvailableTemplates();
void showAvailableProfiles();
void showProfileInfo(const std::string& profileName);

// Validation commands
void validateProject(const std::string& projectPath);
void validateConfiguration(const CliOptions& options);

// Configuration commands
void runConfigurationWizard();
void listProfiles();

// Interactive mode
CliOptions runInteractiveMode();

// Utility functions
void showCliHeader();
std::string detectOS();
CliOptions getSystemSuggestedDefaults();

} // namespace cli_commands

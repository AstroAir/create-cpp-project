#pragma once

#include "../types/cli_options.h"
#include "../types/cli_enums.h"
#include <string>
#include <vector>

// Argument parsing utilities for CLI
namespace argument_parser {

// Main parsing function
CliOptions parseArguments(int argc, char* argv[]);

// Category-specific parsing functions
bool parseBasicOptions(CliOptions& options, const std::vector<std::string>& args, size_t& index);
bool parseProjectOptions(CliOptions& options, const std::vector<std::string>& args, size_t& index);
bool parseBuildOptions(CliOptions& options, const std::vector<std::string>& args, size_t& index);
bool parseTestingOptions(CliOptions& options, const std::vector<std::string>& args, size_t& index);
bool parseDevToolOptions(CliOptions& options, const std::vector<std::string>& args, size_t& index);
bool parseGitOptions(CliOptions& options, const std::vector<std::string>& args, size_t& index);
bool parseDocumentationOptions(CliOptions& options, const std::vector<std::string>& args, size_t& index);
bool parseAdvancedOptions(CliOptions& options, const std::vector<std::string>& args, size_t& index);

// Helper functions
bool isFlag(const std::string& arg);
bool hasValue(const std::vector<std::string>& args, size_t index);
std::string getNextValue(const std::vector<std::string>& args, size_t& index);
std::vector<std::string> parseCommaSeparatedList(const std::string& value);

// Command detection
enum class CommandType {
  Create,
  New,
  Interactive,
  Config,
  ListTemplates,
  ListProfiles,
  ShowProfile,
  Validate,
  ValidateConfig,
  Help,
  Version,
  Unknown
};

CommandType detectCommand(const std::vector<std::string>& args);
std::string extractProjectName(const std::vector<std::string>& args, CommandType command);

} // namespace argument_parser

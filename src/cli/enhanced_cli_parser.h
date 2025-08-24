#pragma once

#include "cli_parser.h"
#include <string>
#include <vector>
#include <map>
#include <optional>
#include <functional>
#include <regex>

namespace cli {

// Command definition for subcommands
struct Command {
    std::string name;
    std::string description;
    std::string usage;
    std::vector<std::string> aliases;
    std::function<int(const std::vector<std::string>&)> handler;
    std::map<std::string, std::string> options;
    std::vector<std::string> examples;
    bool requiresProjectName = false;
};

// Argument definition for validation
struct ArgumentDefinition {
    std::string name;
    std::string description;
    std::string shortFlag;
    std::string longFlag;
    bool required = false;
    bool hasValue = true;
    std::string defaultValue;
    std::vector<std::string> allowedValues;
    std::regex validationPattern;
    std::string validationMessage;
    std::function<bool(const std::string&)> customValidator;
};

// Validation result
struct ValidationResult {
    bool isValid = true;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    std::vector<std::string> suggestions;
};

// Enhanced CLI parser with comprehensive features
class EnhancedCliParser {
public:
    static EnhancedCliParser& getInstance();
    
    // Main parsing methods
    CliOptions parse(int argc, char* argv[]);
    CliOptions parseArgs(const std::vector<std::string>& args);
    
    // Command registration
    void registerCommand(const Command& command);
    void registerBuiltinCommands();
    
    // Argument definition
    void defineArgument(const ArgumentDefinition& arg);
    void defineBuiltinArguments();
    
    // Validation
    ValidationResult validateArguments(const CliOptions& options);
    ValidationResult validateCommand(const std::string& command, const std::vector<std::string>& args);
    bool validateProjectName(const std::string& name);
    bool validatePath(const std::string& path);
    bool validateEmail(const std::string& email);
    bool validateUrl(const std::string& url);
    
    // Help and documentation
    void showHelp(const std::string& command = "");
    void showVersion();
    void showUsage(const std::string& command = "");
    void showExamples(const std::string& command = "");
    void showCommandList();
    void showArgumentList();
    
    // Interactive features
    CliOptions runInteractiveMode();
    std::string promptForMissingArgument(const ArgumentDefinition& arg);
    bool confirmAction(const std::string& message);
    
    // Auto-completion support
    std::vector<std::string> getCompletions(const std::string& partial);
    std::vector<std::string> getCommandCompletions(const std::string& partial);
    std::vector<std::string> getArgumentCompletions(const std::string& partial);
    std::vector<std::string> getValueCompletions(const std::string& argument, const std::string& partial);
    
    // Configuration management
    void loadConfiguration(const std::string& configFile);
    void saveConfiguration(const std::string& configFile, const CliOptions& options);
    CliOptions getDefaultConfiguration();
    
    // Error handling and suggestions
    void showError(const std::string& message, const std::vector<std::string>& suggestions = {});
    void showWarning(const std::string& message);
    std::vector<std::string> suggestSimilarCommands(const std::string& command);
    std::vector<std::string> suggestSimilarArguments(const std::string& argument);
    
    // Advanced parsing features
    bool parseConfigFile(const std::string& configFile, CliOptions& options);
    bool parseEnvironmentVariables(CliOptions& options);
    void setArgumentPrefix(const std::string& prefix); // For custom argument prefixes
    void enableStrictMode(bool strict); // Strict validation mode
    
private:
    EnhancedCliParser() = default;
    
    // Internal parsing helpers
    std::pair<std::string, std::vector<std::string>> parseCommandLine(const std::vector<std::string>& args);
    std::map<std::string, std::string> parseArguments(const std::vector<std::string>& args);
    std::string normalizeArgument(const std::string& arg);
    
    // Validation helpers
    bool validateArgumentValue(const ArgumentDefinition& arg, const std::string& value);
    std::string getValidationError(const ArgumentDefinition& arg, const std::string& value);
    
    // Help formatting
    void printCommandHelp(const Command& command);
    void printArgumentHelp(const ArgumentDefinition& arg);
    std::string formatUsageLine(const Command& command);
    std::string formatArgumentList();
    std::string formatExamples(const std::vector<std::string>& examples);
    
    // String utilities
    std::vector<std::string> splitString(const std::string& str, char delimiter);
    std::string joinStrings(const std::vector<std::string>& strings, const std::string& delimiter);
    std::string trimString(const std::string& str);
    bool stringStartsWith(const std::string& str, const std::string& prefix);
    int calculateLevenshteinDistance(const std::string& a, const std::string& b);
    
    // Internal data
    std::map<std::string, Command> commands_;
    std::map<std::string, ArgumentDefinition> arguments_;
    std::string argumentPrefix_ = "--";
    bool strictMode_ = false;
    CliOptions defaultConfig_;
};

// Built-in command handlers
namespace command_handlers {
    int handleCreate(const std::vector<std::string>& args);
    int handleList(const std::vector<std::string>& args);
    int handleValidate(const std::vector<std::string>& args);
    int handleConfig(const std::vector<std::string>& args);
    int handleUpdate(const std::vector<std::string>& args);
    int handleClean(const std::vector<std::string>& args);
    int handleInfo(const std::vector<std::string>& args);
    int handleSearch(const std::vector<std::string>& args);
    int handleInit(const std::vector<std::string>& args);
    int handleMigrate(const std::vector<std::string>& args);
}

// Utility functions for CLI parsing
namespace cli_utils {
    // Argument parsing utilities
    bool isFlag(const std::string& arg);
    bool isShortFlag(const std::string& arg);
    bool isLongFlag(const std::string& arg);
    std::string extractFlagName(const std::string& arg);
    std::pair<std::string, std::string> splitFlagValue(const std::string& arg);
    
    // Validation utilities
    bool isValidProjectName(const std::string& name);
    bool isValidPath(const std::string& path);
    bool isValidEmail(const std::string& email);
    bool isValidUrl(const std::string& url);
    bool isValidVersion(const std::string& version);
    
    // String formatting utilities
    std::string formatHelpText(const std::string& text, int width = 80, int indent = 0);
    std::string formatTable(const std::vector<std::vector<std::string>>& rows, 
                           const std::vector<std::string>& headers = {});
    std::string colorizeText(const std::string& text, const std::string& color);
    
    // Interactive utilities
    std::string readUserInput(const std::string& prompt, const std::string& defaultValue = "");
    bool readConfirmation(const std::string& prompt, bool defaultValue = true);
    int readChoice(const std::string& prompt, const std::vector<std::string>& options, int defaultChoice = 0);
    std::vector<std::string> readMultipleChoice(const std::string& prompt, 
                                               const std::vector<std::string>& options,
                                               const std::vector<bool>& defaultSelected = {});
    
    // File and path utilities
    std::string expandPath(const std::string& path);
    std::string resolvePath(const std::string& path);
    bool pathExists(const std::string& path);
    bool isDirectory(const std::string& path);
    bool isFile(const std::string& path);
    
    // Configuration utilities
    std::map<std::string, std::string> parseConfigFile(const std::string& filePath);
    bool writeConfigFile(const std::string& filePath, const std::map<std::string, std::string>& config);
    std::map<std::string, std::string> getEnvironmentVariables(const std::string& prefix = "CPP_SCAFFOLD_");
    
    // Error and suggestion utilities
    std::vector<std::string> findSimilarStrings(const std::string& target, 
                                               const std::vector<std::string>& candidates,
                                               int maxDistance = 3);
    std::string formatErrorMessage(const std::string& error, const std::vector<std::string>& suggestions = {});
    std::string formatWarningMessage(const std::string& warning);
    std::string formatSuccessMessage(const std::string& message);
}

} // namespace cli

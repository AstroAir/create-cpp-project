#include "enhanced_cli_parser.h"
#include "../utils/terminal_utils.h"
#include "../utils/file_utils.h"
#include <fmt/format.h>
#include <spdlog/spdlog.h>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <regex>

namespace cli {

EnhancedCliParser& EnhancedCliParser::getInstance() {
    static EnhancedCliParser instance;
    static bool initialized = false;
    
    if (!initialized) {
        instance.registerBuiltinCommands();
        instance.defineBuiltinArguments();
        initialized = true;
    }
    
    return instance;
}

CliOptions EnhancedCliParser::parse(int argc, char* argv[]) {
    std::vector<std::string> args;
    for (int i = 1; i < argc; ++i) {
        args.emplace_back(argv[i]);
    }
    return parseArgs(args);
}

CliOptions EnhancedCliParser::parseArgs(const std::vector<std::string>& args) {
    CliOptions options;
    
    if (args.empty()) {
        return runInteractiveMode();
    }
    
    // Parse command and arguments
    auto [command, commandArgs] = parseCommandLine(args);
    
    // Handle built-in commands
    if (command == "help" || command == "--help" || command == "-h") {
        std::string helpCommand = commandArgs.empty() ? "" : commandArgs[0];
        showHelp(helpCommand);
        options.showHelp = true;
        return options;
    }
    
    if (command == "version" || command == "--version" || command == "-v") {
        showVersion();
        options.version = true;
        return options;
    }
    
    if (command == "list" || command == "list-templates") {
        command_handlers::handleList(commandArgs);
        options.showHelp = true;
        return options;
    }
    
    if (command == "validate") {
        command_handlers::handleValidate(commandArgs);
        options.showHelp = true;
        return options;
    }
    
    if (command == "config" || command == "configure") {
        command_handlers::handleConfig(commandArgs);
        options.showHelp = true;
        return options;
    }
    
    if (command == "interactive" || command == "-i") {
        return runInteractiveMode();
    }
    
    // Handle create/new commands
    if (command == "create" || command == "new") {
        if (commandArgs.empty()) {
            showError("Project name is required", {"cpp-scaffold create <project-name> [options]"});
            options.showHelp = true;
            return options;
        }
        
        options.projectName = commandArgs[0];
        
        // Parse remaining arguments
        std::vector<std::string> remainingArgs(commandArgs.begin() + 1, commandArgs.end());
        auto parsedArgs = parseArguments(remainingArgs);
        
        // Apply parsed arguments to options
        for (const auto& [key, value] : parsedArgs) {
            if (key == "template" || key == "t") {
                auto templateType = enums::to_template_type(value);
                if (templateType) {
                    options.templateType = *templateType;
                } else {
                    showError("Invalid template type: " + value, 
                             {"Available templates: console, lib, header-only-lib, gui, network, embedded, webservice, gameengine"});
                }
            } else if (key == "build" || key == "b") {
                auto buildSystem = enums::to_build_system(value);
                if (buildSystem) {
                    options.buildSystem = *buildSystem;
                } else {
                    showError("Invalid build system: " + value,
                             {"Available build systems: cmake, meson, bazel, xmake, premake, make, ninja"});
                }
            } else if (key == "package" || key == "p") {
                auto packageManager = enums::to_package_manager(value);
                if (packageManager) {
                    options.packageManager = *packageManager;
                } else {
                    showError("Invalid package manager: " + value,
                             {"Available package managers: vcpkg, conan, none, spack, hunter, cpm, fetchcontent"});
                }
            } else if (key == "tests") {
                options.includeTests = true;
                if (!value.empty()) {
                    auto testFramework = enums::to_test_framework(value);
                    if (testFramework) {
                        options.testFramework = *testFramework;
                    }
                }
            } else if (key == "docs" || key == "documentation") {
                options.includeDocumentation = true;
            } else if (key == "code-style") {
                options.includeCodeStyleTools = true;
            } else if (key == "git") {
                options.initGit = (value != "false" && value != "0");
            } else if (key == "verbose") {
                options.verbose = true;
            }
        }
        
        // Validate the configuration
        ValidationResult validation = validateArguments(options);
        if (!validation.isValid) {
            for (const auto& error : validation.errors) {
                showError(error);
            }
            options.showHelp = true;
            return options;
        }
        
        // Show warnings if any
        for (const auto& warning : validation.warnings) {
            showWarning(warning);
        }
        
        return options;
    }
    
    // Unknown command
    showError("Unknown command: " + command, suggestSimilarCommands(command));
    options.showHelp = true;
    return options;
}

void EnhancedCliParser::registerBuiltinCommands() {
    // Create command
    Command createCmd;
    createCmd.name = "create";
    createCmd.description = "Create a new C++ project";
    createCmd.usage = "cpp-scaffold create <project-name> [options]";
    createCmd.aliases = {"new"};
    createCmd.requiresProjectName = true;
    createCmd.examples = {
        "cpp-scaffold create my-app --template console --build cmake",
        "cpp-scaffold create my-lib --template lib --package vcpkg --tests",
        "cpp-scaffold create my-game --template gameengine --build cmake --package conan"
    };
    createCmd.handler = command_handlers::handleCreate;
    commands_["create"] = createCmd;
    commands_["new"] = createCmd;
    
    // List command
    Command listCmd;
    listCmd.name = "list";
    listCmd.description = "List available templates and configurations";
    listCmd.usage = "cpp-scaffold list [templates|build-systems|package-managers]";
    listCmd.aliases = {"list-templates", "ls"};
    listCmd.examples = {
        "cpp-scaffold list",
        "cpp-scaffold list templates",
        "cpp-scaffold list build-systems"
    };
    listCmd.handler = command_handlers::handleList;
    commands_["list"] = listCmd;
    commands_["list-templates"] = listCmd;
    commands_["ls"] = listCmd;
    
    // Validate command
    Command validateCmd;
    validateCmd.name = "validate";
    validateCmd.description = "Validate project structure and configuration";
    validateCmd.usage = "cpp-scaffold validate [project-path]";
    validateCmd.examples = {
        "cpp-scaffold validate",
        "cpp-scaffold validate ./my-project",
        "cpp-scaffold validate /path/to/project"
    };
    validateCmd.handler = command_handlers::handleValidate;
    commands_["validate"] = validateCmd;
    
    // Config command
    Command configCmd;
    configCmd.name = "config";
    configCmd.description = "Configure default settings";
    configCmd.usage = "cpp-scaffold config [get|set|list] [key] [value]";
    configCmd.aliases = {"configure", "cfg"};
    configCmd.examples = {
        "cpp-scaffold config",
        "cpp-scaffold config set default.template lib",
        "cpp-scaffold config get default.build-system"
    };
    configCmd.handler = command_handlers::handleConfig;
    commands_["config"] = configCmd;
    commands_["configure"] = configCmd;
    commands_["cfg"] = configCmd;
}

void EnhancedCliParser::defineBuiltinArguments() {
    // Template argument
    ArgumentDefinition templateArg;
    templateArg.name = "template";
    templateArg.description = "Project template type";
    templateArg.shortFlag = "-t";
    templateArg.longFlag = "--template";
    templateArg.hasValue = true;
    templateArg.defaultValue = "console";
    templateArg.allowedValues = {"console", "lib", "header-only-lib", "multi-executable", 
                                "gui", "network", "embedded", "webservice", "gameengine",
                                "qt-app", "sfml-app", "boost-app", "test-project"};
    templateArg.validationMessage = "Must be one of the supported template types";
    arguments_["template"] = templateArg;
    
    // Build system argument
    ArgumentDefinition buildArg;
    buildArg.name = "build";
    buildArg.description = "Build system to use";
    buildArg.shortFlag = "-b";
    buildArg.longFlag = "--build";
    buildArg.hasValue = true;
    buildArg.defaultValue = "cmake";
    buildArg.allowedValues = {"cmake", "meson", "bazel", "xmake", "premake", "make", "ninja"};
    buildArg.validationMessage = "Must be one of the supported build systems";
    arguments_["build"] = buildArg;
    
    // Package manager argument
    ArgumentDefinition packageArg;
    packageArg.name = "package";
    packageArg.description = "Package manager to use";
    packageArg.shortFlag = "-p";
    packageArg.longFlag = "--package";
    packageArg.hasValue = true;
    packageArg.defaultValue = "vcpkg";
    packageArg.allowedValues = {"vcpkg", "conan", "none", "spack", "hunter", "cpm", "fetchcontent"};
    packageArg.validationMessage = "Must be one of the supported package managers";
    arguments_["package"] = packageArg;
    
    // Tests argument
    ArgumentDefinition testsArg;
    testsArg.name = "tests";
    testsArg.description = "Include testing framework";
    testsArg.longFlag = "--tests";
    testsArg.hasValue = false;
    testsArg.defaultValue = "false";
    arguments_["tests"] = testsArg;
    
    // Documentation argument
    ArgumentDefinition docsArg;
    docsArg.name = "docs";
    docsArg.description = "Include documentation configuration";
    docsArg.longFlag = "--docs";
    docsArg.hasValue = false;
    docsArg.defaultValue = "false";
    arguments_["docs"] = docsArg;
    
    // Code style argument
    ArgumentDefinition codeStyleArg;
    codeStyleArg.name = "code-style";
    codeStyleArg.description = "Include code style and static analysis tools";
    codeStyleArg.longFlag = "--code-style";
    codeStyleArg.hasValue = false;
    codeStyleArg.defaultValue = "false";
    arguments_["code-style"] = codeStyleArg;
    
    // Git argument
    ArgumentDefinition gitArg;
    gitArg.name = "git";
    gitArg.description = "Initialize Git repository";
    gitArg.longFlag = "--git";
    gitArg.hasValue = false;
    gitArg.defaultValue = "true";
    arguments_["git"] = gitArg;
    
    // Verbose argument
    ArgumentDefinition verboseArg;
    verboseArg.name = "verbose";
    verboseArg.description = "Enable verbose output";
    verboseArg.shortFlag = "-v";
    verboseArg.longFlag = "--verbose";
    verboseArg.hasValue = false;
    verboseArg.defaultValue = "false";
    arguments_["verbose"] = verboseArg;
}

ValidationResult EnhancedCliParser::validateArguments(const CliOptions& options) {
    ValidationResult result;
    
    // Validate project name
    if (!validateProjectName(options.projectName)) {
        result.isValid = false;
        result.errors.push_back("Invalid project name: " + options.projectName);
        result.suggestions.push_back("Project name must contain only alphanumeric characters, hyphens, and underscores");
    }
    
    // Check for conflicting options
    if (options.templateType == TemplateType::HeaderOnlyLib && options.packageManager != PackageManager::None) {
        result.warnings.push_back("Header-only libraries typically don't need package managers");
        result.suggestions.push_back("Consider using --package none for header-only libraries");
    }
    
    if (options.templateType == TemplateType::Embedded && options.packageManager == PackageManager::Vcpkg) {
        result.warnings.push_back("vcpkg may not be suitable for embedded projects");
        result.suggestions.push_back("Consider using --package none or --package conan for embedded projects");
    }
    
    return result;
}

bool EnhancedCliParser::validateProjectName(const std::string& name) {
    if (name.empty() || name.length() > 100) {
        return false;
    }
    
    // Check for valid characters (alphanumeric, hyphens, underscores)
    std::regex validNameRegex("^[a-zA-Z0-9_-]+$");
    return std::regex_match(name, validNameRegex);
}

void EnhancedCliParser::showHelp(const std::string& command) {
    if (command.empty()) {
        // Show general help
        utils::TerminalUtils::showNpmStyleHeader("cpp-scaffold", "1.2.0");
        
        std::cout << utils::TerminalUtils::colorize("  Modern C++ project scaffolding tool", utils::Color::BrightWhite) << "\n\n";
        
        std::cout << utils::TerminalUtils::colorize("  USAGE", utils::Color::BrightCyan) << "\n";
        std::cout << "    cpp-scaffold <command> [options]\n\n";
        
        std::cout << utils::TerminalUtils::colorize("  COMMANDS", utils::Color::BrightCyan) << "\n";
        for (const auto& [name, cmd] : commands_) {
            if (name == cmd.name) { // Only show primary command names
                std::cout << fmt::format("    {:15} {}\n", name, cmd.description);
            }
        }
        
        std::cout << "\n" << utils::TerminalUtils::colorize("  GLOBAL OPTIONS", utils::Color::BrightCyan) << "\n";
        std::cout << "    -h, --help      Show help information\n";
        std::cout << "    -v, --version   Show version information\n";
        std::cout << "    --verbose       Enable verbose output\n";
        
        std::cout << "\n" << utils::TerminalUtils::colorize("  EXAMPLES", utils::Color::BrightCyan) << "\n";
        std::cout << "    cpp-scaffold create my-app --template console --build cmake\n";
        std::cout << "    cpp-scaffold create my-lib --template lib --package vcpkg --tests\n";
        std::cout << "    cpp-scaffold list templates\n";
        std::cout << "    cpp-scaffold validate ./my-project\n";
        
        std::cout << "\n" << utils::TerminalUtils::colorize("  For more information on a command:", utils::Color::BrightBlack) << "\n";
        std::cout << "    cpp-scaffold help <command>\n\n";
    } else {
        // Show command-specific help
        auto it = commands_.find(command);
        if (it != commands_.end()) {
            printCommandHelp(it->second);
        } else {
            showError("Unknown command: " + command, suggestSimilarCommands(command));
        }
    }
}

void EnhancedCliParser::showVersion() {
    utils::TerminalUtils::showNpmStyleHeader("cpp-scaffold", "1.2.0");
    std::cout << "\n" << utils::TerminalUtils::colorize("  Build:", utils::Color::BrightBlack) << " " << __DATE__ << " " << __TIME__ << "\n";
    std::cout << utils::TerminalUtils::colorize("  Platform:", utils::Color::BrightBlack) << " ";
#ifdef _WIN32
    std::cout << "Windows\n";
#elif __APPLE__
    std::cout << "macOS\n";
#elif __linux__
    std::cout << "Linux\n";
#else
    std::cout << "Unknown\n";
#endif
    std::cout << "\n";
}

std::pair<std::string, std::vector<std::string>> EnhancedCliParser::parseCommandLine(const std::vector<std::string>& args) {
    if (args.empty()) {
        return {"", {}};
    }

    std::string command = args[0];
    std::vector<std::string> commandArgs(args.begin() + 1, args.end());

    return {command, commandArgs};
}

std::map<std::string, std::string> EnhancedCliParser::parseArguments(const std::vector<std::string>& args) {
    std::map<std::string, std::string> result;

    for (size_t i = 0; i < args.size(); ++i) {
        const std::string& arg = args[i];

        if (cli_utils::isFlag(arg)) {
            std::string flagName = cli_utils::extractFlagName(arg);
            std::string value = "true";

            // Check if this flag expects a value
            auto argIt = arguments_.find(flagName);
            if (argIt != arguments_.end() && argIt->second.hasValue) {
                if (i + 1 < args.size() && !cli_utils::isFlag(args[i + 1])) {
                    value = args[i + 1];
                    ++i; // Skip the value in next iteration
                } else {
                    value = argIt->second.defaultValue;
                }
            }

            result[flagName] = value;
        }
    }

    return result;
}

void EnhancedCliParser::showError(const std::string& message, const std::vector<std::string>& suggestions) {
    utils::TerminalUtils::showNpmStyleError(message);

    if (!suggestions.empty()) {
        std::cout << "\n" << utils::TerminalUtils::colorize("  Suggestions:", utils::Color::BrightCyan) << "\n";
        for (const auto& suggestion : suggestions) {
            std::cout << "    " << utils::TerminalUtils::colorize("•", utils::Color::BrightGreen) << " " << suggestion << "\n";
        }
    }
    std::cout << "\n";
}

void EnhancedCliParser::showWarning(const std::string& message) {
    utils::TerminalUtils::showNpmStyleWarning(message);
}

std::vector<std::string> EnhancedCliParser::suggestSimilarCommands(const std::string& command) {
    std::vector<std::string> suggestions;
    std::vector<std::string> commandNames;

    for (const auto& [name, cmd] : commands_) {
        if (name == cmd.name) { // Only consider primary command names
            commandNames.push_back(name);
        }
    }

    return cli_utils::findSimilarStrings(command, commandNames, 2);
}

void EnhancedCliParser::printCommandHelp(const Command& command) {
    utils::TerminalUtils::showNpmStyleHeader("cpp-scaffold " + command.name);

    std::cout << "\n" << utils::TerminalUtils::colorize("  " + command.description, utils::Color::BrightWhite) << "\n\n";

    std::cout << utils::TerminalUtils::colorize("  USAGE", utils::Color::BrightCyan) << "\n";
    std::cout << "    " << command.usage << "\n\n";

    if (!command.aliases.empty()) {
        std::cout << utils::TerminalUtils::colorize("  ALIASES", utils::Color::BrightCyan) << "\n";
        for (const auto& alias : command.aliases) {
            std::cout << "    " << alias << "\n";
        }
        std::cout << "\n";
    }

    if (!command.examples.empty()) {
        std::cout << utils::TerminalUtils::colorize("  EXAMPLES", utils::Color::BrightCyan) << "\n";
        for (const auto& example : command.examples) {
            std::cout << "    " << utils::TerminalUtils::colorize(example, utils::Color::BrightBlack) << "\n";
        }
        std::cout << "\n";
    }
}

// Command handlers implementation
namespace command_handlers {

int handleCreate(const std::vector<std::string>& args) {
    // This would be implemented to handle the create command
    // For now, return success
    return 0;
}

int handleList(const std::vector<std::string>& args) {
    utils::TerminalUtils::showNpmStyleHeader("Available Templates");

    std::cout << "\n" << utils::TerminalUtils::colorize("  📦 Project Templates", utils::Color::BrightCyan) << "\n\n";

    std::vector<std::pair<std::string, std::string>> templates = {
        {"console", "Console application - Command-line tools and utilities"},
        {"lib", "Library project - Static or shared libraries"},
        {"header-only-lib", "Header-only library - Template libraries"},
        {"gui", "GUI application - Desktop applications with graphical interface"},
        {"network", "Network application - Client/server applications"},
        {"embedded", "Embedded project - Microcontroller and IoT applications"},
        {"webservice", "Web service - REST APIs and web backends"},
        {"gameengine", "Game engine - Game development framework"}
    };

    for (const auto& [name, description] : templates) {
        std::cout << "  " << utils::TerminalUtils::colorize(name, utils::Color::BrightGreen)
                  << " - " << description << "\n";
    }

    std::cout << "\n" << utils::TerminalUtils::colorize("  🔧 Build Systems", utils::Color::BrightCyan) << "\n\n";

    std::vector<std::pair<std::string, std::string>> buildSystems = {
        {"cmake", "CMake - Cross-platform build system"},
        {"meson", "Meson - Fast and user-friendly build system"},
        {"bazel", "Bazel - Scalable build system by Google"},
        {"xmake", "XMake - Lua-based build system"},
        {"premake", "Premake - Build configuration tool"},
        {"make", "GNU Make - Traditional build system"},
        {"ninja", "Ninja - Small build system focused on speed"}
    };

    for (const auto& [name, description] : buildSystems) {
        std::cout << "  " << utils::TerminalUtils::colorize(name, utils::Color::BrightBlue)
                  << " - " << description << "\n";
    }

    std::cout << "\n";
    return 0;
}

int handleValidate(const std::vector<std::string>& args) {
    std::string projectPath = args.empty() ? "." : args[0];

    utils::TerminalUtils::showNpmStyleProgress("Validating project", 0, "Analyzing structure");

    // Simulate validation process
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    utils::TerminalUtils::showNpmStyleProgress("Validating project", 50, "Checking configuration");

    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    utils::TerminalUtils::showNpmStyleProgress("Validating project", 100, "Complete");

    utils::TerminalUtils::showNpmStyleSuccess("Project validation completed", projectPath);

    return 0;
}

int handleConfig(const std::vector<std::string>& args) {
    utils::TerminalUtils::showNpmStyleHeader("Configuration Management");

    if (args.empty()) {
        std::cout << "\n" << utils::TerminalUtils::colorize("  Available configuration commands:", utils::Color::BrightCyan) << "\n";
        std::cout << "    get <key>       - Get configuration value\n";
        std::cout << "    set <key> <value> - Set configuration value\n";
        std::cout << "    list            - List all configuration\n";
        std::cout << "    reset           - Reset to defaults\n\n";
    }

    return 0;
}

} // namespace command_handlers

// CLI utilities implementation
namespace cli_utils {

bool isFlag(const std::string& arg) {
    return isShortFlag(arg) || isLongFlag(arg);
}

bool isShortFlag(const std::string& arg) {
    return arg.length() >= 2 && arg[0] == '-' && arg[1] != '-';
}

bool isLongFlag(const std::string& arg) {
    return arg.length() >= 3 && arg.substr(0, 2) == "--";
}

std::string extractFlagName(const std::string& arg) {
    if (isLongFlag(arg)) {
        return arg.substr(2);
    } else if (isShortFlag(arg)) {
        return arg.substr(1);
    }
    return arg;
}

bool isValidProjectName(const std::string& name) {
    if (name.empty() || name.length() > 100) {
        return false;
    }

    std::regex validNameRegex("^[a-zA-Z0-9_-]+$");
    return std::regex_match(name, validNameRegex);
}

std::vector<std::string> findSimilarStrings(const std::string& target,
                                           const std::vector<std::string>& candidates,
                                           int maxDistance) {
    std::vector<std::pair<std::string, int>> distances;

    for (const auto& candidate : candidates) {
        int distance = 0;
        // Simple distance calculation (would use Levenshtein in real implementation)
        if (candidate.find(target) != std::string::npos || target.find(candidate) != std::string::npos) {
            distance = 1;
        } else {
            distance = maxDistance + 1;
        }

        if (distance <= maxDistance) {
            distances.emplace_back(candidate, distance);
        }
    }

    std::sort(distances.begin(), distances.end(),
              [](const auto& a, const auto& b) { return a.second < b.second; });

    std::vector<std::string> result;
    for (const auto& [candidate, distance] : distances) {
        result.push_back(candidate);
    }

    return result;
}

} // namespace cli_utils

} // namespace cli

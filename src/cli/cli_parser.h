#pragma once
#include <spdlog/fmt/fmt.h>

#include <filesystem>
#include <functional>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <variant>
#include <vector>

#include "../utils/ui/terminal_utils.h"  // 引入终端工具
#include "localization/localization.h"
#include "types/cli_enums.h"
#include "types/cli_options.h"

// Import the enums and types from the types module
using namespace cli_enums;

// All enum conversion functions are available from cli_enums namespace

// CliOptions is now imported from types/cli_options.h

// UserInput class is now available in input/user_input.h
// ConfigManager class is now available in config/config_manager.h

// ProjectWizard class is now available in wizard/project_wizard.h

// CliParser class declaration
class CliParser {
   public:
    static CliOptions parse(int argc, char* argv[]);
    static CliOptions promptUserForOptions(const CliOptions& defaultOptions = {});
    static bool validateOptions(CliOptions& options);
    static CliOptions runInteractiveMode();
    static CliOptions getSystemSuggestedDefaults();
    static CliOptions loadDefaultConfiguration();
    static bool validateProjectName(const std::string& name);
    static bool validateProjectPath(const std::string& path);
    static void handleParsingError(const std::string& error);
};

// fmt formatters are now imported from types/cli_enums.h

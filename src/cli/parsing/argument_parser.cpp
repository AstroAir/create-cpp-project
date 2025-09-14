#include "argument_parser.h"
#include "../localization/localization.h"
#include "../commands/command_handlers.h"
#include "../../config/config_manager.h"
#include "../../config/project_profiles.h"
#include "../../config/config_validator.h"
#include <algorithm>
#include <sstream>
#include <iostream>
#include <spdlog/spdlog.h>

namespace argument_parser {

// Main parsing function
CliOptions parseArguments(int argc, char* argv[]) {
  CliOptions options;

  // Initialize localization
  Localization::initialize();

  if (argc <= 1) {
    // No arguments - run interactive mode
    return cli_commands::runInteractiveMode();
  }

  // Convert to vector for easier handling
  std::vector<std::string> args;
  for (int i = 1; i < argc; ++i) {
    args.push_back(argv[i]);
  }

  // Detect command type
  CommandType command = detectCommand(args);

  // Handle special commands that don't need full parsing
  switch (command) {
    case CommandType::Help:
      options.showHelp = true;
      return options;
    case CommandType::Version:
      options.version = true;
      return options;
    case CommandType::Interactive:
      return cli_commands::runInteractiveMode();
    case CommandType::Config:
      cli_commands::runConfigurationWizard();
      options.showHelp = true; // Exit after configuration
      return options;
    case CommandType::ListTemplates:
      cli_commands::showAvailableTemplates();
      options.showHelp = true;
      return options;
    case CommandType::ListProfiles:
      cli_commands::showAvailableProfiles();
      options.showHelp = true;
      return options;
    case CommandType::ShowProfile:
      if (args.size() > 1) {
        cli_commands::showProfileInfo(args[1]);
      } else {
        std::cout << "â?Please specify a profile name\n";
        std::cout << "Usage: cpp-scaffold show-profile <profile-name>\n";
      }
      options.showHelp = true;
      return options;
    case CommandType::Validate:
      if (args.size() > 1) {
        cli_commands::validateProject(args[1]);
      } else {
        cli_commands::validateProject(".");
      }
      options.showHelp = true;
      return options;
    case CommandType::ValidateConfig:
      // Parse remaining arguments to get configuration
      options = parseArguments(argc, argv);
      cli_commands::validateConfiguration(options);
      options.showHelp = true;
      return options;
    case CommandType::Unknown:
      spdlog::error("Unknown command: {}", args[0]);
      options.showHelp = true;
      return options;
    default:
      break;
  }

  // Extract project name for create/new commands
  options.projectName = extractProjectName(args, command);

  // Parse arguments by category
  for (size_t i = (command == CommandType::Create || command == CommandType::New) ? 2 : 1;
       i < args.size(); ++i) {

    if (parseBasicOptions(options, args, i)) continue;
    if (parseProjectOptions(options, args, i)) continue;
    if (parseBuildOptions(options, args, i)) continue;
    if (parseTestingOptions(options, args, i)) continue;
    if (parseDevToolOptions(options, args, i)) continue;
    if (parseGitOptions(options, args, i)) continue;
    if (parseDocumentationOptions(options, args, i)) continue;
    if (parseAdvancedOptions(options, args, i)) continue;
    if (parseRemoteSourceOptions(options, args, i)) continue;

    // Unknown argument
    spdlog::warn("Unknown argument: {}", args[i]);
  }

  // Apply profile if specified
  if (!options.profileName.empty()) {
    auto& profileManager = config::ProjectProfileManager::getInstance();
    profileManager.loadBuiltInProfiles();

    CliOptions profileOptions = profileManager.applyProfile(options.profileName, options);
    if (profileOptions.projectName != options.projectName) {
      // Profile was found and applied
      options = profileOptions;
      // Restore the project name from command line
      if (!options.projectName.empty()) {
        options.projectName = profileOptions.projectName;
      }
    }
  }

  // Validate configuration if requested
  if (options.validateConfig) {
    auto& validator = config::ConfigValidator::getInstance();
    if (options.strictValidation) {
      validator.setStrictMode(true);
    }

    auto result = validator.validateConfiguration(options);
    config::validation_utils::printValidationResult(result);

    if (!result.isValid && options.strictValidation) {
      std::cout << "â?Configuration validation failed in strict mode. Aborting.\n";
      exit(1);
    }
  }

  // If no project name and it's a create command, prompt for options
  if (options.projectName.empty() &&
      (command == CommandType::Create || command == CommandType::New)) {
    // This would typically call the interactive prompt
    return cli_commands::runInteractiveMode();
  }

  return options;
}

// Parse basic options (help, version, verbose, language)
bool parseBasicOptions(CliOptions& options, const std::vector<std::string>& args, size_t& index) {
  const std::string& arg = args[index];

  if (arg == "--help" || arg == "-h") {
    options.showHelp = true;
    return true;
  }

  if (arg == "--version" || arg == "-v") {
    options.version = true;
    return true;
  }

  if (arg == "--verbose") {
    options.verbose = true;
    return true;
  }

  if (arg == "--language" || arg == "-l") {
    if (hasValue(args, index)) {
      std::string langStr = getNextValue(args, index);
      auto language = cli_enums::to_language(langStr);
      if (language) {
        options.language = *language;
        Localization::setCurrentLanguage(options.language);
      }
    }
    return true;
  }

  return false;
}

// Parse project-specific options
bool parseProjectOptions(CliOptions& options, const std::vector<std::string>& args, size_t& index) {
  const std::string& arg = args[index];

  if (arg == "--template" || arg == "-t") {
    if (hasValue(args, index)) {
      std::string templateStr = getNextValue(args, index);
      auto templateType = cli_enums::to_template_type(templateStr);
      if (templateType) {
        options.templateType = *templateType;
      }
    }
    return true;
  }

  if (arg == "--template-path") {
    if (hasValue(args, index)) {
      options.customTemplatePath = getNextValue(args, index);
    }
    return true;
  }

  if (arg == "--profile") {
    if (hasValue(args, index)) {
      options.profileName = getNextValue(args, index);
    }
    return true;
  }

  return false;
}

// Parse build system options
bool parseBuildOptions(CliOptions& options, const std::vector<std::string>& args, size_t& index) {
  const std::string& arg = args[index];

  if (arg == "--build" || arg == "-b") {
    if (hasValue(args, index)) {
      std::string buildStr = getNextValue(args, index);
      auto buildSystem = cli_enums::to_build_system(buildStr);
      if (buildSystem) {
        options.buildSystem = *buildSystem;
      }
    }
    return true;
  }

  if (arg == "--package" || arg == "-p") {
    if (hasValue(args, index)) {
      std::string packageStr = getNextValue(args, index);
      auto packageManager = cli_enums::to_package_manager(packageStr);
      if (packageManager) {
        options.packageManager = *packageManager;
      }
    }
    return true;
  }

  if (arg == "--std") {
    if (hasValue(args, index)) {
      std::string stdStr = getNextValue(args, index);
      auto cppStandard = cli_enums::to_cpp_standard(stdStr);
      if (cppStandard) {
        options.cppStandard = *cppStandard;
      }
    }
    return true;
  }

  if (arg == "--network-lib") {
    if (hasValue(args, index)) {
      options.networkLibrary = getNextValue(args, index);
    }
    return true;
  }

  return false;
}

// Parse testing options
bool parseTestingOptions(CliOptions& options, const std::vector<std::string>& args, size_t& index) {
  const std::string& arg = args[index];

  if (arg == "--tests") {
    options.includeTests = true;
    if (hasValue(args, index) && !isFlag(args[index + 1])) {
      std::string testStr = getNextValue(args, index);
      auto testFramework = cli_enums::to_test_framework(testStr);
      if (testFramework) {
        options.testFramework = *testFramework;
      }
    }
    return true;
  }

  return false;
}

// Parse development tool options
bool parseDevToolOptions(CliOptions& options, const std::vector<std::string>& args, size_t& index) {
  const std::string& arg = args[index];

  if (arg == "--docs" || arg == "--documentation") {
    options.includeDocumentation = true;
    return true;
  }

  if (arg == "--code-style") {
    options.includeCodeStyleTools = true;
    return true;
  }

  if (arg == "--editor") {
    if (hasValue(args, index)) {
      std::string editorStr = getNextValue(args, index);
      auto editor = cli_enums::to_editor_config(editorStr);
      if (editor) {
        options.editorOptions.push_back(*editor);
      }
    }
    return true;
  }

  if (arg == "--ci" || arg == "--cicd") {
    if (hasValue(args, index)) {
      std::string ciStr = getNextValue(args, index);
      auto ci = cli_enums::to_ci_system(ciStr);
      if (ci) {
        options.ciOptions.push_back(*ci);
      }
    }
    return true;
  }

  return false;
}

// Parse Git-related options
bool parseGitOptions(CliOptions& options, const std::vector<std::string>& args, size_t& index) {
  const std::string& arg = args[index];

  if (arg == "--no-git") {
    options.initGit = false;
    return true;
  }

  if (arg == "--git-workflow") {
    if (hasValue(args, index)) {
      std::string workflowStr = getNextValue(args, index);
      auto workflow = cli_enums::to_git_workflow(workflowStr);
      if (workflow) {
        options.gitWorkflow = *workflow;
      }
    }
    return true;
  }

  if (arg == "--git-branch-strategy") {
    if (hasValue(args, index)) {
      std::string strategyStr = getNextValue(args, index);
      auto strategy = cli_enums::to_git_branch_strategy(strategyStr);
      if (strategy) {
        options.gitBranchStrategy = *strategy;
      }
    }
    return true;
  }

  if (arg == "--license") {
    if (hasValue(args, index)) {
      std::string licenseStr = getNextValue(args, index);
      auto license = cli_enums::to_license_type(licenseStr);
      if (license) {
        options.licenseType = *license;
      }
    }
    return true;
  }

  if (arg == "--git-remote") {
    if (hasValue(args, index)) {
      options.gitRemoteUrl = getNextValue(args, index);
    }
    return true;
  }

  if (arg == "--git-user") {
    if (hasValue(args, index)) {
      options.gitUserName = getNextValue(args, index);
    }
    return true;
  }

  if (arg == "--git-email") {
    if (hasValue(args, index)) {
      options.gitUserEmail = getNextValue(args, index);
    }
    return true;
  }

  if (arg == "--git-hooks") {
    options.setupGitHooks = true;
    return true;
  }

  if (arg == "--no-initial-commit") {
    options.createInitialCommit = false;
    return true;
  }

  return false;
}

// Parse documentation options
bool parseDocumentationOptions(CliOptions& options, const std::vector<std::string>& args, size_t& index) {
  const std::string& arg = args[index];

  if (arg == "--doc-formats") {
    if (hasValue(args, index)) {
      std::string formats = getNextValue(args, index);
      options.docFormats = parseCommaSeparatedList(formats);
    }
    return true;
  }

  if (arg == "--doc-types") {
    if (hasValue(args, index)) {
      std::string types = getNextValue(args, index);
      options.docTypes = parseCommaSeparatedList(types);
    }
    return true;
  }

  if (arg == "--doxygen") {
    options.generateDoxygen = true;
    return true;
  }

  if (arg == "--doxygen-theme") {
    if (hasValue(args, index)) {
      options.doxygenTheme = getNextValue(args, index);
    }
    return true;
  }

  if (arg == "--no-code-examples") {
    options.includeCodeExamples = false;
    return true;
  }

  if (arg == "--changelog") {
    options.generateChangelog = true;
    return true;
  }

  return false;
}

// Parse advanced options
bool parseAdvancedOptions(CliOptions& options, const std::vector<std::string>& args, size_t& index) {
  const std::string& arg = args[index];

  if (arg == "--gui-frameworks") {
    if (hasValue(args, index)) {
      std::string frameworks = getNextValue(args, index);
      options.guiFrameworks = parseCommaSeparatedList(frameworks);
    }
    return true;
  }

  if (arg == "--game-frameworks") {
    if (hasValue(args, index)) {
      std::string frameworks = getNextValue(args, index);
      options.gameFrameworks = parseCommaSeparatedList(frameworks);
    }
    return true;
  }

  if (arg == "--graphics-libs") {
    if (hasValue(args, index)) {
      std::string libs = getNextValue(args, index);
      options.graphicsLibraries = parseCommaSeparatedList(libs);
    }
    return true;
  }

  if (arg == "--include-shaders") {
    options.includeShaders = true;
    return true;
  }

  if (arg == "--validate-config") {
    options.validateConfig = true;
    return true;
  }

  if (arg == "--strict-validation") {
    options.strictValidation = true;
    return true;
  }

  return false;
}

// Helper functions
bool isFlag(const std::string& arg) {
  return !arg.empty() && arg[0] == '-';
}

bool hasValue(const std::vector<std::string>& args, size_t index) {
  return index + 1 < args.size() && !isFlag(args[index + 1]);
}

std::string getNextValue(const std::vector<std::string>& args, size_t& index) {
  if (hasValue(args, index)) {
    return args[++index];
  }
  return "";
}

std::vector<std::string> parseCommaSeparatedList(const std::string& value) {
  std::vector<std::string> result;
  std::stringstream ss(value);
  std::string item;

  while (std::getline(ss, item, ',')) {
    // Trim whitespace
    item.erase(0, item.find_first_not_of(" \t"));
    item.erase(item.find_last_not_of(" \t") + 1);
    if (!item.empty()) {
      result.push_back(item);
    }
  }

  return result;
}

// Command detection
CommandType detectCommand(const std::vector<std::string>& args) {
  if (args.empty()) return CommandType::Interactive;

  const std::string& command = args[0];

  if (command == "help" || command == "--help" || command == "-h") {
    return CommandType::Help;
  }
  if (command == "version" || command == "--version" || command == "-v") {
    return CommandType::Version;
  }
  if (command == "interactive" || command == "-i") {
    return CommandType::Interactive;
  }
  if (command == "config" || command == "configure") {
    return CommandType::Config;
  }
  if (command == "list-templates" || command == "list") {
    return CommandType::ListTemplates;
  }
  if (command == "list-profiles" || command == "profiles") {
    return CommandType::ListProfiles;
  }
  if (command == "show-profile" || command == "profile-info") {
    return CommandType::ShowProfile;
  }
  if (command == "validate") {
    return CommandType::Validate;
  }
  if (command == "validate-config") {
    return CommandType::ValidateConfig;
  }
  if (command == "create") {
    return CommandType::Create;
  }
  if (command == "new") {
    return CommandType::New;
  }

  return CommandType::Unknown;
}

std::string extractProjectName(const std::vector<std::string>& args, CommandType command) {
  if ((command == CommandType::Create || command == CommandType::New) && args.size() > 1) {
    return args[1];
  }
  return "";
}

// Parse remote source options
bool parseRemoteSourceOptions(CliOptions& options, const std::vector<std::string>& args, size_t& index) {
  const std::string& arg = args[index];

  if (arg == "--from-git" || arg == "--git-repo") {
    if (hasValue(args, index)) {
      options.gitRepositoryUrl = getNextValue(args, index);
      options.sourceType = SourceType::GitRepository;
    }
    return true;
  }

  if (arg == "--from-archive" || arg == "--archive") {
    if (hasValue(args, index)) {
      std::string archiveSource = getNextValue(args, index);
      // Check if it's a URL or local file path
      if (archiveSource.substr(0, 7) == "http://" || archiveSource.substr(0, 8) == "https://" ||
          archiveSource.substr(0, 6) == "ftp://") {
        options.archiveUrl = archiveSource;
      } else {
        options.archiveFilePath = archiveSource;
      }
      options.sourceType = SourceType::Archive;
    }
    return true;
  }

  if (arg == "--branch" || arg == "-b") {
    if (hasValue(args, index)) {
      options.gitBranch = getNextValue(args, index);
    }
    return true;
  }

  if (arg == "--tag" || arg == "-t") {
    if (hasValue(args, index)) {
      options.gitTag = getNextValue(args, index);
    }
    return true;
  }

  if (arg == "--commit") {
    if (hasValue(args, index)) {
      options.gitCommit = getNextValue(args, index);
    }
    return true;
  }

  if (arg == "--shallow") {
    options.useShallowClone = true;
    return true;
  }

  if (arg == "--no-shallow") {
    options.useShallowClone = false;
    return true;
  }

  if (arg == "--preserve-git") {
    options.preserveGitHistory = true;
    return true;
  }

  if (arg == "--git-username") {
    if (hasValue(args, index)) {
      options.gitUsername = getNextValue(args, index);
    }
    return true;
  }

  if (arg == "--git-password") {
    if (hasValue(args, index)) {
      options.gitPassword = getNextValue(args, index);
    }
    return true;
  }

  if (arg == "--ssh-key") {
    if (hasValue(args, index)) {
      options.sshKeyPath = getNextValue(args, index);
    }
    return true;
  }

  return false;
}

} // namespace argument_parser

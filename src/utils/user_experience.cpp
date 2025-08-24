#include "user_experience.h"
#include "terminal_utils.h"
#include "file_utils.h"
#include <fmt/format.h>
#include <spdlog/spdlog.h>
#include <algorithm>
#include <filesystem>
#include <fstream>

namespace utils {

UserExperienceManager& UserExperienceManager::getInstance() {
    static UserExperienceManager instance;
    static bool initialized = false;
    
    if (!initialized) {
        instance.initializeTutorials();
        instance.initializeHelpEntries();
        instance.initializeCompletionData();
        instance.loadUserPreferences();
        initialized = true;
    }
    
    return instance;
}

ExperienceLevel UserExperienceManager::detectExperienceLevel() {
    // Detect experience level based on various factors
    int experienceScore = 0;
    
    // Check for existing projects
    if (hasExistingProjects()) {
        experienceScore += 2;
    }
    
    // Check for advanced tools
    if (hasAdvancedToolsInstalled()) {
        experienceScore += 3;
    }
    
    // Check history
    experienceScore += estimateExperienceFromHistory();
    
    // Determine level based on score
    if (experienceScore >= 8) {
        return ExperienceLevel::Expert;
    } else if (experienceScore >= 5) {
        return ExperienceLevel::Advanced;
    } else if (experienceScore >= 2) {
        return ExperienceLevel::Intermediate;
    } else {
        return ExperienceLevel::Beginner;
    }
}

CliOptions UserExperienceManager::runGuidedSetup() {
    TerminalUtils::showNpmStyleHeader("Guided Project Setup");
    
    // Detect or ask for experience level
    if (userLevel_ == ExperienceLevel::Beginner) {
        userLevel_ = detectExperienceLevel();
    }
    
    ux_utils::showBanner("Welcome to C++ Project Scaffold!", 
                        "Let's create your perfect C++ project step by step");
    
    CliOptions options;
    
    // Step 1: Project basics
    TerminalUtils::showWizardHeader("Project Setup", 1, 5);
    TerminalUtils::showWizardProgress(1, 5, "Project Basics");
    
    std::cout << "\n" << TerminalUtils::colorize("  📝 Project Information", Color::BrightCyan) << "\n\n";
    
    // Project name with validation and suggestions
    std::string projectName;
    bool validName = false;
    while (!validName) {
        projectName = ux_utils::askInput("Project name", "my-awesome-project");
        
        if (projectName.empty() || projectName.length() > 100) {
            TerminalUtils::showNpmStyleError("Project name must be between 1 and 100 characters");
            continue;
        }
        
        // Check for valid characters
        bool hasInvalidChars = false;
        for (char c : projectName) {
            if (!std::isalnum(c) && c != '-' && c != '_') {
                hasInvalidChars = true;
                break;
            }
        }
        
        if (hasInvalidChars) {
            TerminalUtils::showNpmStyleError("Project name can only contain letters, numbers, hyphens, and underscores");
            continue;
        }
        
        validName = true;
    }
    options.projectName = projectName;
    
    // Step 2: Template selection with guidance
    TerminalUtils::showWizardHeader("Project Setup", 2, 5);
    TerminalUtils::showWizardProgress(2, 5, "Template Selection");
    
    showContextualHelp(GuidanceContext::TemplateSelection);
    
    std::vector<std::pair<std::string, std::string>> templateOptions = {
        {"console", "Perfect for command-line tools, utilities, and learning C++"},
        {"lib", "Ideal for creating reusable libraries and components"},
        {"header-only-lib", "Best for template libraries and header-only code"},
        {"gui", "Great for desktop applications with graphical interfaces"},
        {"network", "Excellent for client-server applications and networking"},
        {"embedded", "Optimized for microcontrollers and resource-constrained systems"},
        {"webservice", "Perfect for REST APIs and web backends"},
        {"gameengine", "Comprehensive framework for game development"}
    };
    
    std::cout << "\n" << TerminalUtils::colorize("  🎯 Choose Your Project Type", Color::BrightCyan) << "\n\n";
    
    for (size_t i = 0; i < templateOptions.size(); ++i) {
        const auto& [type, description] = templateOptions[i];
        std::cout << "  " << TerminalUtils::colorize(std::to_string(i + 1) + ".", Color::BrightBlack)
                  << " " << TerminalUtils::colorize(type, Color::BrightGreen)
                  << "\n    " << TerminalUtils::colorize(description, Color::BrightWhite) << "\n\n";
    }
    
    int templateChoice = ux_utils::askChoice("Select template", {}, 0) - 1;
    if (templateChoice >= 0 && templateChoice < static_cast<int>(templateOptions.size())) {
        std::string selectedTemplate = templateOptions[templateChoice].first;
        auto templateType = enums::to_template_type(selectedTemplate);
        if (templateType) {
            options.templateType = *templateType;
            TerminalUtils::showNpmStyleSuccess("Template selected", selectedTemplate);
        }
    }
    
    // Step 3: Build system selection
    TerminalUtils::showWizardHeader("Project Setup", 3, 5);
    TerminalUtils::showWizardProgress(3, 5, "Build System");
    
    showContextualHelp(GuidanceContext::BuildSystemSetup);
    
    std::vector<std::pair<std::string, std::string>> buildSystemOptions = {
        {"cmake", "Most popular, excellent cross-platform support, great ecosystem"},
        {"meson", "Fast, user-friendly, modern Python-based build system"},
        {"bazel", "Scalable, used by Google, excellent for large projects"},
        {"xmake", "Lua-based, simple configuration, good for C++ projects"}
    };
    
    std::cout << "\n" << TerminalUtils::colorize("  🔧 Build System Selection", Color::BrightCyan) << "\n\n";
    
    for (size_t i = 0; i < buildSystemOptions.size(); ++i) {
        const auto& [system, description] = buildSystemOptions[i];
        std::cout << "  " << TerminalUtils::colorize(std::to_string(i + 1) + ".", Color::BrightBlack)
                  << " " << TerminalUtils::colorize(system, Color::BrightBlue)
                  << "\n    " << TerminalUtils::colorize(description, Color::BrightWhite) << "\n\n";
    }
    
    int buildChoice = ux_utils::askChoice("Select build system", {}, 1) - 1;
    if (buildChoice >= 0 && buildChoice < static_cast<int>(buildSystemOptions.size())) {
        std::string selectedBuild = buildSystemOptions[buildChoice].first;
        auto buildSystem = enums::to_build_system(selectedBuild);
        if (buildSystem) {
            options.buildSystem = *buildSystem;
            TerminalUtils::showNpmStyleSuccess("Build system selected", selectedBuild);
        }
    }
    
    // Step 4: Package management
    TerminalUtils::showWizardHeader("Project Setup", 4, 5);
    TerminalUtils::showWizardProgress(4, 5, "Package Management");
    
    showContextualHelp(GuidanceContext::DependencyManagement);
    
    std::vector<std::pair<std::string, std::string>> packageOptions = {
        {"vcpkg", "Microsoft's package manager, excellent Windows support, growing ecosystem"},
        {"conan", "Cross-platform, mature, supports many configurations"},
        {"none", "Manual dependency management, full control"},
        {"cpm", "CMake-based, header-only friendly, simple setup"}
    };
    
    std::cout << "\n" << TerminalUtils::colorize("  📦 Package Management", Color::BrightCyan) << "\n\n";
    
    for (size_t i = 0; i < packageOptions.size(); ++i) {
        const auto& [manager, description] = packageOptions[i];
        std::cout << "  " << TerminalUtils::colorize(std::to_string(i + 1) + ".", Color::BrightBlack)
                  << " " << TerminalUtils::colorize(manager, Color::BrightMagenta)
                  << "\n    " << TerminalUtils::colorize(description, Color::BrightWhite) << "\n\n";
    }
    
    int packageChoice = ux_utils::askChoice("Select package manager", {}, 1) - 1;
    if (packageChoice >= 0 && packageChoice < static_cast<int>(packageOptions.size())) {
        std::string selectedPackage = packageOptions[packageChoice].first;
        auto packageManager = enums::to_package_manager(selectedPackage);
        if (packageManager) {
            options.packageManager = *packageManager;
            TerminalUtils::showNpmStyleSuccess("Package manager selected", selectedPackage);
        }
    }
    
    // Step 5: Additional features
    TerminalUtils::showWizardHeader("Project Setup", 5, 5);
    TerminalUtils::showWizardProgress(5, 5, "Additional Features");
    
    std::cout << "\n" << TerminalUtils::colorize("  ✨ Additional Features", Color::BrightCyan) << "\n\n";
    
    std::vector<std::string> featureOptions = {
        "Include testing framework (recommended)",
        "Include documentation setup",
        "Include code style tools (clang-format, clang-tidy)",
        "Initialize Git repository",
        "Enable verbose output"
    };
    
    std::vector<bool> defaultFeatures = {true, false, true, true, false};
    
    auto selectedFeatures = TerminalUtils::showMultiSelectDialog(
        "Select additional features", featureOptions, defaultFeatures);
    
    // Apply feature selections
    options.includeTests = std::find(selectedFeatures.begin(), selectedFeatures.end(), 
                                   "Include testing framework (recommended)") != selectedFeatures.end();
    options.includeDocumentation = std::find(selectedFeatures.begin(), selectedFeatures.end(), 
                                            "Include documentation setup") != selectedFeatures.end();
    options.includeCodeStyleTools = std::find(selectedFeatures.begin(), selectedFeatures.end(), 
                                             "Include code style tools (clang-format, clang-tidy)") != selectedFeatures.end();
    options.initGit = std::find(selectedFeatures.begin(), selectedFeatures.end(), 
                               "Initialize Git repository") != selectedFeatures.end();
    options.verbose = std::find(selectedFeatures.begin(), selectedFeatures.end(), 
                               "Enable verbose output") != selectedFeatures.end();
    
    // Show configuration summary
    TerminalUtils::showWizardSummary({
        {"Project Name", options.projectName},
        {"Template", std::string(enums::to_string(options.templateType))},
        {"Build System", std::string(enums::to_string(options.buildSystem))},
        {"Package Manager", std::string(enums::to_string(options.packageManager))},
        {"Include Tests", options.includeTests ? "Yes" : "No"},
        {"Include Docs", options.includeDocumentation ? "Yes" : "No"},
        {"Code Style Tools", options.includeCodeStyleTools ? "Yes" : "No"},
        {"Initialize Git", options.initGit ? "Yes" : "No"}
    });
    
    if (ux_utils::askYesNo("Create project with these settings?", true)) {
        TerminalUtils::showNpmStyleSuccess("Configuration confirmed!");
        trackProgress("guided_setup_completed", options);
        return options;
    } else {
        TerminalUtils::showNpmStyleWarning("Setup cancelled");
        return {};
    }
}

void UserExperienceManager::showContextualHelp(GuidanceContext context) {
    if (!shouldShowGuidance(context)) {
        return;
    }
    
    switch (context) {
        case GuidanceContext::TemplateSelection:
            ux_utils::showTip("💡 Choose a template that matches your project goals. You can always add features later!");
            break;
            
        case GuidanceContext::BuildSystemSetup:
            ux_utils::showTip("🔧 CMake is recommended for beginners due to its widespread adoption and excellent documentation.");
            break;
            
        case GuidanceContext::DependencyManagement:
            ux_utils::showTip("📦 vcpkg is great for Windows users, while Conan offers excellent cross-platform support.");
            break;
            
        default:
            break;
    }
}

std::vector<std::string> UserExperienceManager::getNextSteps(const CliOptions& options) {
    std::vector<std::string> steps;
    
    steps.push_back("Navigate to your project directory: cd " + options.projectName);
    
    if (options.buildSystem == BuildSystem::CMake) {
        steps.push_back("Create build directory: mkdir build && cd build");
        steps.push_back("Configure project: cmake ..");
        steps.push_back("Build project: cmake --build .");
    } else if (options.buildSystem == BuildSystem::Meson) {
        steps.push_back("Setup build directory: meson setup build");
        steps.push_back("Build project: meson compile -C build");
    }
    
    if (options.includeTests) {
        steps.push_back("Run tests: ctest (for CMake) or meson test -C build (for Meson)");
    }
    
    if (options.initGit) {
        steps.push_back("Make initial commit: git add . && git commit -m \"Initial commit\"");
    }
    
    steps.push_back("Start coding in src/ directory");
    steps.push_back("Add dependencies as needed");
    
    if (options.includeDocumentation) {
        steps.push_back("Generate documentation: doxygen (if configured)");
    }
    
    return steps;
}

void UserExperienceManager::showPostCreationGuide(const CliOptions& options) {
    TerminalUtils::showNpmStyleHeader("🎉 Project Created Successfully!");
    
    std::cout << "\n" << TerminalUtils::colorize("  Your C++ project is ready! Here's what to do next:", Color::BrightWhite) << "\n\n";
    
    auto nextSteps = getNextSteps(options);
    
    for (size_t i = 0; i < nextSteps.size(); ++i) {
        std::cout << "  " << TerminalUtils::colorize(std::to_string(i + 1) + ".", Color::BrightGreen)
                  << " " << nextSteps[i] << "\n";
    }
    
    std::cout << "\n" << TerminalUtils::colorize("  📚 Helpful Resources:", Color::BrightCyan) << "\n";
    std::cout << "    • Project documentation: docs/README.md\n";
    std::cout << "    • Build system guide: docs/building.md\n";
    std::cout << "    • Contributing guidelines: CONTRIBUTING.md\n";
    
    if (options.templateType == TemplateType::GameEngine) {
        std::cout << "    • Game development tutorial: docs/game-dev-guide.md\n";
    } else if (options.templateType == TemplateType::Lib) {
        std::cout << "    • Library development guide: docs/library-guide.md\n";
    }
    
    std::cout << "\n" << TerminalUtils::colorize("  🚀 Quick Start:", Color::BrightYellow) << "\n";
    std::cout << "    cpp-scaffold --help     # Show all available commands\n";
    std::cout << "    cpp-scaffold validate   # Validate your project structure\n";
    std::cout << "    cpp-scaffold examples   # Browse example projects\n\n";
    
    if (ux_utils::askYesNo("Would you like to see a quick tutorial for your project type?", false)) {
        std::string tutorialName = "basic-" + std::string(enums::to_string(options.templateType));
        runTutorial(tutorialName);
    }
}

void UserExperienceManager::initializeTutorials() {
    // Basic console application tutorial
    Tutorial consoleTutorial;
    consoleTutorial.name = "basic-console";
    consoleTutorial.description = "Learn to build and run your first console application";
    consoleTutorial.category = "Getting Started";
    consoleTutorial.targetLevel = ExperienceLevel::Beginner;
    consoleTutorial.estimatedTimeMinutes = 10;
    
    TutorialStep step1;
    step1.title = "Build Your Project";
    step1.description = "Compile your console application";
    step1.instructions = {
        "Navigate to your project directory",
        "Create a build directory: mkdir build && cd build",
        "Configure with CMake: cmake ..",
        "Build the project: cmake --build ."
    };
    step1.examples = {
        "cd my-project",
        "mkdir build && cd build",
        "cmake ..",
        "cmake --build ."
    };
    step1.tips = {
        "Always build in a separate directory to keep source clean",
        "Use cmake --build . for cross-platform building"
    };
    
    consoleTutorial.steps.push_back(step1);
    tutorials_[consoleTutorial.name] = consoleTutorial;
}

void UserExperienceManager::initializeHelpEntries() {
    // Template selection help
    HelpEntry templateHelp;
    templateHelp.topic = "template-selection";
    templateHelp.shortDescription = "Choosing the right project template";
    templateHelp.detailedDescription = "Project templates provide a starting point with appropriate structure and configuration for different types of C++ projects.";
    templateHelp.context = GuidanceContext::TemplateSelection;
    templateHelp.examples = {
        "console - For command-line tools and utilities",
        "lib - For reusable libraries and components",
        "gui - For desktop applications with graphical interfaces"
    };
    templateHelp.relatedTopics = {"build-systems", "project-structure"};
    helpEntries_[templateHelp.topic] = templateHelp;
}

void UserExperienceManager::initializeCompletionData() {
    // Command completions
    std::vector<CompletionSuggestion> commandCompletions = {
        {"create", "Create a new C++ project", "commands", 10, true},
        {"list", "List available templates and options", "commands", 8, true},
        {"validate", "Validate project structure", "commands", 6, true},
        {"config", "Configure default settings", "commands", 7, true},
        {"help", "Show help information", "commands", 9, true}
    };
    
    completionData_["commands"] = commandCompletions;
}

bool UserExperienceManager::shouldShowGuidance(GuidanceContext context) {
    // Show guidance for beginners and intermediate users
    if (userLevel_ == ExperienceLevel::Expert) {
        return false;
    }
    
    // Check user preferences
    auto it = guidancePreferences_.find(context);
    if (it != guidancePreferences_.end()) {
        return it->second;
    }
    
    // Default to showing guidance for beginners
    return userLevel_ == ExperienceLevel::Beginner;
}

bool UserExperienceManager::hasExistingProjects() {
    // Check for common C++ project indicators in current and parent directories
    std::vector<std::string> projectIndicators = {
        "CMakeLists.txt", "meson.build", "Makefile", "BUILD", "WORKSPACE",
        "conanfile.txt", "vcpkg.json", ".git"
    };

    std::filesystem::path currentPath = std::filesystem::current_path();

    // Check current directory and up to 3 parent directories
    for (int i = 0; i < 4; ++i) {
        for (const auto& indicator : projectIndicators) {
            if (std::filesystem::exists(currentPath / indicator)) {
                return true;
            }
        }

        auto parentPath = currentPath.parent_path();
        if (parentPath == currentPath) break; // Reached root
        currentPath = parentPath;
    }

    return false;
}

bool UserExperienceManager::hasAdvancedToolsInstalled() {
    // Check for advanced development tools
    std::vector<std::string> advancedTools = {
        "clang-tidy", "clang-format", "cppcheck", "valgrind", "gdb", "lldb"
    };

    int toolsFound = 0;
    for (const auto& tool : advancedTools) {
        // Simple check - in real implementation would check PATH
        if (system(("which " + tool + " > /dev/null 2>&1").c_str()) == 0) {
            toolsFound++;
        }
    }

    return toolsFound >= 3; // Consider advanced if 3+ tools are available
}

int UserExperienceManager::estimateExperienceFromHistory() {
    // Check for signs of previous C++ development experience
    int score = 0;

    // Check for .cpp-scaffold directory (previous usage)
    std::filesystem::path configDir = std::filesystem::path(std::getenv("HOME") ? std::getenv("HOME") : ".") / ".cpp-scaffold";
    if (std::filesystem::exists(configDir)) {
        score += 2;
    }

    // Check for common C++ directories in home
    std::filesystem::path homeDir = std::getenv("HOME") ? std::getenv("HOME") : ".";
    std::vector<std::string> cppDirs = {"src", "projects", "code", "development", "cpp", "c++"};

    for (const auto& dir : cppDirs) {
        if (std::filesystem::exists(homeDir / dir)) {
            score += 1;
        }
    }

    return std::min(score, 5); // Cap at 5 points
}

void UserExperienceManager::trackProgress(const std::string& action, const CliOptions& options) {
    completedActions_[action] = true;

    // Award achievements
    if (action == "guided_setup_completed" && !hasCompletedAction("first_project")) {
        achievements_.push_back("First Project Created! 🎉");
        completedActions_["first_project"] = true;
    }

    if (options.includeTests && !hasCompletedAction("testing_advocate")) {
        achievements_.push_back("Testing Advocate! 🧪");
        completedActions_["testing_advocate"] = true;
    }

    if (options.includeCodeStyleTools && !hasCompletedAction("code_quality_champion")) {
        achievements_.push_back("Code Quality Champion! ✨");
        completedActions_["code_quality_champion"] = true;
    }

    saveUserPreferences();
}

bool UserExperienceManager::hasCompletedAction(const std::string& action) {
    auto it = completedActions_.find(action);
    return it != completedActions_.end() && it->second;
}

void UserExperienceManager::loadUserPreferences() {
    // Load user preferences from config file
    std::filesystem::path configFile = std::filesystem::path(std::getenv("HOME") ? std::getenv("HOME") : ".") / ".cpp-scaffold" / "user_experience.json";

    if (std::filesystem::exists(configFile)) {
        try {
            std::ifstream file(configFile);
            nlohmann::json config;
            file >> config;

            if (config.contains("experienceLevel")) {
                userLevel_ = static_cast<ExperienceLevel>(config["experienceLevel"].get<int>());
            }

            if (config.contains("completedActions")) {
                for (const auto& [action, completed] : config["completedActions"].items()) {
                    completedActions_[action] = completed.get<bool>();
                }
            }

            if (config.contains("achievements")) {
                achievements_ = config["achievements"].get<std::vector<std::string>>();
            }

        } catch (const std::exception& e) {
            spdlog::warn("Failed to load user preferences: {}", e.what());
        }
    }
}

void UserExperienceManager::saveUserPreferences() {
    std::filesystem::path configDir = std::filesystem::path(std::getenv("HOME") ? std::getenv("HOME") : ".") / ".cpp-scaffold";
    std::filesystem::path configFile = configDir / "user_experience.json";

    try {
        std::filesystem::create_directories(configDir);

        nlohmann::json config;
        config["experienceLevel"] = static_cast<int>(userLevel_);
        config["completedActions"] = completedActions_;
        config["achievements"] = achievements_;
        config["lastUpdated"] = std::time(nullptr);

        std::ofstream file(configFile);
        file << config.dump(2);

    } catch (const std::exception& e) {
        spdlog::warn("Failed to save user preferences: {}", e.what());
    }
}

// Utility functions implementation
namespace ux_utils {

std::string toString(ExperienceLevel level) {
    switch (level) {
        case ExperienceLevel::Beginner: return "beginner";
        case ExperienceLevel::Intermediate: return "intermediate";
        case ExperienceLevel::Advanced: return "advanced";
        case ExperienceLevel::Expert: return "expert";
        default: return "unknown";
    }
}

ExperienceLevel parseExperienceLevel(const std::string& str) {
    if (str == "beginner") return ExperienceLevel::Beginner;
    if (str == "intermediate") return ExperienceLevel::Intermediate;
    if (str == "advanced") return ExperienceLevel::Advanced;
    if (str == "expert") return ExperienceLevel::Expert;
    return ExperienceLevel::Beginner;
}

bool askYesNo(const std::string& question, bool defaultValue) {
    std::string prompt = question + (defaultValue ? " [Y/n]: " : " [y/N]: ");
    std::cout << TerminalUtils::colorize(prompt, Color::BrightCyan);

    std::string input;
    std::getline(std::cin, input);

    if (input.empty()) {
        return defaultValue;
    }

    char first = std::tolower(input[0]);
    return first == 'y';
}

int askChoice(const std::string& question, const std::vector<std::string>& options, int defaultChoice) {
    std::cout << TerminalUtils::colorize(question, Color::BrightCyan) << "\n";

    if (!options.empty()) {
        for (size_t i = 0; i < options.size(); ++i) {
            std::cout << "  " << (i + 1) << ". " << options[i] << "\n";
        }
    }

    std::string prompt = "Enter choice";
    if (defaultChoice > 0) {
        prompt += " [" + std::to_string(defaultChoice) + "]";
    }
    prompt += ": ";

    std::cout << TerminalUtils::colorize(prompt, Color::BrightWhite);

    std::string input;
    std::getline(std::cin, input);

    if (input.empty() && defaultChoice > 0) {
        return defaultChoice;
    }

    try {
        int choice = std::stoi(input);
        return choice;
    } catch (const std::exception&) {
        return defaultChoice > 0 ? defaultChoice : 1;
    }
}

std::string askInput(const std::string& prompt, const std::string& defaultValue) {
    std::string fullPrompt = prompt;
    if (!defaultValue.empty()) {
        fullPrompt += " [" + defaultValue + "]";
    }
    fullPrompt += ": ";

    std::cout << TerminalUtils::colorize(fullPrompt, Color::BrightCyan);

    std::string input;
    std::getline(std::cin, input);

    return input.empty() ? defaultValue : input;
}

void showTip(const std::string& tip) {
    std::cout << "\n" << TerminalUtils::colorize("  💡 Tip: ", Color::BrightYellow)
              << TerminalUtils::colorize(tip, Color::BrightWhite) << "\n\n";
}

void showWarning(const std::string& warning) {
    std::cout << "\n" << TerminalUtils::colorize("  ⚠️  Warning: ", Color::BrightYellow)
              << TerminalUtils::colorize(warning, Color::BrightWhite) << "\n\n";
}

void showNote(const std::string& note) {
    std::cout << "\n" << TerminalUtils::colorize("  📝 Note: ", Color::BrightBlue)
              << TerminalUtils::colorize(note, Color::BrightWhite) << "\n\n";
}

void showBanner(const std::string& title, const std::string& subtitle) {
    std::cout << "\n";
    std::cout << TerminalUtils::colorize("  " + title, Color::BrightCyan) << "\n";
    if (!subtitle.empty()) {
        std::cout << TerminalUtils::colorize("  " + subtitle, Color::BrightBlack) << "\n";
    }
    std::cout << "\n";
}

} // namespace ux_utils

} // namespace utils

#include "enhanced_terminal.h"
#include "string_utils.h"
#include "../cli/types/cli_enums.h"
#include <iostream>
#include <thread>
#include <random>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <cctype>

namespace utils {

EnhancedTerminal& EnhancedTerminal::getInstance() {
    static EnhancedTerminal instance;
    return instance;
}

void EnhancedTerminal::showWelcomeScreen() {
    clearScreenWithStyle();

    auto scheme = getCurrentColorScheme();

    // Show logo with animation
    if (animationsEnabled_) {
        showLogo();
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    // Welcome message
    std::cout << "\n";
    std::cout << TerminalUtils::colorAndStyle(
        getCenteredText("Welcome to CPP-Scaffold"),
        scheme.primary, {TextStyle::Bold}
    ) << "\n";

    std::cout << TerminalUtils::colorize(
        getCenteredText("The Modern C++ Project Generator"),
        scheme.secondary
    ) << "\n\n";

    showVersionInfo();

    // Show a random tip
    showTip(getRandomTip());

    std::cout << "\n" << getBorderLine() << "\n\n";
}

void EnhancedTerminal::showApplicationHeader() {
    auto scheme = getCurrentColorScheme();

    std::cout << TerminalUtils::colorize("╭─────────────────────────────────────────────────────────────────────────────╮", scheme.accent) << "\n";
    std::cout << TerminalUtils::colorize("│", scheme.accent)
              << TerminalUtils::colorAndStyle(getCenteredText("🚀 CPP-Scaffold", 77), scheme.primary, {TextStyle::Bold})
              << TerminalUtils::colorize("│", scheme.accent) << "\n";
    std::cout << TerminalUtils::colorize("│", scheme.accent)
              << TerminalUtils::colorize(getCenteredText("Modern C++ Project Generator", 77), scheme.secondary)
              << TerminalUtils::colorize("│", scheme.accent) << "\n";
    std::cout << TerminalUtils::colorize("╰─────────────────────────────────────────────────────────────────────────────╯", scheme.accent) << "\n\n";
}

void EnhancedTerminal::startProjectCreation(const CliOptions& options) {
    startTime_ = std::chrono::steady_clock::now();

    auto scheme = getCurrentColorScheme();

    std::cout << "\n";
    std::cout << TerminalUtils::colorize("🚀 ", scheme.primary)
              << TerminalUtils::colorAndStyle("Creating project: ", scheme.text, {TextStyle::Bold})
              << TerminalUtils::colorize(options.projectName, scheme.accent) << "\n";

    std::cout << TerminalUtils::colorize("📋 ", scheme.secondary)
              << "Template: " << TerminalUtils::colorize(std::string(cli_enums::to_string(options.templateType)), scheme.accent) << "\n";

    std::cout << TerminalUtils::colorize("🔧 ", scheme.secondary)
              << "Build System: " << TerminalUtils::colorize(std::string(cli_enums::to_string(options.buildSystem)), scheme.accent) << "\n";

    if (options.packageManager != PackageManager::None) {
        std::cout << TerminalUtils::colorize("📦 ", scheme.secondary)
                  << "Package Manager: " << TerminalUtils::colorize(std::string(cli_enums::to_string(options.packageManager)), scheme.accent) << "\n";
    }

    std::cout << "\n" << getBorderLine(60, '-') << "\n\n";
}

void EnhancedTerminal::showProjectCreationProgress(const std::string& step, int progress, const std::string& detail) {
    auto scheme = getCurrentColorScheme();

    // Clear current line
    std::cout << "\r\033[K";

    // Show step indicator
    std::cout << TerminalUtils::colorize("⚡ ", scheme.primary);

    // Show progress bar
    showGradientProgressBar(progress, step);

    if (!detail.empty()) {
        std::cout << " " << TerminalUtils::colorize("(" + detail + ")", scheme.muted);
    }

    std::cout << std::flush;
}

void EnhancedTerminal::finishProjectCreation(const CliOptions& options, bool success) {
    std::cout << "\n\n";

    if (success) {
        showSuccessScreen(options);
    } else {
        showErrorScreen("Project creation failed", "Please check the logs for more details");
    }
}

void EnhancedTerminal::showSuccessScreen(const CliOptions& options) {
    auto scheme = getCurrentColorScheme();
    auto elapsed = std::chrono::steady_clock::now() - startTime_;
    auto elapsedMs = std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count();

    std::cout << "\n";
    std::cout << TerminalUtils::colorize("╭─────────────────────────────────────────────────────────────────────────────╮", scheme.success) << "\n";
    std::cout << TerminalUtils::colorize("│", scheme.success)
              << TerminalUtils::colorAndStyle(getCenteredText("🎉 Project Created Successfully!", 77), scheme.success, {TextStyle::Bold})
              << TerminalUtils::colorize("│", scheme.success) << "\n";
    std::cout << TerminalUtils::colorize("╰─────────────────────────────────────────────────────────────────────────────╯", scheme.success) << "\n\n";

    // Show project summary
    showProjectSummary(options);

    // Show completion time
    std::cout << TerminalUtils::colorize("⏱️  ", scheme.secondary)
              << "Completed in "
              << TerminalUtils::colorize(std::to_string(elapsedMs) + "ms", scheme.accent) << "\n\n";

    // Show next steps
    showNextSteps(options);
}

void EnhancedTerminal::showProjectSummary(const CliOptions& options) {
    auto scheme = getCurrentColorScheme();

    std::cout << TerminalUtils::colorAndStyle("📊 Project Summary:", scheme.text, {TextStyle::Bold}) << "\n";
    std::cout << "   " << TerminalUtils::colorize("Name: ", scheme.secondary)
              << TerminalUtils::colorize(options.projectName, scheme.accent) << "\n";
    std::cout << "   " << TerminalUtils::colorize("Type: ", scheme.secondary)
              << TerminalUtils::colorize(std::string(cli_enums::to_string(options.templateType)), scheme.accent) << "\n";
    std::cout << "   " << TerminalUtils::colorize("Build: ", scheme.secondary)
              << TerminalUtils::colorize(std::string(cli_enums::to_string(options.buildSystem)), scheme.accent) << "\n";

    if (options.includeTests) {
        std::cout << "   " << TerminalUtils::colorize("Tests: ", scheme.secondary)
                  << TerminalUtils::colorize(std::string(cli_enums::to_string(options.testFramework)), scheme.success) << "\n";
    }

    if (options.initGit) {
        std::cout << "   " << TerminalUtils::colorize("Git: ", scheme.secondary)
                  << TerminalUtils::colorize("Initialized", scheme.success) << "\n";
    }

    std::cout << "\n";
}

void EnhancedTerminal::showNextSteps(const CliOptions& options) {
    auto scheme = getCurrentColorScheme();

    std::cout << TerminalUtils::colorAndStyle("🚀 Next Steps:", scheme.text, {TextStyle::Bold}) << "\n\n";

    std::cout << TerminalUtils::colorize("1. ", scheme.primary)
              << "Navigate to your project:\n";
    std::cout << "   " << TerminalUtils::colorize("cd " + options.projectName, scheme.accent) << "\n\n";

    if (options.buildSystem == BuildSystem::CMake) {
        std::cout << TerminalUtils::colorize("2. ", scheme.primary)
                  << "Build your project:\n";
        std::cout << "   " << TerminalUtils::colorize("mkdir build && cd build", scheme.accent) << "\n";
        std::cout << "   " << TerminalUtils::colorize("cmake ..", scheme.accent) << "\n";
        std::cout << "   " << TerminalUtils::colorize("make", scheme.accent) << "\n\n";
    }

    if (options.includeTests) {
        std::cout << TerminalUtils::colorize("3. ", scheme.primary)
                  << "Run tests:\n";
        std::cout << "   " << TerminalUtils::colorize("make test", scheme.accent) << "\n\n";
    }

    std::cout << TerminalUtils::colorize("📚 ", scheme.secondary)
              << "Check out the README.md for detailed instructions!\n";
    std::cout << TerminalUtils::colorize("🌟 ", scheme.secondary)
              << "Happy coding!\n\n";
}

void EnhancedTerminal::showGradientProgressBar(int percent, const std::string& label) {
    if (!TerminalUtils::supportsAnsi()) {
        std::cout << label << ": " << percent << "%";
        return;
    }

    const int barWidth = 30;
    int filled = (barWidth * percent) / 100;

    std::cout << "[";

    for (int i = 0; i < barWidth; ++i) {
        if (i < filled) {
            // Gradient effect based on position
            if (i < filled * 0.3) {
                std::cout << TerminalUtils::colorize("█", Color::BrightRed);
            } else if (i < filled * 0.6) {
                std::cout << TerminalUtils::colorize("█", Color::BrightYellow);
            } else {
                std::cout << TerminalUtils::colorize("█", Color::BrightGreen);
            }
        } else {
            std::cout << TerminalUtils::colorize("░", Color::BrightBlack);
        }
    }

    std::cout << "] " << TerminalUtils::colorize(std::to_string(percent) + "%", Color::BrightCyan);

    if (!label.empty()) {
        std::cout << " " << TerminalUtils::colorize(label, Color::White);
    }
}

EnhancedTerminal::ColorScheme EnhancedTerminal::getCurrentColorScheme() {
    if (colorScheme_ == "dark") {
        return {Color::BrightCyan, Color::Cyan, Color::BrightYellow, Color::BrightGreen,
                Color::BrightYellow, Color::BrightRed, Color::White, Color::BrightBlack};
    } else if (colorScheme_ == "light") {
        return {Color::Blue, Color::BrightBlue, Color::Magenta, Color::Green,
                Color::Yellow, Color::Red, Color::Black, Color::BrightBlack};
    } else if (colorScheme_ == "colorful") {
        return {Color::BrightMagenta, Color::BrightBlue, Color::BrightYellow, Color::BrightGreen,
                Color::BrightYellow, Color::BrightRed, Color::BrightWhite, Color::BrightBlack};
    } else {
        // Default scheme
        return {Color::BrightBlue, Color::Blue, Color::BrightCyan, Color::BrightGreen,
                Color::BrightYellow, Color::BrightRed, Color::White, Color::BrightBlack};
    }
}

std::string EnhancedTerminal::getCenteredText(const std::string& text, int width) {
    if (static_cast<int>(text.length()) >= width) return text;

    int padding = (width - static_cast<int>(text.length())) / 2;
    return std::string(padding, ' ') + text + std::string(width - static_cast<int>(text.length()) - padding, ' ');
}

std::string EnhancedTerminal::getBorderLine(int width, char borderChar) {
    return std::string(width, borderChar);
}

void EnhancedTerminal::showLogo() {
    auto scheme = getCurrentColorScheme();

    std::string logo = R"(
    ╔═══════════════════════════════════════════════════════════════════════════╗
    ║                                                                           ║
    ║     ██████╗██████╗ ██████╗       ███████╗ ██████╗ █████╗ ███████╗███████╗ ║
    ║    ██╔════╝██╔══██╗██╔══██╗      ██╔════╝██╔════╝██╔══██╗██╔════╝██╔════╝ ║
    ║    ██║     ██████╔╝██████╔╝█████╗███████╗██║     ███████║█████╗  █████╗   ║
    ║    ██║     ██╔═══╝ ██╔═══╝ ╚════╝╚════██║██║     ██╔══██║██╔══╝  ██╔══╝   ║
    ║    ╚██████╗██║     ██║           ███████║╚██████╗██║  ██║██║     ██║      ║
    ║     ╚═════╝╚═╝     ╚═╝           ╚══════╝ ╚═════╝╚═╝  ╚═╝╚═╝     ╚═╝      ║
    ║                                                                           ║
    ╚═══════════════════════════════════════════════════════════════════════════╝
    )";

    if (animationsEnabled_) {
        // Animate logo appearance
        std::istringstream iss(logo);
        std::string line;
        while (std::getline(iss, line)) {
            std::cout << TerminalUtils::colorize(line, scheme.primary) << "\n";
            std::this_thread::sleep_for(std::chrono::milliseconds(100 / animationSpeed_));
        }
    } else {
        std::cout << TerminalUtils::colorize(logo, scheme.primary) << "\n";
    }
}

void EnhancedTerminal::showVersionInfo() {
    auto scheme = getCurrentColorScheme();

    std::cout << TerminalUtils::colorize(
        getCenteredText("Version 2.0.0 - The Enhanced Edition"),
        scheme.muted
    ) << "\n";

    std::cout << TerminalUtils::colorize(
        getCenteredText("https://github.com/cpp-scaffold/cpp-scaffold"),
        scheme.muted
    ) << "\n";
}

void EnhancedTerminal::showTip(const std::string& tip) {
    auto scheme = getCurrentColorScheme();

    std::cout << "\n";
    std::cout << TerminalUtils::colorize("💡 ", scheme.accent)
              << TerminalUtils::colorAndStyle("Tip: ", scheme.secondary, {TextStyle::Bold})
              << TerminalUtils::colorize(tip, scheme.text) << "\n";
}

std::string EnhancedTerminal::getRandomTip() {
    std::vector<std::string> tips = {
        "Use --interactive mode for a guided project setup experience",
        "Add --tests to include a testing framework in your project",
        "Try different templates with -t to explore various project types",
        "Use --package conan or --package vcpkg for dependency management",
        "Enable Git initialization with --git for version control",
        "Check out the generated README.md for build instructions",
        "Use --verbose for detailed logging during project creation",
        "Explore different build systems with -b cmake, meson, or bazel"
    };

    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, tips.size() - 1);

    return tips[dis(gen)];
}

void EnhancedTerminal::showErrorScreen(const std::string& error, const std::string& suggestion) {
    auto scheme = getCurrentColorScheme();

    std::cout << "\n";
    std::cout << TerminalUtils::colorize("╭─────────────────────────────────────────────────────────────────────────────╮", scheme.error) << "\n";
    std::cout << TerminalUtils::colorize("│", scheme.error)
              << TerminalUtils::colorAndStyle(getCenteredText("❌ Error Occurred", 77), scheme.error, {TextStyle::Bold})
              << TerminalUtils::colorize("│", scheme.error) << "\n";
    std::cout << TerminalUtils::colorize("╰─────────────────────────────────────────────────────────────────────────────╯", scheme.error) << "\n\n";

    std::cout << TerminalUtils::colorize("Error: ", scheme.error)
              << TerminalUtils::colorize(error, scheme.text) << "\n";

    if (!suggestion.empty()) {
        std::cout << "\n" << TerminalUtils::colorize("💡 Suggestion: ", scheme.accent)
                  << TerminalUtils::colorize(suggestion, scheme.text) << "\n";
    }

    std::cout << "\n" << TerminalUtils::colorize("📋 ", scheme.secondary)
              << "Check the log file for more details: cpp-scaffold.log\n";
    std::cout << TerminalUtils::colorize("🆘 ", scheme.secondary)
              << "Need help? Visit: https://github.com/cpp-scaffold/cpp-scaffold/issues\n\n";
}

void EnhancedTerminal::clearScreenWithStyle() {
    if (TerminalUtils::supportsAnsi()) {
        std::cout << "\033[2J\033[H";
    } else {
        // Fallback for non-ANSI terminals
        for (int i = 0; i < 50; ++i) {
            std::cout << "\n";
        }
    }
}

void EnhancedTerminal::showAnimatedSpinner(const std::string& message, int durationMs) {
    if (!animationsEnabled_ || !TerminalUtils::supportsAnsi()) {
        std::cout << message << "..." << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(durationMs));
        return;
    }

    auto scheme = getCurrentColorScheme();
    std::vector<std::string> spinnerChars = {"⠋", "⠙", "⠹", "⠸", "⠼", "⠴", "⠦", "⠧", "⠇", "⠏"};

    auto startTime = std::chrono::steady_clock::now();
    int frame = 0;

    while (std::chrono::duration_cast<std::chrono::milliseconds>(
               std::chrono::steady_clock::now() - startTime).count() < durationMs) {

        std::cout << "\r" << TerminalUtils::colorize(spinnerChars[frame % spinnerChars.size()], scheme.primary)
                  << " " << TerminalUtils::colorize(message, scheme.text) << std::flush;

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        frame++;
    }

    std::cout << "\r" << TerminalUtils::colorize("✓", scheme.success)
              << " " << TerminalUtils::colorize(message, scheme.text) << "\n";
}

// ScopedProgressIndicator implementation
ScopedProgressIndicator::ScopedProgressIndicator(const std::string& operation)
    : operation_(operation), startTime_(std::chrono::steady_clock::now()) {
    auto& terminal = EnhancedTerminal::getInstance();
    auto scheme = terminal.getCurrentColorScheme();

    std::cout << TerminalUtils::colorize("⚡ ", scheme.primary)
              << TerminalUtils::colorize(operation_, scheme.text) << "..." << std::flush;
}

ScopedProgressIndicator::~ScopedProgressIndicator() {
    if (!finished_) {
        setSuccess();
    }
}

void ScopedProgressIndicator::updateProgress(int percent, const std::string& detail) {
    if (finished_) return;

    auto& terminal = EnhancedTerminal::getInstance();
    std::cout << "\r";
    terminal.showProjectCreationProgress(operation_, percent, detail);
}

void ScopedProgressIndicator::setSuccess(const std::string& message) {
    if (finished_) return;
    finished_ = true;

    auto& terminal = EnhancedTerminal::getInstance();
    auto scheme = terminal.getCurrentColorScheme();
    auto elapsed = std::chrono::steady_clock::now() - startTime_;
    auto elapsedMs = std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count();

    std::cout << "\r" << TerminalUtils::colorize("✓ ", scheme.success)
              << TerminalUtils::colorize(operation_, scheme.text);

    if (!message.empty()) {
        std::cout << " - " << TerminalUtils::colorize(message, scheme.success);
    }

    std::cout << " " << TerminalUtils::colorize("(" + std::to_string(elapsedMs) + "ms)", scheme.muted) << "\n";
}

void ScopedProgressIndicator::setError(const std::string& message) {
    if (finished_) return;
    finished_ = true;

    auto& terminal = EnhancedTerminal::getInstance();
    auto scheme = terminal.getCurrentColorScheme();

    std::cout << "\r" << TerminalUtils::colorize("✗ ", scheme.error)
              << TerminalUtils::colorize(operation_, scheme.text);

    if (!message.empty()) {
        std::cout << " - " << TerminalUtils::colorize(message, scheme.error);
    }

    std::cout << "\n";
}

// Additional method implementations
void EnhancedTerminal::setAnimationSpeed(int speed) {
    animationSpeed_ = std::max(1, std::min(10, speed));
}

void EnhancedTerminal::setColorScheme(const std::string& scheme) {
    colorScheme_ = scheme;
}

void EnhancedTerminal::enableAnimations(bool enable) {
    animationsEnabled_ = enable;
}

void EnhancedTerminal::showMultiStepProgress(int currentStep, int totalSteps, const std::string& stepName) {
    auto scheme = getCurrentColorScheme();

    std::cout << "\n";

    // Progress dots
    for (int i = 1; i <= totalSteps; i++) {
        if (i == currentStep) {
            std::cout << TerminalUtils::colorize("●", scheme.primary);
        } else if (i < currentStep) {
            std::cout << TerminalUtils::colorize("●", scheme.success);
        } else {
            std::cout << TerminalUtils::colorize("○", scheme.muted);
        }

        if (i < totalSteps) {
            std::cout << TerminalUtils::colorize("─", scheme.muted);
        }
    }

    std::cout << "\n";
    std::cout << TerminalUtils::colorAndStyle("Step " + std::to_string(currentStep) + " of " + std::to_string(totalSteps),
                                            scheme.accent, {TextStyle::Bold}) << ": "
              << TerminalUtils::colorize(stepName, scheme.text) << "\n\n";
}

bool EnhancedTerminal::confirmWithStyle(const std::string& message, bool defaultValue) {
    auto scheme = getCurrentColorScheme();

    std::cout << TerminalUtils::colorize("❓ ", scheme.accent)
              << TerminalUtils::colorize(message, scheme.text);

    if (defaultValue) {
        std::cout << " " << TerminalUtils::colorize("[Y/n]", scheme.muted) << " ";
    } else {
        std::cout << " " << TerminalUtils::colorize("[y/N]", scheme.muted) << " ";
    }

    std::string input;
    std::getline(std::cin, input);

    if (input.empty()) {
        return defaultValue;
    }

    char first = std::tolower(input[0]);
    return (first == 'y');
}

int EnhancedTerminal::selectFromMenu(const std::vector<std::string>& options,
                                   const std::string& title,
                                   const std::string& description) {
    auto scheme = getCurrentColorScheme();

    if (!title.empty()) {
        std::cout << "\n" << TerminalUtils::colorAndStyle(title, scheme.primary, {TextStyle::Bold}) << "\n";
    }

    if (!description.empty()) {
        std::cout << TerminalUtils::colorize(description, scheme.secondary) << "\n";
    }

    std::cout << "\n";

    for (size_t i = 0; i < options.size(); ++i) {
        std::cout << TerminalUtils::colorize(std::to_string(i + 1) + ". ", scheme.accent)
                  << TerminalUtils::colorize(options[i], scheme.text) << "\n";
    }

    std::cout << "\n" << TerminalUtils::colorize("Enter your choice (1-" + std::to_string(options.size()) + "): ", scheme.text);

    int choice;
    std::cin >> choice;
    std::cin.ignore(); // Clear the newline

    if (choice >= 1 && choice <= static_cast<int>(options.size())) {
        return choice - 1; // Convert to 0-based index
    }

    return 0; // Default to first option
}

} // namespace utils

#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <random>
#include <regex>
#include <string>
#include <thread>
#include <vector>

#ifdef _WIN32
#include <io.h>
#include <windows.h>
#define isatty _isatty
#define fileno _fileno
#else
#include <sys/ioctl.h>
#include <unistd.h>
#endif

// Enhanced Terminal Utilities with Cross-Platform Support
class EnhancedTerminal {
   public:
    // ANSI Color Codes
    enum class Color {
        Reset = 0,
        Black = 30,
        Red = 31,
        Green = 32,
        Yellow = 33,
        Blue = 34,
        Magenta = 35,
        Cyan = 36,
        White = 37,
        BrightBlack = 90,
        BrightRed = 91,
        BrightGreen = 92,
        BrightYellow = 93,
        BrightBlue = 94,
        BrightMagenta = 95,
        BrightCyan = 96,
        BrightWhite = 97
    };

    enum class Style {
        Reset = 0,
        Bold = 1,
        Dim = 2,
        Italic = 3,
        Underline = 4,
        Blink = 5,
        Reverse = 7,
        Strike = 9
    };

    static bool supportsColor() {
        static bool checked = false;
        static bool supports = false;

        if (!checked) {
            checked = true;
#ifdef _WIN32
            // Enable ANSI escape sequences on Windows 10+
            HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
            DWORD dwMode = 0;
            if (GetConsoleMode(hOut, &dwMode)) {
                dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
                supports = SetConsoleMode(hOut, dwMode);

                // Set UTF-8 code pages for proper Unicode support
                if (supports) {
                    SetConsoleOutputCP(CP_UTF8);
                    SetConsoleCP(CP_UTF8);
                }
            }
#else
            supports = isatty(fileno(stdout)) && getenv("TERM") != nullptr;
#endif
        }
        return supports;
    }

    static std::string colorize(const std::string& text, Color color, Style style = Style::Reset) {
        if (!supportsColor())
            return text;

        std::string result = "\033[";
        if (style != Style::Reset) {
            result += std::to_string(static_cast<int>(style)) + ";";
        }
        result += std::to_string(static_cast<int>(color)) + "m" + text + "\033[0m";
        return result;
    }

    static void clearScreen() {
        if (supportsColor()) {
            std::cout << "\033[2J\033[H";
        } else {
#ifdef _WIN32
            system("cls");
#else
            system("clear");
#endif
        }
    }

    static void moveCursor(int row, int col) {
        if (supportsColor()) {
            std::cout << "\033[" << row << ";" << col << "H";
        }
    }

    static int getTerminalWidth() {
#ifdef _WIN32
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
        return csbi.srWindow.Right - csbi.srWindow.Left + 1;
#else
        struct winsize w;
        ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
        return w.ws_col;
#endif
    }

    static std::string centerText(const std::string& text, int width = 0) {
        if (width == 0)
            width = getTerminalWidth();
        int padding = (width - text.length()) / 2;
        return std::string(padding, ' ') + text;
    }

    static void showProgressBar(int percentage, const std::string& label = "", int width = 50) {
        int filled = (percentage * width) / 100;
        std::string bar = "[";

        for (int i = 0; i < width; ++i) {
            if (i < filled) {
                bar += colorize("█", Color::BrightGreen);
            } else {
                bar += colorize("░", Color::BrightBlack);
            }
        }
        bar += "] " + std::to_string(percentage) + "%";

        if (!label.empty()) {
            bar += " " + label;
        }

        std::cout << "\r" << bar << std::flush;
    }

    static void showSpinner(const std::string& message, int durationMs = 2000) {
        const std::vector<std::string> frames = {"⠋", "⠙", "⠹", "⠸", "⠼", "⠴", "⠦", "⠧", "⠇", "⠏"};
        auto start = std::chrono::steady_clock::now();
        int frame = 0;

        while (std::chrono::duration_cast<std::chrono::milliseconds>(
                       std::chrono::steady_clock::now() - start)
                       .count() < durationMs) {
            std::cout << "\r" << colorize(frames[frame], Color::BrightCyan) << " " << message
                      << std::flush;
            frame = (frame + 1) % frames.size();
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        std::cout << "\r" << colorize("✓", Color::BrightGreen) << " " << message << " Complete!\n";
    }

    static void showBorder(const std::string& title = "", int width = 0) {
        if (width == 0)
            width = getTerminalWidth() - 4;

        std::string topBorder = "+" + std::string(width - 2, '-') + "+";
        std::string bottomBorder = "+" + std::string(width - 2, '-') + "+";

        std::cout << colorize(topBorder, Color::BrightBlue) << "\n";

        if (!title.empty()) {
            int padding = (width - title.length() - 4) / 2;
            std::string titleLine = "| " + std::string(padding, ' ') + title +
                                    std::string(width - title.length() - padding - 4, ' ') + " |";
            std::cout << colorize(titleLine, Color::BrightBlue) << "\n";
            std::cout << colorize("+" + std::string(width - 2, '-') + "+", Color::BrightBlue)
                      << "\n";
        }
    }

    static void showTable(const std::vector<std::vector<std::string>>& data,
                          const std::vector<std::string>& headers = {}) {
        if (data.empty())
            return;

        // Calculate column widths
        std::vector<size_t> widths;
        size_t cols = data[0].size();

        if (!headers.empty()) {
            cols = std::max(cols, headers.size());
            for (size_t i = 0; i < headers.size(); ++i) {
                if (i >= widths.size())
                    widths.push_back(0);
                widths[i] = std::max(widths[i], headers[i].length());
            }
        }

        for (const auto& row : data) {
            for (size_t i = 0; i < row.size(); ++i) {
                if (i >= widths.size())
                    widths.push_back(0);
                widths[i] = std::max(widths[i], row[i].length());
            }
        }

        // Add padding
        for (auto& width : widths)
            width += 2;

        // Print table
        auto printSeparator = [&](char left, char mid, char right, char fill) {
            std::cout << colorize(std::string(1, left), Color::BrightBlue);
            for (size_t i = 0; i < widths.size(); ++i) {
                std::cout << colorize(std::string(widths[i], fill), Color::BrightBlue);
                if (i < widths.size() - 1) {
                    std::cout << colorize(std::string(1, mid), Color::BrightBlue);
                }
            }
            std::cout << colorize(std::string(1, right), Color::BrightBlue) << "\n";
        };

        printSeparator('+', '+', '+', '-');

        // Print headers
        if (!headers.empty()) {
            std::cout << colorize("|", Color::BrightBlue);
            for (size_t i = 0; i < headers.size(); ++i) {
                std::string cell =
                        " " + headers[i] + std::string(widths[i] - headers[i].length() - 1, ' ');
                std::cout << colorize(cell, Color::BrightYellow, Style::Bold);
                std::cout << colorize("|", Color::BrightBlue);
            }
            std::cout << "\n";
            printSeparator('+', '+', '+', '-');
        }

        // Print data rows
        for (const auto& row : data) {
            std::cout << colorize("|", Color::BrightBlue);
            for (size_t i = 0; i < row.size(); ++i) {
                std::string cell = " " + row[i] + std::string(widths[i] - row[i].length() - 1, ' ');
                std::cout << cell;
                std::cout << colorize("|", Color::BrightBlue);
            }
            std::cout << "\n";
        }

        printSeparator('+', '+', '+', '-');
    }
};

// Enhanced Error Handling System
class ErrorHandler {
   public:
    enum class ErrorType { Info, Warning, Error, Critical, Fatal };

    struct ErrorInfo {
        ErrorType type;
        std::string code;
        std::string message;
        std::string suggestion;
        std::string context;

        ErrorInfo(ErrorType t, const std::string& c, const std::string& m,
                  const std::string& s = "", const std::string& ctx = "")
            : type(t), code(c), message(m), suggestion(s), context(ctx) {}
    };

    static void showError(const ErrorInfo& error) {
        EnhancedTerminal::Color color;
        std::string icon;
        std::string typeStr;

        switch (error.type) {
            case ErrorType::Info:
                color = EnhancedTerminal::Color::BrightBlue;
                icon = "ℹ️";
                typeStr = "INFO";
                break;
            case ErrorType::Warning:
                color = EnhancedTerminal::Color::BrightYellow;
                icon = "⚠️";
                typeStr = "WARNING";
                break;
            case ErrorType::Error:
                color = EnhancedTerminal::Color::BrightRed;
                icon = "❌";
                typeStr = "ERROR";
                break;
            case ErrorType::Critical:
                color = EnhancedTerminal::Color::BrightMagenta;
                icon = "🚨";
                typeStr = "CRITICAL";
                break;
            case ErrorType::Fatal:
                color = EnhancedTerminal::Color::Red;
                icon = "💀";
                typeStr = "FATAL";
                break;
        }

        std::cout << "\n";
        std::cout << EnhancedTerminal::colorize(icon + " " + typeStr, color,
                                                EnhancedTerminal::Style::Bold);
        if (!error.code.empty()) {
            std::cout << " [" << EnhancedTerminal::colorize(error.code, color) << "]";
        }
        std::cout << "\n";

        std::cout << "Message: " << error.message << "\n";

        if (!error.context.empty()) {
            std::cout << "Context: "
                      << EnhancedTerminal::colorize(error.context,
                                                    EnhancedTerminal::Color::BrightBlack)
                      << "\n";
        }

        if (!error.suggestion.empty()) {
            std::cout << "Suggestion: "
                      << EnhancedTerminal::colorize(error.suggestion,
                                                    EnhancedTerminal::Color::BrightGreen)
                      << "\n";
        }

        std::cout << "\n";
    }
};

// Enhanced Input Validation System
class InputValidator {
   public:
    struct ValidationResult {
        bool isValid;
        std::string errorMessage;
        std::string suggestion;

        ValidationResult(bool valid = true, const std::string& error = "",
                         const std::string& suggest = "")
            : isValid(valid), errorMessage(error), suggestion(suggest) {}
    };

    static ValidationResult validateProjectName(const std::string& name) {
        if (name.empty()) {
            return ValidationResult(false, "Project name cannot be empty",
                                    "Please enter a valid project name");
        }

        if (name.length() < 2) {
            return ValidationResult(false, "Project name too short",
                                    "Project name must be at least 2 characters long");
        }

        if (name.length() > 50) {
            return ValidationResult(false, "Project name too long",
                                    "Project name must be 50 characters or less");
        }

        // Check for valid characters (alphanumeric, hyphens, underscores)
        std::regex validPattern("^[a-zA-Z0-9_-]+$");
        if (!std::regex_match(name, validPattern)) {
            return ValidationResult(false, "Invalid characters in project name",
                                    "Use only letters, numbers, hyphens, and underscores");
        }

        // Check if starts with letter
        if (!std::isalpha(name[0])) {
            return ValidationResult(false, "Project name must start with a letter",
                                    "Project names should begin with a letter (a-z, A-Z)");
        }

        return ValidationResult(true);
    }

    static ValidationResult validateFilePath(const std::string& path) {
        if (path.empty()) {
            return ValidationResult(false, "File path cannot be empty",
                                    "Please enter a valid file path");
        }

        try {
            std::filesystem::path fsPath(path);

            // Check if parent directory exists
            if (fsPath.has_parent_path() && !std::filesystem::exists(fsPath.parent_path())) {
                return ValidationResult(
                        false, "Parent directory does not exist",
                        "Create the parent directory first or choose a different path");
            }

            // Check for invalid characters
            std::string filename = fsPath.filename().string();
            std::regex invalidChars("[<>:\"|?*]");
            if (std::regex_search(filename, invalidChars)) {
                return ValidationResult(false, "Invalid characters in filename",
                                        "Avoid using < > : \" | ? * in filenames");
            }

        } catch (const std::exception& e) {
            return ValidationResult(false, "Invalid file path format",
                                    "Please check the path format and try again");
        }

        return ValidationResult(true);
    }

    static ValidationResult validateNumericRange(const std::string& input, int min, int max) {
        try {
            int value = std::stoi(input);
            if (value < min || value > max) {
                return ValidationResult(false, "Value out of range",
                                        "Please enter a number between " + std::to_string(min) +
                                                " and " + std::to_string(max));
            }
            return ValidationResult(true);
        } catch (const std::exception& e) {
            return ValidationResult(false, "Invalid number format",
                                    "Please enter a valid integer number");
        }
    }

    static ValidationResult validateEmail(const std::string& email) {
        if (email.empty()) {
            return ValidationResult(false, "Email cannot be empty",
                                    "Please enter a valid email address");
        }

        std::regex emailPattern(R"([a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,})");
        if (!std::regex_match(email, emailPattern)) {
            return ValidationResult(false, "Invalid email format",
                                    "Please enter a valid email address (e.g., user@example.com)");
        }

        return ValidationResult(true);
    }
};

// Enhanced User Experience Manager
class UserExperienceManager {
   public:
    static void showWelcomeScreen() {
        EnhancedTerminal::clearScreen();

        std::cout << "\n";
        std::cout << EnhancedTerminal::centerText(EnhancedTerminal::colorize(
                             "🚀 Enhanced C++ Terminal Application Demo",
                             EnhancedTerminal::Color::BrightCyan, EnhancedTerminal::Style::Bold))
                  << "\n";
        std::cout << EnhancedTerminal::centerText(
                             EnhancedTerminal::colorize("Demonstrating Advanced Terminal Features",
                                                        EnhancedTerminal::Color::BrightBlue))
                  << "\n\n";

        EnhancedTerminal::showBorder("Welcome");
        std::cout
                << "│ This demo showcases:                                                    │\n";
        std::cout << "│ • "
                  << EnhancedTerminal::colorize("Colored output and styling",
                                                EnhancedTerminal::Color::BrightGreen)
                  << "                                           │\n";
        std::cout << "│ • "
                  << EnhancedTerminal::colorize("Progress indicators and animations",
                                                EnhancedTerminal::Color::BrightYellow)
                  << "                                    │\n";
        std::cout << "│ • "
                  << EnhancedTerminal::colorize("Input validation with helpful feedback",
                                                EnhancedTerminal::Color::BrightMagenta)
                  << "                               │\n";
        std::cout << "│ • "
                  << EnhancedTerminal::colorize("Comprehensive error handling",
                                                EnhancedTerminal::Color::BrightRed)
                  << "                                       │\n";
        std::cout << "│ • "
                  << EnhancedTerminal::colorize("Cross-platform terminal support",
                                                EnhancedTerminal::Color::BrightCyan)
                  << "                                     │\n";
        std::cout << "│ • "
                  << EnhancedTerminal::colorize("Enhanced user experience features",
                                                EnhancedTerminal::Color::BrightWhite)
                  << "                                   │\n";
        std::cout << "╰──────────────────────────────────────────────────────────────────────╯\n\n";
    }

    static std::string promptWithValidation(
            const std::string& prompt,
            std::function<InputValidator::ValidationResult(const std::string&)> validator,
            const std::string& hint = "") {
        std::string input;
        InputValidator::ValidationResult result;

        do {
            std::cout << EnhancedTerminal::colorize("? ", EnhancedTerminal::Color::BrightBlue,
                                                    EnhancedTerminal::Style::Bold)
                      << prompt;
            if (!hint.empty()) {
                std::cout << " "
                          << EnhancedTerminal::colorize("(" + hint + ")",
                                                        EnhancedTerminal::Color::BrightBlack);
            }
            std::cout << ": ";

            std::getline(std::cin, input);
            result = validator(input);

            if (!result.isValid) {
                ErrorHandler::showError(ErrorHandler::ErrorInfo(ErrorHandler::ErrorType::Error,
                                                                "VAL001", result.errorMessage,
                                                                result.suggestion));
            }
        } while (!result.isValid);

        return input;
    }

    static bool confirmAction(const std::string& message, bool defaultValue = true) {
        std::string prompt = message + " " + (defaultValue ? "[Y/n]" : "[y/N]") + ": ";

        std::cout << EnhancedTerminal::colorize("? ", EnhancedTerminal::Color::BrightYellow,
                                                EnhancedTerminal::Style::Bold)
                  << prompt;

        std::string input;
        std::getline(std::cin, input);

        if (input.empty())
            return defaultValue;

        char first = std::tolower(input[0]);
        return first == 'y';
    }

    static int showMenu(const std::vector<std::string>& options,
                        const std::string& title = "Select an option") {
        std::cout << "\n"
                  << EnhancedTerminal::colorize(title, EnhancedTerminal::Color::BrightCyan,
                                                EnhancedTerminal::Style::Bold)
                  << "\n\n";

        for (size_t i = 0; i < options.size(); ++i) {
            std::cout << EnhancedTerminal::colorize(std::to_string(i + 1) + ".",
                                                    EnhancedTerminal::Color::BrightBlue)
                      << " " << options[i] << "\n";
        }

        std::cout << "\n";

        auto validator = [&](const std::string& input) -> InputValidator::ValidationResult {
            return InputValidator::validateNumericRange(input, 1, static_cast<int>(options.size()));
        };

        std::string choice = promptWithValidation("Enter your choice", validator,
                                                  "1-" + std::to_string(options.size()));
        return std::stoi(choice) - 1;
    }

    static void showStatus(const std::string& message, bool success) {
        std::string icon = success ? "✓" : "✗";
        EnhancedTerminal::Color color =
                success ? EnhancedTerminal::Color::BrightGreen : EnhancedTerminal::Color::BrightRed;

        std::cout << EnhancedTerminal::colorize(icon, color, EnhancedTerminal::Style::Bold) << " "
                  << message << "\n";
    }

    static void showHelp() {
        std::cout << "\n"
                  << EnhancedTerminal::colorize("📖 Help & Usage Examples",
                                                EnhancedTerminal::Color::BrightCyan,
                                                EnhancedTerminal::Style::Bold)
                  << "\n\n";

        std::vector<std::vector<std::string>> helpData = {
                {"Feature", "Description", "Example"},
                {"Project Creation", "Create a new C++ project",
                 "Enter project name: MyAwesomeApp"},
                {"Input Validation", "Validates user input with feedback",
                 "Project names must start with a letter"},
                {"Error Handling", "Shows detailed error information",
                 "Error codes with suggestions"},
                {"Progress Display", "Visual progress indicators", "Progress bars and spinners"},
                {"Confirmation", "Confirms destructive operations", "Are you sure? [Y/n]"}};

        EnhancedTerminal::showTable(helpData, {"Feature", "Description", "Example"});

        std::cout << "\n"
                  << EnhancedTerminal::colorize("💡 Tips:", EnhancedTerminal::Color::BrightYellow,
                                                EnhancedTerminal::Style::Bold)
                  << "\n";
        std::cout << "• Use Ctrl+C to exit at any time\n";
        std::cout << "• All inputs are validated with helpful error messages\n";
        std::cout << "• The application works on Windows, Linux, and macOS\n";
        std::cout << "• Colors automatically disable on non-compatible terminals\n\n";
    }
};

// Demo Application Class
class EnhancedTerminalDemo {
   public:
    void run() {
        UserExperienceManager::showWelcomeScreen();

        while (true) {
            std::vector<std::string> mainMenu = {"Project Creation Demo",
                                                 "Input Validation Demo",
                                                 "Error Handling Demo",
                                                 "Progress Indicators Demo",
                                                 "Table Display Demo",
                                                 "Help & Documentation",
                                                 "Exit"};

            int choice = UserExperienceManager::showMenu(mainMenu,
                                                         "🎯 Main Menu - Choose a demonstration");

            switch (choice) {
                case 0:
                    demoProjectCreation();
                    break;
                case 1:
                    demoInputValidation();
                    break;
                case 2:
                    demoErrorHandling();
                    break;
                case 3:
                    demoProgressIndicators();
                    break;
                case 4:
                    demoTableDisplay();
                    break;
                case 5:
                    UserExperienceManager::showHelp();
                    break;
                case 6:
                    if (UserExperienceManager::confirmAction("Are you sure you want to exit?")) {
                        std::cout << "\n"
                                  << EnhancedTerminal::colorize(
                                             "👋 Thank you for trying the Enhanced Terminal Demo!",
                                             EnhancedTerminal::Color::BrightCyan)
                                  << "\n\n";
                        return;
                    }
                    break;
            }

            std::cout << "\nPress Enter to continue...";
            std::cin.get();
        }
    }

   private:
    void demoProjectCreation() {
        std::cout << "\n"
                  << EnhancedTerminal::colorize("🏗️ Project Creation Demo",
                                                EnhancedTerminal::Color::BrightGreen,
                                                EnhancedTerminal::Style::Bold)
                  << "\n\n";

        // Get project name with validation
        std::string projectName = UserExperienceManager::promptWithValidation(
                "Enter project name", InputValidator::validateProjectName,
                "letters, numbers, hyphens, underscores only");

        // Get project path with validation
        std::string projectPath = UserExperienceManager::promptWithValidation(
                "Enter project path", InputValidator::validateFilePath,
                "e.g., ./projects/my-project");

        // Simulate project creation with progress
        std::cout << "\n"
                  << EnhancedTerminal::colorize("Creating project...",
                                                EnhancedTerminal::Color::BrightBlue)
                  << "\n";

        std::vector<std::string> steps = {"Creating directory structure",
                                          "Generating CMakeLists.txt", "Creating source files",
                                          "Setting up build configuration",
                                          "Initializing git repository"};

        for (size_t i = 0; i < steps.size(); ++i) {
            EnhancedTerminal::showProgressBar((i + 1) * 20, steps[i]);
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }

        std::cout << "\n\n";
        UserExperienceManager::showStatus("Project '" + projectName + "' created successfully!",
                                          true);

        // Show project summary
        std::vector<std::vector<std::string>> projectInfo = {
                {"Property", "Value"},          {"Project Name", projectName},
                {"Project Path", projectPath},  {"Build System", "CMake"},
                {"Language Standard", "C++17"}, {"Created", "Just now"}};

        std::cout << "\n";
        EnhancedTerminal::showTable(projectInfo, {"Property", "Value"});
    }

    void demoInputValidation() {
        std::cout << "\n"
                  << EnhancedTerminal::colorize("✅ Input Validation Demo",
                                                EnhancedTerminal::Color::BrightYellow,
                                                EnhancedTerminal::Style::Bold)
                  << "\n\n";

        std::cout << "This demo shows various input validation scenarios:\n\n";

        // Email validation
        std::string email = UserExperienceManager::promptWithValidation(
                "Enter your email address", InputValidator::validateEmail, "user@example.com");
        UserExperienceManager::showStatus("Valid email: " + email, true);

        // Numeric range validation
        std::string port = UserExperienceManager::promptWithValidation(
                "Enter port number",
                [](const std::string& input) {
                    return InputValidator::validateNumericRange(input, 1024, 65535);
                },
                "1024-65535");
        UserExperienceManager::showStatus("Valid port: " + port, true);

        std::cout << "\n"
                  << EnhancedTerminal::colorize("All validations passed!",
                                                EnhancedTerminal::Color::BrightGreen)
                  << "\n";
    }

    void demoErrorHandling() {
        std::cout << "\n"
                  << EnhancedTerminal::colorize("🚨 Error Handling Demo",
                                                EnhancedTerminal::Color::BrightRed,
                                                EnhancedTerminal::Style::Bold)
                  << "\n\n";

        std::cout << "Demonstrating different error types and handling:\n\n";

        // Show different error types
        ErrorHandler::showError(ErrorHandler::ErrorInfo(ErrorHandler::ErrorType::Info, "INFO001",
                                                        "This is an informational message",
                                                        "No action required", "Demo context"));

        ErrorHandler::showError(ErrorHandler::ErrorInfo(
                ErrorHandler::ErrorType::Warning, "WARN001", "This is a warning message",
                "Consider reviewing your configuration", "Configuration validation"));

        ErrorHandler::showError(ErrorHandler::ErrorInfo(
                ErrorHandler::ErrorType::Error, "ERR001", "This is an error message",
                "Check file permissions and try again", "File operation failed"));

        ErrorHandler::showError(ErrorHandler::ErrorInfo(
                ErrorHandler::ErrorType::Critical, "CRIT001", "This is a critical error",
                "Immediate attention required - contact support", "System integrity check"));
    }

    void demoProgressIndicators() {
        std::cout << "\n"
                  << EnhancedTerminal::colorize("⏳ Progress Indicators Demo",
                                                EnhancedTerminal::Color::BrightMagenta,
                                                EnhancedTerminal::Style::Bold)
                  << "\n\n";

        // Progress bar demo
        std::cout << "Progress Bar Demo:\n";
        for (int i = 0; i <= 100; i += 10) {
            EnhancedTerminal::showProgressBar(i, "Processing data...");
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
        }
        std::cout << "\n\n";

        // Spinner demo
        std::cout << "Spinner Demo:\n";
        EnhancedTerminal::showSpinner("Loading configuration", 2000);
        EnhancedTerminal::showSpinner("Connecting to server", 1500);
        EnhancedTerminal::showSpinner("Finalizing setup", 1000);
    }

    void demoTableDisplay() {
        std::cout << "\n"
                  << EnhancedTerminal::colorize("📊 Table Display Demo",
                                                EnhancedTerminal::Color::BrightCyan,
                                                EnhancedTerminal::Style::Bold)
                  << "\n\n";

        // System information table
        std::vector<std::vector<std::string>> systemInfo = {
                {"Component", "Status", "Version", "Notes"},
                {"Terminal Colors",
                 EnhancedTerminal::supportsColor() ? "Supported" : "Not Supported", "ANSI",
                 "Auto-detected"},
                {"Platform",
#ifdef _WIN32
                 "Windows",
#elif __linux__
                 "Linux",
#elif __APPLE__
                 "macOS",
#else
                 "Unknown",
#endif
                 "Native", "Compile-time detection"},
                {"C++ Standard", "C++17", "ISO/IEC 14882:2017", "Minimum required"},
                {"Build Type", "Demo", "1.0.0", "Enhanced features enabled"}};

        EnhancedTerminal::showTable(systemInfo, {"Component", "Status", "Version", "Notes"});

        std::cout << "\n";

        // Performance metrics table
        std::vector<std::vector<std::string>> metrics = {
                {"Metric", "Value", "Unit"},
                {"Terminal Width", std::to_string(EnhancedTerminal::getTerminalWidth()),
                 "characters"},
                {"Color Support", EnhancedTerminal::supportsColor() ? "Yes" : "No", "boolean"},
                {"Demo Runtime", "Active", "session"}};

        EnhancedTerminal::showTable(metrics, {"Metric", "Value", "Unit"});
    }
};

// Main function
int main() {
    try {
        EnhancedTerminalDemo demo;
        demo.run();
    } catch (const std::exception& e) {
        ErrorHandler::showError(ErrorHandler::ErrorInfo(
                ErrorHandler::ErrorType::Fatal, "FATAL001",
                "Unhandled exception occurred: " + std::string(e.what()),
                "Please report this issue with your system information", "Application main loop"));
        return 1;
    } catch (...) {
        ErrorHandler::showError(ErrorHandler::ErrorInfo(
                ErrorHandler::ErrorType::Fatal, "FATAL002", "Unknown exception occurred",
                "Please report this issue with your system information", "Application main loop"));
        return 1;
    }

    return 0;
}

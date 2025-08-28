#pragma once

#include "terminal_utils.h"
#include "../cli/types/cli_options.h"
#include <string>
#include <vector>
#include <chrono>
#include <memory>

namespace utils {

// Enhanced terminal experience manager
class EnhancedTerminal {
public:
    static EnhancedTerminal& getInstance();

    // Application branding and welcome
    void showWelcomeScreen();
    void showApplicationHeader();
    void showBrandedSeparator();

    // Enhanced project creation flow
    void startProjectCreation(const CliOptions& options);
    void showProjectCreationProgress(const std::string& step, int progress, const std::string& detail = "");
    void finishProjectCreation(const CliOptions& options, bool success);

    // Interactive project wizard
    CliOptions runEnhancedWizard();

    // Success and completion screens
    void showSuccessScreen(const CliOptions& options);
    void showNextSteps(const CliOptions& options);
    void showProjectSummary(const CliOptions& options);

    // Error handling with style
    void showErrorScreen(const std::string& error, const std::string& suggestion = "");
    void showWarningMessage(const std::string& warning);

    // Enhanced progress indicators
    void showAnimatedSpinner(const std::string& message, int durationMs = 2000);
    void showGradientProgressBar(int percent, const std::string& label = "");
    void showMultiStepProgress(int currentStep, int totalSteps, const std::string& stepName);

    // Interactive elements
    std::string promptWithValidation(const std::string& prompt,
                                   const std::function<bool(const std::string&)>& validator,
                                   const std::string& errorMessage = "Invalid input");

    bool confirmWithStyle(const std::string& message, bool defaultValue = true);
    int selectFromMenu(const std::vector<std::string>& options,
                      const std::string& title = "",
                      const std::string& description = "");

    // Utility methods
    void clearScreenWithStyle();
    void pauseWithMessage(const std::string& message = "Press any key to continue...");
    void showTip(const std::string& tip);
    void showQuote();

    // Configuration
    void setAnimationSpeed(int speed); // 1-10, 10 being fastest
    void setColorScheme(const std::string& scheme); // "default", "dark", "light", "colorful"
    void enableAnimations(bool enable);

    // Color schemes (public for ScopedProgressIndicator)
    struct ColorScheme {
        Color primary;
        Color secondary;
        Color accent;
        Color success;
        Color warning;
        Color error;
        Color text;
        Color muted;
    };

    ColorScheme getCurrentColorScheme();

private:
    EnhancedTerminal() = default;

    // Internal state
    bool animationsEnabled_ = true;
    int animationSpeed_ = 5;
    std::string colorScheme_ = "default";
    std::chrono::steady_clock::time_point startTime_;

    // Helper methods
    void showLogo();
    void showVersionInfo();
    std::string getRandomQuote();
    std::string getRandomTip();
    void animateText(const std::string& text, int delayMs = 50);
    void showLoadingAnimation(const std::string& message, int durationMs);

    // ASCII art and decorations
    std::string getLogoArt();
    std::string getBorderLine(int width = 80, char borderChar = '=');
    std::string getCenteredText(const std::string& text, int width = 80);
};

// RAII class for scoped progress indication
class ScopedProgressIndicator {
public:
    ScopedProgressIndicator(const std::string& operation);
    ~ScopedProgressIndicator();

    void updateProgress(int percent, const std::string& detail = "");
    void setSuccess(const std::string& message = "");
    void setError(const std::string& message = "");

private:
    std::string operation_;
    bool finished_ = false;
    std::chrono::steady_clock::time_point startTime_;
};

// Utility macros for common operations
#define SCOPED_PROGRESS(operation) utils::ScopedProgressIndicator _progress(operation)
#define UPDATE_PROGRESS(percent, detail) _progress.updateProgress(percent, detail)
#define PROGRESS_SUCCESS(msg) _progress.setSuccess(msg)
#define PROGRESS_ERROR(msg) _progress.setError(msg)

} // namespace utils

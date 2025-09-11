#include "progress_indicator.h"

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <sstream>

#include "terminal_utils.h"

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#else
#include <unistd.h>
#endif

namespace utils {

// SpinnerIndicator Implementation
SpinnerIndicator::SpinnerIndicator() : m_spinnerChars{"|", "/", "-", "\\"}, m_spinnerSpeed(100) {}

SpinnerIndicator::~SpinnerIndicator() {
    stop();
}

void SpinnerIndicator::start(const std::string& message) {
    m_message = message;
    m_startTime = std::chrono::steady_clock::now();
    m_running = true;
    m_progress = 0.0;

    m_spinnerThread = std::make_unique<std::thread>(&SpinnerIndicator::spinnerLoop, this);
}

void SpinnerIndicator::update(double progress, const std::string& message) {
    m_progress = std::clamp(progress, 0.0, 1.0);
    if (!message.empty()) {
        m_message = message;
    }
}

void SpinnerIndicator::finish(const std::string& message) {
    m_finishMessage = message;
    stop();

    // Clear the line and show completion message
    std::cout << "\r" << std::string(80, ' ') << "\r";
    if (!m_finishMessage.empty()) {
        std::cout << TerminalUtils::colorize("✓ " + m_finishMessage, utils::Color::BrightGreen)
                  << std::endl;
    } else if (!m_message.empty()) {
        std::cout << TerminalUtils::colorize("✓ " + m_message, utils::Color::BrightGreen)
                  << std::endl;
    }
}

void SpinnerIndicator::stop() {
    if (m_running) {
        m_running = false;
        if (m_spinnerThread && m_spinnerThread->joinable()) {
            m_spinnerThread->join();
        }
        m_spinnerThread.reset();
    }
}

void SpinnerIndicator::setMessage(const std::string& message) {
    m_message = message;
}

void SpinnerIndicator::setProgress(double progress) {
    m_progress = std::clamp(progress, 0.0, 1.0);
}

void SpinnerIndicator::setShowPercentage(bool show) {
    m_showPercentage = show;
}

void SpinnerIndicator::setShowElapsedTime(bool show) {
    m_showElapsedTime = show;
}

void SpinnerIndicator::setShowETA(bool show) {
    m_showETA = show;
}

void SpinnerIndicator::setSpinnerChars(const std::vector<std::string>& chars) {
    if (!chars.empty()) {
        m_spinnerChars = chars;
    }
}

void SpinnerIndicator::setSpinnerSpeed(int milliseconds) {
    m_spinnerSpeed = std::max(50, milliseconds);
}

void SpinnerIndicator::spinnerLoop() {
    size_t charIndex = 0;

    while (m_running) {
        render();
        charIndex = (charIndex + 1) % m_spinnerChars.size();
        std::this_thread::sleep_for(std::chrono::milliseconds(m_spinnerSpeed));
    }
}

void SpinnerIndicator::render() {
    static size_t charIndex = 0;

    std::ostringstream output;
    output << "\r";

    // Spinner character
    output << TerminalUtils::colorize(m_spinnerChars[charIndex % m_spinnerChars.size()],
                                      utils::Color::BrightCyan);
    output << " ";

    // Message
    if (!m_message.empty()) {
        output << m_message;
    }

    // Progress percentage
    if (m_showPercentage && m_progress > 0.0) {
        output << " (" << std::fixed << std::setprecision(1) << (m_progress * 100.0) << "%)";
    }

    // Elapsed time
    if (m_showElapsedTime) {
        output << " [" << getElapsedTime() << "]";
    }

    // ETA
    if (m_showETA && m_progress > 0.0) {
        output << " ETA: " << getETA();
    }

    std::cout << output.str() << std::flush;
    charIndex++;
}

std::string SpinnerIndicator::formatTime(std::chrono::seconds seconds) const {
    auto hours = std::chrono::duration_cast<std::chrono::hours>(seconds);
    auto minutes =
            std::chrono::duration_cast<std::chrono::minutes>(seconds % std::chrono::hours(1));
    auto secs = seconds % std::chrono::minutes(1);

    if (hours.count() > 0) {
        return std::to_string(hours.count()) + "h " + std::to_string(minutes.count()) + "m " +
               std::to_string(secs.count()) + "s";
    } else if (minutes.count() > 0) {
        return std::to_string(minutes.count()) + "m " + std::to_string(secs.count()) + "s";
    } else {
        return std::to_string(secs.count()) + "s";
    }
}

std::string SpinnerIndicator::getElapsedTime() const {
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - m_startTime);
    return formatTime(elapsed);
}

std::string SpinnerIndicator::getETA() const {
    if (m_progress <= 0.0) {
        return "Unknown";
    }

    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - m_startTime);

    if (elapsed.count() == 0) {
        return "Unknown";
    }

    auto totalTime = std::chrono::seconds(static_cast<long long>(elapsed.count() / m_progress));
    auto remaining = totalTime - elapsed;

    if (remaining.count() <= 0) {
        return "0s";
    }

    return formatTime(remaining);
}

// ProgressBarIndicator Implementation
ProgressBarIndicator::ProgressBarIndicator(int width)
    : m_width(width), m_fillChar('='), m_emptyChar(' '), m_leftBracket('['), m_rightBracket(']') {}

ProgressBarIndicator::~ProgressBarIndicator() {
    if (m_started) {
        std::cout << std::endl;
    }
}

void ProgressBarIndicator::start(const std::string& message) {
    m_message = message;
    m_startTime = std::chrono::steady_clock::now();
    m_progress = 0.0;
    m_started = true;
    render();
}

void ProgressBarIndicator::update(double progress, const std::string& message) {
    m_progress = std::clamp(progress, 0.0, 1.0);
    if (!message.empty()) {
        m_message = message;
    }
    render();
}

void ProgressBarIndicator::finish(const std::string& message) {
    m_progress = 1.0;
    if (!message.empty()) {
        m_message = message;
    }
    render();
    std::cout << std::endl;

    if (!message.empty()) {
        std::cout << TerminalUtils::colorize("✓ " + message, utils::Color::BrightGreen)
                  << std::endl;
    }

    m_started = false;
}

void ProgressBarIndicator::stop() {
    if (m_started) {
        std::cout << std::endl;
        m_started = false;
    }
}

void ProgressBarIndicator::setMessage(const std::string& message) {
    m_message = message;
    if (m_started) {
        render();
    }
}

void ProgressBarIndicator::setProgress(double progress) {
    m_progress = std::clamp(progress, 0.0, 1.0);
    if (m_started) {
        render();
    }
}

void ProgressBarIndicator::setShowPercentage(bool show) {
    m_showPercentage = show;
}

void ProgressBarIndicator::setShowElapsedTime(bool show) {
    m_showElapsedTime = show;
}

void ProgressBarIndicator::setShowETA(bool show) {
    m_showETA = show;
}

void ProgressBarIndicator::setWidth(int width) {
    m_width = std::max(10, width);
}

void ProgressBarIndicator::setFillChar(char fillChar) {
    m_fillChar = fillChar;
}

void ProgressBarIndicator::setEmptyChar(char emptyChar) {
    m_emptyChar = emptyChar;
}

void ProgressBarIndicator::setBrackets(char left, char right) {
    m_leftBracket = left;
    m_rightBracket = right;
}

void ProgressBarIndicator::render() {
    std::ostringstream output;
    output << "\r";

    // Message
    if (!m_message.empty()) {
        output << m_message << " ";
    }

    // Progress bar
    output << m_leftBracket;

    int filled = static_cast<int>(m_progress * m_width);
    int empty = m_width - filled;

    // Filled portion
    output << TerminalUtils::colorize(std::string(filled, m_fillChar), utils::Color::BrightGreen);

    // Empty portion
    output << std::string(empty, m_emptyChar);

    output << m_rightBracket;

    // Percentage
    if (m_showPercentage) {
        output << " " << std::fixed << std::setprecision(1) << (m_progress * 100.0) << "%";
    }

    // Elapsed time
    if (m_showElapsedTime) {
        output << " [" << getElapsedTime() << "]";
    }

    // ETA
    if (m_showETA && m_progress > 0.0) {
        output << " ETA: " << getETA();
    }

    std::cout << output.str() << std::flush;
}

std::string ProgressBarIndicator::formatTime(std::chrono::seconds seconds) const {
    auto hours = std::chrono::duration_cast<std::chrono::hours>(seconds);
    auto minutes =
            std::chrono::duration_cast<std::chrono::minutes>(seconds % std::chrono::hours(1));
    auto secs = seconds % std::chrono::minutes(1);

    if (hours.count() > 0) {
        return std::to_string(hours.count()) + "h " + std::to_string(minutes.count()) + "m " +
               std::to_string(secs.count()) + "s";
    } else if (minutes.count() > 0) {
        return std::to_string(minutes.count()) + "m " + std::to_string(secs.count()) + "s";
    } else {
        return std::to_string(secs.count()) + "s";
    }
}

std::string ProgressBarIndicator::getElapsedTime() const {
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - m_startTime);
    return formatTime(elapsed);
}

std::string ProgressBarIndicator::getETA() const {
    if (m_progress <= 0.0) {
        return "Unknown";
    }

    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - m_startTime);

    if (elapsed.count() == 0) {
        return "Unknown";
    }

    auto totalTime = std::chrono::seconds(static_cast<long long>(elapsed.count() / m_progress));
    auto remaining = totalTime - elapsed;

    if (remaining.count() <= 0) {
        return "0s";
    }

    return formatTime(remaining);
}

// MultiStepIndicator Implementation
MultiStepIndicator::MultiStepIndicator() {}

MultiStepIndicator::~MultiStepIndicator() {
    if (m_started) {
        std::cout << std::endl;
    }
}

void MultiStepIndicator::start(const std::string& message) {
    m_startTime = std::chrono::steady_clock::now();
    m_started = true;

    if (!message.empty()) {
        std::cout << TerminalUtils::colorize(message, utils::Color::BrightCyan) << std::endl;
    }

    render();
}

void MultiStepIndicator::update(double progress, const std::string& message) {
    // Update current step progress
    (void)message;  // TODO: Use message to update step description
    if (m_currentStepIndex >= 0 && m_currentStepIndex < static_cast<int>(m_steps.size())) {
        m_currentStepProgress = std::clamp(progress, 0.0, 1.0);
    }

    render();
}

void MultiStepIndicator::finish(const std::string& message) {
    // Complete all remaining steps
    for (auto& step : m_steps) {
        if (!step.completed && !step.failed) {
            step.completed = true;
            step.endTime = std::chrono::steady_clock::now();
        }
    }

    render();
    std::cout << std::endl;

    if (!message.empty()) {
        std::cout << TerminalUtils::colorize("✓ " + message, utils::Color::BrightGreen)
                  << std::endl;
    }

    m_started = false;
}

void MultiStepIndicator::stop() {
    if (m_started) {
        std::cout << std::endl;
        m_started = false;
    }
}

void MultiStepIndicator::setMessage(const std::string& message) {
    // Update current step message
    if (m_currentStepIndex >= 0 && m_currentStepIndex < static_cast<int>(m_steps.size())) {
        m_steps[m_currentStepIndex].description = message;
        render();
    }
}

void MultiStepIndicator::setProgress(double progress) {
    m_currentStepProgress = std::clamp(progress, 0.0, 1.0);
    render();
}

void MultiStepIndicator::setShowPercentage(bool show) {
    m_showPercentage = show;
}

void MultiStepIndicator::setShowElapsedTime(bool show) {
    m_showElapsedTime = show;
}

void MultiStepIndicator::setShowETA(bool show) {
    m_showETA = show;
}

void MultiStepIndicator::addStep(const std::string& name, const std::string& description) {
    Step step;
    step.name = name;
    step.description = description;
    m_steps.push_back(step);
}

void MultiStepIndicator::startStep(const std::string& name) {
    auto* step = findStep(name);
    if (step) {
        step->startTime = std::chrono::steady_clock::now();
        m_currentStepName = name;

        // Find step index
        for (size_t i = 0; i < m_steps.size(); ++i) {
            if (m_steps[i].name == name) {
                m_currentStepIndex = static_cast<int>(i);
                break;
            }
        }

        m_currentStepProgress = 0.0;
        render();
    }
}

void MultiStepIndicator::completeStep(const std::string& name) {
    auto* step = findStep(name);
    if (step) {
        step->completed = true;
        step->endTime = std::chrono::steady_clock::now();

        if (name == m_currentStepName) {
            m_currentStepProgress = 1.0;
        }

        render();
    }
}

void MultiStepIndicator::failStep(const std::string& name, const std::string& error) {
    auto* step = findStep(name);
    if (step) {
        step->failed = true;
        step->endTime = std::chrono::steady_clock::now();

        if (!error.empty()) {
            step->description = error;
        }

        render();
    }
}

void MultiStepIndicator::setStepProgress(const std::string& name, double progress) {
    if (name == m_currentStepName) {
        m_currentStepProgress = std::clamp(progress, 0.0, 1.0);
        render();
    }
}

void MultiStepIndicator::setShowStepTimes(bool show) {
    m_showStepTimes = show;
}

void MultiStepIndicator::setShowStepProgress(bool show) {
    m_showStepProgress = show;
}

void MultiStepIndicator::render() {
    // Clear previous output
    std::cout << "\r" << std::string(80, ' ') << "\r";

    for (size_t i = 0; i < m_steps.size(); ++i) {
        const auto& step = m_steps[i];

        // Step status indicator
        std::string indicator;
        utils::Color color;

        if (step.failed) {
            indicator = "✗";
            color = utils::Color::BrightRed;
        } else if (step.completed) {
            indicator = "✓";
            color = utils::Color::BrightGreen;
        } else if (static_cast<int>(i) == m_currentStepIndex) {
            indicator = "⟳";
            color = utils::Color::BrightYellow;
        } else {
            indicator = "○";
            color = utils::Color::White;
        }

        std::cout << TerminalUtils::colorize(indicator, color) << " " << step.name;

        // Step description
        if (!step.description.empty()) {
            std::cout << ": " << step.description;
        }

        // Step progress
        if (m_showStepProgress && static_cast<int>(i) == m_currentStepIndex &&
            m_currentStepProgress > 0.0) {
            std::cout << " (" << std::fixed << std::setprecision(1)
                      << (m_currentStepProgress * 100.0) << "%)";
        }

        // Step timing
        if (m_showStepTimes && (step.completed || step.failed)) {
            auto duration =
                    std::chrono::duration_cast<std::chrono::seconds>(step.endTime - step.startTime);
            std::cout << " [" << formatTime(duration) << "]";
        }

        std::cout << std::endl;
    }

    // Move cursor back up
    if (!m_steps.empty()) {
        std::cout << "\033[" << m_steps.size() << "A";
    }

    std::cout << std::flush;
}

std::string MultiStepIndicator::formatTime(std::chrono::seconds seconds) const {
    auto hours = std::chrono::duration_cast<std::chrono::hours>(seconds);
    auto minutes =
            std::chrono::duration_cast<std::chrono::minutes>(seconds % std::chrono::hours(1));
    auto secs = seconds % std::chrono::minutes(1);

    if (hours.count() > 0) {
        return std::to_string(hours.count()) + "h " + std::to_string(minutes.count()) + "m " +
               std::to_string(secs.count()) + "s";
    } else if (minutes.count() > 0) {
        return std::to_string(minutes.count()) + "m " + std::to_string(secs.count()) + "s";
    } else {
        return std::to_string(secs.count()) + "s";
    }
}

std::string MultiStepIndicator::getElapsedTime() const {
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - m_startTime);
    return formatTime(elapsed);
}

std::string MultiStepIndicator::getETA() const {
    // Simple ETA calculation based on completed steps
    int completedSteps = 0;
    for (const auto& step : m_steps) {
        if (step.completed) {
            completedSteps++;
        }
    }

    if (completedSteps == 0) {
        return "Unknown";
    }

    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - m_startTime);

    double progress = static_cast<double>(completedSteps) / m_steps.size();
    if (progress <= 0.0) {
        return "Unknown";
    }

    auto totalTime = std::chrono::seconds(static_cast<long long>(elapsed.count() / progress));
    auto remaining = totalTime - elapsed;

    if (remaining.count() <= 0) {
        return "0s";
    }

    return formatTime(remaining);
}

MultiStepIndicator::Step* MultiStepIndicator::findStep(const std::string& name) {
    for (auto& step : m_steps) {
        if (step.name == name) {
            return &step;
        }
    }
    return nullptr;
}

// ProgressManager Implementation
ProgressManager& ProgressManager::getInstance() {
    static ProgressManager instance;
    return instance;
}

std::unique_ptr<SpinnerIndicator> ProgressManager::createSpinner() {
    auto spinner = std::make_unique<SpinnerIndicator>();
    spinner->setShowPercentage(m_globalShowPercentage);
    spinner->setShowElapsedTime(m_globalShowElapsedTime);
    spinner->setShowETA(m_globalShowETA);
    return spinner;
}

std::unique_ptr<ProgressBarIndicator> ProgressManager::createProgressBar(int width) {
    auto progressBar = std::make_unique<ProgressBarIndicator>(width);
    progressBar->setShowPercentage(m_globalShowPercentage);
    progressBar->setShowElapsedTime(m_globalShowElapsedTime);
    progressBar->setShowETA(m_globalShowETA);
    return progressBar;
}

std::unique_ptr<MultiStepIndicator> ProgressManager::createMultiStep() {
    auto multiStep = std::make_unique<MultiStepIndicator>();
    multiStep->setShowPercentage(m_globalShowPercentage);
    multiStep->setShowElapsedTime(m_globalShowElapsedTime);
    multiStep->setShowETA(m_globalShowETA);
    return multiStep;
}

// ScopedProgress Implementation
ScopedProgress::ScopedProgress(std::unique_ptr<ProgressIndicator> indicator,
                               const std::string& message)
    : m_indicator(std::move(indicator)) {
    if (m_indicator) {
        m_indicator->start(message);
    }
}

ScopedProgress::~ScopedProgress() {
    if (m_indicator && !m_finished) {
        m_indicator->finish();
    }
}

void ScopedProgress::update(double progress, const std::string& message) {
    if (m_indicator) {
        m_indicator->update(progress, message);
    }
}

void ScopedProgress::setMessage(const std::string& message) {
    if (m_indicator) {
        m_indicator->setMessage(message);
    }
}

void ScopedProgress::finish(const std::string& message) {
    if (m_indicator && !m_finished) {
        m_indicator->finish(message);
        m_finished = true;
    }
}

ProgressIndicator* ScopedProgress::operator->() {
    return m_indicator.get();
}

ProgressIndicator& ScopedProgress::operator*() {
    return *m_indicator;
}

// Utility functions
namespace progress {
std::unique_ptr<ScopedProgress> spinner(const std::string& message) {
    auto indicator = ProgressManager::getInstance().createSpinner();
    return std::make_unique<ScopedProgress>(std::move(indicator), message);
}

std::unique_ptr<ScopedProgress> progressBar(const std::string& message, int width) {
    auto indicator = ProgressManager::getInstance().createProgressBar(width);
    return std::make_unique<ScopedProgress>(std::move(indicator), message);
}

std::unique_ptr<ScopedProgress> multiStep(const std::string& message) {
    auto indicator = ProgressManager::getInstance().createMultiStep();
    return std::make_unique<ScopedProgress>(std::move(indicator), message);
}

std::unique_ptr<ScopedProgress> templateCreation(const std::string& templateType) {
    return spinner("Creating " + templateType + " template");
}

std::unique_ptr<ScopedProgress> projectGeneration(const std::string& projectName) {
    return spinner("Generating project: " + projectName);
}
}  // namespace progress

}  // namespace utils

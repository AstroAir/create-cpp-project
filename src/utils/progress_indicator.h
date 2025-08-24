#pragma once

#include <string>
#include <chrono>
#include <atomic>
#include <thread>
#include <functional>
#include <vector>
#include <memory>

namespace utils {

// Progress indicator interface
class ProgressIndicator {
public:
    virtual ~ProgressIndicator() = default;
    
    // Progress control
    virtual void start(const std::string& message = "") = 0;
    virtual void update(double progress, const std::string& message = "") = 0;
    virtual void finish(const std::string& message = "") = 0;
    virtual void stop() = 0;
    
    // Status updates
    virtual void setMessage(const std::string& message) = 0;
    virtual void setProgress(double progress) = 0;
    
    // Configuration
    virtual void setShowPercentage(bool show) = 0;
    virtual void setShowElapsedTime(bool show) = 0;
    virtual void setShowETA(bool show) = 0;
};

// Spinner progress indicator
class SpinnerIndicator : public ProgressIndicator {
public:
    SpinnerIndicator();
    ~SpinnerIndicator() override;
    
    void start(const std::string& message = "") override;
    void update(double progress, const std::string& message = "") override;
    void finish(const std::string& message = "") override;
    void stop() override;
    
    void setMessage(const std::string& message) override;
    void setProgress(double progress) override;
    
    void setShowPercentage(bool show) override;
    void setShowElapsedTime(bool show) override;
    void setShowETA(bool show) override;
    
    // Spinner customization
    void setSpinnerChars(const std::vector<std::string>& chars);
    void setSpinnerSpeed(int milliseconds);
    
private:
    std::atomic<bool> m_running{false};
    std::atomic<double> m_progress{0.0};
    std::string m_message;
    std::string m_finishMessage;
    std::vector<std::string> m_spinnerChars;
    int m_spinnerSpeed;
    std::chrono::steady_clock::time_point m_startTime;
    std::unique_ptr<std::thread> m_spinnerThread;
    
    bool m_showPercentage{false};
    bool m_showElapsedTime{false};
    bool m_showETA{false};
    
    void spinnerLoop();
    void render();
    std::string formatTime(std::chrono::seconds seconds) const;
    std::string getElapsedTime() const;
    std::string getETA() const;
};

// Progress bar indicator
class ProgressBarIndicator : public ProgressIndicator {
public:
    ProgressBarIndicator(int width = 50);
    ~ProgressBarIndicator() override;
    
    void start(const std::string& message = "") override;
    void update(double progress, const std::string& message = "") override;
    void finish(const std::string& message = "") override;
    void stop() override;
    
    void setMessage(const std::string& message) override;
    void setProgress(double progress) override;
    
    void setShowPercentage(bool show) override;
    void setShowElapsedTime(bool show) override;
    void setShowETA(bool show) override;
    
    // Progress bar customization
    void setWidth(int width);
    void setFillChar(char fillChar);
    void setEmptyChar(char emptyChar);
    void setBrackets(char left, char right);
    
private:
    int m_width;
    char m_fillChar;
    char m_emptyChar;
    char m_leftBracket;
    char m_rightBracket;
    
    std::atomic<double> m_progress{0.0};
    std::string m_message;
    std::chrono::steady_clock::time_point m_startTime;
    
    bool m_showPercentage{true};
    bool m_showElapsedTime{false};
    bool m_showETA{false};
    bool m_started{false};
    
    void render();
    std::string formatTime(std::chrono::seconds seconds) const;
    std::string getElapsedTime() const;
    std::string getETA() const;
};

// Multi-step progress indicator
class MultiStepIndicator : public ProgressIndicator {
public:
    struct Step {
        std::string name;
        std::string description;
        bool completed{false};
        bool failed{false};
        std::chrono::steady_clock::time_point startTime;
        std::chrono::steady_clock::time_point endTime;
    };
    
    MultiStepIndicator();
    ~MultiStepIndicator() override;
    
    void start(const std::string& message = "") override;
    void update(double progress, const std::string& message = "") override;
    void finish(const std::string& message = "") override;
    void stop() override;
    
    void setMessage(const std::string& message) override;
    void setProgress(double progress) override;
    
    void setShowPercentage(bool show) override;
    void setShowElapsedTime(bool show) override;
    void setShowETA(bool show) override;
    
    // Step management
    void addStep(const std::string& name, const std::string& description = "");
    void startStep(const std::string& name);
    void completeStep(const std::string& name);
    void failStep(const std::string& name, const std::string& error = "");
    void setStepProgress(const std::string& name, double progress);
    
    // Display options
    void setShowStepTimes(bool show);
    void setShowStepProgress(bool show);
    
private:
    std::vector<Step> m_steps;
    int m_currentStepIndex{-1};
    std::string m_currentStepName;
    std::atomic<double> m_currentStepProgress{0.0};
    std::chrono::steady_clock::time_point m_startTime;
    
    bool m_showPercentage{true};
    bool m_showElapsedTime{true};
    bool m_showETA{true};
    bool m_showStepTimes{true};
    bool m_showStepProgress{true};
    bool m_started{false};
    
    void render();
    std::string formatTime(std::chrono::seconds seconds) const;
    std::string getElapsedTime() const;
    std::string getETA() const;
    Step* findStep(const std::string& name);
};

// Progress manager for coordinating multiple indicators
class ProgressManager {
public:
    static ProgressManager& getInstance();
    
    // Indicator creation
    std::unique_ptr<SpinnerIndicator> createSpinner();
    std::unique_ptr<ProgressBarIndicator> createProgressBar(int width = 50);
    std::unique_ptr<MultiStepIndicator> createMultiStep();
    
    // Global progress tracking
    void startGlobalProgress(const std::string& operation);
    void updateGlobalProgress(double progress, const std::string& message = "");
    void finishGlobalProgress(const std::string& message = "");
    
    // Nested progress support
    void pushProgress(std::unique_ptr<ProgressIndicator> indicator);
    void popProgress();
    ProgressIndicator* getCurrentProgress();
    
    // Configuration
    void setDefaultIndicatorType(const std::string& type);
    void setGlobalShowPercentage(bool show);
    void setGlobalShowElapsedTime(bool show);
    void setGlobalShowETA(bool show);
    
private:
    ProgressManager() = default;
    
    std::vector<std::unique_ptr<ProgressIndicator>> m_progressStack;
    std::string m_defaultIndicatorType{"spinner"};
    
    bool m_globalShowPercentage{true};
    bool m_globalShowElapsedTime{false};
    bool m_globalShowETA{false};
};

// RAII progress wrapper
class ScopedProgress {
public:
    ScopedProgress(std::unique_ptr<ProgressIndicator> indicator, const std::string& message = "");
    ~ScopedProgress();
    
    // Progress control
    void update(double progress, const std::string& message = "");
    void setMessage(const std::string& message);
    void finish(const std::string& message = "");
    
    // Access to underlying indicator
    ProgressIndicator* operator->();
    ProgressIndicator& operator*();
    
private:
    std::unique_ptr<ProgressIndicator> m_indicator;
    bool m_finished{false};
};

// Utility functions for common progress patterns
namespace progress {
    // Simple progress for single operations
    std::unique_ptr<ScopedProgress> spinner(const std::string& message = "");
    std::unique_ptr<ScopedProgress> progressBar(const std::string& message = "", int width = 50);
    std::unique_ptr<ScopedProgress> multiStep(const std::string& message = "");
    
    // Progress for file operations
    std::unique_ptr<ScopedProgress> fileOperation(const std::string& operation, const std::string& filename);
    std::unique_ptr<ScopedProgress> directoryOperation(const std::string& operation, const std::string& directory);
    
    // Progress for network operations
    std::unique_ptr<ScopedProgress> download(const std::string& url, const std::string& filename = "");
    std::unique_ptr<ScopedProgress> upload(const std::string& filename, const std::string& destination = "");
    
    // Progress for build operations
    std::unique_ptr<ScopedProgress> buildOperation(const std::string& target = "");
    std::unique_ptr<ScopedProgress> testOperation(const std::string& suite = "");
    
    // Progress for template operations
    std::unique_ptr<ScopedProgress> templateCreation(const std::string& templateType);
    std::unique_ptr<ScopedProgress> projectGeneration(const std::string& projectName);
}

} // namespace utils

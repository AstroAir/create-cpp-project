#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <sstream>
#include <iostream>

#include "../../src/utils/context_sensitive_error_system.h"

using namespace utils;
using ::testing::_;
using ::testing::Return;
using ::testing::HasSubstr;

class ContextSensitiveErrorSystemTest : public ::testing::Test {
protected:
    void SetUp() override {
        system_ = &ContextSensitiveErrorSystem::getInstance();
        
        // Capture cout for testing output
        original_cout_ = std::cout.rdbuf();
        std::cout.rdbuf(captured_output_.rdbuf());
        
        // Capture cin for testing input
        original_cin_ = std::cin.rdbuf();
        
        // Configure system for testing
        system_->setAutoFixEnabled(false);
        system_->setInteractiveMode(false);
        system_->setVerboseErrors(true);
    }
    
    void TearDown() override {
        // Restore original streams
        std::cout.rdbuf(original_cout_);
        std::cin.rdbuf(original_cin_);
    }
    
    void simulateUserInput(const std::string& input) {
        input_stream_.str(input);
        input_stream_.clear();
        std::cin.rdbuf(input_stream_.rdbuf());
    }
    
    std::string getCapturedOutput() {
        return captured_output_.str();
    }
    
    void clearCapturedOutput() {
        captured_output_.str("");
        captured_output_.clear();
    }

protected:
    ContextSensitiveErrorSystem* system_;
    std::stringstream captured_output_;
    std::stringstream input_stream_;
    std::streambuf* original_cout_;
    std::streambuf* original_cin_;
};

// Test basic error reporting
TEST_F(ContextSensitiveErrorSystemTest, BasicErrorReporting) {
    system_->reportError("Test error message", ErrorContext::FileOperations, "Creating file");
    
    std::string output = getCapturedOutput();
    EXPECT_THAT(output, HasSubstr("Test error message"));
    EXPECT_THAT(output, HasSubstr("Creating file"));
}

// Test context-specific error reporting
TEST_F(ContextSensitiveErrorSystemTest, ProjectCreationError) {
    system_->reportProjectCreationError("Failed to create project", "/path/to/project", "console");
    
    std::string output = getCapturedOutput();
    EXPECT_THAT(output, HasSubstr("Failed to create project"));
    EXPECT_THAT(output, HasSubstr("/path/to/project"));
}

TEST_F(ContextSensitiveErrorSystemTest, FileOperationError) {
    system_->reportFileOperationError("write", "/path/to/file.txt", "Permission denied");
    
    std::string output = getCapturedOutput();
    EXPECT_THAT(output, HasSubstr("Failed to write file"));
    EXPECT_THAT(output, HasSubstr("/path/to/file.txt"));
    EXPECT_THAT(output, HasSubstr("Permission denied"));
}

TEST_F(ContextSensitiveErrorSystemTest, BuildSystemError) {
    system_->reportBuildSystemError("cmake", "cmake --build .", "Make not found");
    
    std::string output = getCapturedOutput();
    EXPECT_THAT(output, HasSubstr("cmake build failed"));
    EXPECT_THAT(output, HasSubstr("cmake --build ."));
}

// Test error context types
TEST_F(ContextSensitiveErrorSystemTest, DifferentErrorContexts) {
    std::vector<ErrorContext> contexts = {
        ErrorContext::ProjectCreation,
        ErrorContext::FileOperations,
        ErrorContext::BuildSystem,
        ErrorContext::PackageManagement,
        ErrorContext::GitOperations,
        ErrorContext::Configuration
    };
    
    for (auto context : contexts) {
        clearCapturedOutput();
        system_->reportError("Test error", context, "Test action");
        
        std::string output = getCapturedOutput();
        EXPECT_THAT(output, HasSubstr("Test error"));
        EXPECT_FALSE(output.empty());
    }
}

// Test solution generation
TEST_F(ContextSensitiveErrorSystemTest, SolutionGeneration) {
    // Create a contextual error
    ContextualError error;
    error.message = "Permission denied";
    error.context = ErrorContext::FileOperations;
    error.userAction = "Writing to file";
    error.metadata["operation"] = "write";
    error.metadata["file_path"] = "/protected/file.txt";
    
    system_->reportContextualError(error);
    
    std::string output = getCapturedOutput();
    EXPECT_THAT(output, HasSubstr("Permission denied"));
    EXPECT_THAT(output, HasSubstr("Writing to file"));
}

// Test error history
TEST_F(ContextSensitiveErrorSystemTest, ErrorHistory) {
    system_->reportError("Error 1", ErrorContext::FileOperations, "Action 1");
    system_->reportError("Error 2", ErrorContext::BuildSystem, "Action 2");
    system_->reportError("Error 3", ErrorContext::GitOperations, "Action 3");
    
    auto recentErrors = system_->getRecentErrors(2);
    EXPECT_EQ(recentErrors.size(), 2);
    
    auto allErrors = system_->getRecentErrors(10);
    EXPECT_EQ(allErrors.size(), 3);
}

TEST_F(ContextSensitiveErrorSystemTest, ErrorsByContext) {
    system_->reportError("File Error 1", ErrorContext::FileOperations, "Action 1");
    system_->reportError("Build Error 1", ErrorContext::BuildSystem, "Action 2");
    system_->reportError("File Error 2", ErrorContext::FileOperations, "Action 3");
    
    auto fileErrors = system_->getErrorsByContext(ErrorContext::FileOperations);
    EXPECT_EQ(fileErrors.size(), 2);
    
    auto buildErrors = system_->getErrorsByContext(ErrorContext::BuildSystem);
    EXPECT_EQ(buildErrors.size(), 1);
}

// Test configuration
TEST_F(ContextSensitiveErrorSystemTest, ConfigurationSettings) {
    system_->setAutoFixEnabled(true);
    system_->setInteractiveMode(true);
    system_->setVerboseErrors(false);
    
    // Test that settings are applied
    system_->reportError("Test error", ErrorContext::Configuration, "Test action");
    
    // Should not throw and should handle the configuration
    EXPECT_NO_THROW(system_->reportError("Another error", ErrorContext::FileOperations, "Another action"));
}

// Test error enrichment
TEST_F(ContextSensitiveErrorSystemTest, ErrorEnrichment) {
    std::map<std::string, std::string> metadata;
    metadata["file_path"] = "/test/path";
    metadata["operation"] = "read";
    metadata["error_code"] = "ENOENT";
    
    system_->reportError("File not found", ErrorContext::FileOperations, "Reading configuration", metadata);
    
    std::string output = getCapturedOutput();
    EXPECT_THAT(output, HasSubstr("File not found"));
    EXPECT_THAT(output, HasSubstr("Reading configuration"));
}

// Test error summary
TEST_F(ContextSensitiveErrorSystemTest, ErrorSummary) {
    system_->reportError("Error 1", ErrorContext::FileOperations, "Action 1");
    system_->reportError("Error 2", ErrorContext::FileOperations, "Action 2");
    system_->reportError("Error 3", ErrorContext::BuildSystem, "Action 3");
    
    clearCapturedOutput();
    system_->showErrorSummary();
    
    std::string output = getCapturedOutput();
    EXPECT_THAT(output, HasSubstr("Error Summary"));
    EXPECT_FALSE(output.empty());
}

// Test macro usage
TEST_F(ContextSensitiveErrorSystemTest, MacroUsage) {
    REPORT_PROJECT_ERROR("Project creation failed", "/test/project");
    
    std::string output = getCapturedOutput();
    EXPECT_THAT(output, HasSubstr("Project creation failed"));
    EXPECT_THAT(output, HasSubstr("/test/project"));
}

TEST_F(ContextSensitiveErrorSystemTest, FileErrorMacro) {
    REPORT_FILE_ERROR("write", "/test/file.txt", "Disk full");
    
    std::string output = getCapturedOutput();
    EXPECT_THAT(output, HasSubstr("Failed to write file"));
    EXPECT_THAT(output, HasSubstr("/test/file.txt"));
    EXPECT_THAT(output, HasSubstr("Disk full"));
}

TEST_F(ContextSensitiveErrorSystemTest, BuildErrorMacro) {
    REPORT_BUILD_ERROR("cmake", "cmake --build", "Compiler not found");
    
    std::string output = getCapturedOutput();
    EXPECT_THAT(output, HasSubstr("cmake build failed"));
    EXPECT_THAT(output, HasSubstr("cmake --build"));
}

TEST_F(ContextSensitiveErrorSystemTest, GitErrorMacro) {
    REPORT_GIT_ERROR("git clone", "https://github.com/test/repo.git", "Network error");
    
    std::string output = getCapturedOutput();
    EXPECT_THAT(output, HasSubstr("git clone"));
    EXPECT_THAT(output, HasSubstr("https://github.com/test/repo.git"));
}

// Test edge cases
TEST_F(ContextSensitiveErrorSystemTest, EmptyErrorMessage) {
    system_->reportError("", ErrorContext::FileOperations, "");
    
    std::string output = getCapturedOutput();
    EXPECT_FALSE(output.empty()); // Should still produce some output
}

TEST_F(ContextSensitiveErrorSystemTest, VeryLongErrorMessage) {
    std::string longMessage(5000, 'A');
    std::string longAction(1000, 'B');
    
    system_->reportError(longMessage, ErrorContext::FileOperations, longAction);
    
    std::string output = getCapturedOutput();
    EXPECT_THAT(output, HasSubstr(longMessage.substr(0, 100))); // Should contain part of the message
}

// Test concurrent access
TEST_F(ContextSensitiveErrorSystemTest, SingletonInstance) {
    auto& instance1 = ContextSensitiveErrorSystem::getInstance();
    auto& instance2 = ContextSensitiveErrorSystem::getInstance();
    
    EXPECT_EQ(&instance1, &instance2);
}

// Test error pattern matching (if implemented)
TEST_F(ContextSensitiveErrorSystemTest, ErrorPatternMatching) {
    // Test that similar errors generate appropriate solutions
    system_->reportFileOperationError("read", "/nonexistent/file.txt", "No such file or directory");
    
    std::string output = getCapturedOutput();
    EXPECT_THAT(output, HasSubstr("Failed to read file"));
    EXPECT_THAT(output, HasSubstr("/nonexistent/file.txt"));
}

// Test interactive mode (basic)
TEST_F(ContextSensitiveErrorSystemTest, InteractiveModeBasic) {
    system_->setInteractiveMode(true);
    simulateUserInput("1\n"); // Choose first option
    
    system_->reportError("Test interactive error", ErrorContext::FileOperations, "Test action");
    
    // Should handle interactive mode without crashing
    EXPECT_NO_THROW(system_->reportError("Another error", ErrorContext::BuildSystem, "Another action"));
}

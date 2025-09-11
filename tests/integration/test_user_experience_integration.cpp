#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <filesystem>
#include <fstream>
#include <sstream>

#include "../../src/utils/enhanced_confirmation_system.h"
#include "../../src/utils/context_sensitive_error_system.h"
#include "../../src/utils/operation_rollback_system.h"
#include "../../src/utils/enhanced_help_system.h"
#include "../../src/utils/file_utils.h"

using namespace utils;
using ::testing::_;
using ::testing::Return;
using ::testing::HasSubstr;

class UserExperienceIntegrationTest : public ::testing::Test {
protected:
    void SetUp() override {
        confirmationSystem_ = &EnhancedConfirmationSystem::getInstance();
        errorSystem_ = &ContextSensitiveErrorSystem::getInstance();
        rollbackSystem_ = &OperationRollbackSystem::getInstance();
        helpSystem_ = &EnhancedHelpSystem::getInstance();
        
        // Create a temporary directory for testing
        test_dir_ = std::filesystem::temp_directory_path() / "ux_integration_test";
        std::filesystem::create_directories(test_dir_);
        
        // Capture streams
        original_cout_ = std::cout.rdbuf();
        std::cout.rdbuf(captured_output_.rdbuf());
        
        original_cin_ = std::cin.rdbuf();
        
        // Configure systems for testing
        errorSystem_->setAutoFixEnabled(false);
        errorSystem_->setInteractiveMode(false);
        rollbackSystem_->clearAllCheckpoints();
    }
    
    void TearDown() override {
        // Restore streams
        std::cout.rdbuf(original_cout_);
        std::cin.rdbuf(original_cin_);
        
        // Clean up
        if (std::filesystem::exists(test_dir_)) {
            std::filesystem::remove_all(test_dir_);
        }
        rollbackSystem_->clearAllCheckpoints();
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
    EnhancedConfirmationSystem* confirmationSystem_;
    ContextSensitiveErrorSystem* errorSystem_;
    OperationRollbackSystem* rollbackSystem_;
    EnhancedHelpSystem* helpSystem_;
    
    std::filesystem::path test_dir_;
    std::stringstream captured_output_;
    std::stringstream input_stream_;
    std::streambuf* original_cout_;
    std::streambuf* original_cin_;
};

// Test integrated workflow: Operation with confirmation, error handling, and rollback
TEST_F(UserExperienceIntegrationTest, CompleteWorkflowWithRollback) {
    // Simulate user confirming a risky operation
    simulateUserInput("yes\n");
    
    bool confirmed = confirmationSystem_->confirmWithRisk(
        "Delete important files", 
        "This will permanently delete files", 
        RiskLevel::High
    );
    
    EXPECT_TRUE(confirmed);
    
    // Start a rollback checkpoint
    std::string checkpointId = rollbackSystem_->createCheckpoint("File Deletion Operation");
    
    // Track file operations
    auto testFile = test_dir_ / "important_file.txt";
    std::ofstream file(testFile);
    file << "important data";
    file.close();
    
    rollbackSystem_->trackFileCreation(testFile);
    
    // Simulate an error during operation
    errorSystem_->reportFileOperationError("delete", testFile.string(), "Permission denied");
    
    // Offer rollback due to error
    simulateUserInput("y\n");
    bool rollbackOffered = rollbackSystem_->offerRollback("Operation failed due to permission error");
    
    EXPECT_TRUE(rollbackOffered);
    EXPECT_FALSE(std::filesystem::exists(testFile)); // Should be rolled back
    
    std::string output = getCapturedOutput();
    EXPECT_THAT(output, HasSubstr("Delete important files"));
    EXPECT_THAT(output, HasSubstr("Failed to delete file"));
    EXPECT_THAT(output, HasSubstr("Rollback"));
}

// Test error system generating solutions and confirmation system handling them
TEST_F(UserExperienceIntegrationTest, ErrorSolutionWithConfirmation) {
    // Report a file permission error
    errorSystem_->reportFileOperationError("write", "/protected/file.txt", "Permission denied");
    
    // Simulate user choosing to apply a solution
    simulateUserInput("1\n"); // Choose first solution option
    
    std::string output = getCapturedOutput();
    EXPECT_THAT(output, HasSubstr("Failed to write file"));
    EXPECT_THAT(output, HasSubstr("Permission denied"));
}

// Test help system integration with error context
TEST_F(UserExperienceIntegrationTest, ContextualHelpWithErrors) {
    // Report an error in a specific context
    errorSystem_->reportError("Build failed", ErrorContext::BuildSystem, "Running cmake");
    
    clearCapturedOutput();
    
    // Show contextual help for build system issues
    helpSystem_->showContextualHelp(UserContext::BuildSystemUsage, "cmake");
    
    std::string output = getCapturedOutput();
    EXPECT_FALSE(output.empty());
}

// Test RAII operation with confirmation and error handling
TEST_F(UserExperienceIntegrationTest, ScopedOperationWithConfirmationAndError) {
    // Confirm a complex operation
    simulateUserInput("y\ny\ny\n");
    
    std::vector<std::string> steps = {
        "Create backup",
        "Modify files",
        "Update configuration"
    };
    
    bool confirmed = confirmationSystem_->confirmMultiStep("Complex Update", steps);
    EXPECT_TRUE(confirmed);
    
    auto testFile = test_dir_ / "config.txt";
    
    {
        ScopedOperation operation("Complex Update", "Multi-step configuration update");
        
        // Track operations
        operation.trackFileCreation(testFile);
        
        // Create file
        std::ofstream file(testFile);
        file << "new configuration";
        file.close();
        
        EXPECT_TRUE(std::filesystem::exists(testFile));
        
        // Simulate error - don't commit, should auto-rollback
        errorSystem_->reportError("Configuration validation failed", ErrorContext::Configuration, "Validating config");
    }
    
    // File should be rolled back due to error
    EXPECT_FALSE(std::filesystem::exists(testFile));
    
    std::string output = getCapturedOutput();
    EXPECT_THAT(output, HasSubstr("Complex Update"));
    EXPECT_THAT(output, HasSubstr("Configuration validation failed"));
}

// Test confirmation system with different risk levels and error reporting
TEST_F(UserExperienceIntegrationTest, RiskBasedConfirmationWithErrors) {
    // Test low risk operation
    simulateUserInput("y\n");
    bool lowRiskConfirmed = confirmationSystem_->confirmWithRisk("Safe operation", "This is safe", RiskLevel::Low);
    EXPECT_TRUE(lowRiskConfirmed);
    
    // Test high risk operation with error
    simulateUserInput("yes\n");
    bool highRiskConfirmed = confirmationSystem_->confirmWithRisk("Dangerous operation", "This could cause damage", RiskLevel::High);
    EXPECT_TRUE(highRiskConfirmed);
    
    // Report error during high risk operation
    errorSystem_->reportError("High risk operation failed", ErrorContext::SystemValidation, "Executing dangerous operation");
    
    std::string output = getCapturedOutput();
    EXPECT_THAT(output, HasSubstr("Safe operation"));
    EXPECT_THAT(output, HasSubstr("Dangerous operation"));
    EXPECT_THAT(output, HasSubstr("High risk operation failed"));
}

// Test rollback system with confirmation for dangerous rollbacks
TEST_F(UserExperienceIntegrationTest, RollbackWithConfirmation) {
    std::string checkpointId = rollbackSystem_->createCheckpoint("Test Operation");
    
    auto testFile = test_dir_ / "test_file.txt";
    rollbackSystem_->trackFileModification(testFile, "original content");
    
    // Create and modify file
    std::ofstream file(testFile);
    file << "original content";
    file.close();
    
    // Modify the file
    std::ofstream modFile(testFile);
    modFile << "modified content";
    modFile.close();
    
    // Simulate confirming a potentially dangerous rollback
    simulateUserInput("yes\n");
    
    auto result = rollbackSystem_->rollbackToCheckpoint(checkpointId);
    EXPECT_TRUE(result.success);
    
    // Verify rollback worked
    std::string content = FileUtils::readFromFile(testFile.string());
    EXPECT_EQ(content, "original content");
}

// Test error system with multiple contexts and help integration
TEST_F(UserExperienceIntegrationTest, MultiContextErrorHandling) {
    // Report errors in different contexts
    errorSystem_->reportProjectCreationError("Failed to create project", "/test/project", "console");
    errorSystem_->reportBuildSystemError("cmake", "cmake --build", "Compiler not found");
    errorSystem_->reportFileOperationError("read", "/config/file.txt", "File not found");
    
    // Check error history
    auto recentErrors = errorSystem_->getRecentErrors(5);
    EXPECT_EQ(recentErrors.size(), 3);
    
    auto fileErrors = errorSystem_->getErrorsByContext(ErrorContext::FileOperations);
    EXPECT_EQ(fileErrors.size(), 1);
    
    auto buildErrors = errorSystem_->getErrorsByContext(ErrorContext::BuildSystem);
    EXPECT_EQ(buildErrors.size(), 1);
    
    std::string output = getCapturedOutput();
    EXPECT_THAT(output, HasSubstr("Failed to create project"));
    EXPECT_THAT(output, HasSubstr("cmake build failed"));
    EXPECT_THAT(output, HasSubstr("Failed to read file"));
}

// Test help system with different user contexts
TEST_F(UserExperienceIntegrationTest, ContextualHelpSystem) {
    clearCapturedOutput();
    
    // Test help for different contexts
    helpSystem_->showContextualHelp(UserContext::FirstTimeUser);
    std::string firstTimeOutput = getCapturedOutput();
    EXPECT_FALSE(firstTimeOutput.empty());
    
    clearCapturedOutput();
    helpSystem_->showContextualHelp(UserContext::AdvancedUser);
    std::string advancedOutput = getCapturedOutput();
    EXPECT_FALSE(advancedOutput.empty());
    
    clearCapturedOutput();
    helpSystem_->showContextualHelp(UserContext::BuildSystemUsage);
    std::string buildOutput = getCapturedOutput();
    EXPECT_FALSE(buildOutput.empty());
}

// Test complete failure scenario with all systems
TEST_F(UserExperienceIntegrationTest, CompleteFailureScenario) {
    // Start with confirmation
    simulateUserInput("DELETE\n");
    bool confirmed = confirmationSystem_->confirmDestructive(
        "Delete project", 
        "my-project", 
        "This will permanently delete the entire project", 
        RiskLevel::Critical
    );
    EXPECT_TRUE(confirmed);
    
    // Start rollback tracking
    std::string checkpointId = rollbackSystem_->createCheckpoint("Project Deletion");
    
    // Track multiple file operations
    auto projectDir = test_dir_ / "my-project";
    auto configFile = projectDir / "config.json";
    auto sourceFile = projectDir / "main.cpp";
    
    std::filesystem::create_directories(projectDir);
    rollbackSystem_->trackDirectoryCreation(projectDir);
    
    std::ofstream config(configFile);
    config << "{}";
    config.close();
    rollbackSystem_->trackFileCreation(configFile);
    
    std::ofstream source(sourceFile);
    source << "#include <iostream>";
    source.close();
    rollbackSystem_->trackFileCreation(sourceFile);
    
    // Simulate failure during deletion
    errorSystem_->reportFileOperationError("delete", sourceFile.string(), "File is locked by another process");
    
    // Offer rollback
    simulateUserInput("y\n");
    bool rollbackOffered = rollbackSystem_->offerRollback("Deletion failed");
    EXPECT_TRUE(rollbackOffered);
    
    // Verify everything was rolled back
    EXPECT_FALSE(std::filesystem::exists(projectDir));
    
    std::string output = getCapturedOutput();
    EXPECT_THAT(output, HasSubstr("Delete project"));
    EXPECT_THAT(output, HasSubstr("Failed to delete file"));
    EXPECT_THAT(output, HasSubstr("Rollback"));
}

// Test system configuration and interaction
TEST_F(UserExperienceIntegrationTest, SystemConfiguration) {
    // Configure all systems
    confirmationSystem_->setDefaultTimeout(std::chrono::seconds(30));
    confirmationSystem_->setRequireExplicitConfirmation(true);
    
    errorSystem_->setAutoFixEnabled(true);
    errorSystem_->setInteractiveMode(true);
    
    rollbackSystem_->setMaxCheckpoints(10);
    rollbackSystem_->setAutoCleanup(true);
    
    // Test that systems work with new configuration
    simulateUserInput("y\n");
    bool confirmed = confirmationSystem_->confirm("Test", "Message");
    EXPECT_TRUE(confirmed);
    
    errorSystem_->reportError("Test error", ErrorContext::Configuration, "Test action");
    
    std::string checkpointId = rollbackSystem_->createCheckpoint("Test checkpoint");
    EXPECT_FALSE(checkpointId.empty());
    
    EXPECT_NO_THROW(rollbackSystem_->completeCheckpoint(checkpointId));
}

// Test singleton instances are properly shared
TEST_F(UserExperienceIntegrationTest, SingletonConsistency) {
    auto& confirmSystem1 = EnhancedConfirmationSystem::getInstance();
    auto& confirmSystem2 = EnhancedConfirmationSystem::getInstance();
    EXPECT_EQ(&confirmSystem1, &confirmSystem2);
    
    auto& errorSystem1 = ContextSensitiveErrorSystem::getInstance();
    auto& errorSystem2 = ContextSensitiveErrorSystem::getInstance();
    EXPECT_EQ(&errorSystem1, &errorSystem2);
    
    auto& rollbackSystem1 = OperationRollbackSystem::getInstance();
    auto& rollbackSystem2 = OperationRollbackSystem::getInstance();
    EXPECT_EQ(&rollbackSystem1, &rollbackSystem2);
    
    auto& helpSystem1 = EnhancedHelpSystem::getInstance();
    auto& helpSystem2 = EnhancedHelpSystem::getInstance();
    EXPECT_EQ(&helpSystem1, &helpSystem2);
}

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <thread>
#include <chrono>

#include "../../src/utils/validation/enhanced_confirmation_system.h"
#include "../../src/utils/validation/context_sensitive_error_system.h"
#include "../../src/utils/validation/operation_rollback_system.h"
#include "../../src/utils/validation/enhanced_help_system.h"

using namespace utils;
using ::testing::_;
using ::testing::Return;
using ::testing::HasSubstr;

class UserExperienceEdgeCasesTest : public ::testing::Test {
protected:
    void SetUp() override {
        confirmationSystem_ = &EnhancedConfirmationSystem::getInstance();
        errorSystem_ = &ContextSensitiveErrorSystem::getInstance();
        rollbackSystem_ = &OperationRollbackSystem::getInstance();
        helpSystem_ = &EnhancedHelpSystem::getInstance();
        
        test_dir_ = std::filesystem::temp_directory_path() / "ux_edge_cases_test";
        std::filesystem::create_directories(test_dir_);
        
        // Capture streams
        original_cout_ = std::cout.rdbuf();
        std::cout.rdbuf(captured_output_.rdbuf());
        
        original_cin_ = std::cin.rdbuf();
        
        // Configure for testing
        errorSystem_->setAutoFixEnabled(false);
        errorSystem_->setInteractiveMode(false);
        rollbackSystem_->clearAllCheckpoints();
    }
    
    void TearDown() override {
        std::cout.rdbuf(original_cout_);
        std::cin.rdbuf(original_cin_);
        
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

// Test extremely long inputs
TEST_F(UserExperienceEdgeCasesTest, ExtremelyLongInputs) {
    std::string veryLongTitle(10000, 'A');
    std::string veryLongMessage(50000, 'B');
    
    simulateUserInput("y\n");
    
    EXPECT_NO_THROW({
        bool result = confirmationSystem_->confirm(veryLongTitle, veryLongMessage);
        EXPECT_TRUE(result);
    });
    
    EXPECT_NO_THROW({
        errorSystem_->reportError(veryLongMessage, ErrorContext::FileOperations, veryLongTitle);
    });
    
    EXPECT_NO_THROW({
        std::string checkpointId = rollbackSystem_->createCheckpoint(veryLongTitle, veryLongMessage);
        EXPECT_FALSE(checkpointId.empty());
    });
}

// Test empty and null inputs
TEST_F(UserExperienceEdgeCasesTest, EmptyAndNullInputs) {
    simulateUserInput("y\n");
    
    // Empty strings
    EXPECT_NO_THROW({
        bool result = confirmationSystem_->confirm("", "");
        EXPECT_TRUE(result);
    });
    
    EXPECT_NO_THROW({
        errorSystem_->reportError("", ErrorContext::FileOperations, "");
    });
    
    EXPECT_NO_THROW({
        std::string checkpointId = rollbackSystem_->createCheckpoint("", "");
        EXPECT_FALSE(checkpointId.empty());
    });
    
    // Empty vectors
    std::vector<std::string> emptySteps;
    simulateUserInput("y\n");
    EXPECT_NO_THROW({
        bool result = confirmationSystem_->confirmMultiStep("Empty steps", emptySteps);
        EXPECT_TRUE(result); // Should handle gracefully
    });
    
    std::vector<std::string> emptyOptions;
    EXPECT_NO_THROW({
        int result = confirmationSystem_->confirmWithOptions("Empty options", "Choose:", emptyOptions, 0);
        EXPECT_EQ(result, -1); // Should indicate no valid choice
    });
}

// Test special characters and Unicode
TEST_F(UserExperienceEdgeCasesTest, SpecialCharactersAndUnicode) {
    std::string unicodeText = "🚀 Test with émojis and spëcial chars: ñáéíóú 中文 العربية";
    std::string specialChars = "!@#$%^&*()_+-=[]{}|;':\",./<>?`~";
    
    simulateUserInput("y\n");
    
    EXPECT_NO_THROW({
        bool result = confirmationSystem_->confirm(unicodeText, specialChars);
        EXPECT_TRUE(result);
    });
    
    EXPECT_NO_THROW({
        errorSystem_->reportError(unicodeText, ErrorContext::FileOperations, specialChars);
    });
    
    EXPECT_NO_THROW({
        std::string checkpointId = rollbackSystem_->createCheckpoint(unicodeText, specialChars);
        EXPECT_FALSE(checkpointId.empty());
    });
}

// Test invalid file paths and operations
TEST_F(UserExperienceEdgeCasesTest, InvalidFilePathsAndOperations) {
    std::vector<std::string> invalidPaths = {
        "",
        "/nonexistent/deeply/nested/path/file.txt",
        "C:\\invalid\\windows\\path\\file.txt",
        "/dev/null/cannot/create/here",
        std::string(1000, 'x') + ".txt", // Very long filename
        "file\0with\0nulls.txt", // Null characters
        "file\nwith\nnewlines.txt", // Newlines
        "file\twith\ttabs.txt" // Tabs
    };
    
    std::string checkpointId = rollbackSystem_->createCheckpoint("Invalid Paths Test");
    
    for (const auto& path : invalidPaths) {
        EXPECT_NO_THROW({
            rollbackSystem_->trackFileCreation(path);
            rollbackSystem_->trackFileModification(path, "content");
            rollbackSystem_->trackDirectoryCreation(path);
        });
        
        EXPECT_NO_THROW({
            errorSystem_->reportFileOperationError("read", path, "Invalid path");
        });
    }
    
    // Rollback should handle invalid paths gracefully
    EXPECT_NO_THROW({
        auto result = rollbackSystem_->rollbackToCheckpoint(checkpointId);
        // May succeed or fail, but shouldn't crash
    });
}

// Test memory and resource limits
TEST_F(UserExperienceEdgeCasesTest, MemoryAndResourceLimits) {
    // Create many checkpoints to test memory usage
    std::vector<std::string> checkpointIds;
    
    for (int i = 0; i < 1000; ++i) {
        std::string checkpointId = rollbackSystem_->createCheckpoint("Checkpoint " + std::to_string(i));
        checkpointIds.push_back(checkpointId);
        
        // Track some operations
        rollbackSystem_->trackFileCreation("/fake/path" + std::to_string(i) + ".txt");
        rollbackSystem_->completeCheckpoint(checkpointId);
    }
    
    // System should handle many checkpoints
    auto checkpoints = rollbackSystem_->getCheckpoints(1000);
    EXPECT_LE(checkpoints.size(), 1000);
    
    // Test cleanup
    EXPECT_NO_THROW({
        rollbackSystem_->cleanupOldCheckpoints(std::chrono::hours(0));
    });
}

// Test concurrent access and thread safety
TEST_F(UserExperienceEdgeCasesTest, ConcurrentAccess) {
    const int numThreads = 10;
    const int operationsPerThread = 100;
    std::vector<std::thread> threads;
    
    // Test concurrent error reporting
    for (int t = 0; t < numThreads; ++t) {
        threads.emplace_back([this, t, operationsPerThread]() {
            for (int i = 0; i < operationsPerThread; ++i) {
                errorSystem_->reportError(
                    "Thread " + std::to_string(t) + " Error " + std::to_string(i),
                    ErrorContext::FileOperations,
                    "Concurrent test"
                );
            }
        });
    }
    
    for (auto& thread : threads) {
        thread.join();
    }
    
    // Check that all errors were recorded
    auto errors = errorSystem_->getRecentErrors(numThreads * operationsPerThread);
    EXPECT_EQ(errors.size(), numThreads * operationsPerThread);
    
    threads.clear();
    
    // Test concurrent checkpoint creation
    std::vector<std::string> checkpointIds(numThreads);
    
    for (int t = 0; t < numThreads; ++t) {
        threads.emplace_back([this, t, &checkpointIds]() {
            checkpointIds[t] = rollbackSystem_->createCheckpoint("Thread " + std::to_string(t));
            rollbackSystem_->completeCheckpoint(checkpointIds[t]);
        });
    }
    
    for (auto& thread : threads) {
        thread.join();
    }
    
    // All checkpoints should be created successfully
    for (const auto& id : checkpointIds) {
        EXPECT_FALSE(id.empty());
    }
}

// Test system behavior under low disk space
TEST_F(UserExperienceEdgeCasesTest, LowDiskSpaceSimulation) {
    // Create a very large file to simulate low disk space
    auto largePath = test_dir_ / "large_file.txt";
    
    std::string checkpointId = rollbackSystem_->createCheckpoint("Disk Space Test");
    rollbackSystem_->trackFileCreation(largePath);
    
    // Simulate disk space error
    EXPECT_NO_THROW({
        errorSystem_->reportFileOperationError("write", largePath.string(), "No space left on device");
    });
    
    // Rollback should handle disk space issues
    EXPECT_NO_THROW({
        auto result = rollbackSystem_->rollbackToCheckpoint(checkpointId);
    });
}

// Test interrupted operations
TEST_F(UserExperienceEdgeCasesTest, InterruptedOperations) {
    std::string checkpointId = rollbackSystem_->createCheckpoint("Interrupted Operation");
    
    auto testFile = test_dir_ / "interrupted_file.txt";
    rollbackSystem_->trackFileCreation(testFile);
    
    // Create file
    std::ofstream file(testFile);
    file << "partial content";
    // Don't close file properly to simulate interruption
    
    // Try to rollback with file potentially still open
    EXPECT_NO_THROW({
        auto result = rollbackSystem_->rollbackToCheckpoint(checkpointId);
    });
}

// Test malformed input handling
TEST_F(UserExperienceEdgeCasesTest, MalformedInputHandling) {
    // Test with various malformed inputs
    std::vector<std::string> malformedInputs = {
        "\n\n\n",
        "   \t  \r\n  ",
        "\x00\x01\x02\x03",
        "y\nno\nmaybe\ny\n",
        "1\n2\n3\n4\n5\ny\n"
    };
    
    for (const auto& input : malformedInputs) {
        simulateUserInput(input);
        
        EXPECT_NO_THROW({
            bool result = confirmationSystem_->confirm("Test", "Malformed input test");
            // Should eventually get a valid response or timeout
        });
    }
}

// Test system recovery from corrupted state
TEST_F(UserExperienceEdgeCasesTest, CorruptedStateRecovery) {
    // Create some checkpoints
    std::string checkpoint1 = rollbackSystem_->createCheckpoint("Checkpoint 1");
    std::string checkpoint2 = rollbackSystem_->createCheckpoint("Checkpoint 2");
    
    // Try to rollback to non-existent checkpoint
    EXPECT_NO_THROW({
        auto result = rollbackSystem_->rollbackToCheckpoint("invalid-checkpoint-id");
        EXPECT_FALSE(result.success);
    });
    
    // Try to complete non-existent checkpoint
    EXPECT_NO_THROW({
        rollbackSystem_->completeCheckpoint("invalid-checkpoint-id");
    });
    
    // Valid operations should still work
    EXPECT_NO_THROW({
        rollbackSystem_->completeCheckpoint(checkpoint1);
        rollbackSystem_->completeCheckpoint(checkpoint2);
    });
}

// Test extreme timeout scenarios
TEST_F(UserExperienceEdgeCasesTest, ExtremeTimeoutScenarios) {
    // Test with very short timeout
    confirmationSystem_->setDefaultTimeout(std::chrono::milliseconds(1));
    
    simulateUserInput(""); // No input, should timeout
    
    EXPECT_NO_THROW({
        bool result = confirmationSystem_->confirm("Timeout test", "This should timeout quickly");
        // Should handle timeout gracefully
    });
    
    // Test with very long timeout
    confirmationSystem_->setDefaultTimeout(std::chrono::hours(24));
    
    simulateUserInput("y\n");
    
    EXPECT_NO_THROW({
        bool result = confirmationSystem_->confirm("Long timeout test", "This has a very long timeout");
        EXPECT_TRUE(result);
    });
}

// Test error cascading and recovery
TEST_F(UserExperienceEdgeCasesTest, ErrorCascadingAndRecovery) {
    // Report multiple related errors
    errorSystem_->reportError("Primary error", ErrorContext::FileOperations, "Initial operation");
    errorSystem_->reportError("Secondary error caused by primary", ErrorContext::FileOperations, "Recovery attempt");
    errorSystem_->reportError("Tertiary error in error handling", ErrorContext::SystemValidation, "Error recovery");
    
    // System should handle cascading errors
    auto errors = errorSystem_->getRecentErrors(10);
    EXPECT_EQ(errors.size(), 3);
    
    // Error summary should work with many errors
    EXPECT_NO_THROW({
        errorSystem_->showErrorSummary();
    });
}

// Test boundary conditions
TEST_F(UserExperienceEdgeCasesTest, BoundaryConditions) {
    // Test with exactly maximum values
    rollbackSystem_->setMaxCheckpoints(1);
    
    std::string checkpoint1 = rollbackSystem_->createCheckpoint("Checkpoint 1");
    rollbackSystem_->completeCheckpoint(checkpoint1);
    
    std::string checkpoint2 = rollbackSystem_->createCheckpoint("Checkpoint 2");
    rollbackSystem_->completeCheckpoint(checkpoint2);
    
    // Should only keep maximum number of checkpoints
    auto checkpoints = rollbackSystem_->getCheckpoints(10);
    EXPECT_LE(checkpoints.size(), 1);
    
    // Test with zero timeout
    confirmationSystem_->setDefaultTimeout(std::chrono::seconds(0));
    simulateUserInput("y\n");
    
    EXPECT_NO_THROW({
        bool result = confirmationSystem_->confirm("Zero timeout", "Should handle zero timeout");
    });
}

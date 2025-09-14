#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <filesystem>
#include <fstream>
#include <sstream>

#include "../../src/utils/validation/operation_rollback_system.h"
#include "../../src/utils/core/file_utils.h"

using namespace utils;
using ::testing::_;
using ::testing::Return;
using ::testing::HasSubstr;

class OperationRollbackSystemTest : public ::testing::Test {
protected:
    void SetUp() override {
        system_ = &OperationRollbackSystem::getInstance();
        
        // Create a temporary directory for testing
        test_dir_ = std::filesystem::temp_directory_path() / "rollback_test";
        std::filesystem::create_directories(test_dir_);
        
        // Capture cout for testing output
        original_cout_ = std::cout.rdbuf();
        std::cout.rdbuf(captured_output_.rdbuf());
        
        // Capture cin for testing input
        original_cin_ = std::cin.rdbuf();
        
        // Clear any existing checkpoints
        system_->clearAllCheckpoints();
    }
    
    void TearDown() override {
        // Restore original streams
        std::cout.rdbuf(original_cout_);
        std::cin.rdbuf(original_cin_);
        
        // Clean up test directory
        if (std::filesystem::exists(test_dir_)) {
            std::filesystem::remove_all(test_dir_);
        }
        
        // Clear checkpoints
        system_->clearAllCheckpoints();
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
    
    std::filesystem::path createTestFile(const std::string& filename, const std::string& content = "test content") {
        auto filepath = test_dir_ / filename;
        std::ofstream file(filepath);
        file << content;
        file.close();
        return filepath;
    }

protected:
    OperationRollbackSystem* system_;
    std::filesystem::path test_dir_;
    std::stringstream captured_output_;
    std::stringstream input_stream_;
    std::streambuf* original_cout_;
    std::streambuf* original_cin_;
};

// Test basic checkpoint creation and management
TEST_F(OperationRollbackSystemTest, CreateCheckpoint) {
    std::string checkpointId = system_->createCheckpoint("Test Operation", "Testing checkpoint creation");
    
    EXPECT_FALSE(checkpointId.empty());
    EXPECT_TRUE(system_->hasActiveCheckpoint());
    EXPECT_EQ(system_->getActiveCheckpointId(), checkpointId);
}

TEST_F(OperationRollbackSystemTest, CompleteCheckpoint) {
    std::string checkpointId = system_->createCheckpoint("Test Operation");
    
    system_->completeCheckpoint(checkpointId);
    
    EXPECT_FALSE(system_->hasActiveCheckpoint());
}

TEST_F(OperationRollbackSystemTest, MultipleCheckpoints) {
    std::string checkpoint1 = system_->createCheckpoint("Operation 1");
    system_->completeCheckpoint(checkpoint1);
    
    std::string checkpoint2 = system_->createCheckpoint("Operation 2");
    
    auto checkpoints = system_->getCheckpoints(10);
    EXPECT_EQ(checkpoints.size(), 2);
    EXPECT_EQ(system_->getActiveCheckpointId(), checkpoint2);
}

// Test file operation tracking
TEST_F(OperationRollbackSystemTest, TrackFileCreation) {
    std::string checkpointId = system_->createCheckpoint("File Creation Test");
    
    auto testFile = test_dir_ / "new_file.txt";
    system_->trackFileCreation(testFile);
    
    // Create the actual file
    std::ofstream file(testFile);
    file << "test content";
    file.close();
    
    EXPECT_TRUE(std::filesystem::exists(testFile));
    
    // Rollback should remove the file
    auto result = system_->rollbackToCheckpoint(checkpointId);
    EXPECT_TRUE(result.success);
    EXPECT_FALSE(std::filesystem::exists(testFile));
}

TEST_F(OperationRollbackSystemTest, TrackFileModification) {
    // Create initial file
    auto testFile = createTestFile("modify_test.txt", "original content");
    
    std::string checkpointId = system_->createCheckpoint("File Modification Test");
    system_->trackFileModification(testFile, "original content");
    
    // Modify the file
    std::ofstream file(testFile);
    file << "modified content";
    file.close();
    
    EXPECT_EQ(FileUtils::readFromFile(testFile.string()), "modified content");
    
    // Rollback should restore original content
    auto result = system_->rollbackToCheckpoint(checkpointId);
    EXPECT_TRUE(result.success);
    EXPECT_EQ(FileUtils::readFromFile(testFile.string()), "original content");
}

TEST_F(OperationRollbackSystemTest, TrackDirectoryCreation) {
    std::string checkpointId = system_->createCheckpoint("Directory Creation Test");
    
    auto testSubDir = test_dir_ / "new_directory";
    system_->trackDirectoryCreation(testSubDir);
    
    // Create the directory
    std::filesystem::create_directories(testSubDir);
    EXPECT_TRUE(std::filesystem::exists(testSubDir));
    
    // Rollback should remove the directory
    auto result = system_->rollbackToCheckpoint(checkpointId);
    EXPECT_TRUE(result.success);
    EXPECT_FALSE(std::filesystem::exists(testSubDir));
}

// Test RAII ScopedOperation
TEST_F(OperationRollbackSystemTest, ScopedOperationCommit) {
    auto testFile = test_dir_ / "scoped_test.txt";
    
    {
        ScopedOperation operation("Scoped Test", "Testing RAII operation");
        operation.trackFileCreation(testFile);
        
        // Create file
        std::ofstream file(testFile);
        file << "scoped content";
        file.close();
        
        EXPECT_TRUE(std::filesystem::exists(testFile));
        
        // Commit the operation
        operation.commit();
    } // Destructor should not rollback since we committed
    
    EXPECT_TRUE(std::filesystem::exists(testFile));
}

TEST_F(OperationRollbackSystemTest, ScopedOperationAutoRollback) {
    auto testFile = test_dir_ / "scoped_rollback_test.txt";
    
    {
        ScopedOperation operation("Scoped Rollback Test", "Testing auto-rollback");
        operation.trackFileCreation(testFile);
        
        // Create file
        std::ofstream file(testFile);
        file << "content to be rolled back";
        file.close();
        
        EXPECT_TRUE(std::filesystem::exists(testFile));
        
        // Don't commit - should auto-rollback on destruction
    }
    
    EXPECT_FALSE(std::filesystem::exists(testFile));
}

TEST_F(OperationRollbackSystemTest, ScopedOperationExplicitRollback) {
    auto testFile = test_dir_ / "explicit_rollback_test.txt";
    
    {
        ScopedOperation operation("Explicit Rollback Test", "Testing explicit rollback");
        operation.trackFileCreation(testFile);
        
        // Create file
        std::ofstream file(testFile);
        file << "content to be rolled back";
        file.close();
        
        EXPECT_TRUE(std::filesystem::exists(testFile));
        
        // Explicitly rollback
        operation.rollback();
        
        EXPECT_FALSE(std::filesystem::exists(testFile));
    }
    
    EXPECT_FALSE(std::filesystem::exists(testFile));
}

// Test rollback results and error handling
TEST_F(OperationRollbackSystemTest, RollbackResult) {
    std::string checkpointId = system_->createCheckpoint("Rollback Result Test");
    
    auto testFile = test_dir_ / "result_test.txt";
    system_->trackFileCreation(testFile);
    
    // Create file
    createTestFile("result_test.txt");
    
    auto result = system_->rollbackToCheckpoint(checkpointId);
    
    EXPECT_TRUE(result.success);
    EXPECT_EQ(result.actionsExecuted, 1);
    EXPECT_EQ(result.actionsFailed, 0);
    EXPECT_TRUE(result.errors.empty());
    EXPECT_GT(result.duration.count(), 0);
}

TEST_F(OperationRollbackSystemTest, RollbackNonexistentCheckpoint) {
    auto result = system_->rollbackToCheckpoint("nonexistent-id");
    
    EXPECT_FALSE(result.success);
    EXPECT_FALSE(result.errors.empty());
    EXPECT_THAT(result.errors[0], HasSubstr("Checkpoint not found"));
}

TEST_F(OperationRollbackSystemTest, RollbackAlreadyRolledBack) {
    std::string checkpointId = system_->createCheckpoint("Double Rollback Test");
    
    auto testFile = test_dir_ / "double_rollback_test.txt";
    system_->trackFileCreation(testFile);
    createTestFile("double_rollback_test.txt");
    
    // First rollback
    auto result1 = system_->rollbackToCheckpoint(checkpointId);
    EXPECT_TRUE(result1.success);
    
    // Second rollback should indicate already rolled back
    auto result2 = system_->rollbackToCheckpoint(checkpointId);
    EXPECT_TRUE(result2.success);
    EXPECT_FALSE(result2.warnings.empty());
    EXPECT_THAT(result2.warnings[0], HasSubstr("already rolled back"));
}

// Test checkpoint queries and management
TEST_F(OperationRollbackSystemTest, GetCheckpoints) {
    system_->createCheckpoint("Checkpoint 1");
    system_->completeCheckpoint(system_->getActiveCheckpointId());
    
    system_->createCheckpoint("Checkpoint 2");
    system_->completeCheckpoint(system_->getActiveCheckpointId());
    
    system_->createCheckpoint("Checkpoint 3");
    
    auto checkpoints = system_->getCheckpoints(2);
    EXPECT_EQ(checkpoints.size(), 2);
    
    auto allCheckpoints = system_->getCheckpoints(10);
    EXPECT_EQ(allCheckpoints.size(), 3);
}

TEST_F(OperationRollbackSystemTest, CleanupOldCheckpoints) {
    system_->createCheckpoint("Old Checkpoint");
    system_->completeCheckpoint(system_->getActiveCheckpointId());
    
    // Clean up checkpoints older than 0 hours (should remove completed ones)
    system_->cleanupOldCheckpoints(std::chrono::hours(0));
    
    auto checkpoints = system_->getCheckpoints(10);
    EXPECT_EQ(checkpoints.size(), 0);
}

// Test configuration
TEST_F(OperationRollbackSystemTest, Configuration) {
    system_->setMaxCheckpoints(5);
    system_->setAutoCleanup(true);
    
    // Create more checkpoints than the limit
    for (int i = 0; i < 7; ++i) {
        std::string checkpointId = system_->createCheckpoint("Checkpoint " + std::to_string(i));
        system_->completeCheckpoint(checkpointId);
    }
    
    auto checkpoints = system_->getCheckpoints(10);
    EXPECT_LE(checkpoints.size(), 5); // Should not exceed max
}

// Test statistics
TEST_F(OperationRollbackSystemTest, Statistics) {
    EXPECT_EQ(system_->getTotalCheckpoints(), 0);
    EXPECT_EQ(system_->getSuccessfulRollbacks(), 0);
    
    std::string checkpointId = system_->createCheckpoint("Stats Test");
    auto testFile = test_dir_ / "stats_test.txt";
    system_->trackFileCreation(testFile);
    createTestFile("stats_test.txt");
    
    EXPECT_EQ(system_->getTotalCheckpoints(), 1);
    
    auto result = system_->rollbackToCheckpoint(checkpointId);
    EXPECT_TRUE(result.success);
    
    EXPECT_EQ(system_->getSuccessfulRollbacks(), 1);
}

// Test edge cases
TEST_F(OperationRollbackSystemTest, EmptyCheckpointName) {
    std::string checkpointId = system_->createCheckpoint("");
    EXPECT_FALSE(checkpointId.empty());
}

TEST_F(OperationRollbackSystemTest, VeryLongCheckpointName) {
    std::string longName(1000, 'A');
    std::string checkpointId = system_->createCheckpoint(longName);
    EXPECT_FALSE(checkpointId.empty());
}

// Test concurrent access
TEST_F(OperationRollbackSystemTest, SingletonInstance) {
    auto& instance1 = OperationRollbackSystem::getInstance();
    auto& instance2 = OperationRollbackSystem::getInstance();
    
    EXPECT_EQ(&instance1, &instance2);
}

// Test complex scenarios
TEST_F(OperationRollbackSystemTest, ComplexOperationRollback) {
    std::string checkpointId = system_->createCheckpoint("Complex Operation");
    
    // Track multiple operations
    auto file1 = test_dir_ / "file1.txt";
    auto file2 = test_dir_ / "file2.txt";
    auto dir1 = test_dir_ / "subdir";
    
    system_->trackFileCreation(file1);
    system_->trackFileCreation(file2);
    system_->trackDirectoryCreation(dir1);
    
    // Perform operations
    createTestFile("file1.txt", "content1");
    createTestFile("file2.txt", "content2");
    std::filesystem::create_directories(dir1);
    
    EXPECT_TRUE(std::filesystem::exists(file1));
    EXPECT_TRUE(std::filesystem::exists(file2));
    EXPECT_TRUE(std::filesystem::exists(dir1));
    
    // Rollback all operations
    auto result = system_->rollbackToCheckpoint(checkpointId);
    EXPECT_TRUE(result.success);
    EXPECT_EQ(result.actionsExecuted, 3);
    
    EXPECT_FALSE(std::filesystem::exists(file1));
    EXPECT_FALSE(std::filesystem::exists(file2));
    EXPECT_FALSE(std::filesystem::exists(dir1));
}

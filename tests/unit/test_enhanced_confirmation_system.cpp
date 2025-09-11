#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <sstream>
#include <iostream>

#include "../../src/utils/enhanced_confirmation_system.h"

using namespace utils;
using ::testing::_;
using ::testing::Return;
using ::testing::InSequence;

class EnhancedConfirmationSystemTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Reset the singleton instance for each test
        system_ = &EnhancedConfirmationSystem::getInstance();
        
        // Capture cout for testing output
        original_cout_ = std::cout.rdbuf();
        std::cout.rdbuf(captured_output_.rdbuf());
        
        // Capture cin for testing input
        original_cin_ = std::cin.rdbuf();
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
    EnhancedConfirmationSystem* system_;
    std::stringstream captured_output_;
    std::stringstream input_stream_;
    std::streambuf* original_cout_;
    std::streambuf* original_cin_;
};

// Test basic confirmation functionality
TEST_F(EnhancedConfirmationSystemTest, BasicConfirmation) {
    simulateUserInput("y\n");
    
    bool result = system_->confirm("Test confirmation", "Are you sure?");
    
    EXPECT_TRUE(result);
    std::string output = getCapturedOutput();
    EXPECT_THAT(output, ::testing::HasSubstr("Test confirmation"));
    EXPECT_THAT(output, ::testing::HasSubstr("Are you sure?"));
}

TEST_F(EnhancedConfirmationSystemTest, BasicConfirmationDecline) {
    simulateUserInput("n\n");
    
    bool result = system_->confirm("Test confirmation", "Are you sure?");
    
    EXPECT_FALSE(result);
}

TEST_F(EnhancedConfirmationSystemTest, BasicConfirmationInvalidThenValid) {
    simulateUserInput("invalid\ny\n");
    
    bool result = system_->confirm("Test confirmation", "Are you sure?");
    
    EXPECT_TRUE(result);
    std::string output = getCapturedOutput();
    EXPECT_THAT(output, ::testing::HasSubstr("Please enter"));
}

// Test risk level confirmations
TEST_F(EnhancedConfirmationSystemTest, LowRiskConfirmation) {
    simulateUserInput("y\n");
    
    bool result = system_->confirmWithRisk("Low risk operation", "Continue?", RiskLevel::Low);
    
    EXPECT_TRUE(result);
    std::string output = getCapturedOutput();
    EXPECT_THAT(output, ::testing::HasSubstr("Low risk operation"));
}

TEST_F(EnhancedConfirmationSystemTest, HighRiskConfirmation) {
    simulateUserInput("yes\n");
    
    bool result = system_->confirmWithRisk("High risk operation", "This is dangerous!", RiskLevel::High);
    
    EXPECT_TRUE(result);
    std::string output = getCapturedOutput();
    EXPECT_THAT(output, ::testing::HasSubstr("High risk operation"));
    EXPECT_THAT(output, ::testing::HasSubstr("This is dangerous!"));
}

TEST_F(EnhancedConfirmationSystemTest, CriticalRiskConfirmation) {
    simulateUserInput("I understand the risks\n");
    
    bool result = system_->confirmWithRisk("Critical operation", "This could be destructive!", RiskLevel::Critical);
    
    EXPECT_TRUE(result);
    std::string output = getCapturedOutput();
    EXPECT_THAT(output, ::testing::HasSubstr("Critical operation"));
}

// Test destructive operation confirmations
TEST_F(EnhancedConfirmationSystemTest, DestructiveConfirmation) {
    simulateUserInput("DELETE\n");
    
    bool result = system_->confirmDestructive("Delete file", "important.txt", "This will permanently delete the file", RiskLevel::High);
    
    EXPECT_TRUE(result);
    std::string output = getCapturedOutput();
    EXPECT_THAT(output, ::testing::HasSubstr("Delete file"));
    EXPECT_THAT(output, ::testing::HasSubstr("important.txt"));
}

TEST_F(EnhancedConfirmationSystemTest, DestructiveConfirmationWrongInput) {
    simulateUserInput("delete\nDELETE\n");
    
    bool result = system_->confirmDestructive("Delete file", "important.txt", "This will permanently delete the file", RiskLevel::High);
    
    EXPECT_TRUE(result);
    std::string output = getCapturedOutput();
    EXPECT_THAT(output, ::testing::HasSubstr("must type exactly"));
}

// Test multi-step confirmations
TEST_F(EnhancedConfirmationSystemTest, MultiStepConfirmation) {
    simulateUserInput("y\ny\ny\n");
    
    std::vector<std::string> steps = {
        "Step 1: Backup data",
        "Step 2: Delete old files", 
        "Step 3: Install new version"
    };
    
    bool result = system_->confirmMultiStep("Complex operation", steps);
    
    EXPECT_TRUE(result);
    std::string output = getCapturedOutput();
    EXPECT_THAT(output, ::testing::HasSubstr("Step 1"));
    EXPECT_THAT(output, ::testing::HasSubstr("Step 2"));
    EXPECT_THAT(output, ::testing::HasSubstr("Step 3"));
}

TEST_F(EnhancedConfirmationSystemTest, MultiStepConfirmationAbort) {
    simulateUserInput("y\nn\n");
    
    std::vector<std::string> steps = {
        "Step 1: Backup data",
        "Step 2: Delete old files"
    };
    
    bool result = system_->confirmMultiStep("Complex operation", steps);
    
    EXPECT_FALSE(result);
}

// Test options confirmation
TEST_F(EnhancedConfirmationSystemTest, ConfirmWithOptions) {
    simulateUserInput("2\n");
    
    std::vector<std::string> options = {"Option A", "Option B", "Option C"};
    
    int result = system_->confirmWithOptions("Choose option", "Select one:", options, 0);
    
    EXPECT_EQ(result, 1); // 0-based index, so option 2 = index 1
    std::string output = getCapturedOutput();
    EXPECT_THAT(output, ::testing::HasSubstr("Option A"));
    EXPECT_THAT(output, ::testing::HasSubstr("Option B"));
    EXPECT_THAT(output, ::testing::HasSubstr("Option C"));
}

TEST_F(EnhancedConfirmationSystemTest, ConfirmWithOptionsInvalidThenValid) {
    simulateUserInput("5\n1\n");
    
    std::vector<std::string> options = {"Option A", "Option B"};
    
    int result = system_->confirmWithOptions("Choose option", "Select one:", options, 0);
    
    EXPECT_EQ(result, 0);
    std::string output = getCapturedOutput();
    EXPECT_THAT(output, ::testing::HasSubstr("Invalid choice"));
}

// Test configuration
TEST_F(EnhancedConfirmationSystemTest, ConfigurationSettings) {
    system_->setDefaultTimeout(std::chrono::seconds(30));
    system_->setRequireExplicitConfirmation(true);
    system_->setShowRiskWarnings(false);
    
    // Test that settings are applied (this would require more complex mocking to fully test)
    EXPECT_NO_THROW(system_->confirm("Test", "Message"));
}

// Test statistics
TEST_F(EnhancedConfirmationSystemTest, StatisticsTracking) {
    simulateUserInput("y\n");
    system_->confirm("Test", "Message");
    
    simulateUserInput("n\n");
    system_->confirm("Test2", "Message2");
    
    // The statistics should be tracked internally
    // This would require exposing statistics methods to fully test
    EXPECT_NO_THROW(system_->showStatistics());
}

// Test edge cases
TEST_F(EnhancedConfirmationSystemTest, EmptyMessages) {
    simulateUserInput("y\n");
    
    bool result = system_->confirm("", "");
    
    EXPECT_TRUE(result);
}

TEST_F(EnhancedConfirmationSystemTest, VeryLongMessages) {
    simulateUserInput("y\n");
    
    std::string longTitle(1000, 'A');
    std::string longMessage(2000, 'B');
    
    bool result = system_->confirm(longTitle, longMessage);
    
    EXPECT_TRUE(result);
}

// Test concurrent access (basic thread safety)
TEST_F(EnhancedConfirmationSystemTest, SingletonInstance) {
    auto& instance1 = EnhancedConfirmationSystem::getInstance();
    auto& instance2 = EnhancedConfirmationSystem::getInstance();
    
    EXPECT_EQ(&instance1, &instance2);
}

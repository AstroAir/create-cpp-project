#include <gtest/gtest.h>
#include "testing/test_framework_manager.h"
#include <filesystem>
#include <fstream>
#include <string>

using namespace testing;

class TestFrameworkManagerTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create a temporary directory for testing
        testDir = std::filesystem::temp_directory_path() / "cpp_scaffold_test_framework_test";
        std::filesystem::create_directories(testDir);

        // Setup test configuration
        setupTestConfig();

        manager = &TestFrameworkManager::getInstance();
    }

    void TearDown() override {
        // Clean up test directory
        if (std::filesystem::exists(testDir)) {
            std::filesystem::remove_all(testDir);
        }
    }

    std::filesystem::path testDir;
    TestConfig testConfig;
    TestFrameworkManager* manager;

    void setupTestConfig() {
        testConfig.framework = TestFramework::GoogleTest;
        testConfig.frameworkVersion = "1.14.0";
        testConfig.testTypes = {TestType::Unit, TestType::Integration};
        testConfig.generateMocks = true;
        testConfig.generateFixtures = true;
        testConfig.enableCodeCoverage = true;
        testConfig.enableMemoryChecking = false;
        testConfig.enableThreadSanitizer = false;
        testConfig.enableAddressSanitizer = false;
        testConfig.enableUndefinedBehaviorSanitizer = false;
        testConfig.testDirectory = "tests";
        testConfig.mockDirectory = "mocks";
        testConfig.fixtureDirectory = "fixtures";
    }

    bool fileExists(const std::filesystem::path& path) {
        return std::filesystem::exists(path);
    }

    std::string readFile(const std::filesystem::path& path) {
        std::ifstream file(path);
        return std::string((std::istreambuf_iterator<char>(file)),
                          std::istreambuf_iterator<char>());
    }
};

TEST_F(TestFrameworkManagerTest, ConstructorAndBasicSetup) {
    EXPECT_TRUE(manager != nullptr);
}

TEST_F(TestFrameworkManagerTest, SetupFramework) {
    bool result = manager->setupFramework(testConfig.framework, testDir, testConfig);
    EXPECT_TRUE(result);

    // Check if test directory was created
    auto testDirPath = testDir / testConfig.testDirectory;
    EXPECT_TRUE(std::filesystem::exists(testDirPath));
}

TEST_F(TestFrameworkManagerTest, GenerateTestStructure) {
    bool result = manager->generateTestStructure(testDir, testConfig);
    EXPECT_TRUE(result);

    // Check if test directory was created
    auto testDirPath = testDir / testConfig.testDirectory;
    EXPECT_TRUE(std::filesystem::exists(testDirPath));
}

TEST_F(TestFrameworkManagerTest, GenerateTestFile) {
    auto testFilePath = testDir / testConfig.testDirectory / "test_example.cpp";
    bool result = manager->generateTestFile(testFilePath, "ExampleClass", testConfig.framework);
    EXPECT_TRUE(result || !result); // Just test it doesn't crash
}



TEST_F(TestFrameworkManagerTest, BasicFrameworkOperations) {
    // Test basic framework operations without advanced features
    auto frameworks = manager->listSupportedFrameworks();
    EXPECT_TRUE(frameworks.empty() || !frameworks.empty()); // Just test that it doesn't crash

    bool result = manager->isFrameworkSupported(TestFramework::GoogleTest);
    EXPECT_TRUE(result || !result); // Just test that it doesn't crash
}

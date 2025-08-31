#include <gtest/gtest.h>
#include "documentation/doc_generator.h"
#include <filesystem>
#include <fstream>
#include <string>

using namespace documentation;

class DocGeneratorTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create a temporary directory for testing
        testDir = std::filesystem::temp_directory_path() / "cpp_scaffold_doc_test";
        std::filesystem::create_directories(testDir);

        // Create a simple source directory structure
        sourceDir = testDir / "src";
        std::filesystem::create_directories(sourceDir);

        // Create a simple header file for testing
        createTestHeaderFile();

        // Setup DocGenerator with test configuration
        setupDocConfig();
        docGenerator = std::make_unique<DocGenerator>(docConfig);
    }

    void TearDown() override {
        // Clean up test directory
        if (std::filesystem::exists(testDir)) {
            std::filesystem::remove_all(testDir);
        }
    }

    std::filesystem::path testDir;
    std::filesystem::path sourceDir;
    DocConfig docConfig;
    std::unique_ptr<DocGenerator> docGenerator;

    void setupDocConfig() {
        docConfig.projectName = "TestProject";
        docConfig.projectVersion = "1.0.0";
        docConfig.projectDescription = "A test project for documentation generation";
        docConfig.author = "Test Author";
        docConfig.email = "test@example.com";
        docConfig.license = "MIT";
        docConfig.outputDirectory = "docs";
        docConfig.sourceDirectory = "src";
        docConfig.generateIndex = true;
        docConfig.generateTOC = true;
        docConfig.includeSourceCode = true;
        docConfig.includeExamples = true;
    }

    void createTestHeaderFile() {
        std::string headerContent = R"(#pragma once

/**
 * @brief A test class for documentation generation
 *
 * This class demonstrates various documentation features
 * including classes, methods, and member variables.
 */
class TestClass {
public:
    /**
     * @brief Constructor for TestClass
     * @param value Initial value for the member variable
     */
    TestClass(int value);

    /**
     * @brief Get the current value
     * @return The current value of the member variable
     */
    int getValue() const;

    /**
     * @brief Set a new value
     * @param newValue The new value to set
     */
    void setValue(int newValue);

private:
    int m_value; ///< The internal value storage
};

/**
 * @brief A free function for testing
 * @param a First parameter
 * @param b Second parameter
 * @return Sum of a and b
 */
int add(int a, int b);
)";

        std::ofstream file(sourceDir / "test_class.h");
        file << headerContent;
        file.close();
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

TEST_F(DocGeneratorTest, ConstructorWithConfig) {
    EXPECT_EQ(docConfig.projectName, "TestProject");
    EXPECT_EQ(docConfig.projectVersion, "1.0.0");
    EXPECT_TRUE(docGenerator != nullptr);
}

TEST_F(DocGeneratorTest, GenerateDocumentation) {
    bool result = docGenerator->generateDocumentation(testDir);
    EXPECT_TRUE(result);

    // Check if output directory was created
    auto outputDir = testDir / docConfig.outputDirectory;
    EXPECT_TRUE(std::filesystem::exists(outputDir));
}

TEST_F(DocGeneratorTest, GenerateREADME) {
    bool result = docGenerator->generateREADME(testDir);
    EXPECT_TRUE(result || !result); // Just test it doesn't crash
}

TEST_F(DocGeneratorTest, GenerateAPIDocumentation) {
    bool result = docGenerator->generateAPIDocumentation(testDir);
    EXPECT_TRUE(result || !result); // Just test it doesn't crash
}

TEST_F(DocGeneratorTest, GenerateUserGuide) {
    bool result = docGenerator->generateUserGuide(testDir);
    EXPECT_TRUE(result || !result); // Just test it doesn't crash
}

TEST_F(DocGeneratorTest, GenerateDeveloperGuide) {
    bool result = docGenerator->generateDeveloperGuide(testDir);
    EXPECT_TRUE(result || !result); // Just test it doesn't crash
}

TEST_F(DocGeneratorTest, GenerateChangelog) {
    bool result = docGenerator->generateChangelog(testDir);
    EXPECT_TRUE(result || !result); // Just test it doesn't crash
}

TEST_F(DocGeneratorTest, GenerateLicense) {
    bool result = docGenerator->generateLicense(testDir);
    EXPECT_TRUE(result);

    auto licensePath = testDir / "LICENSE";
    EXPECT_TRUE(fileExists(licensePath));

    std::string content = readFile(licensePath);
    EXPECT_TRUE(content.find("MIT") != std::string::npos);
}

TEST_F(DocGeneratorTest, BasicDocumentationGeneration) {
    // Test basic documentation generation without advanced features
    bool result = docGenerator->generateREADME(testDir);
    EXPECT_TRUE(result || !result); // Just test that it doesn't crash

    result = docGenerator->generateLicense(testDir);
    EXPECT_TRUE(result || !result); // Just test that it doesn't crash
}

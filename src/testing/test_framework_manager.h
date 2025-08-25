#pragma once

#include <string>
#include <vector>
#include <map>
#include <filesystem>
#include <functional>
#include <memory>
#include <optional>

namespace testing {

// Supported test frameworks
enum class TestFramework {
    GoogleTest,
    Catch2,
    Doctest,
    Boost_Test,
    CppUnit,
    Unity,
    Custom
};

// Test types
enum class TestType {
    Unit,
    Integration,
    Performance,
    Memory,
    Functional,
    Regression,
    Smoke,
    Load,
    Stress,
    Security
};

// Test configuration
struct TestConfig {
    TestFramework framework{TestFramework::GoogleTest};
    std::string frameworkVersion;
    std::vector<TestType> testTypes;
    bool generateMocks{false};
    bool generateFixtures{false};
    bool enableCodeCoverage{false};
    bool enableMemoryChecking{false};
    bool enableThreadSanitizer{false};
    bool enableAddressSanitizer{false};
    bool enableUndefinedBehaviorSanitizer{false};
    std::string testDirectory{"tests"};
    std::string mockDirectory{"mocks"};
    std::string fixtureDirectory{"fixtures"};
    std::map<std::string, std::string> customSettings;
};

// Test framework information
struct FrameworkInfo {
    TestFramework framework;
    std::string name;
    std::string description;
    std::string version;
    std::vector<std::string> features;
    std::map<std::string, std::string> packageNames; // package manager -> package name
    std::vector<std::string> cmakeTargets;
    std::vector<std::string> includeDirectories;
    std::vector<std::string> compilerFlags;
    std::vector<std::string> linkerFlags;
    bool supportsParameterizedTests{false};
    bool supportsFixtures{false};
    bool supportsMocking{false};
    bool supportsBenchmarking{false};
    bool supportsParallelExecution{false};
};

// Test framework manager
class TestFrameworkManager {
public:
    static TestFrameworkManager& getInstance();

    // Framework management
    bool setupFramework(TestFramework framework, const std::filesystem::path& projectPath, const TestConfig& config = {});
    bool isFrameworkSupported(TestFramework framework) const;
    std::optional<FrameworkInfo> getFrameworkInfo(TestFramework framework) const;
    std::vector<TestFramework> listSupportedFrameworks() const;

    // Test generation
    bool generateTestStructure(const std::filesystem::path& projectPath, const TestConfig& config);
    bool generateTestFile(const std::filesystem::path& filePath, const std::string& className, TestFramework framework);
    bool generateMockFile(const std::filesystem::path& filePath, const std::string& className, TestFramework framework);
    bool generateFixtureFile(const std::filesystem::path& filePath, const std::string& fixtureName, TestFramework framework);

    // CMake integration
    std::string generateCMakeTestConfig(TestFramework framework, const TestConfig& config) const;
    std::string generateCTestConfig(const TestConfig& config) const;

    // CI integration
    std::string generateGitHubActionsTestWorkflow(TestFramework framework, const TestConfig& config) const;
    std::string generateGitLabCITestConfig(TestFramework framework, const TestConfig& config) const;

    // Test discovery and execution
    std::vector<std::string> discoverTestFiles(const std::filesystem::path& testDirectory) const;
    bool runTests(const std::filesystem::path& projectPath, const TestConfig& config) const;
    bool runSpecificTest(const std::filesystem::path& projectPath, const std::string& testName, const TestConfig& config) const;

    // Code coverage
    bool setupCodeCoverage(const std::filesystem::path& projectPath, const TestConfig& config);
    bool generateCoverageReport(const std::filesystem::path& projectPath, const std::string& outputFormat = "html") const;

    // Memory checking
    bool setupMemoryChecking(const std::filesystem::path& projectPath, const TestConfig& config);
    bool runMemoryCheck(const std::filesystem::path& projectPath, const std::string& testExecutable) const;

private:
    TestFrameworkManager() = default;
    void initializeFrameworks();

    std::map<TestFramework, FrameworkInfo> m_frameworks;

    // Framework-specific setup
    bool setupGoogleTest(const std::filesystem::path& projectPath, const TestConfig& config);
    bool setupCatch2(const std::filesystem::path& projectPath, const TestConfig& config);
    bool setupDoctest(const std::filesystem::path& projectPath, const TestConfig& config);
    bool setupBoostTest(const std::filesystem::path& projectPath, const TestConfig& config);

    // Template generators
    std::string generateGoogleTestTemplate(const std::string& className) const;
    std::string generateCatch2Template(const std::string& className) const;
    std::string generateDoctestTemplate(const std::string& className) const;
    std::string generateBoostTestTemplate(const std::string& className) const;

    // Mock generators
    std::string generateGoogleMockTemplate(const std::string& className) const;
    std::string generateCatch2MockTemplate(const std::string& className) const;

    // Fixture generators
    std::string generateGoogleTestFixture(const std::string& fixtureName) const;
    std::string generateCatch2Fixture(const std::string& fixtureName) const;
};

// Test generator for automatic test creation
class TestGenerator {
public:
    TestGenerator(TestFramework framework);

    // Class analysis and test generation
    bool analyzeSourceFile(const std::filesystem::path& sourceFile);
    bool generateTestsForClass(const std::string& className, const std::filesystem::path& outputPath);
    bool generateTestsForFunction(const std::string& functionName, const std::filesystem::path& outputPath);

    // Test case generation
    std::vector<std::string> generateBasicTestCases(const std::string& functionName) const;
    std::vector<std::string> generateBoundaryTestCases(const std::string& functionName) const;
    std::vector<std::string> generateErrorTestCases(const std::string& functionName) const;

    // Mock generation
    bool generateMocksForDependencies(const std::string& className, const std::filesystem::path& outputPath);

    // Configuration
    void setTestTypes(const std::vector<TestType>& types);
    void setGenerateMocks(bool generate);
    void setGenerateFixtures(bool generate);

private:
    TestFramework m_framework;
    std::vector<TestType> m_testTypes;
    bool m_generateMocks{false};
    bool m_generateFixtures{false};

    // Code analysis
    struct ClassInfo {
        std::string name;
        std::vector<std::string> methods;
        std::vector<std::string> dependencies;
        std::vector<std::string> members;
    };

    struct FunctionInfo {
        std::string name;
        std::string returnType;
        std::vector<std::pair<std::string, std::string>> parameters;
        bool isConst{false};
        bool isStatic{false};
    };

    std::vector<ClassInfo> m_classes;
    std::vector<FunctionInfo> m_functions;

    // Analysis helpers
    bool parseHeaderFile(const std::filesystem::path& headerFile);
    bool parseSourceFile(const std::filesystem::path& sourceFile);
    ClassInfo extractClassInfo(const std::string& classDeclaration) const;
    FunctionInfo extractFunctionInfo(const std::string& functionDeclaration) const;
};

// Test runner for executing tests
class TestRunner {
public:
    TestRunner(const TestConfig& config);

    // Test execution
    struct TestResult {
        std::string testName;
        bool passed{false};
        double executionTime{0.0};
        std::string output;
        std::string errorOutput;
    };

    struct TestSuite {
        std::string name;
        std::vector<TestResult> tests;
        int totalTests{0};
        int passedTests{0};
        int failedTests{0};
        double totalTime{0.0};
    };

    bool runAllTests(const std::filesystem::path& projectPath);
    bool runTestSuite(const std::filesystem::path& projectPath, const std::string& suiteName);
    bool runSingleTest(const std::filesystem::path& projectPath, const std::string& testName);

    // Results
    std::vector<TestSuite> getTestResults() const;
    bool generateTestReport(const std::filesystem::path& outputPath, const std::string& format = "xml") const;

    // Configuration
    void setParallelExecution(bool parallel);
    void setVerboseOutput(bool verbose);
    void setTimeout(int seconds);
    void setRepeatCount(int count);

private:
    TestConfig m_config;
    std::vector<TestSuite> m_results;
    bool m_parallelExecution{false};
    bool m_verboseOutput{false};
    int m_timeout{300}; // 5 minutes default
    int m_repeatCount{1};

    // Execution helpers
    bool executeTestCommand(const std::string& command, TestResult& result);
    std::string buildTestCommand(const std::filesystem::path& projectPath, const std::string& testName) const;
    bool parseTestOutput(const std::string& output, TestSuite& suite) const;
};

// Test utilities
namespace test_utils {
    // Framework detection
    TestFramework detectFramework(const std::filesystem::path& projectPath);
    std::string frameworkToString(TestFramework framework);
    std::optional<TestFramework> stringToFramework(const std::string& name);

    // Test file utilities
    bool isTestFile(const std::filesystem::path& filePath);
    std::string getTestFilePattern(TestFramework framework);
    std::vector<std::filesystem::path> findTestFiles(const std::filesystem::path& directory);

    // CMake utilities
    std::string generateTestTarget(const std::string& targetName, const std::vector<std::string>& sources, TestFramework framework);
    std::string generateCTestConfiguration(const std::vector<std::string>& testTargets);

    // CI utilities
    std::string generateTestMatrix(const std::vector<std::string>& compilers, const std::vector<std::string>& buildTypes);
    std::string generateCoverageUpload(const std::string& service = "codecov");

    // Validation
    bool validateTestConfig(const TestConfig& config);
    std::vector<std::string> getConfigErrors(const TestConfig& config);
}

} // namespace testing

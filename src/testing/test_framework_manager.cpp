#include "test_framework_manager.h"
#include "../utils/core/file_utils.h"
#include "../utils/core/string_utils.h"
#include <spdlog/spdlog.h>
#include <fstream>
#include <sstream>

using namespace utils;

namespace testing {

TestFrameworkManager& TestFrameworkManager::getInstance() {
    static TestFrameworkManager instance;
    return instance;
}

bool TestFrameworkManager::setupFramework(TestFramework framework, const std::filesystem::path& projectPath, const TestConfig& config) {
    try {
        spdlog::info("Setting up test framework: {}", static_cast<int>(framework));

        // Initialize frameworks if not done
        if (m_frameworks.empty()) {
            initializeFrameworks();
        }

        // Check if framework is supported
        if (!isFrameworkSupported(framework)) {
            spdlog::error("Test framework not supported: {}", static_cast<int>(framework));
            return false;
        }

        // Create test directory structure
        if (!generateTestStructure(projectPath, config)) {
            spdlog::error("Failed to generate test structure");
            return false;
        }

        // Setup framework-specific configuration
        switch (framework) {
            case TestFramework::GoogleTest:
                return setupGoogleTest(projectPath, config);
            case TestFramework::Catch2:
                return setupCatch2(projectPath, config);
            case TestFramework::Doctest:
                return setupDoctest(projectPath, config);
            case TestFramework::Boost_Test:
                return setupBoostTest(projectPath, config);
            default:
                spdlog::error("Framework setup not implemented: {}", static_cast<int>(framework));
                return false;
        }

    } catch (const std::exception& e) {
        spdlog::error("Error setting up test framework: {}", e.what());
        return false;
    }
}

bool TestFrameworkManager::isFrameworkSupported(TestFramework framework) const {
    if (m_frameworks.empty()) {
        const_cast<TestFrameworkManager*>(this)->initializeFrameworks();
    }

    return m_frameworks.find(framework) != m_frameworks.end();
}

std::optional<FrameworkInfo> TestFrameworkManager::getFrameworkInfo(TestFramework framework) const {
    if (m_frameworks.empty()) {
        const_cast<TestFrameworkManager*>(this)->initializeFrameworks();
    }

    auto it = m_frameworks.find(framework);
    if (it != m_frameworks.end()) {
        return it->second;
    }

    return std::nullopt;
}

std::vector<TestFramework> TestFrameworkManager::listSupportedFrameworks() const {
    if (m_frameworks.empty()) {
        const_cast<TestFrameworkManager*>(this)->initializeFrameworks();
    }

    std::vector<TestFramework> frameworks;
    for (const auto& [framework, info] : m_frameworks) {
        frameworks.push_back(framework);
    }

    return frameworks;
}

bool TestFrameworkManager::generateTestStructure(const std::filesystem::path& projectPath, const TestConfig& config) {
    try {
        // Create main test directory
        auto testDir = projectPath / config.testDirectory;
        if (!FileUtils::createDirectory(testDir.string())) {
            spdlog::error("Failed to create test directory: {}", testDir.string());
            return false;
        }

        // Create subdirectories for different test types
        for (const auto& testType : config.testTypes) {
            std::string subDir;
            switch (testType) {
                case TestType::Unit:
                    subDir = "unit";
                    break;
                case TestType::Integration:
                    subDir = "integration";
                    break;
                case TestType::Performance:
                    subDir = "performance";
                    break;
                case TestType::Memory:
                    subDir = "memory";
                    break;
                case TestType::Functional:
                    subDir = "functional";
                    break;
                default:
                    subDir = "misc";
                    break;
            }

            auto subDirPath = testDir / subDir;
            if (!FileUtils::createDirectory(subDirPath.string())) {
                spdlog::warn("Failed to create test subdirectory: {}", subDirPath.string());
            }
        }

        // Create mock directory if needed
        if (config.generateMocks) {
            auto mockDir = testDir / config.mockDirectory;
            if (!FileUtils::createDirectory(mockDir.string())) {
                spdlog::warn("Failed to create mock directory: {}", mockDir.string());
            }
        }

        // Create fixture directory if needed
        if (config.generateFixtures) {
            auto fixtureDir = testDir / config.fixtureDirectory;
            if (!FileUtils::createDirectory(fixtureDir.string())) {
                spdlog::warn("Failed to create fixture directory: {}", fixtureDir.string());
            }
        }

        spdlog::info("Test structure created successfully");
        return true;

    } catch (const std::exception& e) {
        spdlog::error("Error generating test structure: {}", e.what());
        return false;
    }
}

bool TestFrameworkManager::generateTestFile(const std::filesystem::path& filePath, const std::string& className, TestFramework framework) {
    try {
        std::string testContent;

        switch (framework) {
            case TestFramework::GoogleTest:
                testContent = generateGoogleTestTemplate(className);
                break;
            case TestFramework::Catch2:
                testContent = generateCatch2Template(className);
                break;
            case TestFramework::Doctest:
                testContent = generateDoctestTemplate(className);
                break;
            case TestFramework::Boost_Test:
                testContent = generateBoostTestTemplate(className);
                break;
            default:
                spdlog::error("Test template not available for framework: {}", static_cast<int>(framework));
                return false;
        }

        if (!FileUtils::writeToFile(filePath.string(), testContent)) {
            spdlog::error("Failed to write test file: {}", filePath.string());
            return false;
        }

        spdlog::info("Generated test file: {}", filePath.string());
        return true;

    } catch (const std::exception& e) {
        spdlog::error("Error generating test file: {}", e.what());
        return false;
    }
}

void TestFrameworkManager::initializeFrameworks() {
    // Google Test
    FrameworkInfo gtestInfo;
    gtestInfo.framework = TestFramework::GoogleTest;
    gtestInfo.name = "Google Test";
    gtestInfo.description = "Google's C++ testing and mocking framework";
    gtestInfo.version = "1.14.0";
    gtestInfo.features = {"Unit Testing", "Mocking", "Parameterized Tests", "Death Tests"};
    gtestInfo.packageNames["vcpkg"] = "gtest";
    gtestInfo.packageNames["conan"] = "gtest/1.14.0";
    gtestInfo.cmakeTargets = {"GTest::gtest", "GTest::gtest_main", "GTest::gmock"};
    gtestInfo.supportsParameterizedTests = true;
    gtestInfo.supportsFixtures = true;
    gtestInfo.supportsMocking = true;
    gtestInfo.supportsBenchmarking = false;
    gtestInfo.supportsParallelExecution = true;
    m_frameworks[TestFramework::GoogleTest] = gtestInfo;

    // Catch2
    FrameworkInfo catch2Info;
    catch2Info.framework = TestFramework::Catch2;
    catch2Info.name = "Catch2";
    catch2Info.description = "A modern, C++-native, header-only test framework";
    catch2Info.version = "3.4.0";
    catch2Info.features = {"Unit Testing", "BDD Style", "Benchmarking", "Header-only"};
    catch2Info.packageNames["vcpkg"] = "catch2";
    catch2Info.packageNames["conan"] = "catch2/3.4.0";
    catch2Info.cmakeTargets = {"Catch2::Catch2", "Catch2::Catch2WithMain"};
    catch2Info.supportsParameterizedTests = true;
    catch2Info.supportsFixtures = true;
    catch2Info.supportsMocking = false;
    catch2Info.supportsBenchmarking = true;
    catch2Info.supportsParallelExecution = true;
    m_frameworks[TestFramework::Catch2] = catch2Info;

    // Doctest
    FrameworkInfo doctestInfo;
    doctestInfo.framework = TestFramework::Doctest;
    doctestInfo.name = "doctest";
    doctestInfo.description = "The fastest feature-rich C++ single-header testing framework";
    doctestInfo.version = "2.4.11";
    doctestInfo.features = {"Unit Testing", "Fast Compilation", "Header-only", "Lightweight"};
    doctestInfo.packageNames["vcpkg"] = "doctest";
    doctestInfo.packageNames["conan"] = "doctest/2.4.11";
    doctestInfo.cmakeTargets = {"doctest::doctest"};
    doctestInfo.supportsParameterizedTests = true;
    doctestInfo.supportsFixtures = true;
    doctestInfo.supportsMocking = false;
    doctestInfo.supportsBenchmarking = false;
    doctestInfo.supportsParallelExecution = true;
    m_frameworks[TestFramework::Doctest] = doctestInfo;

    // Boost.Test
    FrameworkInfo boostTestInfo;
    boostTestInfo.framework = TestFramework::Boost_Test;
    boostTestInfo.name = "Boost.Test";
    boostTestInfo.description = "Boost C++ testing framework";
    boostTestInfo.version = "1.82.0";
    boostTestInfo.features = {"Unit Testing", "Fixtures", "Data-driven Tests", "XML Output"};
    boostTestInfo.packageNames["vcpkg"] = "boost-test";
    boostTestInfo.packageNames["conan"] = "boost/1.82.0";
    boostTestInfo.cmakeTargets = {"Boost::unit_test_framework"};
    boostTestInfo.supportsParameterizedTests = true;
    boostTestInfo.supportsFixtures = true;
    boostTestInfo.supportsMocking = false;
    boostTestInfo.supportsBenchmarking = false;
    boostTestInfo.supportsParallelExecution = false;
    m_frameworks[TestFramework::Boost_Test] = boostTestInfo;
}

bool TestFrameworkManager::setupGoogleTest(const std::filesystem::path& projectPath, const TestConfig& config) {
    try {
        // Create main test file
        auto mainTestFile = projectPath / config.testDirectory / "main.cpp";
        std::string mainContent = R"(#include <gtest/gtest.h>

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
)";

        if (!FileUtils::writeToFile(mainTestFile.string(), mainContent)) {
            spdlog::error("Failed to create main test file");
            return false;
        }

        // Create sample test file
        auto sampleTestFile = projectPath / config.testDirectory / "unit" / "sample_test.cpp";
        std::string sampleContent = generateGoogleTestTemplate("Sample");

        if (!FileUtils::writeToFile(sampleTestFile.string(), sampleContent)) {
            spdlog::error("Failed to create sample test file");
            return false;
        }

        spdlog::info("Google Test setup completed");
        return true;

    } catch (const std::exception& e) {
        spdlog::error("Error setting up Google Test: {}", e.what());
        return false;
    }
}

bool TestFrameworkManager::setupCatch2(const std::filesystem::path& projectPath, const TestConfig& config) {
    try {
        // Create main test file
        auto mainTestFile = projectPath / config.testDirectory / "main.cpp";
        std::string mainContent = R"(#define CATCH_CONFIG_MAIN
#include <catch2/catch_all.hpp>
)";

        if (!FileUtils::writeToFile(mainTestFile.string(), mainContent)) {
            spdlog::error("Failed to create main test file");
            return false;
        }

        // Create sample test file
        auto sampleTestFile = projectPath / config.testDirectory / "unit" / "sample_test.cpp";
        std::string sampleContent = generateCatch2Template("Sample");

        if (!FileUtils::writeToFile(sampleTestFile.string(), sampleContent)) {
            spdlog::error("Failed to create sample test file");
            return false;
        }

        spdlog::info("Catch2 setup completed");
        return true;

    } catch (const std::exception& e) {
        spdlog::error("Error setting up Catch2: {}", e.what());
        return false;
    }
}

bool TestFrameworkManager::setupDoctest(const std::filesystem::path& projectPath, const TestConfig& config) {
    try {
        // Create main test file
        auto mainTestFile = projectPath / config.testDirectory / "main.cpp";
        std::string mainContent = R"(#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>
)";

        if (!FileUtils::writeToFile(mainTestFile.string(), mainContent)) {
            spdlog::error("Failed to create main test file");
            return false;
        }

        // Create sample test file
        auto sampleTestFile = projectPath / config.testDirectory / "unit" / "sample_test.cpp";
        std::string sampleContent = generateDoctestTemplate("Sample");

        if (!FileUtils::writeToFile(sampleTestFile.string(), sampleContent)) {
            spdlog::error("Failed to create sample test file");
            return false;
        }

        spdlog::info("Doctest setup completed");
        return true;

    } catch (const std::exception& e) {
        spdlog::error("Error setting up Doctest: {}", e.what());
        return false;
    }
}

bool TestFrameworkManager::setupBoostTest(const std::filesystem::path& projectPath, const TestConfig& config) {
    try {
        // Create main test file
        auto mainTestFile = projectPath / config.testDirectory / "main.cpp";
        std::string mainContent = R"(#define BOOST_TEST_MODULE Main Test Suite
#include <boost/test/included/unit_test.hpp>
)";

        if (!FileUtils::writeToFile(mainTestFile.string(), mainContent)) {
            spdlog::error("Failed to create main test file");
            return false;
        }

        // Create sample test file
        auto sampleTestFile = projectPath / config.testDirectory / "unit" / "sample_test.cpp";
        std::string sampleContent = generateBoostTestTemplate("Sample");

        if (!FileUtils::writeToFile(sampleTestFile.string(), sampleContent)) {
            spdlog::error("Failed to create sample test file");
            return false;
        }

        spdlog::info("Boost.Test setup completed");
        return true;

    } catch (const std::exception& e) {
        spdlog::error("Error setting up Boost.Test: {}", e.what());
        return false;
    }
}

std::string TestFrameworkManager::generateGoogleTestTemplate(const std::string& className) const {
    return R"(#include <gtest/gtest.h>
// #include ")" + className + R"(.h"

class )" + className + R"(Test : public ::testing::Test {
protected:
    void SetUp() override {
        // Set up test fixtures here
    }

    void TearDown() override {
        // Clean up after tests
    }

    // Test fixture members
};

// Basic test
TEST_F()" + className + R"(Test, BasicTest) {
    EXPECT_TRUE(true);
    ASSERT_FALSE(false);
}

// Parameterized test example
class )" + className + R"(ParameterizedTest : public ::testing::TestWithParam<int> {
};

TEST_P()" + className + R"(ParameterizedTest, ParameterTest) {
    int value = GetParam();
    EXPECT_GE(value, 0);
}

INSTANTIATE_TEST_SUITE_P(
    )" + className + R"(Values,
    )" + className + R"(ParameterizedTest,
    ::testing::Values(1, 2, 3, 4, 5)
);

// Death test example
TEST()" + className + R"(DeathTest, CrashTest) {
    // EXPECT_DEATH(SomeFunction(), "Expected error message");
}
)";
}

std::string TestFrameworkManager::generateCatch2Template(const std::string& className) const {
    return R"(#include <catch2/catch_all.hpp>
// #include ")" + className + R"(.h"

TEST_CASE(")" + className + R"( basic functionality", "[)" + utils::StringUtils::toLower(className) + R"(]") {
    SECTION("Basic test") {
        REQUIRE(true);
        CHECK(1 == 1);
    }

    SECTION("Another test") {
        REQUIRE_FALSE(false);
        CHECK_THAT("Hello World", Catch::Matchers::ContainsSubstring("World"));
    }
}

TEST_CASE(")" + className + R"( parameterized test", "[)" + utils::StringUtils::toLower(className) + R"(][parameterized]") {
    auto value = GENERATE(1, 2, 3, 4, 5);

    REQUIRE(value > 0);
    REQUIRE(value <= 5);
}

TEST_CASE(")" + className + R"( benchmark", "[)" + utils::StringUtils::toLower(className) + R"(][benchmark]") {
    BENCHMARK("Simple operation") {
        return 42;
    };
}

SCENARIO(")" + className + R"( BDD style test", "[)" + utils::StringUtils::toLower(className) + R"(][bdd]") {
    GIVEN("A )" + className + R"( object") {
        // Setup

        WHEN("Something happens") {
            // Action

            THEN("Expected result occurs") {
                REQUIRE(true);
            }
        }
    }
}
)";
}

std::string TestFrameworkManager::generateDoctestTemplate(const std::string& className) const {
    return R"(#include <doctest/doctest.h>
// #include ")" + className + R"(.h"

TEST_CASE(")" + className + R"( basic functionality") {
    CHECK(true);
    REQUIRE(1 == 1);

    SUBCASE("Subtest 1") {
        CHECK_FALSE(false);
    }

    SUBCASE("Subtest 2") {
        CHECK_EQ(2 + 2, 4);
    }
}

TEST_CASE_TEMPLATE(")" + className + R"( template test", T, int, float, double) {
    T value = T(42);
    CHECK(value == T(42));
}

SCENARIO(")" + className + R"( BDD style") {
    GIVEN("A )" + className + R"( object") {
        // Setup

        WHEN("Something happens") {
            // Action

            THEN("Expected result occurs") {
                REQUIRE(true);
            }
        }
    }
}
)";
}

std::string TestFrameworkManager::generateBoostTestTemplate(const std::string& className) const {
    return R"(#include <boost/test/unit_test.hpp>
// #include ")" + className + R"(.h"

BOOST_AUTO_TEST_SUITE()" + className + R"(TestSuite)

BOOST_AUTO_TEST_CASE(BasicTest) {
    BOOST_CHECK(true);
    BOOST_REQUIRE_EQUAL(1, 1);
}

BOOST_AUTO_TEST_CASE(AnotherTest) {
    BOOST_CHECK_NE(1, 2);
    BOOST_WARN_MESSAGE(true, "This is a warning");
}

// Fixture example
struct )" + className + R"(Fixture {
    )" + className + R"(Fixture() {
        // Setup
    }

    ~)" + className + R"(Fixture() {
        // Cleanup
    }

    // Fixture members
};

BOOST_FIXTURE_TEST_CASE(FixtureTest, )" + className + R"(Fixture) {
    BOOST_CHECK(true);
}

BOOST_AUTO_TEST_SUITE_END()
)";
}

std::string TestFrameworkManager::generateCMakeTestConfig(TestFramework framework, const TestConfig& config) const {
    std::ostringstream cmake;

    cmake << "# Test configuration generated by CPP-Scaffold\n\n";
    cmake << "enable_testing()\n\n";

    // Find package based on framework
    switch (framework) {
        case TestFramework::GoogleTest:
            cmake << "find_package(GTest REQUIRED)\n";
            cmake << "include(GoogleTest)\n\n";
            break;
        case TestFramework::Catch2:
            cmake << "find_package(Catch2 3 REQUIRED)\n";
            cmake << "include(Catch)\n\n";
            break;
        case TestFramework::Doctest:
            cmake << "find_package(doctest REQUIRED)\n\n";
            break;
        case TestFramework::Boost_Test:
            cmake << "find_package(Boost REQUIRED COMPONENTS unit_test_framework)\n\n";
            break;
        default:
            break;
    }

    // Test executable
    cmake << "# Test executable\n";
    cmake << "file(GLOB_RECURSE TEST_SOURCES \"" << config.testDirectory << "/*.cpp\")\n";
    cmake << "add_executable(${PROJECT_NAME}_tests ${TEST_SOURCES})\n\n";

    // Link libraries
    cmake << "target_link_libraries(${PROJECT_NAME}_tests\n";
    cmake << "    PRIVATE\n";
    cmake << "        ${PROJECT_NAME}_lib\n";

    auto frameworkInfo = getFrameworkInfo(framework);
    if (frameworkInfo) {
        for (const auto& target : frameworkInfo->cmakeTargets) {
            cmake << "        " << target << "\n";
        }
    }
    cmake << ")\n\n";

    // Test discovery
    switch (framework) {
        case TestFramework::GoogleTest:
            cmake << "gtest_discover_tests(${PROJECT_NAME}_tests)\n";
            break;
        case TestFramework::Catch2:
            cmake << "catch_discover_tests(${PROJECT_NAME}_tests)\n";
            break;
        default:
            cmake << "add_test(NAME ${PROJECT_NAME}_tests COMMAND ${PROJECT_NAME}_tests)\n";
            break;
    }

    // Code coverage
    if (config.enableCodeCoverage) {
        cmake << "\n# Code coverage\n";
        cmake << "if(CMAKE_BUILD_TYPE STREQUAL \"Debug\")\n";
        cmake << "    target_compile_options(${PROJECT_NAME}_tests PRIVATE --coverage)\n";
        cmake << "    target_link_options(${PROJECT_NAME}_tests PRIVATE --coverage)\n";
        cmake << "endif()\n";
    }

    // Sanitizers
    if (config.enableAddressSanitizer) {
        cmake << "\n# Address Sanitizer\n";
        cmake << "target_compile_options(${PROJECT_NAME}_tests PRIVATE -fsanitize=address)\n";
        cmake << "target_link_options(${PROJECT_NAME}_tests PRIVATE -fsanitize=address)\n";
    }

    if (config.enableThreadSanitizer) {
        cmake << "\n# Thread Sanitizer\n";
        cmake << "target_compile_options(${PROJECT_NAME}_tests PRIVATE -fsanitize=thread)\n";
        cmake << "target_link_options(${PROJECT_NAME}_tests PRIVATE -fsanitize=thread)\n";
    }

    if (config.enableUndefinedBehaviorSanitizer) {
        cmake << "\n# Undefined Behavior Sanitizer\n";
        cmake << "target_compile_options(${PROJECT_NAME}_tests PRIVATE -fsanitize=undefined)\n";
        cmake << "target_link_options(${PROJECT_NAME}_tests PRIVATE -fsanitize=undefined)\n";
    }

    return cmake.str();
}

std::string TestFrameworkManager::generateGitHubActionsTestWorkflow(TestFramework framework, const TestConfig& config) const {
    (void)framework; // TODO: Use framework to customize workflow
    std::ostringstream workflow;

    workflow << "name: Tests\n\n";
    workflow << "on:\n";
    workflow << "  push:\n";
    workflow << "    branches: [ main, develop ]\n";
    workflow << "  pull_request:\n";
    workflow << "    branches: [ main ]\n\n";

    workflow << "jobs:\n";
    workflow << "  test:\n";
    workflow << "    runs-on: ${{ matrix.os }}\n";
    workflow << "    strategy:\n";
    workflow << "      matrix:\n";
    workflow << "        os: [ubuntu-latest, windows-latest, macos-latest]\n";
    workflow << "        build_type: [Debug, Release]\n";
    workflow << "        compiler: [gcc, clang]\n";
    workflow << "        exclude:\n";
    workflow << "          - os: windows-latest\n";
    workflow << "            compiler: gcc\n\n";

    workflow << "    steps:\n";
    workflow << "    - uses: actions/checkout@v3\n\n";

    workflow << "    - name: Install dependencies\n";
    workflow << "      run: |\n";
    workflow << "        if [ \"$RUNNER_OS\" == \"Linux\" ]; then\n";
    workflow << "          sudo apt-get update\n";
    workflow << "          sudo apt-get install -y cmake ninja-build\n";
    workflow << "        elif [ \"$RUNNER_OS\" == \"macOS\" ]; then\n";
    workflow << "          brew install cmake ninja\n";
    workflow << "        fi\n";
    workflow << "      shell: bash\n\n";

    workflow << "    - name: Configure CMake\n";
    workflow << "      run: |\n";
    workflow << "        cmake -B build -DCMAKE_BUILD_TYPE=${{ matrix.build_type }} -G Ninja\n\n";

    workflow << "    - name: Build\n";
    workflow << "      run: cmake --build build --config ${{ matrix.build_type }}\n\n";

    workflow << "    - name: Test\n";
    workflow << "      working-directory: build\n";
    workflow << "      run: ctest --build-config ${{ matrix.build_type }} --verbose\n\n";

    if (config.enableCodeCoverage) {
        workflow << "    - name: Generate coverage report\n";
        workflow << "      if: matrix.build_type == 'Debug' && matrix.os == 'ubuntu-latest'\n";
        workflow << "      run: |\n";
        workflow << "        sudo apt-get install -y lcov\n";
        workflow << "        lcov --capture --directory . --output-file coverage.info\n";
        workflow << "        lcov --remove coverage.info '/usr/*' --output-file coverage.info\n";
        workflow << "        lcov --list coverage.info\n\n";

        workflow << "    - name: Upload coverage to Codecov\n";
        workflow << "      if: matrix.build_type == 'Debug' && matrix.os == 'ubuntu-latest'\n";
        workflow << "      uses: codecov/codecov-action@v3\n";
        workflow << "      with:\n";
        workflow << "        file: ./coverage.info\n";
        workflow << "        flags: unittests\n";
        workflow << "        name: codecov-umbrella\n";
    }

    return workflow.str();
}

// Test utilities implementation
namespace test_utils {
    TestFramework detectFramework(const std::filesystem::path& projectPath) {
        // Check for framework-specific files
        if (std::filesystem::exists(projectPath / "tests" / "gtest")) {
            return TestFramework::GoogleTest;
        }

        if (std::filesystem::exists(projectPath / "tests" / "catch2")) {
            return TestFramework::Catch2;
        }

        if (std::filesystem::exists(projectPath / "tests" / "doctest")) {
            return TestFramework::Doctest;
        }

        // Check CMakeLists.txt for framework references
        auto cmakeFile = projectPath / "CMakeLists.txt";
        if (std::filesystem::exists(cmakeFile)) {
            std::ifstream file(cmakeFile);
            std::string content((std::istreambuf_iterator<char>(file)),
                               std::istreambuf_iterator<char>());

            if (content.find("GTest") != std::string::npos ||
                content.find("gtest") != std::string::npos) {
                return TestFramework::GoogleTest;
            }

            if (content.find("Catch2") != std::string::npos) {
                return TestFramework::Catch2;
            }

            if (content.find("doctest") != std::string::npos) {
                return TestFramework::Doctest;
            }

            if (content.find("Boost") != std::string::npos &&
                content.find("unit_test") != std::string::npos) {
                return TestFramework::Boost_Test;
            }
        }

        return TestFramework::GoogleTest; // Default
    }

    std::string frameworkToString(TestFramework framework) {
        switch (framework) {
            case TestFramework::GoogleTest: return "GoogleTest";
            case TestFramework::Catch2: return "Catch2";
            case TestFramework::Doctest: return "Doctest";
            case TestFramework::Boost_Test: return "Boost.Test";
            case TestFramework::CppUnit: return "CppUnit";
            case TestFramework::Unity: return "Unity";
            case TestFramework::Custom: return "Custom";
            default: return "Unknown";
        }
    }

    std::optional<TestFramework> stringToFramework(const std::string& name) {
        std::string lowerName = utils::StringUtils::toLower(name);

        if (lowerName == "googletest" || lowerName == "gtest") {
            return TestFramework::GoogleTest;
        } else if (lowerName == "catch2" || lowerName == "catch") {
            return TestFramework::Catch2;
        } else if (lowerName == "doctest") {
            return TestFramework::Doctest;
        } else if (lowerName == "boost.test" || lowerName == "boost_test") {
            return TestFramework::Boost_Test;
        } else if (lowerName == "cppunit") {
            return TestFramework::CppUnit;
        } else if (lowerName == "unity") {
            return TestFramework::Unity;
        } else if (lowerName == "custom") {
            return TestFramework::Custom;
        }

        return std::nullopt;
    }

    bool isTestFile(const std::filesystem::path& filePath) {
        std::string filename = filePath.filename().string();
        std::string lowerFilename = utils::StringUtils::toLower(filename);

        return lowerFilename.find("test") != std::string::npos ||
               lowerFilename.find("spec") != std::string::npos ||
               filePath.parent_path().filename().string() == "tests" ||
               filePath.parent_path().filename().string() == "test";
    }

    std::vector<std::filesystem::path> findTestFiles(const std::filesystem::path& directory) {
        std::vector<std::filesystem::path> testFiles;

        if (!std::filesystem::exists(directory)) {
            return testFiles;
        }

        for (const auto& entry : std::filesystem::recursive_directory_iterator(directory)) {
            if (entry.is_regular_file()) {
                auto path = entry.path();
                if ((path.extension() == ".cpp" || path.extension() == ".cc") &&
                    isTestFile(path)) {
                    testFiles.push_back(path);
                }
            }
        }

        return testFiles;
    }

    bool validateTestConfig(const TestConfig& config) {
        // Basic validation
        if (config.testDirectory.empty()) {
            return false;
        }

        if (config.testTypes.empty()) {
            return false;
        }

        // Framework-specific validation
        switch (config.framework) {
            case TestFramework::GoogleTest:
                // Google Test specific validation
                break;
            case TestFramework::Catch2:
                // Catch2 specific validation
                break;
            default:
                break;
        }

        return true;
    }
}

} // namespace testing

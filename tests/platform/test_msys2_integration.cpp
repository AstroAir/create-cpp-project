#include <gtest/gtest.h>
#include "../src/templates/console_template.h"
#include "../src/templates/library_template.h"
#include "../src/templates/gui_template.h"
#include "../src/utils/msys2_validator.h"
#include "../../src/utils/core/file_utils.h"
#include "../src/cli/types/cli_enums.h"
#include <filesystem>
#include <fstream>

class MSYS2IntegrationTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create temporary test directory
        testDir = std::filesystem::temp_directory_path() / "msys2_test";
        std::filesystem::create_directories(testDir);
        
        // Set up basic project options
        options.projectName = "test_project";
        options.packageManager = PackageManager::MSYS2;
        options.buildSystem = BuildSystem::CMake;
        options.includeTests = true;
        options.testFramework = TestFramework::GTest;
    }
    
    void TearDown() override {
        // Clean up test directory
        if (std::filesystem::exists(testDir)) {
            std::filesystem::remove_all(testDir);
        }
    }
    
    bool validatePKGBUILDExists(const std::string& projectPath) {
        std::string pkgbuildPath = FileUtils::combinePath(projectPath, "PKGBUILD");
        return FileUtils::fileExists(pkgbuildPath);
    }
    
    bool validatePKGBUILDContent(const std::string& projectPath, const std::string& expectedType) {
        std::string pkgbuildPath = FileUtils::combinePath(projectPath, "PKGBUILD");
        if (!FileUtils::fileExists(pkgbuildPath)) {
            return false;
        }
        
        std::string content = FileUtils::readFile(pkgbuildPath);
        
        // Check for required PKGBUILD elements
        bool hasRealname = content.find("_realname=test_project") != std::string::npos;
        bool hasPkgname = content.find("pkgname=\"${MINGW_PACKAGE_PREFIX}-${_realname}\"") != std::string::npos;
        bool hasBuildFunc = content.find("build() {") != std::string::npos;
        bool hasPackageFunc = content.find("package() {") != std::string::npos;
        bool hasDescription = content.find("pkgdesc=") != std::string::npos;
        bool hasArch = content.find("arch=(\"any\")") != std::string::npos;
        bool hasMingwArch = content.find("mingw_arch=") != std::string::npos;
        
        // Check for type-specific content
        bool hasTypeSpecificContent = true;
        if (expectedType == "library") {
            hasTypeSpecificContent = content.find("-DBUILD_SHARED_LIBS=ON") != std::string::npos;
        } else if (expectedType == "gui") {
            hasTypeSpecificContent = content.find("qt6-base") != std::string::npos ||
                                   content.find("gtk3") != std::string::npos ||
                                   content.find("wxwidgets") != std::string::npos;
        }
        
        return hasRealname && hasPkgname && hasBuildFunc && hasPackageFunc && 
               hasDescription && hasArch && hasMingwArch && hasTypeSpecificContent;
    }
    
    std::filesystem::path testDir;
    ProjectOptions options;
};

TEST_F(MSYS2IntegrationTest, ConsoleTemplateGeneratesPKGBUILD) {
    // Change to test directory
    std::filesystem::current_path(testDir);
    
    // Create console template
    ConsoleTemplate consoleTemplate(options);
    
    // Test PKGBUILD content generation
    std::string pkgbuildContent = consoleTemplate.getMSYS2PKGBUILDContent();
    
    EXPECT_FALSE(pkgbuildContent.empty());
    EXPECT_NE(pkgbuildContent.find("_realname=test_project"), std::string::npos);
    EXPECT_NE(pkgbuildContent.find("pkgdesc=\"A C++ console application (mingw-w64)\""), std::string::npos);
    EXPECT_NE(pkgbuildContent.find("build() {"), std::string::npos);
    EXPECT_NE(pkgbuildContent.find("package() {"), std::string::npos);
    EXPECT_NE(pkgbuildContent.find("${MINGW_PREFIX}/bin/cmake.exe"), std::string::npos);
}

TEST_F(MSYS2IntegrationTest, LibraryTemplateGeneratesPKGBUILD) {
    // Change to test directory
    std::filesystem::current_path(testDir);
    
    // Create library template
    LibraryTemplate libraryTemplate(options);
    
    // Test PKGBUILD content generation
    std::string pkgbuildContent = libraryTemplate.getMSYS2PKGBUILDContent();
    
    EXPECT_FALSE(pkgbuildContent.empty());
    EXPECT_NE(pkgbuildContent.find("_realname=test_project"), std::string::npos);
    EXPECT_NE(pkgbuildContent.find("pkgdesc=\"A C++ library (mingw-w64)\""), std::string::npos);
    EXPECT_NE(pkgbuildContent.find("-DBUILD_SHARED_LIBS=ON"), std::string::npos);
    EXPECT_NE(pkgbuildContent.find("build() {"), std::string::npos);
    EXPECT_NE(pkgbuildContent.find("package() {"), std::string::npos);
}

TEST_F(MSYS2IntegrationTest, GUITemplateGeneratesPKGBUILD) {
    // Change to test directory
    std::filesystem::current_path(testDir);
    
    // Create GUI template with Qt
    GuiTemplate guiTemplate(options, "qt");
    
    // Test PKGBUILD content generation
    std::string pkgbuildContent = guiTemplate.getMSYS2PKGBUILDContent();
    
    EXPECT_FALSE(pkgbuildContent.empty());
    EXPECT_NE(pkgbuildContent.find("_realname=test_project"), std::string::npos);
    EXPECT_NE(pkgbuildContent.find("pkgdesc=\"A C++ GUI application (mingw-w64)\""), std::string::npos);
    EXPECT_NE(pkgbuildContent.find("qt6-base"), std::string::npos);
    EXPECT_NE(pkgbuildContent.find("build() {"), std::string::npos);
    EXPECT_NE(pkgbuildContent.find("package() {"), std::string::npos);
}

TEST_F(MSYS2IntegrationTest, PackageManagerSetupCreatesPKGBUILD) {
    // Change to test directory
    std::filesystem::current_path(testDir);
    
    // Create console template
    ConsoleTemplate consoleTemplate(options);
    
    // Test package manager setup
    bool setupResult = consoleTemplate.setupPackageManager();
    
    EXPECT_TRUE(setupResult);
    EXPECT_TRUE(validatePKGBUILDExists(options.projectName));
    EXPECT_TRUE(validatePKGBUILDContent(options.projectName, "console"));
}

TEST_F(MSYS2IntegrationTest, PKGBUILDValidationWorks) {
    // Create a test PKGBUILD file
    std::string testPKGBUILD = R"(
# Test PKGBUILD
_realname=test_project
pkgbase=mingw-w64-${_realname}
pkgname="${MINGW_PACKAGE_PREFIX}-${_realname}"
pkgver=1.0.0
pkgrel=1
pkgdesc="A test package"
arch=('any')
mingw_arch=('mingw32' 'mingw64' 'ucrt64' 'clang64')
license=('MIT')
makedepends=("${MINGW_PACKAGE_PREFIX}-cc")
depends=("${MINGW_PACKAGE_PREFIX}-gcc-libs")
source=("${_realname}-${pkgver}.tar.gz")
sha256sums=('SKIP')

build() {
  echo "Building..."
}

package() {
  echo "Packaging..."
}
)";
    
    // Test PKGBUILD content validation
    auto result = MSYS2Validator::validatePKGBUILDContent(testPKGBUILD);
    
    EXPECT_TRUE(result.isValid);
    EXPECT_TRUE(result.errors.empty());
}

TEST_F(MSYS2IntegrationTest, InvalidPKGBUILDDetected) {
    // Create an invalid PKGBUILD (missing required fields)
    std::string invalidPKGBUILD = R"(
# Invalid PKGBUILD - missing required fields
_realname=test_project
pkgver=1.0.0
)";
    
    // Test validation catches errors
    auto result = MSYS2Validator::validatePKGBUILDContent(invalidPKGBUILD);
    
    EXPECT_FALSE(result.isValid);
    EXPECT_FALSE(result.errors.empty());
    
    // Should detect missing required fields
    bool foundMissingPkgname = false;
    bool foundMissingPkgdesc = false;
    bool foundMissingPackageFunc = false;
    
    for (const auto& error : result.errors) {
        if (error.find("pkgname") != std::string::npos) foundMissingPkgname = true;
        if (error.find("pkgdesc") != std::string::npos) foundMissingPkgdesc = true;
        if (error.find("package()") != std::string::npos) foundMissingPackageFunc = true;
    }
    
    EXPECT_TRUE(foundMissingPkgname);
    EXPECT_TRUE(foundMissingPkgdesc);
    EXPECT_TRUE(foundMissingPackageFunc);
}

TEST_F(MSYS2IntegrationTest, TestFrameworkDependenciesIncluded) {
    // Test with Google Test
    options.testFramework = TestFramework::GTest;
    ConsoleTemplate gtestTemplate(options);
    std::string gtestPKGBUILD = gtestTemplate.getMSYS2PKGBUILDContent();
    
    EXPECT_NE(gtestPKGBUILD.find("gtest"), std::string::npos);
    
    // Test with Catch2
    options.testFramework = TestFramework::Catch2;
    ConsoleTemplate catch2Template(options);
    std::string catch2PKGBUILD = catch2Template.getMSYS2PKGBUILDContent();
    
    EXPECT_NE(catch2PKGBUILD.find("catch2"), std::string::npos);
}

TEST_F(MSYS2IntegrationTest, GUIFrameworkDependenciesIncluded) {
    // Test Qt dependencies
    GuiTemplate qtTemplate(options, "qt");
    std::string qtPKGBUILD = qtTemplate.getMSYS2PKGBUILDContent();
    
    EXPECT_NE(qtPKGBUILD.find("qt6-base"), std::string::npos);
    EXPECT_NE(qtPKGBUILD.find("qt6-tools"), std::string::npos);
    
    // Test GTK dependencies
    GuiTemplate gtkTemplate(options, "gtk");
    std::string gtkPKGBUILD = gtkTemplate.getMSYS2PKGBUILDContent();
    
    EXPECT_NE(gtkPKGBUILD.find("gtk3"), std::string::npos);
    
    // Test wxWidgets dependencies
    GuiTemplate wxTemplate(options, "wxwidgets");
    std::string wxPKGBUILD = wxTemplate.getMSYS2PKGBUILDContent();
    
    EXPECT_NE(wxPKGBUILD.find("wxwidgets"), std::string::npos);
}

// Integration test to verify the complete workflow
TEST_F(MSYS2IntegrationTest, CompleteWorkflowTest) {
    // Change to test directory
    std::filesystem::current_path(testDir);
    
    // Test different project types
    std::vector<std::pair<std::string, std::function<bool()>>> testCases = {
        {"console", [this]() {
            ConsoleTemplate template(options);
            return template.setupPackageManager() && 
                   validatePKGBUILDExists(options.projectName) &&
                   validatePKGBUILDContent(options.projectName, "console");
        }},
        {"library", [this]() {
            options.projectName = "test_library";
            LibraryTemplate template(options);
            return template.setupPackageManager() && 
                   validatePKGBUILDExists(options.projectName) &&
                   validatePKGBUILDContent(options.projectName, "library");
        }},
        {"gui", [this]() {
            options.projectName = "test_gui";
            GuiTemplate template(options, "qt");
            return template.setupPackageManager() && 
                   validatePKGBUILDExists(options.projectName) &&
                   validatePKGBUILDContent(options.projectName, "gui");
        }}
    };
    
    for (const auto& testCase : testCases) {
        SCOPED_TRACE("Testing " + testCase.first + " template");
        EXPECT_TRUE(testCase.second()) << "Failed for " << testCase.first << " template";
    }
}

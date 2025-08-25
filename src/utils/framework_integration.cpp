#include "framework_integration.h"
#include "file_utils.h"
#include "string_utils.h"
#include <spdlog/spdlog.h>
#include <iostream>
#include <algorithm>
#include <sstream>

namespace utils {

// Static member initialization
std::map<FrameworkIntegration::Framework, FrameworkIntegration::FrameworkInfo> FrameworkIntegration::s_frameworkRegistry;

void FrameworkIntegration::initializeFrameworkRegistry() {
    if (!s_frameworkRegistry.empty()) {
        return; // Already initialized
    }

    // Qt Framework
    FrameworkInfo qtInfo;
    qtInfo.framework = Framework::Qt;
    qtInfo.name = "Qt";
    qtInfo.description = "Cross-platform application development framework";
    qtInfo.version = "6.5.0";
    qtInfo.packageNames[PackageManager::vcpkg] = "qt6";
    qtInfo.packageNames[PackageManager::Conan] = "qt/6.5.0";
    qtInfo.cmakeTargets = {"Qt6::Core", "Qt6::Widgets", "Qt6::Gui"};
    qtInfo.requiresSpecialSetup = true;
    qtInfo.setupFunction = setupQt;
    s_frameworkRegistry[Framework::Qt] = qtInfo;

    // SFML Framework
    FrameworkInfo sfmlInfo;
    sfmlInfo.framework = Framework::SFML;
    sfmlInfo.name = "SFML";
    sfmlInfo.description = "Simple and Fast Multimedia Library";
    sfmlInfo.version = "2.6.0";
    sfmlInfo.packageNames[PackageManager::vcpkg] = "sfml";
    sfmlInfo.packageNames[PackageManager::Conan] = "sfml/2.6.0";
    sfmlInfo.cmakeTargets = {"sfml-graphics", "sfml-window", "sfml-system"};
    sfmlInfo.requiresSpecialSetup = true;
    sfmlInfo.setupFunction = setupSFML;
    s_frameworkRegistry[Framework::SFML] = sfmlInfo;

    // Boost Libraries
    FrameworkInfo boostInfo;
    boostInfo.framework = Framework::Boost;
    boostInfo.name = "Boost";
    boostInfo.description = "Portable C++ source libraries";
    boostInfo.version = "1.82.0";
    boostInfo.packageNames[PackageManager::vcpkg] = "boost";
    boostInfo.packageNames[PackageManager::Conan] = "boost/1.82.0";
    boostInfo.cmakeTargets = {"Boost::system", "Boost::filesystem", "Boost::thread"};
    boostInfo.requiresSpecialSetup = true;
    boostInfo.setupFunction = setupBoost;
    s_frameworkRegistry[Framework::Boost] = boostInfo;

    // Google Test
    FrameworkInfo gtestInfo;
    gtestInfo.framework = Framework::GoogleTest;
    gtestInfo.name = "Google Test";
    gtestInfo.description = "Google's C++ testing and mocking framework";
    gtestInfo.version = "1.14.0";
    gtestInfo.packageNames[PackageManager::vcpkg] = "gtest";
    gtestInfo.packageNames[PackageManager::Conan] = "gtest/1.14.0";
    gtestInfo.cmakeTargets = {"GTest::gtest", "GTest::gtest_main", "GTest::gmock"};
    gtestInfo.requiresSpecialSetup = true;
    gtestInfo.setupFunction = setupGoogleTest;
    s_frameworkRegistry[Framework::GoogleTest] = gtestInfo;

    // OpenCV
    FrameworkInfo opencvInfo;
    opencvInfo.framework = Framework::OpenCV;
    opencvInfo.name = "OpenCV";
    opencvInfo.description = "Open Source Computer Vision Library";
    opencvInfo.version = "4.8.0";
    opencvInfo.packageNames[PackageManager::vcpkg] = "opencv";
    opencvInfo.packageNames[PackageManager::Conan] = "opencv/4.8.0";
    opencvInfo.cmakeTargets = {"opencv_core", "opencv_imgproc", "opencv_highgui"};
    opencvInfo.requiresSpecialSetup = true;
    opencvInfo.setupFunction = setupOpenCV;
    s_frameworkRegistry[Framework::OpenCV] = opencvInfo;

    // Dear ImGui
    FrameworkInfo imguiInfo;
    imguiInfo.framework = Framework::Dear_ImGui;
    imguiInfo.name = "Dear ImGui";
    imguiInfo.description = "Bloat-free Graphical User interface for C++";
    imguiInfo.version = "1.89.0";
    imguiInfo.packageNames[PackageManager::vcpkg] = "imgui";
    imguiInfo.packageNames[PackageManager::Conan] = "imgui/1.89.0";
    imguiInfo.cmakeTargets = {"imgui::imgui"};
    imguiInfo.requiresSpecialSetup = true;
    imguiInfo.setupFunction = setupImGui;
    s_frameworkRegistry[Framework::Dear_ImGui] = imguiInfo;

    // fmt library
    FrameworkInfo fmtInfo;
    fmtInfo.framework = Framework::fmt;
    fmtInfo.name = "fmt";
    fmtInfo.description = "A modern formatting library";
    fmtInfo.version = "10.1.0";
    fmtInfo.packageNames[PackageManager::vcpkg] = "fmt";
    fmtInfo.packageNames[PackageManager::Conan] = "fmt/10.1.0";
    fmtInfo.cmakeTargets = {"fmt::fmt"};
    fmtInfo.requiresSpecialSetup = false;
    s_frameworkRegistry[Framework::fmt] = fmtInfo;

    // spdlog
    FrameworkInfo spdlogInfo;
    spdlogInfo.framework = Framework::spdlog;
    spdlogInfo.name = "spdlog";
    spdlogInfo.description = "Fast C++ logging library";
    spdlogInfo.version = "1.12.0";
    spdlogInfo.packageNames[PackageManager::vcpkg] = "spdlog";
    spdlogInfo.packageNames[PackageManager::Conan] = "spdlog/1.12.0";
    spdlogInfo.cmakeTargets = {"spdlog::spdlog"};
    spdlogInfo.requiresSpecialSetup = false;
    s_frameworkRegistry[Framework::spdlog] = spdlogInfo;

    // nlohmann/json
    FrameworkInfo jsonInfo;
    jsonInfo.framework = Framework::JSON_nlohmann;
    jsonInfo.name = "nlohmann/json";
    jsonInfo.description = "JSON for Modern C++";
    jsonInfo.version = "3.11.2";
    jsonInfo.packageNames[PackageManager::vcpkg] = "nlohmann-json";
    jsonInfo.packageNames[PackageManager::Conan] = "nlohmann_json/3.11.2";
    jsonInfo.cmakeTargets = {"nlohmann_json::nlohmann_json"};
    jsonInfo.requiresSpecialSetup = false;
    s_frameworkRegistry[Framework::JSON_nlohmann] = jsonInfo;

    // Additional GUI Frameworks

    // GTK Framework
    FrameworkInfo gtkInfo;
    gtkInfo.framework = Framework::GTK;
    gtkInfo.name = "GTK";
    gtkInfo.description = "Multi-platform toolkit for creating graphical user interfaces";
    gtkInfo.version = "4.12.0";
    gtkInfo.packageNames[PackageManager::vcpkg] = "gtk";
    gtkInfo.packageNames[PackageManager::Conan] = "gtk/4.12.0";
    gtkInfo.cmakeTargets = {"PkgConfig::GTK4"};
    gtkInfo.requiresSpecialSetup = true;
    gtkInfo.setupFunction = setupGTK;
    s_frameworkRegistry[Framework::GTK] = gtkInfo;

    // FLTK Framework
    FrameworkInfo fltkInfo;
    fltkInfo.framework = Framework::FLTK;
    fltkInfo.name = "FLTK";
    fltkInfo.description = "Fast Light Toolkit - Cross-platform C++ GUI toolkit";
    fltkInfo.version = "1.3.8";
    fltkInfo.packageNames[PackageManager::vcpkg] = "fltk";
    fltkInfo.packageNames[PackageManager::Conan] = "fltk/1.3.8";
    fltkInfo.cmakeTargets = {"fltk", "fltk_forms", "fltk_images"};
    fltkInfo.requiresSpecialSetup = true;
    fltkInfo.setupFunction = setupFLTK;
    s_frameworkRegistry[Framework::FLTK] = fltkInfo;

    // wxWidgets Framework
    FrameworkInfo wxInfo;
    wxInfo.framework = Framework::wxWidgets;
    wxInfo.name = "wxWidgets";
    wxInfo.description = "Cross-platform GUI library";
    wxInfo.version = "3.2.0";
    wxInfo.packageNames[PackageManager::vcpkg] = "wxwidgets";
    wxInfo.packageNames[PackageManager::Conan] = "wxwidgets/3.2.0";
    wxInfo.cmakeTargets = {"wx::core", "wx::base"};
    wxInfo.requiresSpecialSetup = true;
    wxInfo.setupFunction = setupWxWidgets;
    s_frameworkRegistry[Framework::wxWidgets] = wxInfo;

    // Game Development Frameworks

    // SDL2 Framework
    FrameworkInfo sdl2Info;
    sdl2Info.framework = Framework::SDL2;
    sdl2Info.name = "SDL2";
    sdl2Info.description = "Simple DirectMedia Layer - Cross-platform development library";
    sdl2Info.version = "2.28.0";
    sdl2Info.packageNames[PackageManager::vcpkg] = "sdl2";
    sdl2Info.packageNames[PackageManager::Conan] = "sdl/2.28.0";
    sdl2Info.cmakeTargets = {"SDL2::SDL2", "SDL2::SDL2main"};
    sdl2Info.requiresSpecialSetup = true;
    sdl2Info.setupFunction = setupSDL2;
    s_frameworkRegistry[Framework::SDL2] = sdl2Info;

    // Allegro Framework
    FrameworkInfo allegroInfo;
    allegroInfo.framework = Framework::Allegro;
    allegroInfo.name = "Allegro";
    allegroInfo.description = "Game programming library";
    allegroInfo.version = "5.2.8";
    allegroInfo.packageNames[PackageManager::vcpkg] = "allegro5";
    allegroInfo.packageNames[PackageManager::Conan] = "allegro/5.2.8";
    allegroInfo.cmakeTargets = {"allegro", "allegro_main", "allegro_primitives"};
    allegroInfo.requiresSpecialSetup = true;
    allegroInfo.setupFunction = setupAllegro;
    s_frameworkRegistry[Framework::Allegro] = allegroInfo;

    // Graphics Libraries

    // GLFW Framework
    FrameworkInfo glfwInfo;
    glfwInfo.framework = Framework::GLFW;
    glfwInfo.name = "GLFW";
    glfwInfo.description = "Multi-platform library for OpenGL, OpenGL ES and Vulkan";
    glfwInfo.version = "3.3.8";
    glfwInfo.packageNames[PackageManager::vcpkg] = "glfw3";
    glfwInfo.packageNames[PackageManager::Conan] = "glfw/3.3.8";
    glfwInfo.cmakeTargets = {"glfw"};
    glfwInfo.requiresSpecialSetup = true;
    glfwInfo.setupFunction = setupGLFW;
    s_frameworkRegistry[Framework::GLFW] = glfwInfo;

    // OpenGL Framework
    FrameworkInfo openglInfo;
    openglInfo.framework = Framework::OpenGL;
    openglInfo.name = "OpenGL";
    openglInfo.description = "Cross-platform graphics API";
    openglInfo.version = "4.6";
    openglInfo.packageNames[PackageManager::vcpkg] = "opengl";
    openglInfo.packageNames[PackageManager::Conan] = "opengl/system";
    openglInfo.cmakeTargets = {"OpenGL::GL"};
    openglInfo.requiresSpecialSetup = true;
    openglInfo.setupFunction = setupOpenGL;
    s_frameworkRegistry[Framework::OpenGL] = openglInfo;

    // Vulkan Framework
    FrameworkInfo vulkanInfo;
    vulkanInfo.framework = Framework::Vulkan;
    vulkanInfo.name = "Vulkan";
    vulkanInfo.description = "Low-overhead, cross-platform 3D graphics and computing API";
    vulkanInfo.version = "1.3.0";
    vulkanInfo.packageNames[PackageManager::vcpkg] = "vulkan";
    vulkanInfo.packageNames[PackageManager::Conan] = "vulkan-loader/1.3.0";
    vulkanInfo.cmakeTargets = {"Vulkan::Vulkan"};
    vulkanInfo.requiresSpecialSetup = true;
    vulkanInfo.setupFunction = setupVulkan;
    s_frameworkRegistry[Framework::Vulkan] = vulkanInfo;
}

bool FrameworkIntegration::isFrameworkAvailable(Framework framework, PackageManager packageManager) {
    initializeFrameworkRegistry();

    auto it = s_frameworkRegistry.find(framework);
    if (it == s_frameworkRegistry.end()) {
        return false;
    }

    const auto& info = it->second;
    auto pmIt = info.packageNames.find(packageManager);
    return pmIt != info.packageNames.end();
}

bool FrameworkIntegration::installFramework(Framework framework, const std::filesystem::path& projectPath, PackageManager packageManager) {
    try {
        initializeFrameworkRegistry();

        auto it = s_frameworkRegistry.find(framework);
        if (it == s_frameworkRegistry.end()) {
            spdlog::error("Unknown framework: {}", static_cast<int>(framework));
            return false;
        }

        const auto& info = it->second;
        auto pmIt = info.packageNames.find(packageManager);
        if (pmIt == info.packageNames.end()) {
            spdlog::error("Framework {} not available for package manager {}", info.name, static_cast<int>(packageManager));
            return false;
        }

        spdlog::info("Installing framework: {} using {}", info.name, packageManagerToString(packageManager));

        // Install based on package manager
        switch (packageManager) {
            case PackageManager::vcpkg:
                return setupVcpkg(projectPath, {framework});
            case PackageManager::Conan:
                return setupConan(projectPath, {framework});
            case PackageManager::CPM:
                return setupCPM(projectPath, {framework});
            default:
                spdlog::error("Unsupported package manager");
                return false;
        }

    } catch (const std::exception& e) {
        spdlog::error("Error installing framework: {}", e.what());
        return false;
    }
}

bool FrameworkIntegration::configureFramework(Framework framework, const std::filesystem::path& projectPath) {
    try {
        initializeFrameworkRegistry();

        auto it = s_frameworkRegistry.find(framework);
        if (it == s_frameworkRegistry.end()) {
            spdlog::error("Unknown framework: {}", static_cast<int>(framework));
            return false;
        }

        const auto& info = it->second;

        if (info.requiresSpecialSetup && info.setupFunction) {
            return info.setupFunction(projectPath);
        }

        return true;

    } catch (const std::exception& e) {
        spdlog::error("Error configuring framework: {}", e.what());
        return false;
    }
}

std::optional<FrameworkIntegration::FrameworkInfo> FrameworkIntegration::getFrameworkInfo(Framework framework) {
    initializeFrameworkRegistry();

    auto it = s_frameworkRegistry.find(framework);
    if (it != s_frameworkRegistry.end()) {
        return it->second;
    }

    return std::nullopt;
}

std::vector<FrameworkIntegration::Framework> FrameworkIntegration::listAvailableFrameworks() {
    initializeFrameworkRegistry();

    std::vector<Framework> frameworks;
    for (const auto& [framework, info] : s_frameworkRegistry) {
        frameworks.push_back(framework);
    }

    return frameworks;
}

std::string FrameworkIntegration::generateCMakeConfig(const std::vector<Framework>& frameworks) {
    initializeFrameworkRegistry();

    std::ostringstream cmake;
    cmake << "# Framework configuration generated by CPP-Scaffold\n\n";

    // Find package commands
    for (const auto& framework : frameworks) {
        auto it = s_frameworkRegistry.find(framework);
        if (it != s_frameworkRegistry.end()) {
            const auto& info = it->second;

            if (framework == Framework::Qt) {
                cmake << "find_package(Qt6 REQUIRED COMPONENTS Core Widgets Gui)\n";
            } else if (framework == Framework::SFML) {
                cmake << "find_package(sfml REQUIRED)\n";
            } else if (framework == Framework::Boost) {
                cmake << "find_package(Boost REQUIRED COMPONENTS system filesystem thread)\n";
            } else if (framework == Framework::GoogleTest) {
                cmake << "find_package(GTest REQUIRED)\n";
            } else if (framework == Framework::OpenCV) {
                cmake << "find_package(OpenCV REQUIRED)\n";
            } else {
                cmake << "find_package(" << info.name << " REQUIRED)\n";
            }
        }
    }

    cmake << "\n";
    return cmake.str();
}

std::string FrameworkIntegration::generateTargetLinkLibraries(const std::vector<Framework>& frameworks, const std::string& targetName) {
    initializeFrameworkRegistry();

    std::ostringstream cmake;
    cmake << "target_link_libraries(" << targetName << "\n";

    for (const auto& framework : frameworks) {
        auto it = s_frameworkRegistry.find(framework);
        if (it != s_frameworkRegistry.end()) {
            const auto& info = it->second;
            for (const auto& target : info.cmakeTargets) {
                cmake << "    " << target << "\n";
            }
        }
    }

    cmake << ")\n";
    return cmake.str();
}

// Framework-specific setup functions
bool FrameworkIntegration::setupQt(const std::filesystem::path& projectPath) {
    spdlog::info("Setting up Qt framework");

    // Create a basic Qt application template
    std::string mainCpp = R"(#include <QApplication>
#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QWidget window;
    window.setWindowTitle("Qt Application");
    window.resize(400, 300);

    QVBoxLayout *layout = new QVBoxLayout(&window);

    QLabel *label = new QLabel("Hello, Qt!");
    layout->addWidget(label);

    QPushButton *button = new QPushButton("Click me!");
    layout->addWidget(button);

    QObject::connect(button, &QPushButton::clicked, [&]() {
        label->setText("Button clicked!");
    });

    window.show();

    return app.exec();
}
)";

    std::string mainPath = (projectPath / "main.cpp").string();
    return FileUtils::writeToFile(mainPath, mainCpp);
}

bool FrameworkIntegration::setupSFML(const std::filesystem::path& projectPath) {
    spdlog::info("Setting up SFML framework");

    std::string mainCpp = R"(#include <SFML/Graphics.hpp>

int main()
{
    sf::RenderWindow window(sf::VideoMode(800, 600), "SFML Application");

    sf::CircleShape shape(100.f);
    shape.setFillColor(sf::Color::Green);
    shape.setPosition(350.f, 250.f);

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear();
        window.draw(shape);
        window.display();
    }

    return 0;
}
)";

    std::string mainPath = (projectPath / "main.cpp").string();
    return FileUtils::writeToFile(mainPath, mainCpp);
}

bool FrameworkIntegration::setupBoost(const std::filesystem::path& projectPath) {
    spdlog::info("Setting up Boost libraries");

    std::string mainCpp = R"(#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <iostream>
#include <vector>
#include <string>

int main()
{
    // Boost.Filesystem example
    boost::filesystem::path currentPath = boost::filesystem::current_path();
    std::cout << "Current path: " << currentPath << std::endl;

    // Boost.Algorithm example
    std::string text = "Hello, Boost World!";
    std::vector<std::string> words;
    boost::split(words, text, boost::is_any_of(" "));

    std::cout << "Words: ";
    for (const auto& word : words) {
        std::cout << word << " ";
    }
    std::cout << std::endl;

    return 0;
}
)";

    std::string mainPath = (projectPath / "main.cpp").string();
    return FileUtils::writeToFile(mainPath, mainCpp);
}

bool FrameworkIntegration::setupGoogleTest(const std::filesystem::path& projectPath) {
    spdlog::info("Setting up Google Test framework");

    // Create test directory
    std::filesystem::path testDir = projectPath / "tests";
    std::filesystem::create_directories(testDir);

    std::string testCpp = R"(#include <gtest/gtest.h>

// Example test
TEST(ExampleTest, BasicAssertions) {
    EXPECT_EQ(7 * 6, 42);
    EXPECT_TRUE(true);
    EXPECT_STREQ("hello", "hello");
}

// Another test
TEST(ExampleTest, StringOperations) {
    std::string str = "Hello, World!";
    EXPECT_EQ(str.length(), 13);
    EXPECT_TRUE(str.find("World") != std::string::npos);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
)";

    std::string testPath = (testDir / "test_main.cpp").string();
    return FileUtils::writeToFile(testPath, testCpp);
}

bool FrameworkIntegration::setupOpenCV(const std::filesystem::path& projectPath) {
    (void)projectPath; // TODO: Implement OpenCV-specific setup
    spdlog::info("Setting up OpenCV framework");
    return true; // Basic setup, no special files needed
}

bool FrameworkIntegration::setupImGui(const std::filesystem::path& projectPath) {
    (void)projectPath; // TODO: Implement ImGui-specific setup
    spdlog::info("Setting up Dear ImGui framework");
    return true; // Basic setup, no special files needed
}

bool FrameworkIntegration::setupOpenGL(const std::filesystem::path& projectPath) {
    (void)projectPath; // TODO: Implement OpenGL-specific setup
    spdlog::info("Setting up OpenGL framework");
    return true; // Basic setup, no special files needed
}

// Package manager setup functions
bool FrameworkIntegration::setupVcpkg(const std::filesystem::path& projectPath, const std::vector<Framework>& frameworks) {
    spdlog::info("Setting up vcpkg package manager");

    std::ostringstream vcpkgJson;
    vcpkgJson << "{\n";
    vcpkgJson << "  \"name\": \"" << projectPath.filename().string() << "\",\n";
    vcpkgJson << "  \"version\": \"1.0.0\",\n";
    vcpkgJson << "  \"dependencies\": [\n";

    bool first = true;
    for (const auto& framework : frameworks) {
        auto it = s_frameworkRegistry.find(framework);
        if (it != s_frameworkRegistry.end()) {
            const auto& info = it->second;
            auto pmIt = info.packageNames.find(PackageManager::vcpkg);
            if (pmIt != info.packageNames.end()) {
                if (!first) vcpkgJson << ",\n";
                vcpkgJson << "    \"" << pmIt->second << "\"";
                first = false;
            }
        }
    }

    vcpkgJson << "\n  ]\n";
    vcpkgJson << "}\n";

    std::string vcpkgPath = (projectPath / "vcpkg.json").string();
    return FileUtils::writeToFile(vcpkgPath, vcpkgJson.str());
}

bool FrameworkIntegration::setupConan(const std::filesystem::path& projectPath, const std::vector<Framework>& frameworks) {
    spdlog::info("Setting up Conan package manager");

    std::ostringstream conanfile;
    conanfile << "[requires]\n";

    for (const auto& framework : frameworks) {
        auto it = s_frameworkRegistry.find(framework);
        if (it != s_frameworkRegistry.end()) {
            const auto& info = it->second;
            auto pmIt = info.packageNames.find(PackageManager::Conan);
            if (pmIt != info.packageNames.end()) {
                conanfile << pmIt->second << "\n";
            }
        }
    }

    conanfile << "\n[generators]\n";
    conanfile << "CMakeDeps\n";
    conanfile << "CMakeToolchain\n";

    std::string conanPath = (projectPath / "conanfile.txt").string();
    return FileUtils::writeToFile(conanPath, conanfile.str());
}

bool FrameworkIntegration::setupCPM(const std::filesystem::path& projectPath, const std::vector<Framework>& frameworks) {
    (void)projectPath; // TODO: Implement CPM-specific setup
    (void)frameworks;  // TODO: Use frameworks to configure CPM
    spdlog::info("Setting up CPM package manager");
    // CPM setup would be integrated into CMakeLists.txt
    return true;
}

// Utility functions
std::string FrameworkIntegration::frameworkToString(Framework framework) {
    initializeFrameworkRegistry();

    auto it = s_frameworkRegistry.find(framework);
    if (it != s_frameworkRegistry.end()) {
        return it->second.name;
    }

    return "Unknown";
}

std::string FrameworkIntegration::packageManagerToString(PackageManager pm) {
    switch (pm) {
        case PackageManager::vcpkg: return "vcpkg";
        case PackageManager::Conan: return "Conan";
        case PackageManager::Hunter: return "Hunter";
        case PackageManager::CPM: return "CPM";
        case PackageManager::FetchContent: return "FetchContent";
        case PackageManager::System: return "System";
        default: return "Unknown";
    }
}

// Additional GUI framework setup functions
bool FrameworkIntegration::setupGTK(const std::filesystem::path& projectPath) {
    try {
        spdlog::info("Setting up GTK framework for project: {}", projectPath.string());

        // Create GTK-specific directories
        auto includeDir = projectPath / "include";
        auto srcDir = projectPath / "src";

        if (!std::filesystem::exists(includeDir)) {
            std::filesystem::create_directories(includeDir);
        }

        // Create basic GTK application template
        std::string gtkMain = R"(#include <gtk/gtk.h>

static void activate(GtkApplication* app, gpointer user_data) {
    GtkWidget *window;
    GtkWidget *button;
    GtkWidget *box;

    window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "GTK Application");
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 300);

    box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_halign(box, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(box, GTK_ALIGN_CENTER);
    gtk_window_set_child(GTK_WINDOW(window), box);

    button = gtk_button_new_with_label("Hello GTK!");
    g_signal_connect_swapped(button, "clicked", G_CALLBACK(gtk_window_destroy), window);
    gtk_box_append(GTK_BOX(box), button);

    gtk_widget_show(window);
}

int main(int argc, char **argv) {
    GtkApplication *app;
    int status;

    app = gtk_application_new("org.example.GtkApplication", G_APPLICATION_FLAGS_NONE);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);

    return status;
}
)";

        auto mainPath = srcDir / "main.cpp";
        if (!FileUtils::writeToFile(mainPath.string(), gtkMain)) {
            spdlog::error("Failed to create GTK main.cpp");
            return false;
        }

        spdlog::info("GTK framework setup completed");
        return true;

    } catch (const std::exception& e) {
        spdlog::error("Error setting up GTK framework: {}", e.what());
        return false;
    }
}

bool FrameworkIntegration::setupFLTK(const std::filesystem::path& projectPath) {
    try {
        spdlog::info("Setting up FLTK framework for project: {}", projectPath.string());

        auto srcDir = projectPath / "src";

        // Create basic FLTK application template
        std::string fltkMain = R"(#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Button.H>
#include <FL/fl_ask.H>

void button_callback(Fl_Widget* widget, void* data) {
    fl_message("Hello from FLTK!");
}

int main(int argc, char **argv) {
    Fl_Window *window = new Fl_Window(400, 300, "FLTK Application");

    Fl_Button *button = new Fl_Button(150, 125, 100, 50, "Click Me!");
    button->callback(button_callback);

    window->end();
    window->show(argc, argv);

    return Fl::run();
}
)";

        auto mainPath = srcDir / "main.cpp";
        if (!FileUtils::writeToFile(mainPath.string(), fltkMain)) {
            spdlog::error("Failed to create FLTK main.cpp");
            return false;
        }

        spdlog::info("FLTK framework setup completed");
        return true;

    } catch (const std::exception& e) {
        spdlog::error("Error setting up FLTK framework: {}", e.what());
        return false;
    }
}

bool FrameworkIntegration::setupWxWidgets(const std::filesystem::path& projectPath) {
    try {
        spdlog::info("Setting up wxWidgets framework for project: {}", projectPath.string());

        auto srcDir = projectPath / "src";
        auto includeDir = projectPath / "include";

        if (!std::filesystem::exists(includeDir)) {
            std::filesystem::create_directories(includeDir);
        }

        // Create wxWidgets application header
        std::string wxHeader = R"(#pragma once
#include <wx/wx.h>

class MyApp : public wxApp {
public:
    virtual bool OnInit();
};

class MyFrame : public wxFrame {
public:
    MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size);

private:
    void OnHello(wxCommandEvent& event);
    void OnExit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    void OnClose(wxCloseEvent& event);

    wxDECLARE_EVENT_TABLE();
};

enum {
    ID_Hello = 1
};
)";

        auto headerPath = includeDir / "app.h";
        if (!FileUtils::writeToFile(headerPath.string(), wxHeader)) {
            spdlog::error("Failed to create wxWidgets app.h");
            return false;
        }

        // Create wxWidgets application implementation
        std::string wxMain = R"(#include "app.h"

wxIMPLEMENT_APP(MyApp);

bool MyApp::OnInit() {
    MyFrame *frame = new MyFrame("wxWidgets Application", wxPoint(50, 50), wxSize(450, 340));
    frame->Show(true);
    return true;
}

wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(ID_Hello,   MyFrame::OnHello)
    EVT_MENU(wxID_EXIT,  MyFrame::OnExit)
    EVT_MENU(wxID_ABOUT, MyFrame::OnAbout)
    EVT_CLOSE(MyFrame::OnClose)
wxEND_EVENT_TABLE()

MyFrame::MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
        : wxFrame(NULL, wxID_ANY, title, pos, size) {
    wxMenu *menuFile = new wxMenu;
    menuFile->Append(ID_Hello, "&Hello...\tCtrl-H", "Help string shown in status bar for this menu item");
    menuFile->AppendSeparator();
    menuFile->Append(wxID_EXIT);

    wxMenu *menuHelp = new wxMenu;
    menuHelp->Append(wxID_ABOUT);

    wxMenuBar *menuBar = new wxMenuBar;
    menuBar->Append(menuFile, "&File");
    menuBar->Append(menuHelp, "&Help");

    SetMenuBar(menuBar);

    CreateStatusBar();
    SetStatusText("Welcome to wxWidgets!");
}

void MyFrame::OnExit(wxCommandEvent& event) {
    Close(true);
}

void MyFrame::OnAbout(wxCommandEvent& event) {
    wxMessageBox("This is a wxWidgets Hello World sample", "About Hello World", wxOK | wxICON_INFORMATION);
}

void MyFrame::OnHello(wxCommandEvent& event) {
    wxLogMessage("Hello world from wxWidgets!");
}

void MyFrame::OnClose(wxCloseEvent& event) {
    Destroy();
}
)";

        auto mainPath = srcDir / "main.cpp";
        if (!FileUtils::writeToFile(mainPath.string(), wxMain)) {
            spdlog::error("Failed to create wxWidgets main.cpp");
            return false;
        }

        spdlog::info("wxWidgets framework setup completed");
        return true;

    } catch (const std::exception& e) {
        spdlog::error("Error setting up wxWidgets framework: {}", e.what());
        return false;
    }
}

// Game development framework setup functions
bool FrameworkIntegration::setupSDL2(const std::filesystem::path& projectPath) {
    try {
        spdlog::info("Setting up SDL2 framework for project: {}", projectPath.string());

        auto srcDir = projectPath / "src";

        // Create basic SDL2 application template
        std::string sdl2Main = R"(#include <SDL.h>
#include <iostream>

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

int main(int argc, char* args[]) {
    SDL_Window* window = nullptr;
    SDL_Surface* screenSurface = nullptr;

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    window = SDL_CreateWindow("SDL2 Application",
                             SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                             SCREEN_WIDTH, SCREEN_HEIGHT,
                             SDL_WINDOW_SHOWN);

    if (window == nullptr) {
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    screenSurface = SDL_GetWindowSurface(window);
    SDL_FillRect(screenSurface, nullptr, SDL_MapRGB(screenSurface->format, 0x00, 0x80, 0xFF));
    SDL_UpdateWindowSurface(window);

    // Main loop
    bool quit = false;
    SDL_Event e;

    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }
        }

        // Render here
        SDL_Delay(16); // ~60 FPS
    }

    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
)";

        auto mainPath = srcDir / "main.cpp";
        if (!FileUtils::writeToFile(mainPath.string(), sdl2Main)) {
            spdlog::error("Failed to create SDL2 main.cpp");
            return false;
        }

        spdlog::info("SDL2 framework setup completed");
        return true;

    } catch (const std::exception& e) {
        spdlog::error("Error setting up SDL2 framework: {}", e.what());
        return false;
    }
}

bool FrameworkIntegration::setupAllegro(const std::filesystem::path& projectPath) {
    try {
        spdlog::info("Setting up Allegro framework for project: {}", projectPath.string());

        auto srcDir = projectPath / "src";

        // Create basic Allegro application template
        std::string allegroMain = R"(#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <iostream>

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

int main() {
    if (!al_init()) {
        std::cerr << "Failed to initialize Allegro!" << std::endl;
        return 1;
    }

    if (!al_init_primitives_addon()) {
        std::cerr << "Failed to initialize primitives addon!" << std::endl;
        return 1;
    }

    if (!al_install_keyboard()) {
        std::cerr << "Failed to install keyboard!" << std::endl;
        return 1;
    }

    ALLEGRO_DISPLAY* display = al_create_display(SCREEN_WIDTH, SCREEN_HEIGHT);
    if (!display) {
        std::cerr << "Failed to create display!" << std::endl;
        return 1;
    }

    ALLEGRO_EVENT_QUEUE* event_queue = al_create_event_queue();
    if (!event_queue) {
        std::cerr << "Failed to create event queue!" << std::endl;
        al_destroy_display(display);
        return 1;
    }

    al_register_event_source(event_queue, al_get_display_event_source(display));
    al_register_event_source(event_queue, al_get_keyboard_event_source());

    al_set_window_title(display, "Allegro Application");

    bool running = true;
    while (running) {
        ALLEGRO_EVENT event;
        al_wait_for_event(event_queue, &event);

        if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            running = false;
        } else if (event.type == ALLEGRO_EVENT_KEY_DOWN) {
            if (event.keyboard.keycode == ALLEGRO_KEY_ESCAPE) {
                running = false;
            }
        }

        // Clear screen
        al_clear_to_color(al_map_rgb(0, 128, 255));

        // Draw a circle
        al_draw_filled_circle(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 50, al_map_rgb(255, 255, 255));

        al_flip_display();
    }

    al_destroy_event_queue(event_queue);
    al_destroy_display(display);

    return 0;
}
)";

        auto mainPath = srcDir / "main.cpp";
        if (!FileUtils::writeToFile(mainPath.string(), allegroMain)) {
            spdlog::error("Failed to create Allegro main.cpp");
            return false;
        }

        spdlog::info("Allegro framework setup completed");
        return true;

    } catch (const std::exception& e) {
        spdlog::error("Error setting up Allegro framework: {}", e.what());
        return false;
    }
}

// Graphics library setup functions
bool FrameworkIntegration::setupGLFW(const std::filesystem::path& projectPath) {
    try {
        spdlog::info("Setting up GLFW framework for project: {}", projectPath.string());

        auto srcDir = projectPath / "src";

        // Create basic GLFW + OpenGL application template
        std::string glfwMain = R"(#include <GLFW/glfw3.h>
#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}

int main() {
    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    // Configure GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create window
    GLFWwindow* window = glfwCreateWindow(800, 600, "GLFW Application", nullptr, nullptr);
    if (window == nullptr) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // Render loop
    while (!glfwWindowShouldClose(window)) {
        // Input
        processInput(window);

        // Render
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Swap buffers and poll events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
)";

        auto mainPath = srcDir / "main.cpp";
        if (!FileUtils::writeToFile(mainPath.string(), glfwMain)) {
            spdlog::error("Failed to create GLFW main.cpp");
            return false;
        }

        spdlog::info("GLFW framework setup completed");
        return true;

    } catch (const std::exception& e) {
        spdlog::error("Error setting up GLFW framework: {}", e.what());
        return false;
    }
}

bool FrameworkIntegration::setupVulkan(const std::filesystem::path& projectPath) {
    try {
        spdlog::info("Setting up Vulkan framework for project: {}", projectPath.string());

        auto srcDir = projectPath / "src";
        auto includeDir = projectPath / "include";

        if (!std::filesystem::exists(includeDir)) {
            std::filesystem::create_directories(includeDir);
        }

        // Create basic Vulkan application template
        std::string vulkanMain = R"(#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
#include <iostream>
#include <stdexcept>
#include <vector>

class VulkanApplication {
public:
    void run() {
        initWindow();
        initVulkan();
        mainLoop();
        cleanup();
    }

private:
    GLFWwindow* window;
    VkInstance instance;

    void initWindow() {
        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

        window = glfwCreateWindow(800, 600, "Vulkan Application", nullptr, nullptr);
    }

    void initVulkan() {
        createInstance();
    }

    void createInstance() {
        VkApplicationInfo appInfo{};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "Vulkan Application";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "No Engine";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_0;

        VkInstanceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;

        uint32_t glfwExtensionCount = 0;
        const char** glfwExtensions;
        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        createInfo.enabledExtensionCount = glfwExtensionCount;
        createInfo.ppEnabledExtensionNames = glfwExtensions;
        createInfo.enabledLayerCount = 0;

        if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create Vulkan instance!");
        }
    }

    void mainLoop() {
        while (!glfwWindowShouldClose(window)) {
            glfwPollEvents();
        }
    }

    void cleanup() {
        vkDestroyInstance(instance, nullptr);
        glfwDestroyWindow(window);
        glfwTerminate();
    }
};

int main() {
    VulkanApplication app;

    try {
        app.run();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
)";

        auto mainPath = srcDir / "main.cpp";
        if (!FileUtils::writeToFile(mainPath.string(), vulkanMain)) {
            spdlog::error("Failed to create Vulkan main.cpp");
            return false;
        }

        spdlog::info("Vulkan framework setup completed");
        return true;

    } catch (const std::exception& e) {
        spdlog::error("Error setting up Vulkan framework: {}", e.what());
        return false;
    }
}

} // namespace utils

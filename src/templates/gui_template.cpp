#include "gui_template.h"
#include "../utils/file_utils.h"
#include "../utils/string_utils.h"
#include <fmt/format.h>
#include <iostream>

using namespace utils;
using namespace cli_enums;

GuiTemplate::GuiTemplate(const CliOptions &options) : TemplateBase(options) {
  // 确定GUI框架类型 (默认为Qt)
  guiFramework_ = "qt";

  // 优先使用命令行指定的GUI框架
  if (!options.guiFrameworks.empty()) {
    guiFramework_ = options.guiFrameworks[0]; // 使用第一个指定的框架
  } else {
    // 检查是否有框架指定在项目名称中
    std::string projectName = StringUtils::toLower(options.projectName);
    if (projectName.find("qt") != std::string::npos) {
      guiFramework_ = "qt";
    } else if (projectName.find("wx") != std::string::npos ||
               projectName.find("wxwidgets") != std::string::npos) {
      guiFramework_ = "wxwidgets";
    } else if (projectName.find("gtk") != std::string::npos) {
      guiFramework_ = "gtk";
    } else if (projectName.find("fltk") != std::string::npos) {
      guiFramework_ = "fltk";
    } else if (projectName.find("imgui") != std::string::npos) {
      guiFramework_ = "imgui";
    }
  }

  std::cout << "🎨 选择的GUI框架: " << guiFramework_ << std::endl;
}

bool GuiTemplate::create() {
  std::string projectPath = options_.projectName;

  // 检查项目目录是否已存在
  if (FileUtils::directoryExists(projectPath)) {
    std::cout << "错误: 目录 '" << projectPath << "' 已存在。\n";
    return false;
  }

  std::cout << "🚀 正在创建GUI项目 (" << guiFramework_ << ")...\n";

  // 创建基本结构
  if (!createProjectStructure()) {
    std::cout << "创建项目结构失败\n";
    return false;
  }
  std::cout << "�?项目结构已创建\n";

  // 创建GUI特定文件
  if (!createGuiSpecificFiles()) {
    std::cout << "创建GUI特定文件失败\n";
    return false;
  }
  std::cout << "�?GUI特定文件已创建\n";

  // 创建构建系统
  if (!createBuildSystem()) {
    std::cout << "配置构建系统失败\n";
    return false;
  }
  std::cout << "�?构建系统已配置\n";

  // 设置包管理器
  if (!setupPackageManager()) {
    std::cout << "设置包管理器失败\n";
    return false;
  }
  std::cout << "�?包管理器已设置\n";

  // 设置测试框架
  if (options_.includeTests) {
    if (!setupTestFramework()) {
      std::cout << "设置测试框架失败\n";
      return false;
    }
    std::cout << "�?测试框架已配置\n";
  }

  // 设置文档
  if (options_.includeDocumentation) {
    if (!setupDocumentation()) {
      std::cout << "设置文档失败\n";
      return false;
    }
    std::cout << "�?文档已配置\n";
  }

  // 初始化Git
  if (options_.initGit) {
    if (!initializeGit(projectPath)) {
      std::cout << "初始化Git仓库失败\n";
      return false;
    }
    std::cout << "�?已初始化Git仓库\n";
  }

  std::cout << "\n你的GUI项目已准备就绪！\n\n";

  // 打印使用说明
  std::cout << "cd " << options_.projectName << "\n";

  if (to_string(options_.buildSystem) == "cmake") {
    std::cout << "mkdir build && cd build\n";
    std::cout << "cmake ..\n";
    std::cout << "make\n";
  } else if (to_string(options_.buildSystem) == "meson") {
    std::cout << "meson setup build\n";
    std::cout << "cd build\n";
    std::cout << "meson compile\n";
  } else if (to_string(options_.buildSystem) == "bazel") {
    std::cout << "bazel build //...\n";
  }

  std::cout << "\n祝编码愉�? 🎉\n";

  return true;
}

bool GuiTemplate::createProjectStructure() {
  std::string projectPath = options_.projectName;

  // 创建主目�?
  if (!FileUtils::createDirectory(projectPath)) {
    return false;
  }

  // 创建src目录
  std::string srcPath = FileUtils::combinePath(projectPath, "src");
  if (!FileUtils::createDirectory(srcPath)) {
    return false;
  }

  // 创建include目录
  std::string includePath = FileUtils::combinePath(projectPath, "include");
  if (!FileUtils::createDirectory(includePath)) {
    return false;
  }

  std::string includeProjectPath =
      FileUtils::combinePath(includePath, options_.projectName);
  if (!FileUtils::createDirectory(includeProjectPath)) {
    return false;
  }

  // 创建资源目录
  std::string resourcesPath = FileUtils::combinePath(projectPath, "resources");
  if (!FileUtils::createDirectory(resourcesPath)) {
    return false;
  }

  // 创建UI目录 (对于界面设计文件)
  if (guiFramework_ == "qt" || guiFramework_ == "gtk") {
    std::string uiPath = FileUtils::combinePath(projectPath, "ui");
    if (!FileUtils::createDirectory(uiPath)) {
      return false;
    }
  }

  // 写入README.md
  if (!FileUtils::writeToFile(FileUtils::combinePath(projectPath, "README.md"),
                              getReadmeContent())) {
    return false;
  }

  return true;
}

bool GuiTemplate::createGuiSpecificFiles() {
  std::string projectPath = options_.projectName;
  std::string srcPath = FileUtils::combinePath(projectPath, "src");
  std::string includePath = FileUtils::combinePath(projectPath, "include");
  std::string includeProjectPath =
      FileUtils::combinePath(includePath, options_.projectName);

  // 创建主要源文�?
  if (!FileUtils::writeToFile(FileUtils::combinePath(srcPath, "main.cpp"),
                              getMainCppContent())) {
    return false;
  }

  // 根据不同的GUI框架创建不同的文�?
  if (guiFramework_ == "qt") {
    // Qt特定文件
    if (!FileUtils::writeToFile(
            FileUtils::combinePath(includeProjectPath, "main_window.h"),
            getMainWindowHeaderContent())) {
      return false;
    }

    if (!FileUtils::writeToFile(
            FileUtils::combinePath(srcPath, "main_window.cpp"),
            getMainWindowCppContent())) {
      return false;
    }

    if (!FileUtils::writeToFile(
            FileUtils::combinePath(includeProjectPath, "application.h"),
            getAppHeaderContent())) {
      return false;
    }

    if (!FileUtils::writeToFile(
            FileUtils::combinePath(srcPath, "application.cpp"),
            getAppCppContent())) {
      return false;
    }
  } else if (guiFramework_ == "gtk" || guiFramework_ == "fltk" ||
             guiFramework_ == "wxwidgets" || guiFramework_ == "imgui") {
    // 对于其他框架，创建简化的结构
    if (!FileUtils::writeToFile(
            FileUtils::combinePath(includeProjectPath, "app.h"),
            getFrameworkSpecificHeaderContent())) {
      return false;
    }

    if (!FileUtils::writeToFile(
            FileUtils::combinePath(srcPath, "app.cpp"),
            getFrameworkSpecificSourceContent())) {
      return false;
    }
  }

  // 创建日志工具�?
  if (!FileUtils::writeToFile(
          FileUtils::combinePath(includeProjectPath, "logging.h"),
          getLoggingHeaderContent())) {
    return false;
  }

  if (!FileUtils::writeToFile(FileUtils::combinePath(srcPath, "logging.cpp"),
                              getLoggingCppContent())) {
    return false;
  }

  // 为不同框架创建特定文�?
  if (guiFramework_ == "qt") {
    std::string uiPath = FileUtils::combinePath(projectPath, "ui");
    if (!FileUtils::writeToFile(
            FileUtils::combinePath(uiPath, "main_window.ui"),
            getQtUiContent())) {
      return false;
    }

    std::string resourcesPath =
        FileUtils::combinePath(projectPath, "resources");
    if (!FileUtils::writeToFile(
            FileUtils::combinePath(resourcesPath, "resources.qrc"),
            getQtResourceContent())) {
      return false;
    }
  } else if (guiFramework_ == "wxwidgets") {
    std::string resourcesPath =
        FileUtils::combinePath(projectPath, "resources");
    if (!FileUtils::writeToFile(
            FileUtils::combinePath(resourcesPath, "resource.rc"),
            getWxResourceContent())) {
      return false;
    }
  } else if (guiFramework_ == "gtk") {
    std::string uiPath = FileUtils::combinePath(projectPath, "ui");
    if (!FileUtils::writeToFile(
            FileUtils::combinePath(uiPath, "main_window.glade"),
            getGtkGladeContent())) {
      return false;
    }
  }

  return true;
}

bool GuiTemplate::createBuildSystem() {
  std::string projectPath = options_.projectName;

  if (to_string(options_.buildSystem) == "cmake") {
    // 创建CMakeLists.txt
    if (!FileUtils::writeToFile(
            FileUtils::combinePath(projectPath, "CMakeLists.txt"),
            getCMakeContent())) {
      return false;
    }
  } else if (to_string(options_.buildSystem) == "meson") {
    // 创建meson.build
    if (!FileUtils::writeToFile(
            FileUtils::combinePath(projectPath, "meson.build"),
            getMesonContent())) {
      return false;
    }
  } else if (to_string(options_.buildSystem) == "bazel") {
    // 创建WORKSPACE和BUILD文件
    if (!FileUtils::writeToFile(
            FileUtils::combinePath(projectPath, "WORKSPACE"),
            "workspace(name = \"" + options_.projectName + "\")\n")) {
      return false;
    }

    if (!FileUtils::writeToFile(FileUtils::combinePath(projectPath, "BUILD"),
                                getBazelContent())) {
      return false;
    }
  }

  return true;
}

bool GuiTemplate::setupPackageManager() {
  std::string projectPath = options_.projectName;

  if (to_string(options_.packageManager) == "vcpkg") {
    // 创建vcpkg.json
    if (!FileUtils::writeToFile(
            FileUtils::combinePath(projectPath, "vcpkg.json"),
            getVcpkgJsonContent())) {
      return false;
    }
  } else if (to_string(options_.packageManager) == "conan") {
    // 创建conanfile.txt
    if (!FileUtils::writeToFile(
            FileUtils::combinePath(projectPath, "conanfile.txt"),
            getConanfileContent())) {
      return false;
    }
  }

  return true;
}

bool GuiTemplate::setupTestFramework() {
  if (!options_.includeTests) {
    return true;
  }

  std::string projectPath = options_.projectName;
  std::string testsPath = FileUtils::combinePath(projectPath, "tests");

  if (!FileUtils::createDirectory(testsPath)) {
    return false;
  }

  std::string testContent;
  if (to_string(options_.testFramework) == "gtest") {
    testContent = getGTestContent();
  } else if (to_string(options_.testFramework) == "catch2") {
    testContent = getCatch2Content();
  } else if (to_string(options_.testFramework) == "doctest") {
    testContent = getDocTestContent();
  }

  if (!FileUtils::writeToFile(
          FileUtils::combinePath(testsPath, "test_main.cpp"), testContent)) {
    return false;
  }

  // 更新构建系统以包含测�?
  if (to_string(options_.buildSystem) == "cmake") {
    std::string cmakePath =
        FileUtils::combinePath(projectPath, "CMakeLists.txt");
    std::string cmakeContent = FileUtils::readFromFile(cmakePath);

    // 添加测试配置
    cmakeContent += R"(
# Tests
if(BUILD_TESTING)
    enable_testing()
    add_subdirectory(tests)
endif()
)";

    if (!FileUtils::writeToFile(cmakePath, cmakeContent)) {
      return false;
    }

    // 创建tests/CMakeLists.txt
    std::string testCmakeContent;
    if (to_string(options_.testFramework) == "gtest") {
      testCmakeContent = R"(
find_package(GTest REQUIRED)
add_executable(${PROJECT_NAME}_tests
    test_main.cpp
)
target_link_libraries(${PROJECT_NAME}_tests PRIVATE
    ${PROJECT_NAME}_lib
    GTest::GTest
    GTest::Main
    spdlog::spdlog
)
add_test(NAME ${PROJECT_NAME}_tests COMMAND ${PROJECT_NAME}_tests)
)";
    } else if (to_string(options_.testFramework) == "catch2") {
      testCmakeContent = R"(
find_package(Catch2 REQUIRED)
add_executable(${PROJECT_NAME}_tests
    test_main.cpp
)
target_link_libraries(${PROJECT_NAME}_tests PRIVATE
    ${PROJECT_NAME}_lib
    Catch2::Catch2
    spdlog::spdlog
)
add_test(NAME ${PROJECT_NAME}_tests COMMAND ${PROJECT_NAME}_tests)
)";
    } else if (to_string(options_.testFramework) == "doctest") {
      testCmakeContent = R"(
find_package(doctest REQUIRED)
add_executable(${PROJECT_NAME}_tests
    test_main.cpp
)
target_link_libraries(${PROJECT_NAME}_tests PRIVATE
    ${PROJECT_NAME}_lib
    doctest::doctest
    spdlog::spdlog
)
add_test(NAME ${PROJECT_NAME}_tests COMMAND ${PROJECT_NAME}_tests)
)";
    }

    if (!FileUtils::writeToFile(
            FileUtils::combinePath(testsPath, "CMakeLists.txt"),
            testCmakeContent)) {
      return false;
    }
  }

  return true;
}

std::string GuiTemplate::getMainCppContent() {
  if (guiFramework_ == "qt") {
    return R"(#include <QApplication>
#include ")" +
           options_.projectName + R"(/logging.h"

int main(int argc, char* argv[]) {
    // 初始化日志系�?
    )" + options_.projectName +
           R"(::Logging::init("logs/app.log");

    // 创建应用实例
    QApplication qtApp(argc, argv);
    )" + options_.projectName +
           R"(::Application app;

    // 显示主窗�?
    if (!app.initialize()) {
        SPDLOG_ERROR("应用初始化失�?);
        return 1;
    }

    // 运行应用主循�?
    SPDLOG_INFO("应用启动成功");
    int result = qtApp.exec();
    SPDLOG_INFO("应用退出，返回�? {}", result);

    return result;
}
)";
  } else if (guiFramework_ == "wxwidgets") {
    return R"(#include <wx/wx.h>
#include ")" +
           options_.projectName + R"(/logging.h"

// 注册wx应用
wxIMPLEMENT_APP_NO_MAIN()" +
           options_.projectName + R"(::Application);

int main(int argc, char* argv[]) {
    // 初始化日志系�?
    )" + options_.projectName +
           R"(::Logging::init("logs/app.log");
    SPDLOG_INFO("应用启动�?);

    // 运行wxWidgets应用
    wxEntryStart(argc, argv);
    int result = wxEntry(argc, argv);
    wxEntryCleanup();

    SPDLOG_INFO("应用退出，返回�? {}", result);
    return result;
}
)";
  } else if (guiFramework_ == "gtk") {
    return R"(#include <gtk/gtk.h>
#include ")" +
           options_.projectName + R"(/logging.h"

int main(int argc, char* argv[]) {
    // 初始化日志系�?
    )" + options_.projectName +
           R"(::Logging::init("logs/app.log");
    SPDLOG_INFO("应用启动�?);

    // 初始化GTK
    gtk_init(&argc, &argv);

    // 创建应用实例
    )" + options_.projectName +
           R"(::Application app;

    if (!app.initialize()) {
        SPDLOG_ERROR("应用初始化失�?);
        return 1;
    }

    // 运行GTK主循�?
    SPDLOG_INFO("应用启动成功");
    int result = app.run();
    SPDLOG_INFO("应用退出，返回�? {}", result);

    return result;
}
)";
  }

  // 默认返回Qt实现
  return R"(#include <iostream>
#include ")" +
         options_.projectName + R"(/logging.h"

int main(int argc, char* argv[]) {
    // 初始化日志系�?
    )" + options_.projectName +
         R"(::Logging::init("logs/app.log");
    SPDLOG_INFO("应用启动�?);

    // 创建应用实例
    )" + options_.projectName +
         R"(::Application app;
    if (!app.initialize()) {
        SPDLOG_ERROR("应用初始化失�?);
        return 1;
    }

    // 运行应用主循�?
    SPDLOG_INFO("应用启动成功");
    int result = app.run();
    SPDLOG_INFO("应用退出，返回�? {}", result);

    return result;
}
)";
}

std::string GuiTemplate::getMainWindowHeaderContent() {
  std::string headerGuard =
      StringUtils::toUpper(options_.projectName) + "_MAIN_WINDOW_H";

  if (guiFramework_ == "qt") {
    return R"(#pragma once
#ifndef )" +
           headerGuard + R"(
#define )" +
           headerGuard + R"(

#include <QMainWindow>
#include <memory>
#include <spdlog/spdlog.h>

// 前向声明
namespace Ui {
class MainWindow;
}

namespace )" +
           options_.projectName + R"( {

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

    // 初始化窗�?
    bool initialize();

private slots:
    // 槽函�?
    void onActionExit();
    void onActionAbout();

private:
    // UI设计器生成的UI�?
    std::unique_ptr<Ui::MainWindow> ui;

    // 初始化菜�?
    void setupMenus();

    // 初始化状态栏
    void setupStatusBar();

    // 连接信号和槽
    void connectSignals();
};

} // namespace )" +
           options_.projectName + R"(

#endif // )" +
           headerGuard + R"(
)";
  } else if (guiFramework_ == "wxwidgets") {
    return R"(#pragma once
#ifndef )" +
           headerGuard + R"(
#define )" +
           headerGuard + R"(

#include <spdlog/spdlog.h>
#include <wx/frame.h>
#include <wx/wx.h>

namespace )" +
           options_.projectName + R"( {

// 窗口标识�?
enum {
    ID_MAIN_WINDOW = wxID_HIGHEST + 1,
    ID_MENU_EXIT,
    ID_MENU_ABOUT
};

class MainWindow : public wxFrame {
public:
    MainWindow(const wxString& title, const wxPoint& pos, const wxSize& size);
    ~MainWindow() override;

    // 初始化窗�?
    bool initialize();

private:
    // UI控件
    wxMenuBar* menuBar{nullptr};
    wxStatusBar* statusBar{nullptr};
    wxPanel* mainPanel{nullptr};

    // 初始化UI
    void setupMenus();
    void setupStatusBar();
    void setupControls();

    // 事件处理
    void onExit(wxCommandEvent& event);
    void onAbout(wxCommandEvent& event);

    // 事件�?
    wxDECLARE_EVENT_TABLE();
};

} // namespace )" +
           options_.projectName + R"(

#endif // )" +
           headerGuard + R"(
)";
  } else if (guiFramework_ == "gtk") {
    return R"(#pragma once
#ifndef )" +
           headerGuard + R"(
#define )" +
           headerGuard + R"(

#include <gtk/gtk.h>
#include <spdlog/spdlog.h>
#include <string>

namespace )" +
           options_.projectName + R"( {

class MainWindow {
public:
    MainWindow();
    ~MainWindow();

    // 初始化窗�?
    bool initialize();

    // 显示窗口
    void show();

    // 获取GTK窗口控件
    GtkWidget* getWidget() const { return window; }

private:
    // GTK控件
    GtkWidget* window{nullptr};
    GtkWidget* mainBox{nullptr};
    GtkWidget* menuBar{nullptr};
    GtkWidget* statusBar{nullptr};

    // UI文件路径
    std::string uiFilePath;

    // 初始化UI
    void setupUI();
    void setupMenus();
    void setupStatusBar();
    void setupSignals();

    // 回调函数
    static void onExit(GtkWidget* widget, gpointer data);
    static void onAbout(GtkWidget* widget, gpointer data);
};

} // namespace )" +
           options_.projectName + R"(

#endif // )" +
           headerGuard + R"(
)";
  }

  // 默认返回一个通用实现
  return R"(#pragma once
#ifndef )" +
         headerGuard + R"(
#define )" +
         headerGuard + R"(

#include <spdlog/spdlog.h>
#include <string>

namespace )" +
         options_.projectName + R"( {

class MainWindow {
public:
    MainWindow();
    ~MainWindow();

    // 初始化窗�?
    bool initialize();

    // 显示窗口
    void show();

    // 窗口是否可见
    bool isVisible() const;

private:
    // 窗口是否已初始化
    bool initialized{false};

    // 窗口是否可见
    bool visible{false};

    // 初始化UI
    void setupUI();
};

} // namespace )" +
         options_.projectName + R"(

#endif // )" +
         headerGuard + R"(
)";
}

std::string GuiTemplate::getMainWindowCppContent() {
  if (guiFramework_ == "qt") {
    return R"(#include ")" + options_.projectName + R"(/main_window.h"
#include "ui_main_window.h"
#include <QCloseEvent>
#include <QMessageBox>

namespace )" +
           options_.projectName + R"( {

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    SPDLOG_DEBUG("MainWindow构造函�?);
}

MainWindow::~MainWindow()
{
    SPDLOG_DEBUG("MainWindow析构函数");
}

bool MainWindow::initialize()
{
    SPDLOG_DEBUG("初始化MainWindow");

    // 设置UI
    ui->setupUi(this);

    // 设置窗口标题
    setWindowTitle(tr(")" +
           options_.projectName + R"("));

    // 初始化菜�?
    setupMenus();

    // 初始化状态栏
    setupStatusBar();

    // 连接信号和槽
    connectSignals();

    SPDLOG_INFO("MainWindow初始化完�?);
    return true;
}

void MainWindow::setupMenus()
{
    SPDLOG_DEBUG("设置菜单");

    // 连接退出操�?
    connect(ui->actionExit, &QAction::triggered, this, &MainWindow::onActionExit);

    // 连接关于操作
    connect(ui->actionAbout, &QAction::triggered, this, &MainWindow::onActionAbout);
}

void MainWindow::setupStatusBar()
{
    SPDLOG_DEBUG("设置状态栏");
    statusBar()->showMessage(tr("就绪"));
}

void MainWindow::connectSignals()
{
    SPDLOG_DEBUG("连接信号和槽");
}

void MainWindow::onActionExit()
{
    SPDLOG_DEBUG("触发退出操�?);
    close();
}

void MainWindow::onActionAbout()
{
    SPDLOG_DEBUG("触发关于操作");
    QMessageBox::about(this, tr("关于"),
                       tr(")" +
           options_.projectName + R"( v1.0\n\n"
                          "一个使用Qt框架的GUI应用�?));
}

} // namespace )" +
           options_.projectName + R"(
)";
  } else if (guiFramework_ == "wxwidgets") {
    return R"(#include ")" + options_.projectName + R"(/main_window.h"
#include <wx/aboutdlg.h>
#include <wx/stattext.h>

namespace )" +
           options_.projectName + R"( {

// 事件表定�?
wxBEGIN_EVENT_TABLE(MainWindow, wxFrame)
    EVT_MENU(ID_MENU_EXIT, MainWindow::onExit)
    EVT_MENU(ID_MENU_ABOUT, MainWindow::onAbout)
wxEND_EVENT_TABLE()

MainWindow::MainWindow(const wxString& title, const wxPoint& pos, const wxSize& size)
    : wxFrame(nullptr, ID_MAIN_WINDOW, title, pos, size)
{
    SPDLOG_DEBUG("MainWindow构造函�?);
}

MainWindow::~MainWindow()
{
    SPDLOG_DEBUG("MainWindow析构函数");
}

bool MainWindow::initialize()
{
    SPDLOG_DEBUG("初始化MainWindow");

    // 设置图标
    // SetIcon(wxIcon("APPICON"));

    // 创建UI元素
    setupMenus();
    setupStatusBar();
    setupControls();

    SPDLOG_INFO("MainWindow初始化完�?);
    return true;
}

void MainWindow::setupMenus()
{
    SPDLOG_DEBUG("设置菜单");

    // 创建文件菜单
    wxMenu *menuFile = new wxMenu;
    menuFile->Append(ID_MENU_EXIT, "退出\tAlt+F4", "退出应用程�?);

    // 创建帮助菜单
    wxMenu *menuHelp = new wxMenu;
    menuHelp->Append(ID_MENU_ABOUT, "关于...\tF1", "显示关于对话�?);

    // 创建菜单�?
    menuBar = new wxMenuBar;
    menuBar->Append(menuFile, "文件");
    menuBar->Append(menuHelp, "帮助");

    // 设置菜单�?
    SetMenuBar(menuBar);
}

void MainWindow::setupStatusBar()
{
    SPDLOG_DEBUG("设置状态栏");

    // 创建状态栏
    statusBar = CreateStatusBar(1);
    statusBar->SetStatusText("就绪");
}

void MainWindow::setupControls()
{
    SPDLOG_DEBUG("设置控件");

    // 创建主面�?
    mainPanel = new wxPanel(this, wxID_ANY);

    // 创建一个简单的布局
    wxBoxSizer *sizer = new wxBoxSizer(wxVERTICAL);

    // 添加一个文本标�?
    sizer->Add(
        new wxStaticText(mainPanel, wxID_ANY, "欢迎使用 )" +
           options_.projectName + R"("),
        0, wxALL | wxALIGN_CENTER_HORIZONTAL, 20
    );

    // 设置面板布局
    mainPanel->SetSizer(sizer);
    mainPanel->Layout();
}

void MainWindow::onExit(wxCommandEvent& event)
{
    SPDLOG_DEBUG("触发退出操�?);
    Close(true);
}

void MainWindow::onAbout(wxCommandEvent& event)
{
    SPDLOG_DEBUG("触发关于操作");

    wxAboutDialogInfo aboutInfo;
    aboutInfo.SetName(")" +
           options_.projectName + R"(");
    aboutInfo.SetVersion("1.0");
    aboutInfo.SetDescription("一个使用wxWidgets框架的GUI应用");
    aboutInfo.SetCopyright("(C) 2025");

    wxAboutBox(aboutInfo);
}

} // namespace )" +
           options_.projectName + R"(
)";
  } else if (guiFramework_ == "gtk") {
    return R"(#include ")" + options_.projectName + R"(/main_window.h"
#include <iostream>

namespace )" +
           options_.projectName + R"( {

MainWindow::MainWindow()
    : uiFilePath("ui/main_window.glade")
{
    SPDLOG_DEBUG("MainWindow构造函�?);
}

MainWindow::~MainWindow()
{
    SPDLOG_DEBUG("MainWindow析构函数");
    if (window) {
        gtk_widget_destroy(window);
    }
}

bool MainWindow::initialize()
{
    SPDLOG_DEBUG("初始化MainWindow");

    // 从Glade文件加载界面
    GtkBuilder* builder = gtk_builder_new();

    if (gtk_builder_add_from_file(builder, uiFilePath.c_str(), nullptr) == 0) {
        SPDLOG_ERROR("无法加载UI文件: {}", uiFilePath);
        g_object_unref(builder);
        return false;
    }

    // 获取主窗口控�?
    window = GTK_WIDGET(gtk_builder_get_object(builder, "main_window"));
    if (!window) {
        SPDLOG_ERROR("无法获取主窗口控�?);
        g_object_unref(builder);
        return false;
    }

    // 获取其他控件
    menuBar = GTK_WIDGET(gtk_builder_get_object(builder, "menubar"));
    statusBar = GTK_WIDGET(gtk_builder_get_object(builder, "statusbar"));

    // 设置信号
    g_signal_connect(gtk_builder_get_object(builder, "menu_exit"), "activate",
                    G_CALLBACK(onExit), this);
    g_signal_connect(gtk_builder_get_object(builder, "menu_about"), "activate",
                    G_CALLBACK(onAbout), this);

    // 窗口关闭时退出应�?
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), nullptr);

    // 显示所有控�?
    gtk_widget_show_all(window);

    // 释放构建�?
    g_object_unref(builder);

    SPDLOG_INFO("MainWindow初始化完�?);
    return true;
}

void MainWindow::show()
{
    SPDLOG_DEBUG("显示MainWindow");
    if (window) {
        gtk_widget_show(window);
    }
}

void MainWindow::setupUI()
{
    SPDLOG_DEBUG("设置UI");
}

void MainWindow::setupMenus()
{
    SPDLOG_DEBUG("设置菜单");
}

void MainWindow::setupStatusBar()
{
    SPDLOG_DEBUG("设置状态栏");
}

void MainWindow::setupSignals()
{
    SPDLOG_DEBUG("设置信号");
}

void MainWindow::onExit(GtkWidget* widget, gpointer data)
{
    SPDLOG_DEBUG("触发退出操�?);
    gtk_main_quit();
}

void MainWindow::onAbout(GtkWidget* widget, gpointer data)
{
    SPDLOG_DEBUG("触发关于操作");

    GtkWidget* dialog = gtk_about_dialog_new();
    GtkAboutDialog* about_dialog = GTK_ABOUT_DIALOG(dialog);

    gtk_about_dialog_set_program_name(about_dialog, ")" +
           options_.projectName + R"(");
    gtk_about_dialog_set_version(about_dialog, "1.0");
    gtk_about_dialog_set_copyright(about_dialog, "(C) 2025");
    gtk_about_dialog_set_comments(about_dialog, "一个使用GTK框架的GUI应用");

    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

} // namespace )" +
           options_.projectName + R"(
)";
  }

  // 默认返回一个通用�?
  return R"(#include ")" + options_.projectName + R"(/main_window.h"

namespace )" +
         options_.projectName + R"( {

MainWindow::MainWindow()
{
    SPDLOG_DEBUG("MainWindow构造函�?);
}

MainWindow::~MainWindow()
{
    SPDLOG_DEBUG("MainWindow析构函数");
}

bool MainWindow::initialize()
{
    SPDLOG_DEBUG("初始化MainWindow");
    initialized = true;
    SPDLOG_INFO("MainWindow初始化完�?);
    return true;
}

void MainWindow::show()
{
    SPDLOG_DEBUG("显示MainWindow");
    if (initialized) {
        visible = true;
        SPDLOG_INFO("MainWindow现在可见");
    } else {
        SPDLOG_ERROR("尝试显示未初始化的窗�?);
    }
}

bool MainWindow::isVisible() const
{
    return visible;
}

void MainWindow::setupUI()
{
    SPDLOG_DEBUG("设置UI");
}

} // namespace )" +
         options_.projectName + R"(
)";
}

std::string GuiTemplate::getAppHeaderContent() {
  std::string headerGuard =
      StringUtils::toUpper(options_.projectName) + "_APPLICATION_H";

  if (guiFramework_ == "qt") {
    return R"(#pragma once
#ifndef )" +
           headerGuard + R"(
#define )" +
           headerGuard + R"(

#include "main_window.h"
#include <QObject>
#include <memory>
#include <spdlog/spdlog.h>

namespace )" +
           options_.projectName + R"( {

class Application : public QObject {
    Q_OBJECT

public:
    Application();
    ~Application() override;

    // 初始化应用程�?
    bool initialize();

    // 获取主窗�?
    MainWindow* mainWindow() { return mainWindow_.get(); }

private:
    // 主窗�?
    std::unique_ptr<MainWindow> mainWindow_;

    // 加载应用程序配置
    bool loadSettings();

    // 保存应用程序配置
    bool saveSettings();
};

} // namespace )" +
           options_.projectName + R"(

#endif // )" +
           headerGuard + R"(
)";
  } else if (guiFramework_ == "wxwidgets") {
    return R"(#pragma once
#ifndef )" +
           headerGuard + R"(
#define )" +
           headerGuard + R"(

#include "main_window.h"
#include <memory>
#include <spdlog/spdlog.h>
#include <wx/wx.h>

namespace )" +
           options_.projectName + R"( {

class Application : public wxApp {
public:
    Application();
    virtual ~Application();

    // wxWidgets应用初始�?
    bool OnInit() override;

    // wxWidgets应用退�?
    int OnExit() override;

    // 获取主窗�?
    MainWindow* getMainWindow() { return mainWindow_; }

private:
    // 主窗�?
    MainWindow* mainWindow_{nullptr};

    // 加载应用程序配置
    bool loadSettings();

    // 保存应用程序配置
    bool saveSettings();
};

} // namespace )" +
           options_.projectName + R"(

#endif // )" +
           headerGuard + R"(
)";
  } else if (guiFramework_ == "gtk") {
    return R"(#pragma once
#ifndef )" +
           headerGuard + R"(
#define )" +
           headerGuard + R"(

#include "main_window.h"
#include <memory>
#include <spdlog/spdlog.h>

namespace )" +
           options_.projectName + R"( {

class Application {
public:
    Application();
    ~Application();

    // 初始化应用程�?
    bool initialize();

    // 运行应用程序
    int run();

    // 获取主窗�?
    MainWindow* getMainWindow() { return mainWindow_.get(); }

private:
    // 主窗�?
    std::unique_ptr<MainWindow> mainWindow_;

    // 加载应用程序配置
    bool loadSettings();

    // 保存应用程序配置
    bool saveSettings();
};

} // namespace )" +
           options_.projectName + R"(

#endif // )" +
           headerGuard + R"(
)";
  }

  // 默认返回通用�?
  return R"(#pragma once
#ifndef )" +
         headerGuard + R"(
#define )" +
         headerGuard + R"(

#include "main_window.h"
#include <memory>
#include <spdlog/spdlog.h>

namespace )" +
         options_.projectName + R"( {

class Application {
public:
    Application();
    ~Application();

    // 初始化应用程�?
    bool initialize();

    // 运行应用程序主循�?
    int run();

    // 获取主窗�?
    MainWindow* getMainWindow() { return mainWindow_.get(); }

private:
    // 主窗�?
    std::unique_ptr<MainWindow> mainWindow_;

    // 应用程序是否正在运行
    bool running{false};

    // 加载应用程序配置
    bool loadSettings();

    // 保存应用程序配置
    bool saveSettings();
};

} // namespace )" +
         options_.projectName + R"(

#endif // )" +
         headerGuard + R"(
)";
}

std::string GuiTemplate::getAppCppContent() {
  if (guiFramework_ == "qt") {
    return R"(#include ")" + options_.projectName + R"(/application.h"

namespace )" +
           options_.projectName + R"( {

Application::Application()
    : QObject(nullptr)
{
    SPDLOG_DEBUG("Application构造函�?);
}

Application::~Application()
{
    SPDLOG_DEBUG("Application析构函数");
    saveSettings();
}

bool Application::initialize()
{
    SPDLOG_INFO("初始化应用程�?);

    // 加载设置
    if (!loadSettings()) {
        SPDLOG_WARN("无法加载应用程序设置");
    }

    // 创建并初始化主窗�?
    mainWindow_ = std::make_unique<MainWindow>();
    if (!mainWindow_->initialize()) {
        SPDLOG_ERROR("初始化主窗口失败");
        return false;
    }

    // 显示主窗�?
    mainWindow_->show();

    SPDLOG_INFO("应用程序初始化完�?);
    return true;
}

bool Application::loadSettings()
{
    SPDLOG_DEBUG("加载应用程序设置");
    // TODO: 实现设置加载逻辑
    return true;
}

bool Application::saveSettings()
{
    SPDLOG_DEBUG("保存应用程序设置");
    // TODO: 实现设置保存逻辑
    return true;
}

} // namespace )" +
           options_.projectName + R"(
)";
  } else if (guiFramework_ == "wxwidgets") {
    return R"(#include ")" + options_.projectName + R"(/application.h"

namespace )" +
           options_.projectName + R"( {

Application::Application()
{
    SPDLOG_DEBUG("Application构造函�?);
}

Application::~Application()
{
    SPDLOG_DEBUG("Application析构函数");
    saveSettings();
}

bool Application::OnInit()
{
    SPDLOG_INFO("初始化应用程�?);

    // 设置应用程序名称
    SetAppName(")" +
           options_.projectName + R"(");

    // 加载设置
    if (!loadSettings()) {
        SPDLOG_WARN("无法加载应用程序设置");
    }

    // 创建并初始化主窗�?
    mainWindow_ = new MainWindow(")" +
           options_.projectName + R"(", wxPoint(50, 50), wxSize(800, 600));
    if (!mainWindow_->initialize()) {
        SPDLOG_ERROR("初始化主窗口失败");
        return false;
    }

    // 显示主窗�?
    mainWindow_->Show(true);

    SPDLOG_INFO("应用程序初始化完�?);
    return true;
}

int Application::OnExit()
{
    SPDLOG_INFO("应用程序退�?);

    // 保存设置
    saveSettings();

    return wxApp::OnExit();
}

bool Application::loadSettings()
{
    SPDLOG_DEBUG("加载应用程序设置");
    // TODO: 实现设置加载逻辑
    return true;
}

bool Application::saveSettings()
{
    SPDLOG_DEBUG("保存应用程序设置");
    // TODO: 实现设置保存逻辑
    return true;
}

} // namespace )" +
           options_.projectName + R"(
)";
  } else if (guiFramework_ == "gtk") {
    return R"(#include ")" + options_.projectName + R"(/application.h"

namespace )" +
           options_.projectName + R"( {

Application::Application()
{
    SPDLOG_DEBUG("Application构造函�?);
}

Application::~Application()
{
    SPDLOG_DEBUG("Application析构函数");
    saveSettings();
}

bool Application::initialize()
{
    SPDLOG_INFO("初始化应用程�?);

    // 加载设置
    if (!loadSettings()) {
        SPDLOG_WARN("无法加载应用程序设置");
    }

    // 创建并初始化主窗�?
    mainWindow_ = std::make_unique<MainWindow>();
    if (!mainWindow_->initialize()) {
        SPDLOG_ERROR("初始化主窗口失败");
        return false;
    }

    SPDLOG_INFO("应用程序初始化完�?);
    return true;
}

int Application::run()
{
    SPDLOG_INFO("运行应用程序");

    // 运行GTK主循�?
    gtk_main();

    return 0;
}

bool Application::loadSettings()
{
    SPDLOG_DEBUG("加载应用程序设置");
    // TODO: 实现设置加载逻辑
    return true;
}

bool Application::saveSettings()
{
    SPDLOG_DEBUG("保存应用程序设置");
    // TODO: 实现设置保存逻辑
    return true;
}

} // namespace )" +
           options_.projectName + R"(
)";
  }

  // 默认返回通用�?
  return R"(#include ")" + options_.projectName + R"(/application.h"
#include <chrono>
#include <thread>

namespace )" +
         options_.projectName + R"( {

Application::Application()
{
    SPDLOG_DEBUG("Application构造函�?);
}

Application::~Application()
{
    SPDLOG_DEBUG("Application析构函数");
    saveSettings();
}

bool Application::initialize()
{
    SPDLOG_INFO("初始化应用程�?);

    // 加载设置
    if (!loadSettings()) {
        SPDLOG_WARN("无法加载应用程序设置");
    }

    // 创建并初始化主窗�?
    mainWindow_ = std::make_unique<MainWindow>();
    if (!mainWindow_->initialize()) {
        SPDLOG_ERROR("初始化主窗口失败");
        return false;
    }

    SPDLOG_INFO("应用程序初始化完�?);
    return true;
}

int Application::run()
{
    SPDLOG_INFO("运行应用程序");

    if (!mainWindow_) {
        SPDLOG_ERROR("主窗口未初始�?);
        return 1;
    }

    // 显示主窗�?
    mainWindow_->show();

    // 模拟事件循环
    running = true;

    while (running) {
        // 处理事件...

        // 模拟事件循环的时间片
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        // 在实际应用中，这里会有一个真正的事件循环
        // 对于本示例，我们只是模拟几秒钟然后退�?
        static int counter = 0;
        if (++counter > 50) { // 运行�?�?
            running = false;
        }
    }

    SPDLOG_INFO("应用程序主循环结�?);
    return 0;
}

bool Application::loadSettings()
{
    SPDLOG_DEBUG("加载应用程序设置");
    // TODO: 实现设置加载逻辑
    return true;
}

bool Application::saveSettings()
{
    SPDLOG_DEBUG("保存应用程序设置");
    // TODO: 实现设置保存逻辑
    return true;
}

} // namespace )" +
         options_.projectName + R"(
)";
}

std::string GuiTemplate::getLoggingHeaderContent() {
  std::string headerGuard =
      StringUtils::toUpper(options_.projectName) + "_LOGGING_H";

  return R"(#pragma once
#ifndef )" +
         headerGuard + R"(
#define )" +
         headerGuard + R"(

#include <memory>
#include <spdlog/spdlog.h>
#include <string>

// 定义使用spdlog的宏，方便在项目中调�?
#define SPDLOG_TRACE(...) SPDLOG_LOGGER_TRACE(spdlog::default_logger_raw(), __VA_ARGS__)
#define SPDLOG_DEBUG(...) SPDLOG_LOGGER_DEBUG(spdlog::default_logger_raw(), __VA_ARGS__)
#define SPDLOG_INFO(...) SPDLOG_LOGGER_INFO(spdlog::default_logger_raw(), __VA_ARGS__)
#define SPDLOG_WARN(...) SPDLOG_LOGGER_WARN(spdlog::default_logger_raw(), __VA_ARGS__)
#define SPDLOG_ERROR(...) SPDLOG_LOGGER_ERROR(spdlog::default_logger_raw(), __VA_ARGS__)
#define SPDLOG_CRITICAL(...) SPDLOG_LOGGER_CRITICAL(spdlog::default_logger_raw(), __VA_ARGS__)

namespace )" +
         options_.projectName + R"( {

class Logging {
public:
    // 初始化日志系�?
    static bool init(const std::string& logFilePath,
                   spdlog::level::level_enum level = spdlog::level::info);

    // 关闭日志系统
    static void shutdown();

    // 设置日志级别
    static void setLevel(spdlog::level::level_enum level);

    // 获取当前日志级别
    static spdlog::level::level_enum getLevel();

    // 获取格式化时间戳
    static std::string getFormattedTimestamp();

private:
    // 是否已初始化
    static bool initialized_;

    // 日志文件路径
    static std::string logFilePath_;

    // 当前日志级别
    static spdlog::level::level_enum level_;

    // 创建日志目录
    static bool createLogDirectory(const std::string& path);
};

} // namespace )" +
         options_.projectName + R"(

#endif // )" +
         headerGuard + R"(
)";
}

std::string GuiTemplate::getLoggingCppContent() {
  return R"(#include ")" + options_.projectName + R"(/logging.h"
#include <chrono>
#include <filesystem>
#include <iomanip>
#include <spdlog/sinks/daily_file_sink.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <sstream>

namespace )" +
         options_.projectName + R"( {

// 静态成员初始化
bool Logging::initialized_ = false;
std::string Logging::logFilePath_ = "logs/app.log";
spdlog::level::level_enum Logging::level_ = spdlog::level::info;

bool Logging::init(const std::string& logFilePath, spdlog::level::level_enum level) {
    if (initialized_) {
        return true; // 已经初始化过
    }

    try {
        logFilePath_ = logFilePath;
        level_ = level;

        // 创建日志目录
        if (!createLogDirectory(logFilePath_)) {
            // 如果创建目录失败，回退到控制台日志
            auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
            auto logger = std::make_shared<spdlog::logger>("console_logger", console_sink);
            logger->set_level(level_);
            spdlog::set_default_logger(logger);

            spdlog::warn("无法创建日志目录，仅使用控制台输�?);
            initialized_ = true;
            return false;
        }

        // 创建一个旋转文件日志，最�?MB，保�?个备�?
        auto rotating_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
            logFilePath_, 5 * 1024 * 1024, 3);

        // 同时输出到控制台
        auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();

        // 创建带有两个接收器的记录�?
        std::vector<spdlog::sink_ptr> sinks {rotating_sink, console_sink};
        auto logger = std::make_shared<spdlog::logger>("multi_sink", sinks.begin(), sinks.end());

        // 设置记录格式
        logger->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] [%t] %v");

        // 设置日志级别
        logger->set_level(level_);

        // 设置为默认记录器
        spdlog::set_default_logger(logger);

        // 记录初始化信�?
        spdlog::info("日志系统初始化成功，级别: {}, 路径: {}",
                   spdlog::level::to_string_view(level_), logFilePath_);

        initialized_ = true;
        return true;
    }
    catch (const spdlog::spdlog_ex& ex) {
        std::cerr << "日志初始化失�? " << ex.what() << std::endl;
        return false;
    }
}

void Logging::shutdown() {
    if (initialized_) {
        spdlog::info("关闭日志系统");
        spdlog::shutdown();
        initialized_ = false;
    }
}

void Logging::setLevel(spdlog::level::level_enum level) {
    level_ = level;
    if (initialized_) {
        spdlog::set_level(level_);
        spdlog::info("日志级别设置�? {}", spdlog::level::to_string_view(level_));
    }
}

spdlog::level::level_enum Logging::getLevel() {
    return level_;
}

std::string Logging::getFormattedTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;

    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    ss << '.' << std::setfill('0') << std::setw(3) << ms.count();

    return ss.str();
}

bool Logging::createLogDirectory(const std::string& path) {
    try {
        std::filesystem::path p(path);
        auto dir = p.parent_path();

        if (!dir.empty() && !std::filesystem::exists(dir)) {
            return std::filesystem::create_directories(dir);
        }
        return true;
    }
    catch (const std::exception& e) {
        std::cerr << "创建日志目录失败: " << e.what() << std::endl;
        return false;
    }
}

} // namespace )" +
         options_.projectName + R"(
)";
}

std::string GuiTemplate::getReadmeContent() {
  // 确定GUI框架名称
  std::string guiName = guiFramework_ == "qt"          ? "Qt"
                        : guiFramework_ == "wxwidgets" ? "wxWidgets"
                        : guiFramework_ == "gtk"       ? "GTK"
                                                       : "通用UI框架";

  // 确定GUI框架依赖
  std::string guiDependency = guiFramework_ == "qt"          ? "Qt 6.x"
                              : guiFramework_ == "wxwidgets" ? "wxWidgets 3.x"
                              : guiFramework_ == "gtk"       ? "GTK 3.x"
                                                             : "GUI框架库";

  // 包管理器信息
  std::string packageManagerInfo =
      to_string(options_.packageManager) != "none"
          ? fmt::format("- {}包管理\n", to_string(options_.packageManager))
          : "";

  // 测试框架信息
  std::string testFrameworkInfo =
      options_.includeTests
          ? fmt::format("- 集成{}测试框架\n", to_string(options_.testFramework))
          : "";

  // 包管理器依赖
  std::string packageManagerDep =
      to_string(options_.packageManager) != "none"
          ? fmt::format("- {}\n", to_string(options_.packageManager))
          : "";

  // 构建步骤
  std::string buildSteps;
  if (to_string(options_.buildSystem) == "cmake") {
    buildSteps = R"(# 创建构建目录
mkdir build && cd build

# 配置项目
cmake ..

# 编译
make)";
  } else if (to_string(options_.buildSystem) == "meson") {
    buildSteps = R"(# 配置项目
meson setup build

# 编译
cd build
meson compile)";
  } else {
    buildSteps = R"(# 使用Bazel构建
bazel build //...)";
  }

  // 测试部分
  std::string testSection = "";
  if (options_.includeTests) {
    std::string testCmd;
    if (to_string(options_.buildSystem) == "cmake") {
      testCmd = R"(cd build
ctest)";
    } else if (to_string(options_.buildSystem) == "meson") {
      testCmd = R"(cd build
meson test)";
    } else {
      testCmd = R"(bazel test //...)";
    }

    testSection = fmt::format(R"(### 运行测试

```bash
{}
```
)",
                              testCmd);
  }

  // 运行命令
  std::string runCmd;
  if (to_string(options_.buildSystem) == "cmake" || to_string(options_.buildSystem) == "meson") {
    runCmd = fmt::format(R"(cd build
./{})",
                         options_.projectName);
  } else {
    runCmd = fmt::format(R"(bazel run //:{})", options_.projectName);
  }

  // UI部分
  std::string uiSection = "";
  if (guiFramework_ == "qt" || guiFramework_ == "gtk") {
    uiSection = fmt::format(R"(├── ui/                    # UI设计文件
�?  └── main_window.{}  # 主窗口界面设�?
)",
                            (guiFramework_ == "qt" ? "ui" : "glade"));
  }

  // 测试目录
  std::string testsDir = options_.includeTests
                             ?
                             R"(├── tests/                 # 测试目录
�?  └── test_main.cpp       # 测试入口
)"
                             : "";

  // 使用fmt格式化整个README内容
  return fmt::format(R"(# {0}

一个使用{1}开发的C++ GUI应用，由CPP-Scaffold创建�?

## 功能特点

- 现代化的C++17 GUI应用
- 使用{1}作为界面框架
- 集成spdlog日志系统
- {2}构建系统
{3}{4}

## 构建说明

### 依赖�?

- C++17兼容编译�?
- {5}
- spdlog
- {2} 构建系统
{6}

### 编译步骤

```bash
{7}
```

{8}

### 运行应用

```bash
{9}
```

## 项目结构

```
{0}/
├── include/               # 头文件目�?
�?  └── {0}/
�?      ├── application.h  # 应用�?
�?      ├── main_window.h  # 主窗�?
�?      └── logging.h      # 日志工具
├── src/                   # 源文件目�?
�?  ├── main.cpp           # 程序入口�?
�?  ├── application.cpp    # 应用实现
�?  ├── main_window.cpp    # 主窗口实�?
�?  └── logging.cpp        # 日志工具实现
{10}├── resources/             # 资源文件目录
{11}├── README.md              # 项目说明文档
└── CMakeLists.txt          # CMake构建脚本
```

## 使用spdlog日志系统

项目集成了spdlog日志系统，提供了方便的日志宏供项目使用：

```cpp
// 示例用法
SPDLOG_TRACE("这是一个跟踪日�?);
SPDLOG_DEBUG("这是一个调试日�?);
SPDLOG_INFO("这是一个信息日�?);
SPDLOG_WARN("这是一个警告日�?);
SPDLOG_ERROR("这是一个错误日�? {{0}}", error_code);
SPDLOG_CRITICAL("这是一个严重错误日�?);
```

## 许可�?

此项目使用MIT许可�?- 详见LICENSE文件)",
                     options_.projectName, guiName, options_.buildSystem,
                     packageManagerInfo, testFrameworkInfo, guiDependency,
                     packageManagerDep, buildSteps, testSection, runCmd,
                     uiSection, testsDir);
}

std::string GuiTemplate::getCMakeContent() {
  std::string content;

  content = R"(cmake_minimum_required(VERSION 3.14)
project()" + options_.projectName +
            R"( VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Options
option(BUILD_TESTING "Build tests" )" +
            (options_.includeTests ? "ON" : "OFF") + R"()

# Find packages
find_package(spdlog REQUIRED)
)";

  // GUI框架特定配置
  if (guiFramework_ == "qt") {
    content += R"(
# Qt configuration
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 COMPONENTS Core Gui Widgets QUIET)
if (NOT Qt6_FOUND)
    find_package(Qt5 COMPONENTS Core Gui Widgets REQUIRED)
endif()

set(QT_LIBS Qt::Core Qt::Gui Qt::Widgets)
)";
  } else if (guiFramework_ == "wxwidgets") {
    content += R"(
# wxWidgets configuration
find_package(wxWidgets REQUIRED COMPONENTS core base)
include(${wxWidgets_USE_FILE})
)";
  } else if (guiFramework_ == "gtk") {
    content += R"(
# GTK configuration
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)
include_directories(${GTK3_INCLUDE_DIRS})
link_directories(${GTK3_LIBRARY_DIRS})
add_definitions(${GTK3_CFLAGS_OTHER})
)";
  }

  // vcpkg集成
  if (to_string(options_.packageManager) == "vcpkg") {
    content += R"(
# vcpkg integration
if(DEFINED ENV{VCPKG_ROOT})
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
endif()
)";
  }

  // 源文件定�?
  content += R"(
# Source files
set(SOURCES
    src/main.cpp
    src/application.cpp
    src/main_window.cpp
    src/logging.cpp
)

# Include directories
include_directories(include)

# Resources)";

  // 资源文件
  if (guiFramework_ == "qt") {
    content += R"(
set(RESOURCES
    resources/resources.qrc
)

set(UI_FILES
    ui/main_window.ui
))";
  } else if (guiFramework_ == "wxwidgets") {
    content += R"(
set(RESOURCES
    resources/resource.rc
))";
  }

  // 主可执行文件
  content += R"(

# Main executable
add_executable(${PROJECT_NAME} ${SOURCES})";

  if (guiFramework_ == "qt") {
    content += R"( ${RESOURCES} ${UI_FILES})";
  } else if (guiFramework_ == "wxwidgets") {
    content += R"( ${RESOURCES})";
  }

  content += R"(
target_include_directories(${PROJECT_NAME} PRIVATE include)
)";

  // 链接�?
  if (guiFramework_ == "qt") {
    content += R"(
target_link_libraries(${PROJECT_NAME} PRIVATE ${QT_LIBS} spdlog::spdlog)
)";
  } else if (guiFramework_ == "wxwidgets") {
    content += R"(
target_link_libraries(${PROJECT_NAME} PRIVATE ${wxWidgets_LIBRARIES} spdlog::spdlog)
)";
  } else if (guiFramework_ == "gtk") {
    content += R"(
target_link_libraries(${PROJECT_NAME} PRIVATE ${GTK3_LIBRARIES} spdlog::spdlog)
)";
  } else {
    content += R"(
target_link_libraries(${PROJECT_NAME} PRIVATE spdlog::spdlog)
)";
  }

  // 库目�?用于测试)
  content += R"(
# Library target (for reuse in tests)
add_library(${PROJECT_NAME}_lib STATIC ${SOURCES})";

  if (guiFramework_ == "qt") {
    content += R"( ${RESOURCES} ${UI_FILES})";
  } else if (guiFramework_ == "wxwidgets") {
    content += R"( ${RESOURCES})";
  }

  content += R"(
target_include_directories(${PROJECT_NAME}_lib PUBLIC include)
)";

  // 链接库目标的�?
  if (guiFramework_ == "qt") {
    content += R"(
target_link_libraries(${PROJECT_NAME}_lib PUBLIC ${QT_LIBS} spdlog::spdlog)
)";
  } else if (guiFramework_ == "wxwidgets") {
    content += R"(
target_link_libraries(${PROJECT_NAME}_lib PUBLIC ${wxWidgets_LIBRARIES} spdlog::spdlog)
)";
  } else if (guiFramework_ == "gtk") {
    content += R"(
target_link_libraries(${PROJECT_NAME}_lib PUBLIC ${GTK3_LIBRARIES} spdlog::spdlog)
)";
  } else {
    content += R"(
target_link_libraries(${PROJECT_NAME}_lib PUBLIC spdlog::spdlog)
)";
  }

  // 安装目标
  content += R"(
# Installation
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
)
)";

  // 测试配置
  if (options_.includeTests) {
    content += R"(
# Tests
if(BUILD_TESTING)
    enable_testing()
    add_subdirectory(tests)
endif()
)";
  }

  return content;
}

std::string GuiTemplate::getMesonContent() {
  std::string content = R"(project(')" + options_.projectName + R"(', 'cpp',
  version : '0.1.0',
  default_options : ['warning_level=3', 'cpp_std=c++17']
)

# Dependencies
spdlog_dep = dependency('spdlog')
)";

  // GUI框架特定配置
  if (guiFramework_ == "qt") {
    content += R"(
# Qt dependencies
qt5_dep = dependency('qt5', modules : ['Core', 'Gui', 'Widgets'])
qt_deps = [qt5_dep]
)";
  } else if (guiFramework_ == "wxwidgets") {
    content += R"(
# wxWidgets dependencies
wx_dep = dependency('wxwidgets', version : '>=3.0.0')
)";
  } else if (guiFramework_ == "gtk") {
    content += R"(
# GTK dependencies
gtk_dep = dependency('gtk+-3.0', version : '>=3.20')
)";
  }

  // 测试框架依赖
  if (options_.includeTests) {
    if (to_string(options_.testFramework) == "gtest") {
      content += R"(
# Test dependencies
gtest_dep = dependency('gtest', main : true)
test_deps = [gtest_dep]
)";
    } else if (to_string(options_.testFramework) == "catch2") {
      content += R"(
# Test dependencies
catch2_dep = dependency('catch2')
test_deps = [catch2_dep]
)";
    } else if (to_string(options_.testFramework) == "doctest") {
      content += R"(
# Test dependencies
doctest_dep = dependency('doctest')
test_deps = [doctest_dep]
)";
    }
  }

  // 源文�?
  content += R"(
# Source files
src_files = [
  'src/main.cpp',
  'src/application.cpp',
  'src/main_window.cpp',
  'src/logging.cpp',
]

# Include directories
inc_dirs = include_directories('include')

# Dependencies list
dependencies = [spdlog_dep, )";

  if (guiFramework_ == "qt") {
    content += "qt_deps";
  } else if (guiFramework_ == "wxwidgets") {
    content += "wx_dep";
  } else if (guiFramework_ == "gtk") {
    content += "gtk_dep";
  } else {
    content += "[]"; // 空依赖列�?
  }

  content += R"(]

# Main executable
executable(')" +
             options_.projectName + R"(',
  src_files,
  include_directories : inc_dirs,
  dependencies : dependencies,
  install : true
)

# Tests
)";

  if (options_.includeTests) {
    content += R"(test_exe = executable('test_runner',
  ['tests/test_main.cpp'],
  include_directories : inc_dirs,
  dependencies : dependencies + test_deps
)

test(')" + options_.projectName +
               R"(_tests', test_exe)
)";
  } else {
    content += "# No tests configured\n";
  }

  return content;
}

std::string GuiTemplate::getBazelContent() {
  std::string content =
      R"(load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library", "cc_test")

)";

  if (guiFramework_ == "qt") {
    content += R"(# Qt rules - requires qt_rules repository setup in WORKSPACE
load("@qt_rules//:qt.bzl", "qt_cc_library", "qt_resource", "qt_ui_library")

qt_ui_library(
    name = "ui_main_window",
    ui = "ui/main_window.ui",
)

qt_resource(
    name = "resources",
    resource_files = ["resources/resources.qrc"],
)
)";
  }

  // 主库
  content += R"(
cc_library(
    name = ")" +
             options_.projectName + R"(_lib",
    srcs = [
        "src/application.cpp",
        "src/main_window.cpp",
        "src/logging.cpp",
    ],
    hdrs = glob(["include//*.h"]),
    includes = ["include"],
    deps = [
        "@spdlog",)";

  if (guiFramework_ == "qt") {
    content += R"(
        ":resources",
        ":ui_main_window",
        "@qt//:qt_widgets",)";
  } else if (guiFramework_ == "wxwidgets") {
    content += R"(
        "@wxwidgets//:wx",)";
  } else if (guiFramework_ == "gtk") {
    content += R"(
        "@gtk//:gtk",)";
  }

  content += R"(
    ],
    visibility = ["//visibility:public"],
)

# Main executable
cc_binary(
    name = ")" +
             options_.projectName + R"(",
    srcs = ["src/main.cpp"],
    deps = [":)" +
             options_.projectName + R"(_lib"],
)
)";

  // 测试
  if (options_.includeTests) {
    content += R"(
# Tests
cc_test(
    name = ")" +
               options_.projectName + R"(_test",
    srcs = ["tests/test_main.cpp"],
    deps = [
        ":)" + options_.projectName +
               R"(_lib",
        "@)" +
               (to_string(options_.testFramework) == "gtest"
                    ? "com_google_googletest//:gtest_main"
                : to_string(options_.testFramework) == "catch2" ? "catch2//:catch2"
                                                     : "doctest//:doctest") +
               R"(",
    ],
)
)";
  }

  return content;
}

std::string GuiTemplate::getVcpkgJsonContent() {
  // 创建依赖项列表，从spdlog开�?
  std::vector<std::string> dependencies = {
      R"({
        "name": "spdlog"
      })"};

  // 根据GUI框架添加相应依赖
  if (guiFramework_ == "qt") {
    dependencies.push_back(R"({
        "name": "qtbase"
      })");
  } else if (guiFramework_ == "wxwidgets") {
    dependencies.push_back(R"({
        "name": "wxwidgets"
      })");
  } else if (guiFramework_ == "gtk") {
    dependencies.push_back(R"({
        "name": "gtk"
      })");
  }

  // 根据测试配置添加测试框架依赖
  if (options_.includeTests) {
    std::string testFrameworkName;
    if (to_string(options_.testFramework) == "gtest") {
      testFrameworkName = "gtest";
    } else if (to_string(options_.testFramework) == "catch2") {
      testFrameworkName = "catch2";
    } else {
      testFrameworkName = "doctest";
    }
    dependencies.push_back(fmt::format(R"({{
        "name": "{}"
      }})",
                                       testFrameworkName));
  }

  // 构建依赖项字符串，每个依赖项之间用逗号和换行连�?
  std::string dependenciesStr;
  for (size_t i = 0; i < dependencies.size(); ++i) {
    dependenciesStr += dependencies[i];
    if (i < dependencies.size() - 1) {
      dependenciesStr += ",";
    }
    dependenciesStr += "\n    ";
  }

  // 使用fmt创建完整JSON内容
  return fmt::format(R"({{
    "name": "{}",
    "version": "0.1.0",
    "dependencies": [
      {}
    ]
  }}
  )",
                     options_.projectName, dependenciesStr);
}

std::string GuiTemplate::getConanfileContent() {
  std::string content = R"([requires]
spdlog/1.10.0
)";

  if (guiFramework_ == "qt") {
    content += "qt/6.2.3\n";
  } else if (guiFramework_ == "wxwidgets") {
    content += "wxwidgets/3.1.7\n";
  } else if (guiFramework_ == "gtk") {
    content += "gtk/3.24.30\n";
  }

  if (options_.includeTests) {
    if (to_string(options_.testFramework) == "gtest") {
      content += "gtest/1.12.1\n";
    } else if (to_string(options_.testFramework) == "catch2") {
      content += "catch2/3.1.0\n";
    } else if (to_string(options_.testFramework) == "doctest") {
      content += "doctest/2.4.9\n";
    }
  }

  content += R"(
[generators]
)";

  if (to_string(options_.buildSystem) == "cmake") {
    content += "cmake\n";
  } else if (to_string(options_.buildSystem) == "meson") {
    content += "pkg_config\n";
  } else {
    content += "cmake_find_package\n";
  }

  return content;
}

std::string GuiTemplate::getGTestContent() {
  return R"(#include <gtest/gtest.h>
#include ")" +
         options_.projectName + R"(/logging.h"
#include <memory>

// 初始化日志系�?
class LoggingEnvironment : public ::testing::Environment {
public:
    ~LoggingEnvironment() override = default;

    // 测试开始前设置
    void SetUp() override {
        )" +
         options_.projectName +
         R"(::Logging::init("logs/test.log", spdlog::level::debug);
        SPDLOG_INFO("测试开�?);
    }

    // 测试结束后清�?
    void TearDown() override {
        SPDLOG_INFO("测试结束");
        )" +
         options_.projectName + R"(::Logging::shutdown();
    }
};

// 测试Logging�?
TEST(LoggingTest, InitializationWorks) {
    // 由于在Environment中已经初始化，所以这里应该可以正常使�?
    EXPECT_EQ()" +
         options_.projectName + R"(::Logging::getLevel(), spdlog::level::debug);

    // 测试日志级别设置
    )" + options_.projectName +
         R"(::Logging::setLevel(spdlog::level::info);
    EXPECT_EQ()" +
         options_.projectName + R"(::Logging::getLevel(), spdlog::level::info);

    // 测试获取格式化时间戳
    auto timestamp = )" +
         options_.projectName + R"(::Logging::getFormattedTimestamp();
    EXPECT_FALSE(timestamp.empty());
}

// 注册全局环境
int main(int argc, char argv) {
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::AddGlobalTestEnvironment(new LoggingEnvironment);
    return RUN_ALL_TESTS();
}
)";
}

std::string GuiTemplate::getCatch2Content() {
  return R"(#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include ")" +
         options_.projectName + R"(/logging.h"
#include <memory>

// 初始化日志系�?
struct LoggingFixture {
    LoggingFixture() {
        )" +
         options_.projectName +
         R"(::Logging::init("logs/test.log", spdlog::level::debug);
        SPDLOG_INFO("测试开�?);
    }

    ~LoggingFixture() {
        SPDLOG_INFO("测试结束");
        )" +
         options_.projectName + R"(::Logging::shutdown();
    }
};

TEST_CASE_METHOD(LoggingFixture, "测试Logging�?, "[logging]") {
    SECTION("测试日志级别") {
        REQUIRE()" +
         options_.projectName +
         R"(::Logging::getLevel() == spdlog::level::debug);

        // 测试日志级别设置
        )" +
         options_.projectName + R"(::Logging::setLevel(spdlog::level::info);
        REQUIRE()" +
         options_.projectName +
         R"(::Logging::getLevel() == spdlog::level::info);
    }

    SECTION("测试格式化时间戳") {
        auto timestamp = )" +
         options_.projectName + R"(::Logging::getFormattedTimestamp();
        REQUIRE_FALSE(timestamp.empty());
    }
}
)";
}

std::string GuiTemplate::getDocTestContent() {
  return R"(#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>
#include ")" +
         options_.projectName + R"(/logging.h"
#include <memory>

// 初始化日志系�?
class LoggingFixture {
public:
    LoggingFixture() {
        )" +
         options_.projectName +
         R"(::Logging::init("logs/test.log", spdlog::level::debug);
        SPDLOG_INFO("测试开�?);
    }

    ~LoggingFixture() {
        SPDLOG_INFO("测试结束");
        )" +
         options_.projectName + R"(::Logging::shutdown();
    }
};

TEST_CASE_FIXTURE(LoggingFixture, "测试Logging�?) {
    SUBCASE("测试日志级别") {
        CHECK()" +
         options_.projectName +
         R"(::Logging::getLevel() == spdlog::level::debug);

        // 测试日志级别设置
        )" +
         options_.projectName + R"(::Logging::setLevel(spdlog::level::info);
        CHECK()" +
         options_.projectName +
         R"(::Logging::getLevel() == spdlog::level::info);
    }

    SUBCASE("测试格式化时间戳") {
        auto timestamp = )" +
         options_.projectName + R"(::Logging::getFormattedTimestamp();
        CHECK(!timestamp.empty());
    }
}
)";
}

std::string GuiTemplate::getQtUiContent() {
  return R"(<?xml version="1.0" encoding="UTF-8"?>
<ui version="4.0">
 <class>MainWindow</class>
 <widget class="QMainWindow" name="MainWindow">
  <property name="geometry">
   <rect>
    <x>0</x>
    <y>0</y>
    <width>800</width>
    <height>600</height>
   </rect>
  </property>
  <property name="windowTitle">
   <string>)" +
         options_.projectName + R"(</string>
  </property>
  <widget class="QWidget" name="centralwidget">
   <layout class="QVBoxLayout" name="verticalLayout">
    <item>
     <widget class="QLabel" name="label">
      <property name="font">
       <font>
        <pointsize>14</pointsize>
       </font>
      </property>
      <property name="text">
       <string>欢迎使用 )" +
         options_.projectName + R"(</string>
      </property>
      <property name="alignment">
       <set>Qt::AlignCenter</set>
      </property>
     </widget>
    </item>
   </layout>
  </widget>
  <widget class="QMenuBar" name="menubar">
   <property name="geometry">
    <rect>
     <x>0</x>
     <y>0</y>
     <width>800</width>
     <height>22</height>
    </rect>
   </property>
   <widget class="QMenu" name="menuFile">
    <property name="title">
     <string>文件</string>
    </property>
    <addaction name="actionExit"/>
   </widget>
   <widget class="QMenu" name="menuHelp">
    <property name="title">
     <string>帮助</string>
    </property>
    <addaction name="actionAbout"/>
   </widget>
   <addaction name="menuFile"/>
   <addaction name="menuHelp"/>
  </widget>
  <widget class="QStatusBar" name="statusbar"/>
  <action name="actionExit">
   <property name="text">
    <string>退�?/string>
   </property>
   <property name="shortcut">
    <string>Alt+F4</string>
   </property>
  </action>
  <action name="actionAbout">
   <property name="text">
    <string>关于</string>
   </property>
   <property name="shortcut">
    <string>F1</string>
   </property>
  </action>
 </widget>
 <resources/>
 <connections/>
</ui>
)";
}

std::string GuiTemplate::getQtResourceContent() {
  return R"(<!DOCTYPE RCC>
<RCC version="1.0">
    <qresource prefix="/">
        <file>icons/app_icon.png</file>
    </qresource>
</RCC>
)";
}

std::string GuiTemplate::getWxResourceContent() {
  return R"(// wxWidgets Resource File
#include "wx/msw/wx.rc"
)";
}

std::string GuiTemplate::getGtkGladeContent() {
  return R"(<?xml version="1.0" encoding="UTF-8"?>
<!-- Generated with glade 3.38.2 -->
<interface>
  <requires lib="gtk+" version="3.24"/>
  <object class="GtkWindow" id="main_window">
    <property name="can-focus">False</property>
    <property name="title" translatable="yes">)" +
         options_.projectName + R"(</property>
    <property name="default-width">800</property>
    <property name="default-height">600</property>
    <child>
      <object class="GtkBox">
        <property name="visible">True</property>
        <property name="can-focus">False</property>
        <property name="orientation">vertical</property>
        <child>
          <object class="GtkMenuBar" id="menubar">
            <property name="visible">True</property>
            <property name="can-focus">False</property>
            <child>
              <object class="GtkMenuItem">
                <property name="visible">True</property>
                <property name="can-focus">False</property>
                <property name="label" translatable="yes">文件</property>
                <child type="submenu">
                  <object class="GtkMenu">
                    <property name="visible">True</property>
                    <property name="can-focus">False</property>
                    <child>
                      <object class="GtkMenuItem" id="menu_exit">
                        <property name="visible">True</property>
                        <property name="can-focus">False</property>
                        <property name="label" translatable="yes">退�?/property>
                      </object>
                    </child>
                  </object>
                </child>
              </object>
            </child>
            <child>
              <object class="GtkMenuItem">
                <property name="visible">True</property>
                <property name="can-focus">False</property>
                <property name="label" translatable="yes">帮助</property>
                <child type="submenu">
                  <object class="GtkMenu">
                    <property name="visible">True</property>
                    <property name="can-focus">False</property>
                    <child>
                      <object class="GtkMenuItem" id="menu_about">
                        <property name="visible">True</property>
                        <property name="can-focus">False</property>
                        <property name="label" translatable="yes">关于</property>
                      </object>
                    </child>
                  </object>
                </child>
              </object>
            </child>
          </object>
          <packing>
            <property name="expand">False</property>
            <property name="fill">True</property>
            <property name="position">0</property>
          </packing>
        </child>
        <child>
          <object class="GtkLabel">
            <property name="visible">True</property>
            <property name="can-focus">False</property>
            <property name="label" translatable="yes">欢迎使用 )" +
         options_.projectName + R"(</property>
            <attributes>
              <attribute name="font-desc" value="Sans 14"/>
            </attributes>
          </object>
          <packing>
            <property name="expand">True</property>
            <property name="fill">True</property>
            <property name="position">1</property>
          </packing>
        </child>
        <child>
          <object class="GtkStatusbar" id="statusbar">
            <property name="visible">True</property>
            <property name="can-focus">False</property>
            <property name="margin-start">10</property>
            <property name="margin-end">10</property>
            <property name="margin-top">6</property>
            <property name="margin-bottom">6</property>
            <property name="orientation">vertical</property>
            <property name="spacing">2</property>
          </object>
          <packing>
            <property name="expand">False</property>
            <property name="fill">True</property>
            <property name="position">2</property>
          </packing>
        </child>
      </object>
    </child>
  </object>
</interface>
)";
}

bool GuiTemplate::setupDocumentation() {
  std::string projectPath = options_.projectName;
  std::string docsPath = FileUtils::combinePath(projectPath, "docs");

  if (!FileUtils::createDirectory(docsPath)) {
    std::cout << "Failed to create documentation directory\n";
    return false;
  }

  // Create basic documentation files
  std::string readmeContent = fmt::format(R"(# {} Documentation

This directory contains the documentation for the {} GUI application.

## Building Documentation

To build the documentation, you can use Doxygen:

```bash
doxygen Doxyfile
```

## Documentation Structure

- `api/` - API documentation
- `user/` - User guide
- `developer/` - Developer documentation
)", options_.projectName, options_.projectName);

  if (!FileUtils::writeToFile(FileUtils::combinePath(docsPath, "README.md"), readmeContent)) {
    std::cout << "Failed to create documentation README\n";
    return false;
  }

  // Create subdirectories
  FileUtils::createDirectory(FileUtils::combinePath(docsPath, "api"));
  FileUtils::createDirectory(FileUtils::combinePath(docsPath, "user"));
  FileUtils::createDirectory(FileUtils::combinePath(docsPath, "developer"));

  return true;
}

// Framework-specific content generators
std::string GuiTemplate::getFrameworkSpecificHeaderContent() {
  if (guiFramework_ == "gtk") {
    return getGTKSpecificContent();
  } else if (guiFramework_ == "fltk") {
    return getFLTKSpecificContent();
  } else if (guiFramework_ == "imgui") {
    return getImGuiSpecificContent();
  } else if (guiFramework_ == "wxwidgets") {
    return R"(#pragma once
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
  }

  return ""; // Default empty content
}

std::string GuiTemplate::getFrameworkSpecificSourceContent() {
  if (guiFramework_ == "gtk") {
    return R"(#include "app.h"
#include <gtk/gtk.h>

static void activate(GtkApplication* app, gpointer user_data) {
    GtkWidget *window;
    GtkWidget *button;
    GtkWidget *box;

    window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), ")" + options_.projectName + R"(");
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 300);

    box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_halign(box, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(box, GTK_ALIGN_CENTER);
    gtk_window_set_child(GTK_WINDOW(window), box);

    button = gtk_button_new_with_label("Hello from )" + options_.projectName + R"(!");
    g_signal_connect_swapped(button, "clicked", G_CALLBACK(gtk_window_destroy), window);
    gtk_box_append(GTK_BOX(box), button);

    gtk_widget_show(window);
}

int main(int argc, char **argv) {
    GtkApplication *app;
    int status;

    app = gtk_application_new("org.example.)" + options_.projectName + R"(", G_APPLICATION_FLAGS_NONE);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);

    return status;
}
)";
  } else if (guiFramework_ == "fltk") {
    return R"(#include "app.h"
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Button.H>
#include <FL/fl_ask.H>

void button_callback(Fl_Widget* widget, void* data) {
    fl_message("Hello from )" + options_.projectName + R"(!");
}

int main(int argc, char **argv) {
    Fl_Window *window = new Fl_Window(400, 300, ")" + options_.projectName + R"(");

    Fl_Button *button = new Fl_Button(150, 125, 100, 50, "Click Me!");
    button->callback(button_callback);

    window->end();
    window->show(argc, argv);

    return Fl::run();
}
)";
  } else if (guiFramework_ == "wxwidgets") {
    return R"(#include "app.h"

wxIMPLEMENT_APP(MyApp);

bool MyApp::OnInit() {
    MyFrame *frame = new MyFrame(")" + options_.projectName + R"(", wxPoint(50, 50), wxSize(450, 340));
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
    SetStatusText("Welcome to )" + options_.projectName + R"(!");
}

void MyFrame::OnExit(wxCommandEvent& event) {
    Close(true);
}

void MyFrame::OnAbout(wxCommandEvent& event) {
    wxMessageBox("This is a )" + options_.projectName + R"( sample", "About )" + options_.projectName + R"(", wxOK | wxICON_INFORMATION);
}

void MyFrame::OnHello(wxCommandEvent& event) {
    wxLogMessage("Hello world from )" + options_.projectName + R"(!");
}

void MyFrame::OnClose(wxCloseEvent& event) {
    Destroy();
}
)";
  }

  return ""; // Default empty content
}

// Helper methods for specific frameworks
std::string GuiTemplate::getGTKSpecificContent() {
  return R"(#pragma once
#include <gtk/gtk.h>

// GTK Application class declarations
void activate(GtkApplication* app, gpointer user_data);
)";
}

std::string GuiTemplate::getFLTKSpecificContent() {
  return R"(#pragma once
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Button.H>
#include <FL/fl_ask.H>

// FLTK Application class declarations
void button_callback(Fl_Widget* widget, void* data);
)";
}

std::string GuiTemplate::getImGuiSpecificContent() {
  return R"(#pragma once
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>

// ImGui Application class declarations
void glfw_error_callback(int error, const char* description);
)";
}

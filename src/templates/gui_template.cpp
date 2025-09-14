#include "gui_template.h"

#include <spdlog/fmt/fmt.h>

#include <iostream>

#include "../utils/core/file_utils.h"
#include "../utils/core/string_utils.h"

using namespace utils;
using namespace cli_enums;

GuiTemplate::GuiTemplate(const CliOptions& options) : TemplateBase(options) {
    // ȷ��GUI������� (Ĭ��ΪQt)
    guiFramework_ = "qt";

    // ����ʹ��������ָ����GUI���
    if (!options.guiFrameworks.empty()) {
        guiFramework_ = options.guiFrameworks[0];  // ʹ�õ�һ��ָ���Ŀ��
    } else {
        // ����Ƿ��п��ָ������Ŀ������
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

    std::cout << "?? ѡ���GUI���: " << guiFramework_ << std::endl;
}

bool GuiTemplate::create() {
    std::string projectPath = options_.projectName;

    // �����ĿĿ¼�Ƿ��Ѵ���
    if (FileUtils::directoryExists(projectPath)) {
        std::cout << "����: Ŀ¼ '" << projectPath << "' �Ѵ��ڡ�\n";
        return false;
    }

    std::cout << "?? ���ڴ���GUI��Ŀ (" << guiFramework_ << ")...\n";

    // ���������ṹ
    if (!createProjectStructure()) {
        std::cout << "������Ŀ�ṹʧ��\n";
        return false;
    }
    std::cout << "??��Ŀ�ṹ�Ѵ���\n";

    // ����GUI�ض��ļ�
    if (!createGuiSpecificFiles()) {
        std::cout << "����GUI�ض��ļ�ʧ��\n";
        return false;
    }
    std::cout << "??GUI�ض��ļ��Ѵ���\n";

    // ��������ϵͳ
    if (!createBuildSystem()) {
        std::cout << "���ù���ϵͳʧ��\n";
        return false;
    }
    std::cout << "??����ϵͳ������\n";

    // ���ð�������
    if (!setupPackageManager()) {
        std::cout << "���ð�������ʧ��\n";
        return false;
    }
    std::cout << "??��������������\n";

    // ���ò��Կ��
    if (options_.includeTests) {
        if (!setupTestFramework()) {
            std::cout << "���ò��Կ��ʧ��\n";
            return false;
        }
        std::cout << "??���Կ��������\n";
    }

    // �����ĵ�
    if (options_.includeDocumentation) {
        if (!setupDocumentation()) {
            std::cout << "�����ĵ�ʧ��\n";
            return false;
        }
        std::cout << "??�ĵ�������\n";
    }

    // ��ʼ��Git
    if (options_.initGit) {
        if (!initializeGit(projectPath)) {
            std::cout << "��ʼ��Git�ֿ�ʧ��\n";
            return false;
        }
        std::cout << "??�ѳ�ʼ��Git�ֿ�\n";
    }

    std::cout << "\n���GUI��Ŀ��׼��������\n\n";

    // ��ӡʹ��˵��
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
    } else if (to_string(options_.buildSystem) == "xmake") {
        std::cout << "xmake\n";
    } else if (to_string(options_.buildSystem) == "premake") {
        std::cout << "premake5 gmake2\n";
        std::cout << "make config=release\n";
    }

    std::cout << "\nף������?? ??\n";

    return true;
}

bool GuiTemplate::createProjectStructure() {
    std::string projectPath = options_.projectName;

    // ������Ŀ??
    if (!FileUtils::createDirectory(projectPath)) {
        return false;
    }

    // ����srcĿ¼
    std::string srcPath = FileUtils::combinePath(projectPath, "src");
    if (!FileUtils::createDirectory(srcPath)) {
        return false;
    }

    // ����includeĿ¼
    std::string includePath = FileUtils::combinePath(projectPath, "include");
    if (!FileUtils::createDirectory(includePath)) {
        return false;
    }

    std::string includeProjectPath = FileUtils::combinePath(includePath, options_.projectName);
    if (!FileUtils::createDirectory(includeProjectPath)) {
        return false;
    }

    // ������ԴĿ¼
    std::string resourcesPath = FileUtils::combinePath(projectPath, "resources");
    if (!FileUtils::createDirectory(resourcesPath)) {
        return false;
    }

    // ����UIĿ¼ (���ڽ�������ļ�)
    if (guiFramework_ == "qt" || guiFramework_ == "gtk") {
        std::string uiPath = FileUtils::combinePath(projectPath, "ui");
        if (!FileUtils::createDirectory(uiPath)) {
            return false;
        }
    }

    // д��README.md
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
    std::string includeProjectPath = FileUtils::combinePath(includePath, options_.projectName);

    // ������ҪԴ��??
    if (!FileUtils::writeToFile(FileUtils::combinePath(srcPath, "main.cpp"), getMainCppContent())) {
        return false;
    }

    // ���ݲ�ͬ��GUI��ܴ�����ͬ����??
    if (guiFramework_ == "qt") {
        // Qt�ض��ļ�
        if (!FileUtils::writeToFile(FileUtils::combinePath(includeProjectPath, "main_window.h"),
                                    getMainWindowHeaderContent())) {
            return false;
        }

        if (!FileUtils::writeToFile(FileUtils::combinePath(srcPath, "main_window.cpp"),
                                    getMainWindowCppContent())) {
            return false;
        }

        if (!FileUtils::writeToFile(FileUtils::combinePath(includeProjectPath, "application.h"),
                                    getAppHeaderContent())) {
            return false;
        }

        if (!FileUtils::writeToFile(FileUtils::combinePath(srcPath, "application.cpp"),
                                    getAppCppContent())) {
            return false;
        }
    } else if (guiFramework_ == "gtk" || guiFramework_ == "fltk" || guiFramework_ == "wxwidgets" ||
               guiFramework_ == "imgui") {
        // ����������ܣ������򻯵Ľṹ
        if (!FileUtils::writeToFile(FileUtils::combinePath(includeProjectPath, "app.h"),
                                    getFrameworkSpecificHeaderContent())) {
            return false;
        }

        if (!FileUtils::writeToFile(FileUtils::combinePath(srcPath, "app.cpp"),
                                    getFrameworkSpecificSourceContent())) {
            return false;
        }
    }

    // ������־����??
    if (!FileUtils::writeToFile(FileUtils::combinePath(includeProjectPath, "logging.h"),
                                getLoggingHeaderContent())) {
        return false;
    }

    if (!FileUtils::writeToFile(FileUtils::combinePath(srcPath, "logging.cpp"),
                                getLoggingCppContent())) {
        return false;
    }

    // Ϊ��ͬ��ܴ����ض���??
    if (guiFramework_ == "qt") {
        std::string uiPath = FileUtils::combinePath(projectPath, "ui");
        if (!FileUtils::writeToFile(FileUtils::combinePath(uiPath, "main_window.ui"),
                                    getQtUiContent())) {
            return false;
        }

        std::string resourcesPath = FileUtils::combinePath(projectPath, "resources");
        if (!FileUtils::writeToFile(FileUtils::combinePath(resourcesPath, "resources.qrc"),
                                    getQtResourceContent())) {
            return false;
        }
    } else if (guiFramework_ == "wxwidgets") {
        std::string resourcesPath = FileUtils::combinePath(projectPath, "resources");
        if (!FileUtils::writeToFile(FileUtils::combinePath(resourcesPath, "resource.rc"),
                                    getWxResourceContent())) {
            return false;
        }
    } else if (guiFramework_ == "gtk") {
        std::string uiPath = FileUtils::combinePath(projectPath, "ui");
        if (!FileUtils::writeToFile(FileUtils::combinePath(uiPath, "main_window.glade"),
                                    getGtkGladeContent())) {
            return false;
        }
    }

    return true;
}

bool GuiTemplate::createBuildSystem() {
    std::string projectPath = options_.projectName;

    if (to_string(options_.buildSystem) == "cmake") {
        // ����CMakeLists.txt
        if (!FileUtils::writeToFile(FileUtils::combinePath(projectPath, "CMakeLists.txt"),
                                    getCMakeContent())) {
            return false;
        }
    } else if (to_string(options_.buildSystem) == "meson") {
        // ����meson.build
        if (!FileUtils::writeToFile(FileUtils::combinePath(projectPath, "meson.build"),
                                    getMesonContent())) {
            return false;
        }
    } else if (to_string(options_.buildSystem) == "bazel") {
        // ����WORKSPACE��BUILD�ļ�
        if (!FileUtils::writeToFile(FileUtils::combinePath(projectPath, "WORKSPACE"),
                                    "workspace(name = \"" + options_.projectName + "\")\n")) {
            return false;
        }

        if (!FileUtils::writeToFile(FileUtils::combinePath(projectPath, "BUILD"),
                                    getBazelContent())) {
            return false;
        }
    } else if (to_string(options_.buildSystem) == "xmake") {
        // ����xmake.lua
        if (!FileUtils::writeToFile(FileUtils::combinePath(projectPath, "xmake.lua"),
                                    getXMakeContent())) {
            return false;
        }
    } else if (to_string(options_.buildSystem) == "premake") {
        // ����premake5.lua
        if (!FileUtils::writeToFile(FileUtils::combinePath(projectPath, "premake5.lua"),
                                    getPremakeContent())) {
            return false;
        }
    }

    return true;
}

bool GuiTemplate::setupPackageManager() {
    std::string projectPath = options_.projectName;

    if (to_string(options_.packageManager) == "vcpkg") {
        // ����vcpkg.json
        if (!FileUtils::writeToFile(FileUtils::combinePath(projectPath, "vcpkg.json"),
                                    getVcpkgJsonContent())) {
            return false;
        }
    } else if (to_string(options_.packageManager) == "conan") {
        // ����conanfile.txt
        if (!FileUtils::writeToFile(FileUtils::combinePath(projectPath, "conanfile.txt"),
                                    getConanfileContent())) {
            return false;
        }
    } else if (to_string(options_.packageManager) == "msys2") {
        // ����PKGBUILD
        if (!FileUtils::writeToFile(FileUtils::combinePath(projectPath, "PKGBUILD"),
                                    getMSYS2PKGBUILDContent())) {
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

    if (!FileUtils::writeToFile(FileUtils::combinePath(testsPath, "test_main.cpp"), testContent)) {
        return false;
    }

    // ���¹���ϵͳ�԰�����??
    if (to_string(options_.buildSystem) == "cmake") {
        std::string cmakePath = FileUtils::combinePath(projectPath, "CMakeLists.txt");
        std::string cmakeContent = FileUtils::readFromFile(cmakePath);

        // ��Ӳ�������
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

        // ����tests/CMakeLists.txt
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

        if (!FileUtils::writeToFile(FileUtils::combinePath(testsPath, "CMakeLists.txt"),
                                    testCmakeContent)) {
            return false;
        }
    }

    return true;
}

std::string GuiTemplate::getMainCppContent() {
    if (guiFramework_ == "qt") {
        return R"(#include <QApplication>
#include ")" + options_.projectName +
               R"(/logging.h"

int main(int argc, char* argv[]) {
    // ��ʼ����־ϵ??
    )" + options_.projectName +
               R"(::Logging::init("logs/app.log");

    // ����Ӧ��ʵ��
    QApplication qtApp(argc, argv);
    )" + options_.projectName +
               R"(::Application app;

    // ��ʾ����??
    if (!app.initialize()) {
        SPDLOG_ERROR("Ӧ�ó�ʼ��ʧ??);
        return 1;
    }

    // ����Ӧ����ѭ??
    SPDLOG_INFO("Ӧ�������ɹ�");
    int result = qtApp.exec();
    SPDLOG_INFO("Ӧ���˳�������?? {}", result);

    return result;
}
)";
    } else if (guiFramework_ == "wxwidgets") {
        return R"(#include <wx/wx.h>
#include ")" + options_.projectName +
               R"(/logging.h"

// ע��wxӦ��
wxIMPLEMENT_APP_NO_MAIN()" +
               options_.projectName + R"(::Application);

int main(int argc, char* argv[]) {
    // ��ʼ����־ϵ??
    )" + options_.projectName +
               R"(::Logging::init("logs/app.log");
    SPDLOG_INFO("Ӧ������??);

    // ����wxWidgetsӦ��
    wxEntryStart(argc, argv);
    int result = wxEntry(argc, argv);
    wxEntryCleanup();

    SPDLOG_INFO("Ӧ���˳�������?? {}", result);
    return result;
}
)";
    } else if (guiFramework_ == "gtk") {
        return R"(#include <gtk/gtk.h>
#include ")" + options_.projectName +
               R"(/logging.h"

int main(int argc, char* argv[]) {
    // ��ʼ����־ϵ??
    )" + options_.projectName +
               R"(::Logging::init("logs/app.log");
    SPDLOG_INFO("Ӧ������??);

    // ��ʼ��GTK
    gtk_init(&argc, &argv);

    // ����Ӧ��ʵ��
    )" + options_.projectName +
               R"(::Application app;

    if (!app.initialize()) {
        SPDLOG_ERROR("Ӧ�ó�ʼ��ʧ??);
        return 1;
    }

    // ����GTK��ѭ??
    SPDLOG_INFO("Ӧ�������ɹ�");
    int result = app.run();
    SPDLOG_INFO("Ӧ���˳�������?? {}", result);

    return result;
}
)";
    }

    // Ĭ�Ϸ���Qtʵ��
    return R"(#include <iostream>
#include ")" +
           options_.projectName + R"(/logging.h"

int main(int argc, char* argv[]) {
    // ��ʼ����־ϵ??
    )" + options_.projectName +
           R"(::Logging::init("logs/app.log");
    SPDLOG_INFO("Ӧ������??);

    // ����Ӧ��ʵ��
    )" + options_.projectName +
           R"(::Application app;
    if (!app.initialize()) {
        SPDLOG_ERROR("Ӧ�ó�ʼ��ʧ??);
        return 1;
    }

    // ����Ӧ����ѭ??
    SPDLOG_INFO("Ӧ�������ɹ�");
    int result = app.run();
    SPDLOG_INFO("Ӧ���˳�������?? {}", result);

    return result;
}
)";
}

std::string GuiTemplate::getMainWindowHeaderContent() {
    std::string headerGuard = StringUtils::toUpper(options_.projectName) + "_MAIN_WINDOW_H";

    if (guiFramework_ == "qt") {
        return R"(#pragma once
#ifndef )" + headerGuard +
               R"(
#define )" + headerGuard +
               R"(

#include <QMainWindow>
#include <memory>
#include <spdlog/spdlog.h>

// ǰ������
namespace Ui {
class MainWindow;
}

namespace )" + options_.projectName +
               R"( {

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

    // ��ʼ����??
    bool initialize();

private slots:
    // �ۺ�??
    void onActionExit();
    void onActionAbout();

private:
    // UI��������ɵ�UI??
    std::unique_ptr<Ui::MainWindow> ui;

    // ��ʼ����??
    void setupMenus();

    // ��ʼ��״̬��
    void setupStatusBar();

    // �����źźͲ�
    void connectSignals();
};

} // namespace )" +
               options_.projectName + R"(

#endif // )" + headerGuard +
               R"(
)";
    } else if (guiFramework_ == "wxwidgets") {
        return R"(#pragma once
#ifndef )" + headerGuard +
               R"(
#define )" + headerGuard +
               R"(

#include <spdlog/spdlog.h>
#include <wx/frame.h>
#include <wx/wx.h>

namespace )" + options_.projectName +
               R"( {

// ���ڱ�ʶ??
enum {
    ID_MAIN_WINDOW = wxID_HIGHEST + 1,
    ID_MENU_EXIT,
    ID_MENU_ABOUT
};

class MainWindow : public wxFrame {
public:
    MainWindow(const wxString& title, const wxPoint& pos, const wxSize& size);
    ~MainWindow() override;

    // ��ʼ����??
    bool initialize();

private:
    // UI�ؼ�
    wxMenuBar* menuBar{nullptr};
    wxStatusBar* statusBar{nullptr};
    wxPanel* mainPanel{nullptr};

    // ��ʼ��UI
    void setupMenus();
    void setupStatusBar();
    void setupControls();

    // �¼�����
    void onExit(wxCommandEvent& event);
    void onAbout(wxCommandEvent& event);

    // �¼�??
    wxDECLARE_EVENT_TABLE();
};

} // namespace )" +
               options_.projectName + R"(

#endif // )" + headerGuard +
               R"(
)";
    } else if (guiFramework_ == "gtk") {
        return R"(#pragma once
#ifndef )" + headerGuard +
               R"(
#define )" + headerGuard +
               R"(

#include <gtk/gtk.h>
#include <spdlog/spdlog.h>
#include <string>

namespace )" + options_.projectName +
               R"( {

class MainWindow {
public:
    MainWindow();
    ~MainWindow();

    // ��ʼ����??
    bool initialize();

    // ��ʾ����
    void show();

    // ��ȡGTK���ڿؼ�
    GtkWidget* getWidget() const { return window; }

private:
    // GTK�ؼ�
    GtkWidget* window{nullptr};
    GtkWidget* mainBox{nullptr};
    GtkWidget* menuBar{nullptr};
    GtkWidget* statusBar{nullptr};

    // UI�ļ�·��
    std::string uiFilePath;

    // ��ʼ��UI
    void setupUI();
    void setupMenus();
    void setupStatusBar();
    void setupSignals();

    // �ص�����
    static void onExit(GtkWidget* widget, gpointer data);
    static void onAbout(GtkWidget* widget, gpointer data);
};

} // namespace )" +
               options_.projectName + R"(

#endif // )" + headerGuard +
               R"(
)";
    }

    // Ĭ�Ϸ���һ��ͨ��ʵ��
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

    // ��ʼ����??
    bool initialize();

    // ��ʾ����
    void show();

    // �����Ƿ�ɼ�
    bool isVisible() const;

private:
    // �����Ƿ��ѳ�ʼ��
    bool initialized{false};

    // �����Ƿ�ɼ�
    bool visible{false};

    // ��ʼ��UI
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
#include <QCloseEvent>
#include <QMessageBox>

#include "ui_main_window.h"

namespace )" + options_.projectName +
               R"( {

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    SPDLOG_DEBUG("MainWindow���캯??);
}

MainWindow::~MainWindow()
{
    SPDLOG_DEBUG("MainWindow��������");
}

bool MainWindow::initialize()
{
    SPDLOG_DEBUG("��ʼ��MainWindow");

    // ����UI
    ui->setupUi(this);

    // ���ô��ڱ���
    setWindowTitle(tr(")" +
               options_.projectName + R"("));

    // ��ʼ����??
    setupMenus();

    // ��ʼ��״̬��
    setupStatusBar();

    // �����źźͲ�
    connectSignals();

    SPDLOG_INFO("MainWindow��ʼ����??);
    return true;
}

void MainWindow::setupMenus()
{
    SPDLOG_DEBUG("���ò˵�");

    // �����˳���??
    connect(ui->actionExit, &QAction::triggered, this, &MainWindow::onActionExit);

    // ���ӹ��ڲ���
    connect(ui->actionAbout, &QAction::triggered, this, &MainWindow::onActionAbout);
}

void MainWindow::setupStatusBar()
{
    SPDLOG_DEBUG("����״̬��");
    statusBar()->showMessage(tr("����"));
}

void MainWindow::connectSignals()
{
    SPDLOG_DEBUG("�����źźͲ�");
}

void MainWindow::onActionExit()
{
    SPDLOG_DEBUG("�����˳���??);
    close();
}

void MainWindow::onActionAbout()
{
    SPDLOG_DEBUG("�������ڲ���");
    QMessageBox::about(this, tr("����"),
                       tr(")" +
               options_.projectName + R"( v1.0\n\n"
                          "һ��ʹ��Qt��ܵ�GUIӦ��??));
}

} // namespace )" +
               options_.projectName + R"(
)";
    } else if (guiFramework_ == "wxwidgets") {
        return R"(#include ")" + options_.projectName + R"(/main_window.h"
#include <wx/aboutdlg.h>
#include <wx/stattext.h>

namespace )" + options_.projectName +
               R"( {

// �¼���??
wxBEGIN_EVENT_TABLE(MainWindow, wxFrame)
    EVT_MENU(ID_MENU_EXIT, MainWindow::onExit)
    EVT_MENU(ID_MENU_ABOUT, MainWindow::onAbout)
wxEND_EVENT_TABLE()

MainWindow::MainWindow(const wxString& title, const wxPoint& pos, const wxSize& size)
    : wxFrame(nullptr, ID_MAIN_WINDOW, title, pos, size)
{
    SPDLOG_DEBUG("MainWindow���캯??);
}

MainWindow::~MainWindow()
{
    SPDLOG_DEBUG("MainWindow��������");
}

bool MainWindow::initialize()
{
    SPDLOG_DEBUG("��ʼ��MainWindow");

    // ����ͼ��
    // SetIcon(wxIcon("APPICON"));

    // ����UIԪ��
    setupMenus();
    setupStatusBar();
    setupControls();

    SPDLOG_INFO("MainWindow��ʼ����??);
    return true;
}

void MainWindow::setupMenus()
{
    SPDLOG_DEBUG("���ò˵�");

    // �����ļ��˵�
    wxMenu *menuFile = new wxMenu;
    menuFile->Append(ID_MENU_EXIT, "�˳�\tAlt+F4", "�˳�Ӧ�ó�??);

    // ���������˵�
    wxMenu *menuHelp = new wxMenu;
    menuHelp->Append(ID_MENU_ABOUT, "����...\tF1", "��ʾ���ڶԻ�??);

    // �����˵�??
    menuBar = new wxMenuBar;
    menuBar->Append(menuFile, "�ļ�");
    menuBar->Append(menuHelp, "����");

    // ���ò˵�??
    SetMenuBar(menuBar);
}

void MainWindow::setupStatusBar()
{
    SPDLOG_DEBUG("����״̬��");

    // ����״̬��
    statusBar = CreateStatusBar(1);
    statusBar->SetStatusText("����");
}

void MainWindow::setupControls()
{
    SPDLOG_DEBUG("���ÿؼ�");

    // ��������??
    mainPanel = new wxPanel(this, wxID_ANY);

    // ����һ���򵥵Ĳ���
    wxBoxSizer *sizer = new wxBoxSizer(wxVERTICAL);

    // ���һ���ı���??
    sizer->Add(
        new wxStaticText(mainPanel, wxID_ANY, "��ӭʹ�� )" +
               options_.projectName + R"("),
        0, wxALL | wxALIGN_CENTER_HORIZONTAL, 20
    );

    // ������岼��
    mainPanel->SetSizer(sizer);
    mainPanel->Layout();
}

void MainWindow::onExit(wxCommandEvent& event)
{
    SPDLOG_DEBUG("�����˳���??);
    Close(true);
}

void MainWindow::onAbout(wxCommandEvent& event)
{
    SPDLOG_DEBUG("�������ڲ���");

    wxAboutDialogInfo aboutInfo;
    aboutInfo.SetName(")" +
               options_.projectName + R"(");
    aboutInfo.SetVersion("1.0");
    aboutInfo.SetDescription("һ��ʹ��wxWidgets��ܵ�GUIӦ��");
    aboutInfo.SetCopyright("(C) 2025");

    wxAboutBox(aboutInfo);
}

} // namespace )" +
               options_.projectName + R"(
)";
    } else if (guiFramework_ == "gtk") {
        return R"(#include ")" + options_.projectName + R"(/main_window.h"
#include <iostream>

namespace )" + options_.projectName +
               R"( {

MainWindow::MainWindow()
    : uiFilePath("ui/main_window.glade")
{
    SPDLOG_DEBUG("MainWindow���캯??);
}

MainWindow::~MainWindow()
{
    SPDLOG_DEBUG("MainWindow��������");
    if (window) {
        gtk_widget_destroy(window);
    }
}

bool MainWindow::initialize()
{
    SPDLOG_DEBUG("��ʼ��MainWindow");

    // ��Glade�ļ����ؽ���
    GtkBuilder* builder = gtk_builder_new();

    if (gtk_builder_add_from_file(builder, uiFilePath.c_str(), nullptr) == 0) {
        SPDLOG_ERROR("�޷�����UI�ļ�: {}", uiFilePath);
        g_object_unref(builder);
        return false;
    }

    // ��ȡ�����ڿ�??
    window = GTK_WIDGET(gtk_builder_get_object(builder, "main_window"));
    if (!window) {
        SPDLOG_ERROR("�޷���ȡ�����ڿ�??);
        g_object_unref(builder);
        return false;
    }

    // ��ȡ�����ؼ�
    menuBar = GTK_WIDGET(gtk_builder_get_object(builder, "menubar"));
    statusBar = GTK_WIDGET(gtk_builder_get_object(builder, "statusbar"));

    // �����ź�
    g_signal_connect(gtk_builder_get_object(builder, "menu_exit"), "activate",
                    G_CALLBACK(onExit), this);
    g_signal_connect(gtk_builder_get_object(builder, "menu_about"), "activate",
                    G_CALLBACK(onAbout), this);

    // ���ڹر�ʱ�˳�Ӧ??
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), nullptr);

    // ��ʾ���п�??
    gtk_widget_show_all(window);

    // �ͷŹ���??
    g_object_unref(builder);

    SPDLOG_INFO("MainWindow��ʼ����??);
    return true;
}

void MainWindow::show()
{
    SPDLOG_DEBUG("��ʾMainWindow");
    if (window) {
        gtk_widget_show(window);
    }
}

void MainWindow::setupUI()
{
    SPDLOG_DEBUG("����UI");
}

void MainWindow::setupMenus()
{
    SPDLOG_DEBUG("���ò˵�");
}

void MainWindow::setupStatusBar()
{
    SPDLOG_DEBUG("����״̬��");
}

void MainWindow::setupSignals()
{
    SPDLOG_DEBUG("�����ź�");
}

void MainWindow::onExit(GtkWidget* widget, gpointer data)
{
    SPDLOG_DEBUG("�����˳���??);
    gtk_main_quit();
}

void MainWindow::onAbout(GtkWidget* widget, gpointer data)
{
    SPDLOG_DEBUG("�������ڲ���");

    GtkWidget* dialog = gtk_about_dialog_new();
    GtkAboutDialog* about_dialog = GTK_ABOUT_DIALOG(dialog);

    gtk_about_dialog_set_program_name(about_dialog, ")" +
               options_.projectName + R"(");
    gtk_about_dialog_set_version(about_dialog, "1.0");
    gtk_about_dialog_set_copyright(about_dialog, "(C) 2025");
    gtk_about_dialog_set_comments(about_dialog, "һ��ʹ��GTK��ܵ�GUIӦ��");

    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

} // namespace )" +
               options_.projectName + R"(
)";
    }

    // Ĭ�Ϸ���һ��ͨ��??
    return R"(#include ")" + options_.projectName + R"(/main_window.h"

namespace )" +
           options_.projectName + R"( {

MainWindow::MainWindow()
{
    SPDLOG_DEBUG("MainWindow���캯??);
}

MainWindow::~MainWindow()
{
    SPDLOG_DEBUG("MainWindow��������");
}

bool MainWindow::initialize()
{
    SPDLOG_DEBUG("��ʼ��MainWindow");
    initialized = true;
    SPDLOG_INFO("MainWindow��ʼ����??);
    return true;
}

void MainWindow::show()
{
    SPDLOG_DEBUG("��ʾMainWindow");
    if (initialized) {
        visible = true;
        SPDLOG_INFO("MainWindow���ڿɼ�");
    } else {
        SPDLOG_ERROR("������ʾδ��ʼ���Ĵ�??);
    }
}

bool MainWindow::isVisible() const
{
    return visible;
}

void MainWindow::setupUI()
{
    SPDLOG_DEBUG("����UI");
}

} // namespace )" +
           options_.projectName + R"(
)";
}

std::string GuiTemplate::getAppHeaderContent() {
    std::string headerGuard = StringUtils::toUpper(options_.projectName) + "_APPLICATION_H";

    if (guiFramework_ == "qt") {
        return R"(#pragma once
#ifndef )" + headerGuard +
               R"(
#define )" + headerGuard +
               R"(

#include "main_window.h"
#include <QObject>
#include <memory>
#include <spdlog/spdlog.h>

namespace )" + options_.projectName +
               R"( {

class Application : public QObject {
    Q_OBJECT

public:
    Application();
    ~Application() override;

    // ��ʼ��Ӧ�ó�??
    bool initialize();

    // ��ȡ����??
    MainWindow* mainWindow() { return mainWindow_.get(); }

private:
    // ����??
    std::unique_ptr<MainWindow> mainWindow_;

    // ����Ӧ�ó�������
    bool loadSettings();

    // ����Ӧ�ó�������
    bool saveSettings();
};

} // namespace )" +
               options_.projectName + R"(

#endif // )" + headerGuard +
               R"(
)";
    } else if (guiFramework_ == "wxwidgets") {
        return R"(#pragma once
#ifndef )" + headerGuard +
               R"(
#define )" + headerGuard +
               R"(

#include "main_window.h"
#include <memory>
#include <spdlog/spdlog.h>
#include <wx/wx.h>

namespace )" + options_.projectName +
               R"( {

class Application : public wxApp {
public:
    Application();
    virtual ~Application();

    // wxWidgetsӦ�ó�ʼ??
    bool OnInit() override;

    // wxWidgetsӦ����??
    int OnExit() override;

    // ��ȡ����??
    MainWindow* getMainWindow() { return mainWindow_; }

private:
    // ����??
    MainWindow* mainWindow_{nullptr};

    // ����Ӧ�ó�������
    bool loadSettings();

    // ����Ӧ�ó�������
    bool saveSettings();
};

} // namespace )" +
               options_.projectName + R"(

#endif // )" + headerGuard +
               R"(
)";
    } else if (guiFramework_ == "gtk") {
        return R"(#pragma once
#ifndef )" + headerGuard +
               R"(
#define )" + headerGuard +
               R"(

#include "main_window.h"
#include <memory>
#include <spdlog/spdlog.h>

namespace )" + options_.projectName +
               R"( {

class Application {
public:
    Application();
    ~Application();

    // ��ʼ��Ӧ�ó�??
    bool initialize();

    // ����Ӧ�ó���
    int run();

    // ��ȡ����??
    MainWindow* getMainWindow() { return mainWindow_.get(); }

private:
    // ����??
    std::unique_ptr<MainWindow> mainWindow_;

    // ����Ӧ�ó�������
    bool loadSettings();

    // ����Ӧ�ó�������
    bool saveSettings();
};

} // namespace )" +
               options_.projectName + R"(

#endif // )" + headerGuard +
               R"(
)";
    }

    // Ĭ�Ϸ���ͨ��??
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

    // ��ʼ��Ӧ�ó�??
    bool initialize();

    // ����Ӧ�ó�����ѭ??
    int run();

    // ��ȡ����??
    MainWindow* getMainWindow() { return mainWindow_.get(); }

private:
    // ����??
    std::unique_ptr<MainWindow> mainWindow_;

    // Ӧ�ó����Ƿ���������
    bool running{false};

    // ����Ӧ�ó�������
    bool loadSettings();

    // ����Ӧ�ó�������
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

namespace )" + options_.projectName +
               R"( {

Application::Application()
    : QObject(nullptr)
{
    SPDLOG_DEBUG("Application���캯??);
}

Application::~Application()
{
    SPDLOG_DEBUG("Application��������");
    saveSettings();
}

bool Application::initialize()
{
    SPDLOG_INFO("��ʼ��Ӧ�ó�??);

    // ��������
    if (!loadSettings()) {
        SPDLOG_WARN("�޷�����Ӧ�ó�������");
    }

    // ��������ʼ������??
    mainWindow_ = std::make_unique<MainWindow>();
    if (!mainWindow_->initialize()) {
        SPDLOG_ERROR("��ʼ��������ʧ��");
        return false;
    }

    // ��ʾ����??
    mainWindow_->show();

    SPDLOG_INFO("Ӧ�ó����ʼ����??);
    return true;
}

bool Application::loadSettings()
{
    SPDLOG_DEBUG("����Ӧ�ó�������");
    // TODO: ʵ�����ü����߼�
    return true;
}

bool Application::saveSettings()
{
    SPDLOG_DEBUG("����Ӧ�ó�������");
    // TODO: ʵ�����ñ����߼�
    return true;
}

} // namespace )" +
               options_.projectName + R"(
)";
    } else if (guiFramework_ == "wxwidgets") {
        return R"(#include ")" + options_.projectName + R"(/application.h"

namespace )" + options_.projectName +
               R"( {

Application::Application()
{
    SPDLOG_DEBUG("Application���캯??);
}

Application::~Application()
{
    SPDLOG_DEBUG("Application��������");
    saveSettings();
}

bool Application::OnInit()
{
    SPDLOG_INFO("��ʼ��Ӧ�ó�??);

    // ����Ӧ�ó�������
    SetAppName(")" +
               options_.projectName + R"(");

    // ��������
    if (!loadSettings()) {
        SPDLOG_WARN("�޷�����Ӧ�ó�������");
    }

    // ��������ʼ������??
    mainWindow_ = new MainWindow(")" +
               options_.projectName + R"(", wxPoint(50, 50), wxSize(800, 600));
    if (!mainWindow_->initialize()) {
        SPDLOG_ERROR("��ʼ��������ʧ��");
        return false;
    }

    // ��ʾ����??
    mainWindow_->Show(true);

    SPDLOG_INFO("Ӧ�ó����ʼ����??);
    return true;
}

int Application::OnExit()
{
    SPDLOG_INFO("Ӧ�ó�����??);

    // ��������
    saveSettings();

    return wxApp::OnExit();
}

bool Application::loadSettings()
{
    SPDLOG_DEBUG("����Ӧ�ó�������");
    // TODO: ʵ�����ü����߼�
    return true;
}

bool Application::saveSettings()
{
    SPDLOG_DEBUG("����Ӧ�ó�������");
    // TODO: ʵ�����ñ����߼�
    return true;
}

} // namespace )" +
               options_.projectName + R"(
)";
    } else if (guiFramework_ == "gtk") {
        return R"(#include ")" + options_.projectName + R"(/application.h"

namespace )" + options_.projectName +
               R"( {

Application::Application()
{
    SPDLOG_DEBUG("Application���캯??);
}

Application::~Application()
{
    SPDLOG_DEBUG("Application��������");
    saveSettings();
}

bool Application::initialize()
{
    SPDLOG_INFO("��ʼ��Ӧ�ó�??);

    // ��������
    if (!loadSettings()) {
        SPDLOG_WARN("�޷�����Ӧ�ó�������");
    }

    // ��������ʼ������??
    mainWindow_ = std::make_unique<MainWindow>();
    if (!mainWindow_->initialize()) {
        SPDLOG_ERROR("��ʼ��������ʧ��");
        return false;
    }

    SPDLOG_INFO("Ӧ�ó����ʼ����??);
    return true;
}

int Application::run()
{
    SPDLOG_INFO("����Ӧ�ó���");

    // ����GTK��ѭ??
    gtk_main();

    return 0;
}

bool Application::loadSettings()
{
    SPDLOG_DEBUG("����Ӧ�ó�������");
    // TODO: ʵ�����ü����߼�
    return true;
}

bool Application::saveSettings()
{
    SPDLOG_DEBUG("����Ӧ�ó�������");
    // TODO: ʵ�����ñ����߼�
    return true;
}

} // namespace )" +
               options_.projectName + R"(
)";
    }

    // Ĭ�Ϸ���ͨ��??
    return R"(#include ")" + options_.projectName + R"(/application.h"
#include <chrono>
#include <thread>

namespace )" +
           options_.projectName + R"( {

Application::Application()
{
    SPDLOG_DEBUG("Application���캯??);
}

Application::~Application()
{
    SPDLOG_DEBUG("Application��������");
    saveSettings();
}

bool Application::initialize()
{
    SPDLOG_INFO("��ʼ��Ӧ�ó�??);

    // ��������
    if (!loadSettings()) {
        SPDLOG_WARN("�޷�����Ӧ�ó�������");
    }

    // ��������ʼ������??
    mainWindow_ = std::make_unique<MainWindow>();
    if (!mainWindow_->initialize()) {
        SPDLOG_ERROR("��ʼ��������ʧ��");
        return false;
    }

    SPDLOG_INFO("Ӧ�ó����ʼ����??);
    return true;
}

int Application::run()
{
    SPDLOG_INFO("����Ӧ�ó���");

    if (!mainWindow_) {
        SPDLOG_ERROR("������δ��ʼ??);
        return 1;
    }

    // ��ʾ����??
    mainWindow_->show();

    // ģ���¼�ѭ��
    running = true;

    while (running) {
        // �����¼�...

        // ģ���¼�ѭ����ʱ��Ƭ
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        // ��ʵ��Ӧ���У��������һ���������¼�ѭ��
        // ���ڱ�ʾ��������ֻ��ģ�⼸����Ȼ����??
        static int counter = 0;
        if (++counter > 50) { // ����????
            running = false;
        }
    }

    SPDLOG_INFO("Ӧ�ó�����ѭ����??);
    return 0;
}

bool Application::loadSettings()
{
    SPDLOG_DEBUG("����Ӧ�ó�������");
    // TODO: ʵ�����ü����߼�
    return true;
}

bool Application::saveSettings()
{
    SPDLOG_DEBUG("����Ӧ�ó�������");
    // TODO: ʵ�����ñ����߼�
    return true;
}

} // namespace )" +
           options_.projectName + R"(
)";
}

std::string GuiTemplate::getLoggingHeaderContent() {
    std::string headerGuard = StringUtils::toUpper(options_.projectName) + "_LOGGING_H";

    return R"(#pragma once
#ifndef )" +
           headerGuard + R"(
#define )" +
           headerGuard + R"(

#include <memory>
#include <spdlog/spdlog.h>
#include <string>

// ����ʹ��spdlog�ĺ꣬��������Ŀ�е�??
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
    // ��ʼ����־ϵ??
    static bool init(const std::string& logFilePath,
                   spdlog::level::level_enum level = spdlog::level::info);

    // �ر���־ϵͳ
    static void shutdown();

    // ������־����
    static void setLevel(spdlog::level::level_enum level);

    // ��ȡ��ǰ��־����
    static spdlog::level::level_enum getLevel();

    // ��ȡ��ʽ��ʱ���
    static std::string getFormattedTimestamp();

private:
    // �Ƿ��ѳ�ʼ��
    static bool initialized_;

    // ��־�ļ�·��
    static std::string logFilePath_;

    // ��ǰ��־����
    static spdlog::level::level_enum level_;

    // ������־Ŀ¼
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
#include <spdlog/sinks/daily_file_sink.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include <chrono>
#include <filesystem>
#include <iomanip>
#include <sstream>

namespace )" +
           options_.projectName + R"( {

// ��̬��Ա��ʼ��
bool Logging::initialized_ = false;
std::string Logging::logFilePath_ = "logs/app.log";
spdlog::level::level_enum Logging::level_ = spdlog::level::info;

bool Logging::init(const std::string& logFilePath, spdlog::level::level_enum level) {
    if (initialized_) {
        return true; // �Ѿ���ʼ����
    }

    try {
        logFilePath_ = logFilePath;
        level_ = level;

        // ������־Ŀ¼
        if (!createLogDirectory(logFilePath_)) {
            // �������Ŀ¼ʧ�ܣ����˵�����̨��־
            auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
            auto logger = std::make_shared<spdlog::logger>("console_logger", console_sink);
            logger->set_level(level_);
            spdlog::set_default_logger(logger);

            spdlog::warn("�޷�������־Ŀ¼����ʹ�ÿ���̨��??);
            initialized_ = true;
            return false;
        }

        // ����һ����ת�ļ���־����??MB����??����??
        auto rotating_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
            logFilePath_, 5 * 1024 * 1024, 3);

        // ͬʱ���������̨
        auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();

        // �������������������ļ�¼??
        std::vector<spdlog::sink_ptr> sinks {rotating_sink, console_sink};
        auto logger = std::make_shared<spdlog::logger>("multi_sink", sinks.begin(), sinks.end());

        // ���ü�¼��ʽ
        logger->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] [%t] %v");

        // ������־����
        logger->set_level(level_);

        // ����ΪĬ�ϼ�¼��
        spdlog::set_default_logger(logger);

        // ��¼��ʼ����??
        spdlog::info("��־ϵͳ��ʼ���ɹ�������: {}, ·��: {}",
                   spdlog::level::to_string_view(level_), logFilePath_);

        initialized_ = true;
        return true;
    }
    catch (const spdlog::spdlog_ex& ex) {
        std::cerr << "��־��ʼ��ʧ?? " << ex.what() << std::endl;
        return false;
    }
}

void Logging::shutdown() {
    if (initialized_) {
        spdlog::info("�ر���־ϵͳ");
        spdlog::shutdown();
        initialized_ = false;
    }
}

void Logging::setLevel(spdlog::level::level_enum level) {
    level_ = level;
    if (initialized_) {
        spdlog::set_level(level_);
        spdlog::info("��־��������?? {}", spdlog::level::to_string_view(level_));
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
        std::cerr << "������־Ŀ¼ʧ��: " << e.what() << std::endl;
        return false;
    }
}

} // namespace )" +
           options_.projectName + R"(
)";
}

std::string GuiTemplate::getReadmeContent() {
    // ȷ��GUI�������
    std::string guiName = guiFramework_ == "qt"          ? "Qt"
                          : guiFramework_ == "wxwidgets" ? "wxWidgets"
                          : guiFramework_ == "gtk"       ? "GTK"
                                                         : "ͨ��UI���";

    // ȷ��GUI�������
    std::string guiDependency = guiFramework_ == "qt"          ? "Qt 6.x"
                                : guiFramework_ == "wxwidgets" ? "wxWidgets 3.x"
                                : guiFramework_ == "gtk"       ? "GTK 3.x"
                                                               : "GUI��ܿ�";

    // ����������Ϣ
    std::string packageManagerInfo =
            to_string(options_.packageManager) != "none"
                    ? fmt::format("- {}������\n", to_string(options_.packageManager))
                    : "";

    // ���Կ����Ϣ
    std::string testFrameworkInfo =
            options_.includeTests
                    ? fmt::format("- ����{}���Կ��\n", to_string(options_.testFramework))
                    : "";

    // ������������
    std::string packageManagerDep =
            to_string(options_.packageManager) != "none"
                    ? fmt::format("- {}\n", to_string(options_.packageManager))
                    : "";

    // ��������
    std::string buildSteps;
    if (to_string(options_.buildSystem) == "cmake") {
        buildSteps = R"(# ��������Ŀ¼
mkdir build && cd build

# ������Ŀ
cmake ..

# ����
make)";
    } else if (to_string(options_.buildSystem) == "meson") {
        buildSteps = R"(# ������Ŀ
meson setup build

# ����
cd build
meson compile)";
    } else if (to_string(options_.buildSystem) == "xmake") {
        buildSteps = R"(# ʹ��XMake����
xmake)";
    } else if (to_string(options_.buildSystem) == "premake") {
        buildSteps = R"(# ʹ��Premake����
premake5 gmake2
make config=release)";
    } else {
        buildSteps = R"(# ʹ��Bazel����
bazel build //...)";
    }

    // ���Բ���
    std::string testSection = "";
    if (options_.includeTests) {
        std::string testCmd;
        if (to_string(options_.buildSystem) == "cmake") {
            testCmd = R"(cd build
ctest)";
        } else if (to_string(options_.buildSystem) == "meson") {
            testCmd = R"(cd build
meson test)";
        } else if (to_string(options_.buildSystem) == "xmake") {
            testCmd = R"(xmake test)";
        } else if (to_string(options_.buildSystem) == "premake") {
            testCmd = fmt::format(R"(bin/Release/{}_tests)", options_.projectName);
        } else {
            testCmd = R"(bazel test //...)";
        }

        testSection = fmt::format(R"(### ���в���

```bash
{}
```
)",
                                  testCmd);
    }

    // ��������
    std::string runCmd;
    if (to_string(options_.buildSystem) == "cmake" || to_string(options_.buildSystem) == "meson") {
        runCmd = fmt::format(R"(cd build
./{})",
                             options_.projectName);
    } else if (to_string(options_.buildSystem) == "xmake") {
        runCmd = fmt::format(R"(xmake run {})", options_.projectName);
    } else if (to_string(options_.buildSystem) == "premake") {
        runCmd = fmt::format(R"(bin/Release/{})", options_.projectName);
    } else {
        runCmd = fmt::format(R"(bazel run //:{})", options_.projectName);
    }

    // UI����
    std::string uiSection = "";
    if (guiFramework_ == "qt" || guiFramework_ == "gtk") {
        uiSection = fmt::format(R"(������ ui/                    # UI����ļ�
??  ������ main_window.{}  # �����ڽ�����??
)",
                                (guiFramework_ == "qt" ? "ui" : "glade"));
    }

    // ����Ŀ¼
    std::string testsDir = options_.includeTests ?
                                                 R"(������ tests/                 # ����Ŀ¼
??  ������ test_main.cpp       # �������
)"
                                                 : "";

    // ʹ��fmt��ʽ������README����
    return fmt::format(R"(# {0}

һ��ʹ��{1}������C++ GUIӦ�ã���CPP-Scaffold����??

## �����ص�

- �ִ�����C++17 GUIӦ��
- ʹ��{1}��Ϊ������
- ����spdlog��־ϵͳ
- {2}����ϵͳ
{3}{4}

## ����˵��

### ����??

- C++17���ݱ���??
- {5}
- spdlog
- {2} ����ϵͳ
{6}

### ���벽��

```bash
{7}
```

{8}

### ����Ӧ��

```bash
{9}
```

## ��Ŀ�ṹ

```
{0}/
������ include/               # ͷ�ļ�Ŀ??
??  ������ {0}/
??      ������ application.h  # Ӧ��??
??      ������ main_window.h  # ����??
??      ������ logging.h      # ��־����
������ src/                   # Դ�ļ�Ŀ??
??  ������ main.cpp           # �������??
??  ������ application.cpp    # Ӧ��ʵ��
??  ������ main_window.cpp    # ������ʵ??
??  ������ logging.cpp        # ��־����ʵ��
{10}������ resources/             # ��Դ�ļ�Ŀ¼
{11}������ README.md              # ��Ŀ˵���ĵ�
������ CMakeLists.txt          # CMake�����ű�
```

## ʹ��spdlog��־ϵͳ

��Ŀ������spdlog��־ϵͳ���ṩ�˷������־�깩��Ŀʹ�ã�

```cpp
// ʾ���÷�
SPDLOG_TRACE("����һ��������??);
SPDLOG_DEBUG("����һ��������??);
SPDLOG_INFO("����һ����Ϣ��??);
SPDLOG_WARN("����һ��������??);
SPDLOG_ERROR("����һ��������?? {{0}}", error_code);
SPDLOG_CRITICAL("����һ�����ش�����??);
```

## ���??

����Ŀʹ��MIT���??- ���LICENSE�ļ�)",
                       options_.projectName, guiName, options_.buildSystem, packageManagerInfo,
                       testFrameworkInfo, guiDependency, packageManagerDep, buildSteps, testSection,
                       runCmd, uiSection, testsDir);
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

    // GUI����ض�����
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

    // vcpkg����
    if (to_string(options_.packageManager) == "vcpkg") {
        content += R"(
# vcpkg integration
if(DEFINED ENV{VCPKG_ROOT})
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
endif()
)";
    }

    // Դ�ļ���??
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

    // ��Դ�ļ�
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

    // ����ִ���ļ�
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

    // ����??
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

    // ��Ŀ??���ڲ���)
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

    // ���ӿ�Ŀ���??
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

    // ��װĿ��
    content += R"(
# Installation
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
)
)";

    // ��������
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

    // GUI����ض�����
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

    // ���Կ������
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

    // Դ��??
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
        content += "[]";  // ��������??
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

    // ����
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

    // ����
    if (options_.includeTests) {
        content +=
                R"(
# Tests
cc_test(
    name = ")" + options_.projectName +
                R"(_test",
    srcs = ["tests/test_main.cpp"],
    deps = [
        ":)" + options_.projectName +
                R"(_lib",
        "@)" +
                (to_string(options_.testFramework) == "gtest" ? "com_google_googletest//:gtest_main"
                 : to_string(options_.testFramework) == "catch2" ? "catch2//:catch2"
                                                                 : "doctest//:doctest") +
                R"(",
    ],
)
)";
    }

    return content;
}

std::string GuiTemplate::getXMakeContent() {
    std::string guiDependencies;
    std::string guiPackages;
    std::string guiFiles;

    if (guiFramework_ == "qt") {
        guiDependencies = R"(add_requires("qt6base", "qt6tools"))";
        guiPackages = R"(add_packages("qt6base", "qt6tools"))";
        guiFiles = R"(add_files("src/*.cpp", "include/{}/*.h")
    add_files("ui/*.ui")
    add_files("resources/*.qrc"))";
    } else if (guiFramework_ == "wxwidgets") {
        guiDependencies = R"(add_requires("wxwidgets"))";
        guiPackages = R"(add_packages("wxwidgets"))";
        guiFiles = R"(add_files("src/*.cpp", "include/{}/*.h"))";
    } else if (guiFramework_ == "gtk") {
        guiDependencies = R"(add_requires("gtk+3"))";
        guiPackages = R"(add_packages("gtk+3"))";
        guiFiles = R"(add_files("src/*.cpp", "include/{}/*.h")
    add_files("ui/*.glade"))";
    } else {
        guiDependencies = R"(-- Add GUI framework dependencies here)";
        guiPackages = R"(-- Add GUI framework packages here)";
        guiFiles = R"(add_files("src/*.cpp", "include/{}/*.h"))";
    }

    std::string testSection;
    if (options_.includeTests) {
        std::string testFramework;
        if (to_string(options_.testFramework) == "gtest") {
            testFramework = "gtest";
        } else if (to_string(options_.testFramework) == "catch2") {
            testFramework = "catch2";
        } else if (to_string(options_.testFramework) == "doctest") {
            testFramework = "doctest";
        }

        if (!testFramework.empty()) {
            testSection = fmt::format(R"(
add_requires("{}")

target("{}_tests")
    set_kind("binary")
    add_files("tests/test_main.cpp")
    add_packages("{}")
    add_deps("{}")
    set_targetdir("tests/bin")
)",
                                      testFramework, options_.projectName, testFramework,
                                      options_.projectName);
        }
    }

    return fmt::format(R"(set_project("{0}")
set_version("1.0.0")

-- Set C++ standard
set_languages("c++17")

-- Add build modes
add_rules("mode.debug", "mode.release")

-- GUI dependencies
{1}
add_requires("spdlog")

-- Main executable
target("{2}")
    set_kind("binary")
    {3}
    add_headerfiles("include/{4}/*.h")
    add_includedirs("include", {{public = true}})

    -- Add packages
    {5}
    add_packages("spdlog")

    -- Set output directory
    set_targetdir("bin")

    -- Enable C++ features
    set_languages("c++17")

    -- Add compile flags
    if is_mode("debug") then
        add_defines("DEBUG")
        set_symbols("debug")
        set_optimize("none")
    elseif is_mode("release") then
        add_defines("NDEBUG")
        set_symbols("hidden")
        set_optimize("fastest")
    end
{6})",
                       options_.projectName, guiDependencies, options_.projectName,
                       fmt::format(guiFiles, options_.projectName), options_.projectName,
                       guiPackages, testSection);
}

std::string GuiTemplate::getPremakeContent() {
    std::string guiLinks;
    std::string guiIncludes;

    if (guiFramework_ == "qt") {
        guiLinks = R"(links {
        "Qt6Core",
        "Qt6Gui",
        "Qt6Widgets"
    })";
        guiIncludes = R"(includedirs {
        "/usr/include/qt6",
        "/usr/include/qt6/QtCore",
        "/usr/include/qt6/QtGui",
        "/usr/include/qt6/QtWidgets"
    })";
    } else if (guiFramework_ == "wxwidgets") {
        guiLinks = R"(links {
        "wx_gtk3u_core-3.2",
        "wx_baseu-3.2"
    })";
        guiIncludes = R"(includedirs {
        "/usr/include/wx-3.2"
    })";
    } else if (guiFramework_ == "gtk") {
        guiLinks = R"(links {
        "gtk-3",
        "gdk-3",
        "glib-2.0",
        "gobject-2.0"
    })";
        guiIncludes = R"(includedirs {
        "/usr/include/gtk-3.0",
        "/usr/include/glib-2.0"
    })";
    }

    std::string testSection;
    if (options_.includeTests) {
        testSection = fmt::format(R"(
project "{}_tests"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"
    targetdir "bin/%{{cfg.buildcfg}}"

    files {{
        "tests/**.cpp",
        "tests/**.h"
    }}

    includedirs {{
        "include",
        "tests"
    }}

    links {{
        "{}"
    }}
)",
                                  options_.projectName, options_.projectName);
    }

    return fmt::format(R"(workspace "{0}"
    configurations {{ "Debug", "Release" }}
    platforms {{ "x64" }}

project "{1}"
    kind "WindowedApp"
    language "C++"
    cppdialect "C++17"
    targetdir "bin/%{{cfg.buildcfg}}"

    files {{
        "src/**.cpp",
        "include/**.h"
    }}

    includedirs {{
        "include"
    }}

    {2}

    {3}

    links {{
        "spdlog"
    }}

    filter "configurations:Debug"
        defines {{ "DEBUG" }}
        symbols "On"
        optimize "Off"

    filter "configurations:Release"
        defines {{ "NDEBUG" }}
        symbols "Off"
        optimize "Speed"
{4})",
                       options_.projectName, options_.projectName, guiIncludes, guiLinks,
                       testSection);
}

std::string GuiTemplate::getVcpkgJsonContent() {
    // �����������б���spdlog��??
    std::vector<std::string> dependencies = {
            R"({
        "name": "spdlog"
      })"};

    // ����GUI��������Ӧ����
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

    // ���ݲ���������Ӳ��Կ������
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

    // �����������ַ�����ÿ��������֮���ö��źͻ�����??
    std::string dependenciesStr;
    for (size_t i = 0; i < dependencies.size(); ++i) {
        dependenciesStr += dependencies[i];
        if (i < dependencies.size() - 1) {
            dependenciesStr += ",";
        }
        dependenciesStr += "\n    ";
    }

    // ʹ��fmt��������JSON����
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

std::string GuiTemplate::getMSYS2PKGBUILDContent() {
    std::string testDependencies;
    if (options_.includeTests) {
        if (to_string(options_.testFramework) == "gtest") {
            testDependencies = "  \"${MINGW_PACKAGE_PREFIX}-gtest\"";
        } else if (to_string(options_.testFramework) == "catch2") {
            testDependencies = "  \"${MINGW_PACKAGE_PREFIX}-catch2\"";
        }
    }

    std::string guiDependencies;
    if (guiFramework_ == "qt") {
        guiDependencies =
                "  \"${MINGW_PACKAGE_PREFIX}-qt6-base\"\n  \"${MINGW_PACKAGE_PREFIX}-qt6-tools\"";
    } else if (guiFramework_ == "wxwidgets") {
        guiDependencies = "  \"${MINGW_PACKAGE_PREFIX}-wxwidgets\"";
    } else if (guiFramework_ == "gtk") {
        guiDependencies = "  \"${MINGW_PACKAGE_PREFIX}-gtk3\"";
    }

    std::string pkgbuildContent =
            "# Maintainer: Your Name <your.email@example.com>\n"
            "_realname=" +
            options_.projectName +
            "\n"
            "pkgbase=mingw-w64-${_realname}\n"
            "pkgname=\"${MINGW_PACKAGE_PREFIX}-${_realname}\"\n"
            "pkgver=1.0.0\n"
            "pkgrel=1\n"
            "pkgdesc=\"A C++ GUI application (mingw-w64)\"\n"
            "arch=(\"any\")\n"
            "mingw_arch=(\"mingw32\" \"mingw64\" \"ucrt64\" \"clang64\" \"clangarm64\")\n"
            "url=\"https://github.com/yourname/" +
            options_.projectName +
            "\"\n"
            "license=(\"MIT\")\n"
            "makedepends=(\n"
            "  \"${MINGW_PACKAGE_PREFIX}-cc\"\n"
            "  \"${MINGW_PACKAGE_PREFIX}-cmake\"\n"
            "  \"${MINGW_PACKAGE_PREFIX}-ninja\"\n"
            ")\n"
            "depends=(\n"
            "  \"${MINGW_PACKAGE_PREFIX}-gcc-libs\"\n" +
            guiDependencies + "\n" + testDependencies +
            "\n"
            ")\n"
            "source=(\"${_realname}-${pkgver}.tar.gz\")\n"
            "sha256sums=(\"SKIP\")\n\n"
            "build() {\n"
            "  cd \"${srcdir}/${_realname}-${pkgver}\"\n"
            "  \n"
            "  mkdir -p build && cd build\n"
            "  \n"
            "  MSYS2_ARG_CONV_EXCL=\"-DCMAKE_INSTALL_PREFIX=\" \\\n"
            "  ${MINGW_PREFIX}/bin/cmake.exe \\\n"
            "    -GNinja \\\n"
            "    -DCMAKE_INSTALL_PREFIX=${MINGW_PREFIX} \\\n"
            "    -DCMAKE_BUILD_TYPE=Release \\\n"
            "    ..\n"
            "  \n"
            "  ${MINGW_PREFIX}/bin/cmake.exe --build .\n"
            "}\n\n"
            "check() {\n"
            "  cd \"${srcdir}/${_realname}-${pkgver}/build\"\n"
            "  \n"
            "  # Run tests if available\n"
            "  if [ -f \"test_" +
            options_.projectName +
            "\" ]; then\n"
            "    ./test_" +
            options_.projectName +
            "\n"
            "  fi\n"
            "}\n\n"
            "package() {\n"
            "  cd \"${srcdir}/${_realname}-${pkgver}/build\"\n"
            "  \n"
            "  DESTDIR=\"${pkgdir}\" ${MINGW_PREFIX}/bin/cmake.exe --install .\n"
            "  \n"
            "  # Install license\n"
            "  install -Dm644 \"${srcdir}/${_realname}-${pkgver}/LICENSE\" \\\n"
            "    \"${pkgdir}${MINGW_PREFIX}/share/licenses/${_realname}/LICENSE\"\n"
            "}\n";

    return pkgbuildContent;
}

std::string GuiTemplate::getGTestContent() {
    return R"(#include <gtest/gtest.h>
#include ")" +
           options_.projectName + R"(/logging.h"
#include <memory>

// ��ʼ����־ϵ??
class LoggingEnvironment : public ::testing::Environment {
public:
    ~LoggingEnvironment() override = default;

    // ���Կ�ʼǰ����
    void SetUp() override {
        )" +
           options_.projectName +
           R"(::Logging::init("logs/test.log", spdlog::level::debug);
        SPDLOG_INFO("���Կ�??);
    }

    // ���Խ�������??
    void TearDown() override {
        SPDLOG_INFO("���Խ���");
        )" +
           options_.projectName + R"(::Logging::shutdown();
    }
};

// ����Logging??
TEST(LoggingTest, InitializationWorks) {
    // ������Environment���Ѿ���ʼ������������Ӧ�ÿ�������ʹ??
    EXPECT_EQ()" +
           options_.projectName + R"(::Logging::getLevel(), spdlog::level::debug);

    // ������־��������
    )" + options_.projectName +
           R"(::Logging::setLevel(spdlog::level::info);
    EXPECT_EQ()" +
           options_.projectName + R"(::Logging::getLevel(), spdlog::level::info);

    // ���Ի�ȡ��ʽ��ʱ���
    auto timestamp = )" +
           options_.projectName + R"(::Logging::getFormattedTimestamp();
    EXPECT_FALSE(timestamp.empty());
}

// ע��ȫ�ֻ���
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

// ��ʼ����־ϵ??
struct LoggingFixture {
    LoggingFixture() {
        )" +
           options_.projectName +
           R"(::Logging::init("logs/test.log", spdlog::level::debug);
        SPDLOG_INFO("���Կ�??);
    }

    ~LoggingFixture() {
        SPDLOG_INFO("���Խ���");
        )" +
           options_.projectName + R"(::Logging::shutdown();
    }
};

TEST_CASE_METHOD(LoggingFixture, "����Logging??, "[logging]") {
    SECTION("������־����") {
        REQUIRE()" +
           options_.projectName +
           R"(::Logging::getLevel() == spdlog::level::debug);

        // ������־��������
        )" +
           options_.projectName + R"(::Logging::setLevel(spdlog::level::info);
        REQUIRE()" +
           options_.projectName +
           R"(::Logging::getLevel() == spdlog::level::info);
    }

    SECTION("���Ը�ʽ��ʱ���") {
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

// ��ʼ����־ϵ??
class LoggingFixture {
public:
    LoggingFixture() {
        )" +
           options_.projectName +
           R"(::Logging::init("logs/test.log", spdlog::level::debug);
        SPDLOG_INFO("���Կ�??);
    }

    ~LoggingFixture() {
        SPDLOG_INFO("���Խ���");
        )" +
           options_.projectName + R"(::Logging::shutdown();
    }
};

TEST_CASE_FIXTURE(LoggingFixture, "����Logging??) {
    SUBCASE("������־����") {
        CHECK()" +
           options_.projectName +
           R"(::Logging::getLevel() == spdlog::level::debug);

        // ������־��������
        )" +
           options_.projectName + R"(::Logging::setLevel(spdlog::level::info);
        CHECK()" +
           options_.projectName +
           R"(::Logging::getLevel() == spdlog::level::info);
    }

    SUBCASE("���Ը�ʽ��ʱ���") {
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
       <string>��ӭʹ�� )" +
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
     <string>�ļ�</string>
    </property>
    <addaction name="actionExit"/>
   </widget>
   <widget class="QMenu" name="menuHelp">
    <property name="title">
     <string>����</string>
    </property>
    <addaction name="actionAbout"/>
   </widget>
   <addaction name="menuFile"/>
   <addaction name="menuHelp"/>
  </widget>
  <widget class="QStatusBar" name="statusbar"/>
  <action name="actionExit">
   <property name="text">
    <string>��??/string>
   </property>
   <property name="shortcut">
    <string>Alt+F4</string>
   </property>
  </action>
  <action name="actionAbout">
   <property name="text">
    <string>����</string>
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
                <property name="label" translatable="yes">�ļ�</property>
                <child type="submenu">
                  <object class="GtkMenu">
                    <property name="visible">True</property>
                    <property name="can-focus">False</property>
                    <child>
                      <object class="GtkMenuItem" id="menu_exit">
                        <property name="visible">True</property>
                        <property name="can-focus">False</property>
                        <property name="label" translatable="yes">��??/property>
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
                <property name="label" translatable="yes">����</property>
                <child type="submenu">
                  <object class="GtkMenu">
                    <property name="visible">True</property>
                    <property name="can-focus">False</property>
                    <child>
                      <object class="GtkMenuItem" id="menu_about">
                        <property name="visible">True</property>
                        <property name="can-focus">False</property>
                        <property name="label" translatable="yes">����</property>
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
            <property name="label" translatable="yes">��ӭʹ�� )" +
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
)",
                                            options_.projectName, options_.projectName);

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

    return "";  // Default empty content
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
    gtk_window_set_title(GTK_WINDOW(window), ")" +
               options_.projectName + R"(");
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 300);

    box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_halign(box, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(box, GTK_ALIGN_CENTER);
    gtk_window_set_child(GTK_WINDOW(window), box);

    button = gtk_button_new_with_label("Hello from )" +
               options_.projectName + R"(!");
    g_signal_connect_swapped(button, "clicked", G_CALLBACK(gtk_window_destroy), window);
    gtk_box_append(GTK_BOX(box), button);

    gtk_widget_show(window);
}

int main(int argc, char **argv) {
    GtkApplication *app;
    int status;

    app = gtk_application_new("org.example.)" +
               options_.projectName + R"(", G_APPLICATION_FLAGS_NONE);
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
    fl_message("Hello from )" +
               options_.projectName + R"(!");
}

int main(int argc, char **argv) {
    Fl_Window *window = new Fl_Window(400, 300, ")" +
               options_.projectName + R"(");

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
    MyFrame *frame = new MyFrame(")" +
               options_.projectName + R"(", wxPoint(50, 50), wxSize(450, 340));
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
    SetStatusText("Welcome to )" +
               options_.projectName + R"(!");
}

void MyFrame::OnExit(wxCommandEvent& event) {
    Close(true);
}

void MyFrame::OnAbout(wxCommandEvent& event) {
    wxMessageBox("This is a )" +
               options_.projectName + R"( sample", "About )" + options_.projectName +
               R"(", wxOK | wxICON_INFORMATION);
}

void MyFrame::OnHello(wxCommandEvent& event) {
    wxLogMessage("Hello world from )" +
               options_.projectName + R"(!");
}

void MyFrame::OnClose(wxCloseEvent& event) {
    Destroy();
}
)";
    }

    return "";  // Default empty content
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

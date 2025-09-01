#include "test-meson-gui/main_window.h"
#include <QCloseEvent>
#include <QMessageBox>

#include "ui_main_window.h"

namespace test-meson-gui {

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
    setWindowTitle(tr("test-meson-gui"));

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
                       tr("test-meson-gui v1.0\n\n"
                          "一个使用Qt框架的GUI应用�?));
}

} // namespace test-meson-gui

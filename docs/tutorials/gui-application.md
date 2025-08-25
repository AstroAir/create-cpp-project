# GUI Application Tutorial

Create a desktop application using modern GUI frameworks with CPP-Scaffold.

## What You'll Build

A simple text editor application with:

- File operations (new, open, save, save as)
- Basic text editing features
- Menu system and toolbar
- Status bar with file information
- Cross-platform compatibility

## Prerequisites

- CPP-Scaffold installed
- C++17 compatible compiler
- Qt or other GUI framework installed
- Basic understanding of GUI programming

## Step 1: Create the GUI Project

```bash
cpp-scaffold TextEditor \
  --type gui \
  --build cmake \
  --package vcpkg \
  --framework qt \
  --test gtest \
  --ci github
```

## Step 2: Project Structure

The generated project includes:

```
TextEditor/
├── CMakeLists.txt
├── src/
│   ├── main.cpp
│   ├── mainwindow.h
│   ├── mainwindow.cpp
│   └── mainwindow.ui
├── resources/
│   ├── icons/
│   └── resources.qrc
└── tests/
    └── test_mainwindow.cpp
```

## Step 3: Implement Main Window

```cpp title="src/mainwindow.h"
#pragma once

#include <QtWidgets/QMainWindow>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QLabel>
#include <QString>

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void newFile();
    void openFile();
    void saveFile();
    void saveAsFile();
    void exitApplication();
    void aboutApplication();
    void documentModified();

private:
    void setupUI();
    void setupMenus();
    void setupStatusBar();
    void updateWindowTitle();
    bool saveDocument();
    bool saveDocumentAs();
    
    QTextEdit *textEdit;
    QLabel *statusLabel;
    QString currentFile;
    bool isModified;
};
```

## Step 4: Implementation

```cpp title="src/mainwindow.cpp"
#include "mainwindow.h"
#include <QtWidgets/QApplication>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QVBoxLayout>
#include <QtCore/QFileInfo>
#include <QtCore/QTextStream>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), isModified(false) {
    setupUI();
    setupMenus();
    setupStatusBar();
    updateWindowTitle();
}

void MainWindow::setupUI() {
    textEdit = new QTextEdit(this);
    setCentralWidget(textEdit);
    
    connect(textEdit, &QTextEdit::textChanged, 
            this, &MainWindow::documentModified);
    
    resize(800, 600);
}

void MainWindow::setupMenus() {
    // File menu
    QMenu *fileMenu = menuBar()->addMenu("&File");
    
    fileMenu->addAction("&New", this, &MainWindow::newFile, 
                       QKeySequence::New);
    fileMenu->addAction("&Open...", this, &MainWindow::openFile, 
                       QKeySequence::Open);
    fileMenu->addSeparator();
    fileMenu->addAction("&Save", this, &MainWindow::saveFile, 
                       QKeySequence::Save);
    fileMenu->addAction("Save &As...", this, &MainWindow::saveAsFile, 
                       QKeySequence::SaveAs);
    fileMenu->addSeparator();
    fileMenu->addAction("E&xit", this, &MainWindow::exitApplication, 
                       QKeySequence::Quit);
    
    // Help menu
    QMenu *helpMenu = menuBar()->addMenu("&Help");
    helpMenu->addAction("&About", this, &MainWindow::aboutApplication);
}

void MainWindow::newFile() {
    if (isModified) {
        // Ask user to save changes
        auto reply = QMessageBox::question(this, "Save Changes",
            "Do you want to save changes to the current document?",
            QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
            
        if (reply == QMessageBox::Save) {
            if (!saveDocument()) return;
        } else if (reply == QMessageBox::Cancel) {
            return;
        }
    }
    
    textEdit->clear();
    currentFile.clear();
    isModified = false;
    updateWindowTitle();
}

void MainWindow::openFile() {
    QString fileName = QFileDialog::getOpenFileName(this,
        "Open File", "", "Text Files (*.txt);;All Files (*)");
        
    if (!fileName.isEmpty()) {
        QFile file(fileName);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&file);
            textEdit->setPlainText(in.readAll());
            currentFile = fileName;
            isModified = false;
            updateWindowTitle();
        }
    }
}

bool MainWindow::saveDocument() {
    if (currentFile.isEmpty()) {
        return saveDocumentAs();
    }
    
    QFile file(currentFile);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out << textEdit->toPlainText();
        isModified = false;
        updateWindowTitle();
        return true;
    }
    return false;
}
```

## Step 5: Build and Run

```bash
mkdir build && cd build
cmake .. -G Ninja
ninja

# Run the application
./TextEditor
```

## Step 6: Testing GUI Components

```cpp title="tests/test_mainwindow.cpp"
#include <gtest/gtest.h>
#include <QtWidgets/QApplication>
#include <QtTest/QTest>
#include "../src/mainwindow.h"

class MainWindowTest : public ::testing::Test {
protected:
    void SetUp() override {
        if (!QApplication::instance()) {
            int argc = 0;
            char **argv = nullptr;
            app = new QApplication(argc, argv);
        }
        window = new MainWindow();
    }
    
    void TearDown() override {
        delete window;
    }
    
    QApplication *app = nullptr;
    MainWindow *window = nullptr;
};

TEST_F(MainWindowTest, WindowCreation) {
    EXPECT_NE(window, nullptr);
    EXPECT_FALSE(window->isVisible());
}

TEST_F(MainWindowTest, ShowWindow) {
    window->show();
    EXPECT_TRUE(window->isVisible());
}
```

## What You've Learned

✅ Created a GUI application with CPP-Scaffold  
✅ Implemented file operations and menus  
✅ Used Qt framework for cross-platform GUI  
✅ Added keyboard shortcuts and status bar  
✅ Created basic GUI tests  

## Next Steps

- **Add more features**: Find/replace, syntax highlighting
- **Improve UI**: Better icons, themes, preferences
- **Advanced functionality**: Multiple tabs, plugins
- **Packaging**: Create installers for different platforms

## Related Tutorials

- [Console Application Tutorial](console-app.md) - Command-line applications
- [Advanced Features Tutorial](advanced-features.md) - Custom templates and deployment

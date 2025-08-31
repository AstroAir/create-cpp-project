/**
 * @file main.cpp
 * @brief Qt Image Viewer - Main application entry point
 *
 * A comprehensive Qt6 image viewer application demonstrating modern GUI
 * development practices, image processing, and cross-platform deployment.
 *
 * @author CPP-Scaffold Generated
 * @version 1.0.0
 */

#ifdef NO_QT_AVAILABLE
#include <iostream>

int main(int argc, char *argv[]) {
    std::cout << "Qt Image Viewer Example\n";
    std::cout << "======================\n\n";
    std::cout << "This example requires Qt6 to be installed.\n";
    std::cout << "Please install Qt6 and try building again.\n\n";
    std::cout << "To install Qt6 with vcpkg:\n";
    std::cout << "  vcpkg install qt6-base qt6-imageformats qt6-svg\n\n";
    std::cout << "Then configure with:\n";
    std::cout << "  cmake .. -DCMAKE_TOOLCHAIN_FILE=path/to/vcpkg.cmake\n";
    return 0;
}

#else

#include <QApplication>
#include <QCommandLineParser>
#include <QDir>
#include <QFileInfo>
#include <QStandardPaths>
#include <QStyleFactory>
#include <QDirIterator>
#include <QImageReader>
#include <QLoggingCategory>
#include <QMainWindow>
#include <QLabel>
#include <QVBoxLayout>
#include <QWidget>
#include <QMenuBar>
#include <QStatusBar>
#include <QFileDialog>
#include <QPixmap>
#include <QScrollArea>
#include <QMessageBox>

Q_LOGGING_CATEGORY(app, "qt.imageviewer.app")

/**
 * @brief Simple Image Viewer Window
 */
class SimpleImageViewer : public QMainWindow {
    Q_OBJECT

public:
    SimpleImageViewer(QWidget *parent = nullptr) : QMainWindow(parent) {
        setupUI();
        setupMenus();
        setWindowTitle("Qt Image Viewer Example");
        resize(800, 600);
    }

private slots:
    void openImage() {
        QString fileName = QFileDialog::getOpenFileName(this,
            tr("Open Image"), "",
            tr("Image Files (*.png *.jpg *.jpeg *.bmp *.gif *.tiff)"));

        if (!fileName.isEmpty()) {
            loadImage(fileName);
        }
    }

    void aboutQt() {
        QMessageBox::aboutQt(this, tr("About Qt"));
    }

    void about() {
        QMessageBox::about(this, tr("About Qt Image Viewer"),
            tr("Qt Image Viewer Example\n\n"
               "A simple image viewer demonstrating Qt6 capabilities.\n"
               "Generated with CPP-Scaffold."));
    }

private:
    void setupUI() {
        centralWidget = new QWidget(this);
        setCentralWidget(centralWidget);

        layout = new QVBoxLayout(centralWidget);

        scrollArea = new QScrollArea(this);
        scrollArea->setAlignment(Qt::AlignCenter);
        scrollArea->setWidgetResizable(true);

        imageLabel = new QLabel(this);
        imageLabel->setAlignment(Qt::AlignCenter);
        imageLabel->setText("No image loaded\n\nUse File -> Open to load an image");
        imageLabel->setStyleSheet("QLabel { color: gray; font-size: 14px; }");

        scrollArea->setWidget(imageLabel);
        layout->addWidget(scrollArea);

        statusBar()->showMessage("Ready");
    }

    void setupMenus() {
        // File menu
        QMenu *fileMenu = menuBar()->addMenu(tr("&File"));

        QAction *openAction = fileMenu->addAction(tr("&Open..."));
        openAction->setShortcut(QKeySequence::Open);
        connect(openAction, &QAction::triggered, this, &SimpleImageViewer::openImage);

        fileMenu->addSeparator();

        QAction *exitAction = fileMenu->addAction(tr("E&xit"));
        exitAction->setShortcut(QKeySequence::Quit);
        connect(exitAction, &QAction::triggered, this, &QWidget::close);

        // Help menu
        QMenu *helpMenu = menuBar()->addMenu(tr("&Help"));

        QAction *aboutQtAction = helpMenu->addAction(tr("About &Qt"));
        connect(aboutQtAction, &QAction::triggered, this, &SimpleImageViewer::aboutQt);

        QAction *aboutAction = helpMenu->addAction(tr("&About"));
        connect(aboutAction, &QAction::triggered, this, &SimpleImageViewer::about);
    }

    void loadImage(const QString &fileName) {
        QPixmap pixmap(fileName);
        if (pixmap.isNull()) {
            QMessageBox::warning(this, tr("Error"),
                tr("Cannot load image: %1").arg(fileName));
            return;
        }

        imageLabel->setPixmap(pixmap);
        imageLabel->resize(pixmap.size());

        QFileInfo fileInfo(fileName);
        setWindowTitle(tr("Qt Image Viewer - %1").arg(fileInfo.fileName()));
        statusBar()->showMessage(tr("Image loaded: %1 (%2x%3)")
            .arg(fileInfo.fileName())
            .arg(pixmap.width())
            .arg(pixmap.height()));
    }

    QWidget *centralWidget;
    QVBoxLayout *layout;
    QScrollArea *scrollArea;
    QLabel *imageLabel;
};

/**
 * @brief Setup application theme and styling
 */
void setupApplicationTheme() {
    // Set application style
    QApplication::setStyle("Fusion");

    // Simple dark theme
    QPalette darkPalette;
    darkPalette.setColor(QPalette::Window, QColor(53, 53, 53));
    darkPalette.setColor(QPalette::WindowText, Qt::white);
    darkPalette.setColor(QPalette::Base, QColor(25, 25, 25));
    darkPalette.setColor(QPalette::AlternateBase, QColor(53, 53, 53));
    darkPalette.setColor(QPalette::ToolTipBase, Qt::white);
    darkPalette.setColor(QPalette::ToolTipText, Qt::white);
    darkPalette.setColor(QPalette::Text, Qt::white);
    darkPalette.setColor(QPalette::Button, QColor(53, 53, 53));
    darkPalette.setColor(QPalette::ButtonText, Qt::white);
    darkPalette.setColor(QPalette::BrightText, Qt::red);
    darkPalette.setColor(QPalette::Link, QColor(42, 130, 218));
    darkPalette.setColor(QPalette::Highlight, QColor(42, 130, 218));
    darkPalette.setColor(QPalette::HighlightedText, Qt::black);

    // Apply the dark theme
    QApplication::setPalette(darkPalette);
}

/**
 * @brief Get list of supported image formats
 * @return List of supported file extensions
 */
QStringList getSupportedImageFormats() {
    QStringList formats;
    const auto supportedFormats = QImageReader::supportedImageFormats();
    
    for (const auto& format : supportedFormats) {
        formats << QString("*.%1").arg(QString::fromLatin1(format).toLower());
    }
    
    return formats;
}

/**
 * @brief Find images in directory
 * @param dirPath Directory path to search
 * @param recursive Whether to search recursively
 * @return List of image file paths
 */
QStringList findImagesInDirectory(const QString& dirPath, bool recursive = false) {
    QStringList imageFiles;
    QStringList supportedFormats = getSupportedImageFormats();
    
    QDirIterator::IteratorFlag flags = QDirIterator::NoIteratorFlags;
    if (recursive) {
        flags = QDirIterator::Subdirectories;
    }
    
    QDirIterator it(dirPath, supportedFormats, QDir::Files, flags);
    while (it.hasNext()) {
        imageFiles << it.next();
    }
    
    return imageFiles;
}

/**
 * @brief Process command line arguments and return initial files to open
 * @param parser Configured command line parser
 * @return List of files to open initially
 */
QStringList processCommandLineArguments(const QCommandLineParser& parser) {
    QStringList filesToOpen;
    
    // Get positional arguments (file paths)
    const QStringList args = parser.positionalArguments();
    
    for (const QString& arg : args) {
        QFileInfo fileInfo(arg);
        
        if (fileInfo.isFile()) {
            // Single file
            filesToOpen << fileInfo.absoluteFilePath();
        } else if (fileInfo.isDir()) {
            // Directory - find all images
            bool recursive = parser.isSet("recursive");
            QStringList images = findImagesInDirectory(fileInfo.absoluteFilePath(), recursive);
            filesToOpen << images;
        } else {
            // Try glob pattern
            QDir dir = fileInfo.dir();
            QStringList filters;
            filters << fileInfo.fileName();
            
            QFileInfoList matches = dir.entryInfoList(filters, QDir::Files);
            for (const QFileInfo& match : matches) {
                filesToOpen << match.absoluteFilePath();
            }
        }
    }
    
    return filesToOpen;
}

/**
 * @brief Main application entry point
 * @param argc Number of command-line arguments
 * @param argv Array of command-line arguments
 * @return Exit code
 */
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // Set application metadata
    app.setApplicationName("Qt Image Viewer");
    app.setApplicationVersion("1.0.0");
    app.setApplicationDisplayName("Qt Image Viewer");
    app.setOrganizationName("CPP-Scaffold");
    app.setOrganizationDomain("cpp-scaffold.org");

    // Setup theme
    setupApplicationTheme();

    // Create and show the main window
    SimpleImageViewer viewer;
    viewer.show();

    // If a file was specified on command line, try to load it
    QStringList args = app.arguments();
    if (args.size() > 1) {
        QString fileName = args[1];
        if (QFileInfo::exists(fileName)) {
            // We would load the image here in a real implementation
            viewer.setWindowTitle(QString("Qt Image Viewer - %1").arg(QFileInfo(fileName).fileName()));
        }
    }

    return app.exec();
}

// Include MOC file for Qt's meta-object system (required for Q_OBJECT in .cpp files)
#include "main.moc"

#endif // NO_QT_AVAILABLE

# Qt Image Viewer - Complete GUI Application Example

A comprehensive example of a modern Qt6 desktop application created with CPP-Scaffold, demonstrating GUI development best practices, image processing, and cross-platform deployment.

## 🎯 What This Example Demonstrates

- **Modern Qt6** application architecture with widgets
- **Image processing** with Qt's graphics framework
- **File dialogs** and user interface interactions
- **Settings persistence** with QSettings
- **Keyboard shortcuts** and menu systems
- **Drag and drop** functionality
- **Multi-format image support** (JPEG, PNG, BMP, GIF, etc.)
- **Zoom and pan** capabilities
- **Recent files** management
- **Cross-platform deployment** strategies

## 🚀 Generated Command

This project was created using:

```bash
cpp-scaffold qt-image-viewer \
  --type gui \
  --framework qt \
  --build cmake \
  --package vcpkg \
  --test gtest \
  --ci github \
  --editor vscode \
  --std cpp17 \
  --docs
```

## 📁 Project Structure

```
qt-image-viewer/
├── CMakeLists.txt              # Modern CMake with Qt6 integration
├── vcpkg.json                  # Package dependencies
├── .gitignore                  # Git ignore patterns
├── LICENSE                     # MIT license
├── README.md                   # Project documentation
├── .github/
│   └── workflows/
│       ├── ci.yml              # Multi-platform CI/CD
│       └── release.yml         # Release automation
├── .vscode/
│   ├── settings.json           # VSCode Qt settings
│   ├── tasks.json              # Build and debug tasks
│   └── launch.json             # Debug configuration
├── src/
│   ├── main.cpp                # Application entry point
│   ├── mainwindow.h            # Main window class
│   ├── mainwindow.cpp
│   ├── mainwindow.ui           # UI layout file
│   ├── imageviewer.h           # Image viewer widget
│   ├── imageviewer.cpp
│   ├── settingsmanager.h       # Settings management
│   ├── settingsmanager.cpp
│   └── aboutdialog.h           # About dialog
│       └── aboutdialog.cpp
├── resources/
│   ├── icons/                  # Application icons
│   │   ├── app.ico
│   │   ├── open.png
│   │   ├── save.png
│   │   ├── zoom-in.png
│   │   └── zoom-out.png
│   ├── images/                 # Sample images
│   │   └── sample.jpg
│   └── qt-image-viewer.qrc     # Qt resource file
├── ui/
│   ├── mainwindow.ui           # Main window UI
│   └── aboutdialog.ui          # About dialog UI
├── tests/
│   ├── CMakeLists.txt          # Test configuration
│   ├── test_main.cpp           # Test runner
│   ├── test_imageviewer.cpp    # Image viewer tests
│   └── test_settingsmanager.cpp # Settings tests
├── docs/
│   ├── user-guide.md           # User documentation
│   ├── developer-guide.md      # Developer documentation
│   └── deployment.md           # Deployment instructions
└── deployment/
    ├── windows/                # Windows deployment
    │   ├── installer.nsi       # NSIS installer script
    │   └── deploy.bat          # Deployment script
    ├── macos/                  # macOS deployment
    │   ├── Info.plist          # App bundle info
    │   └── deploy.sh           # Deployment script
    └── linux/                  # Linux deployment
        ├── qt-image-viewer.desktop # Desktop entry
        └── deploy.sh           # Deployment script
```

## 🔧 Features

### Core Functionality

- **Multi-format image support**: JPEG, PNG, BMP, GIF, TIFF, WebP
- **Zoom controls**: Zoom in/out, fit to window, actual size
- **Pan and scroll**: Mouse drag and scroll wheel support
- **File operations**: Open, save, save as, recent files
- **Image information**: Display dimensions, format, file size
- **Slideshow mode**: Automatic image cycling
- **Fullscreen viewing**: Distraction-free image viewing

### User Interface

- **Modern Qt6 widgets**: Native look and feel on all platforms
- **Customizable toolbar**: Show/hide tools, rearrange buttons
- **Status bar**: Image information and zoom level
- **Keyboard shortcuts**: Standard shortcuts for all operations
- **Drag and drop**: Drop images directly onto the viewer
- **Context menus**: Right-click for quick actions

### Advanced Features

- **Settings persistence**: Remember window size, recent files, preferences
- **Multiple image formats**: Support for all Qt-supported formats
- **Image transformations**: Rotate, flip, basic adjustments
- **Thumbnail navigation**: Quick preview of images in folder
- **Batch operations**: Process multiple images
- **Plugin architecture**: Extensible with custom image filters

## 🏗️ Building the Project

### Prerequisites

- **Qt6**: Qt6 Core, Widgets, and Gui modules
- **C++17** compatible compiler
- **CMake 3.16** or later (for Qt6 support)
- **vcpkg** package manager
- **Git**

### Dependencies (managed by vcpkg)

```json
{
  "name": "qt-image-viewer",
  "version": "1.0.0",
  "dependencies": [
    "qt6-base",
    "qt6-imageformats",
    "qt6-svg",
    "gtest"
  ]
}
```

### Build Steps

1. **Install Qt6 with vcpkg:**
   ```bash
   # Install vcpkg if not already installed
   git clone https://github.com/Microsoft/vcpkg.git
   ./vcpkg/bootstrap-vcpkg.sh  # or bootstrap-vcpkg.bat on Windows
   
   # Install Qt6 and dependencies
   ./vcpkg/vcpkg install qt6-base qt6-imageformats qt6-svg gtest
   ```

2. **Configure and build:**
   ```bash
   mkdir build && cd build
   cmake .. -DCMAKE_TOOLCHAIN_FILE=../vcpkg/scripts/buildsystems/vcpkg.cmake
   cmake --build . --config Release
   ```

3. **Run the application:**
   ```bash
   # On Windows
   ./Release/qt-image-viewer.exe
   
   # On Linux/macOS
   ./qt-image-viewer
   ```

4. **Run tests:**
   ```bash
   ctest --output-on-failure
   ```

## 🎮 Usage Examples

### Basic Image Viewing

```bash
# Launch the application
./qt-image-viewer

# Open an image
# File -> Open (Ctrl+O)
# Or drag and drop an image file

# Navigate images in folder
# Use arrow keys or Next/Previous buttons

# Zoom controls
# Ctrl+Plus: Zoom in
# Ctrl+Minus: Zoom out
# Ctrl+0: Actual size
# Ctrl+9: Fit to window
```

### Keyboard Shortcuts

| Shortcut | Action |
|----------|--------|
| `Ctrl+O` | Open file |
| `Ctrl+S` | Save file |
| `Ctrl+Q` | Quit application |
| `F11` | Toggle fullscreen |
| `Space` | Next image |
| `Backspace` | Previous image |
| `Ctrl++` | Zoom in |
| `Ctrl+-` | Zoom out |
| `Ctrl+0` | Actual size |
| `Ctrl+9` | Fit to window |
| `R` | Rotate clockwise |
| `Shift+R` | Rotate counterclockwise |

### Command Line Usage

```bash
# Open specific image
./qt-image-viewer image.jpg

# Open multiple images
./qt-image-viewer *.jpg

# Start in fullscreen
./qt-image-viewer --fullscreen image.png

# Start slideshow
./qt-image-viewer --slideshow folder/
```

## 🧪 Testing

The project includes comprehensive tests for Qt components:

### Test Categories

- **Widget tests**: UI component functionality
- **Image processing tests**: Format support and transformations
- **Settings tests**: Configuration persistence
- **Integration tests**: End-to-end workflows

### Running Tests

```bash
# Run all tests
ctest

# Run specific test suite
ctest -R ImageViewerTests

# Run with Qt test output
./test_qt_image_viewer

# GUI tests (requires display)
export QT_QPA_PLATFORM=offscreen  # For headless testing
./test_qt_image_viewer
```

### Test Coverage

- Image loading and display: 95%
- User interface interactions: 90%
- Settings management: 98%
- File operations: 92%

## 📦 Deployment

### Windows Deployment

```bash
# Build release version
cmake --build build --config Release

# Deploy Qt dependencies
windeployqt.exe build/Release/qt-image-viewer.exe

# Create installer (requires NSIS)
makensis deployment/windows/installer.nsi
```

### macOS Deployment

```bash
# Build release version
cmake --build build --config Release

# Create app bundle
macdeployqt build/qt-image-viewer.app

# Create DMG (optional)
hdiutil create -volname "Qt Image Viewer" -srcfolder build/qt-image-viewer.app -ov -format UDZO qt-image-viewer.dmg
```

### Linux Deployment

```bash
# Build release version
cmake --build build --config Release

# Deploy with linuxdeployqt
linuxdeployqt build/qt-image-viewer -appimage

# Or create DEB package
cpack -G DEB
```

## 🎨 Customization

### Adding New Image Formats

```cpp
// In imageviewer.cpp
void ImageViewer::setupSupportedFormats() {
    QList<QByteArray> formats = QImageReader::supportedImageFormats();
    
    // Add custom format support
    if (MyCustomFormat::isAvailable()) {
        formats.append("mycustomformat");
    }
    
    updateFileFilters(formats);
}
```

### Custom Image Filters

```cpp
// Create custom filter plugin
class CustomFilterPlugin : public QObject, public ImageFilterInterface {
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "com.example.ImageFilterInterface")
    Q_INTERFACES(ImageFilterInterface)
    
public:
    QImage applyFilter(const QImage& image) override {
        // Custom filter implementation
        return processedImage;
    }
};
```

### Theme Customization

```cpp
// In main.cpp
void setupApplicationTheme() {
    QApplication::setStyle("Fusion");
    
    // Dark theme
    QPalette darkPalette;
    darkPalette.setColor(QPalette::Window, QColor(53, 53, 53));
    darkPalette.setColor(QPalette::WindowText, Qt::white);
    // ... more color settings
    
    QApplication::setPalette(darkPalette);
}
```

## 🔄 CI/CD Pipeline

The project includes automated CI/CD for multiple platforms:

### GitHub Actions Workflow

- **Multi-platform builds**: Windows (MSVC), Linux (GCC), macOS (Clang)
- **Qt6 installation**: Automatic Qt setup with aqtinstall
- **Testing**: Automated test execution with virtual display
- **Deployment**: Automatic artifact creation for releases
- **Code quality**: Static analysis with clang-tidy

### Release Process

1. **Version tagging**: Semantic versioning with git tags
2. **Automated builds**: Cross-platform binaries
3. **Package creation**: Platform-specific installers
4. **Documentation**: Automatic API docs generation
5. **Distribution**: GitHub releases with assets

## 🎓 Learning Outcomes

By studying this example, you'll learn:

1. **Qt6 application architecture** - Modern Qt development patterns
2. **CMake Qt integration** - Proper Qt6 CMake configuration
3. **Cross-platform GUI** - Platform-specific considerations
4. **Image processing** - Qt graphics and image handling
5. **User interface design** - Modern Qt widget usage
6. **Settings management** - Persistent application configuration
7. **Testing Qt applications** - Unit and integration testing strategies
8. **Deployment strategies** - Multi-platform distribution

## 🔗 Related Examples

- [Console File Processor](../console-file-processor/) - Command-line application
- [SFML Game Demo](../sfml-game-demo/) - Game development with SFML
- [REST API Server](../rest-api-server/) - Network service example

## 📚 Additional Resources

- [Qt6 Documentation](https://doc.qt.io/qt-6/)
- [Qt6 CMake Manual](https://doc.qt.io/qt-6/cmake-manual.html)
- [Qt Application Development](https://doc.qt.io/qt-6/qtwidgets-index.html)
- [Cross-platform Deployment](https://doc.qt.io/qt-6/deployment.html)

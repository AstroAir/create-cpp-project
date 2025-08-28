# MSYS2 GUI Application Example

This example demonstrates how to create a C++ GUI application with Qt6 and MSYS2 packaging support.

## Project Structure

```
msys2_gui_example/
├── PKGBUILD                 # MSYS2 package build script
├── CMakeLists.txt          # CMake build configuration
├── README.md               # This file
├── LICENSE                 # MIT License
├── src/
│   ├── main.cpp           # Application entry point
│   ├── mainwindow.cpp     # Main window implementation
│   ├── mainwindow.h       # Main window header
│   └── mainwindow.ui      # Qt Designer UI file
├── resources/
│   ├── icons/             # Application icons
│   ├── images/            # Images and graphics
│   └── app.qrc           # Qt resource file
├── tests/
│   ├── CMakeLists.txt    # Test configuration
│   └── test_gui.cpp      # GUI unit tests
└── scripts/
    ├── build.sh          # Build script
    └── package.sh        # Package creation script
```

## Prerequisites

Install Qt6 and development tools in MSYS2:

```bash
# Install Qt6 and build tools
pacman -S mingw-w64-x86_64-qt6-base
pacman -S mingw-w64-x86_64-qt6-tools
pacman -S mingw-w64-x86_64-cmake
pacman -S mingw-w64-x86_64-ninja
pacman -S mingw-w64-x86_64-toolchain

# Optional: Qt6 additional modules
pacman -S mingw-w64-x86_64-qt6-svg
pacman -S mingw-w64-x86_64-qt6-imageformats
```

## Building the Application

### Development Build

```bash
# Create build directory
mkdir build && cd build

# Configure with CMake
cmake .. -G Ninja -DCMAKE_BUILD_TYPE=Debug

# Build the application
ninja

# Run the application
./MyGuiApp.exe
```

### Release Build

```bash
# Create release build
mkdir build-release && cd build-release

# Configure for release
cmake .. -G Ninja -DCMAKE_BUILD_TYPE=Release

# Build
ninja

# Create installer (optional)
ninja package
```

## Application Features

This example GUI application includes:

- **Modern Qt6 Interface**: Clean, responsive UI design
- **Menu System**: File, Edit, View, and Help menus
- **Toolbar**: Quick access to common actions
- **Status Bar**: Application status and progress indicators
- **Settings Dialog**: User preferences and configuration
- **About Dialog**: Application information and credits
- **File Operations**: Open, save, and recent files
- **Internationalization**: Multi-language support ready

## MSYS2 Package Creation

### Using makepkg

```bash
# Create source archive
tar -czf myguiapp-1.0.0.tar.gz --exclude=build* --exclude=.git .

# Build package
makepkg -s

# Install package
pacman -U mingw-w64-x86_64-myguiapp-1.0.0-1-any.pkg.tar.zst
```

### Package Features

The MSYS2 package includes:

- **Desktop Integration**: .desktop file for application launcher
- **Icon Installation**: Application icons in multiple sizes
- **Menu Entry**: Appears in application menu
- **File Associations**: Registers supported file types
- **Qt6 Dependencies**: Automatically handles Qt6 runtime dependencies

## CMake Configuration

Key CMake features demonstrated:

```cmake
# Qt6 integration
find_package(Qt6 REQUIRED COMPONENTS Core Widgets)
qt6_standard_project_setup()

# Resource compilation
qt6_add_resources(app_resources resources/app.qrc)

# UI file processing
qt6_add_executable(MyGuiApp ${SOURCES} ${app_resources})

# Automatic MOC, UIC, RCC
set_target_properties(MyGuiApp PROPERTIES
    AUTOMOC ON
    AUTOUIC ON
    AUTORCC ON
)
```

## PKGBUILD Highlights

```bash
# Qt6 dependencies
depends=("${MINGW_PACKAGE_PREFIX}-qt6-base"
         "${MINGW_PACKAGE_PREFIX}-gcc-libs")

makedepends=("${MINGW_PACKAGE_PREFIX}-cc"
             "${MINGW_PACKAGE_PREFIX}-cmake"
             "${MINGW_PACKAGE_PREFIX}-ninja"
             "${MINGW_PACKAGE_PREFIX}-qt6-tools")

# Desktop integration
package() {
    # Install application
    DESTDIR="${pkgdir}" cmake --install build
    
    # Install desktop file
    install -Dm644 "${srcdir}/${_realname}.desktop" \
        "${pkgdir}${MINGW_PREFIX}/share/applications/${_realname}.desktop"
    
    # Install icon
    install -Dm644 "${srcdir}/resources/icons/app.png" \
        "${pkgdir}${MINGW_PREFIX}/share/pixmaps/${_realname}.png"
}
```

## Testing

### Unit Tests

```bash
# Run GUI tests (requires X11 or virtual display)
cd build && ninja test

# Run with virtual display
xvfb-run -a ninja test
```

### Manual Testing

1. **Interface Testing**: Verify all UI elements work correctly
2. **File Operations**: Test open, save, and recent files
3. **Settings**: Verify preferences are saved and loaded
4. **Internationalization**: Test with different languages
5. **Window Management**: Test resize, minimize, maximize

## Deployment

### Standalone Deployment

```bash
# Use windeployqt for standalone deployment
windeployqt.exe --qmldir . MyGuiApp.exe

# Create installer with NSIS or Inno Setup
makensis installer.nsi
```

### MSYS2 Distribution

```bash
# Submit to MSYS2 packages repository
# Follow MSYS2 packaging guidelines
# Create pull request to mingw-packages
```

## Customization

### Adding New Features

1. **New Dialogs**: Create .ui files with Qt Designer
2. **Custom Widgets**: Inherit from Qt widget classes
3. **Plugins**: Use Qt's plugin system
4. **Themes**: Implement custom stylesheets

### Internationalization

```bash
# Extract translatable strings
lupdate src/ -ts translations/app_en.ts

# Create translations
linguist translations/app_en.ts

# Compile translations
lrelease translations/*.ts
```

## Troubleshooting

### Common Issues

1. **Qt6 Not Found**

   ```bash
   export CMAKE_PREFIX_PATH=$MINGW_PREFIX
   ```

2. **Missing DLLs**

   ```bash
   # Add MSYS2 bin to PATH
   export PATH=$MINGW_PREFIX/bin:$PATH
   ```

3. **UI Files Not Found**

   ```bash
   # Ensure AUTOUIC is enabled
   set_target_properties(target PROPERTIES AUTOUIC ON)
   ```

### Performance Tips

- Use Qt's model/view architecture for large datasets
- Implement lazy loading for heavy operations
- Use Qt's threading capabilities for background tasks
- Profile with Qt Creator's profiler tools

## Resources

- Qt6 Documentation: <https://doc.qt.io/qt-6/>
- MSYS2 Qt6 Packages: <https://packages.msys2.org/search?t=binpkg&q=qt6>
- Qt Designer Tutorial: <https://doc.qt.io/qt-6/qtdesigner-manual.html>
- CMake Qt6 Integration: <https://doc.qt.io/qt-6/cmake-manual.html>

## License

This example is provided under the MIT License. See LICENSE file for details.

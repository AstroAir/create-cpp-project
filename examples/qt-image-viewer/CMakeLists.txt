cmake_minimum_required(VERSION 3.16)

project(qt-image-viewer
    VERSION 1.0.0
    DESCRIPTION "Qt6 Image Viewer Example"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Qt6
find_package(Qt6 QUIET COMPONENTS Core Widgets Gui)

if(Qt6_FOUND)
    message(STATUS "Qt6 found, building Qt Image Viewer")

    # Enable Qt's automatic MOC, UIC, and RCC processing
    set(CMAKE_AUTOMOC ON)
    set(CMAKE_AUTOUIC ON)
    set(CMAKE_AUTORCC ON)

    # Create executable
    add_executable(qt-image-viewer
        src/main.cpp
    )

    target_link_libraries(qt-image-viewer
        Qt6::Core
        Qt6::Widgets
        Qt6::Gui
    )

    # Set target properties
    set_target_properties(qt-image-viewer PROPERTIES
        WIN32_EXECUTABLE TRUE
        MACOSX_BUNDLE TRUE
    )
    
else()
    message(WARNING "Qt6 not found. Skipping Qt Image Viewer example.")
    
    # Create a simple placeholder executable that shows the error
    add_executable(qt-image-viewer-placeholder
        src/main.cpp
    )
    
    target_compile_definitions(qt-image-viewer-placeholder PRIVATE NO_QT_AVAILABLE)
    
endif()

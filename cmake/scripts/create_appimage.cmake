# AppImage creation script for CPack External generator
# This script is called by CPack to create AppImage packages

cmake_minimum_required(VERSION 3.20)

# Get variables from CPack
set(CPACK_PACKAGE_NAME "${CPACK_PACKAGE_NAME}")
set(CPACK_PACKAGE_VERSION "${CPACK_PACKAGE_VERSION}")
set(CPACK_TEMPORARY_DIRECTORY "${CPACK_TEMPORARY_DIRECTORY}")
set(CPACK_TOPLEVEL_DIRECTORY "${CPACK_TOPLEVEL_DIRECTORY}")

# Find required tools
find_program(LINUXDEPLOY_EXECUTABLE linuxdeploy)
if(NOT LINUXDEPLOY_EXECUTABLE)
    message(FATAL_ERROR "linuxdeploy not found. Please install it from https://github.com/linuxdeploy/linuxdeploy")
endif()

# Set up directories
set(APPDIR "${CPACK_TOPLEVEL_DIRECTORY}/${CPACK_PACKAGE_NAME}.AppDir")
set(OUTPUT_DIR "${CPACK_TOPLEVEL_DIRECTORY}")

message(STATUS "Creating AppImage for ${CPACK_PACKAGE_NAME}")
message(STATUS "AppDir: ${APPDIR}")
message(STATUS "Output: ${OUTPUT_DIR}")

# Create AppDir structure
file(MAKE_DIRECTORY "${APPDIR}")
file(MAKE_DIRECTORY "${APPDIR}/usr")

# Copy installed files to AppDir
execute_process(
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CPACK_TEMPORARY_DIRECTORY}"
    "${APPDIR}/usr"
    RESULT_VARIABLE COPY_RESULT
)

if(NOT COPY_RESULT EQUAL 0)
    message(FATAL_ERROR "Failed to copy files to AppDir")
endif()

# Create desktop file if it doesn't exist
set(DESKTOP_FILE "${APPDIR}/${CPACK_PACKAGE_NAME}.desktop")
if(NOT EXISTS "${DESKTOP_FILE}")
    file(WRITE "${DESKTOP_FILE}" "[Desktop Entry]
Version=1.0
Type=Application
Name=${CPACK_PACKAGE_NAME}
Comment=C++ Project Scaffolding Tool
Exec=${CPACK_PACKAGE_NAME}
Icon=${CPACK_PACKAGE_NAME}
Terminal=false
Categories=Development;
")
endif()

# Create AppRun script
set(APPRUN_FILE "${APPDIR}/AppRun")
file(WRITE "${APPRUN_FILE}" "#!/bin/bash
cd \"$(dirname \"$0\")\"
exec ./usr/bin/${CPACK_PACKAGE_NAME} \"$@\"
")

# Make AppRun executable
execute_process(
    COMMAND chmod +x "${APPRUN_FILE}"
    RESULT_VARIABLE CHMOD_RESULT
)

if(NOT CHMOD_RESULT EQUAL 0)
    message(FATAL_ERROR "Failed to make AppRun executable")
endif()

# Create AppImage
execute_process(
    COMMAND ${LINUXDEPLOY_EXECUTABLE}
    --appdir "${APPDIR}"
    --output appimage
    WORKING_DIRECTORY "${OUTPUT_DIR}"
    RESULT_VARIABLE APPIMAGE_RESULT
)

if(NOT APPIMAGE_RESULT EQUAL 0)
    message(FATAL_ERROR "Failed to create AppImage")
endif()

# Find the created AppImage and rename it properly
file(GLOB APPIMAGE_FILES "${OUTPUT_DIR}/*.AppImage")
if(APPIMAGE_FILES)
    list(GET APPIMAGE_FILES 0 APPIMAGE_FILE)
    get_filename_component(APPIMAGE_NAME "${APPIMAGE_FILE}" NAME)
    set(NEW_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-x86_64.AppImage")
    
    if(NOT "${APPIMAGE_NAME}" STREQUAL "${NEW_NAME}")
        file(RENAME "${APPIMAGE_FILE}" "${OUTPUT_DIR}/${NEW_NAME}")
        message(STATUS "Created AppImage: ${NEW_NAME}")
    endif()
else()
    message(FATAL_ERROR "AppImage creation failed - no AppImage file found")
endif()

message(STATUS "AppImage creation completed successfully")

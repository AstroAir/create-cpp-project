# Packaging.cmake
# Advanced package configuration and installation rules with modern packaging support
# Supports: NSIS, DEB, RPM, DMG, AppImage, ZIP, TGZ, MSI, MSYS2, and more

# Package signing configuration with enhanced security
function(configure_package_signing)
    set(options ENABLE_SIGNING VERIFY_SIGNATURES)
    set(oneValueArgs WINDOWS_CERT_PATH WINDOWS_CERT_PASSWORD MACOS_IDENTITY LINUX_GPG_KEY TIMESTAMP_SERVER)
    set(multiValueArgs WINDOWS_CERT_STORE MACOS_KEYCHAIN)
    cmake_parse_arguments(SIGN "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if(SIGN_ENABLE_SIGNING)
        set(CPACK_PACKAGE_SIGNING_ENABLED ON PARENT_SCOPE)
        message(STATUS "Package signing enabled")

        # Windows code signing
        if(WIN32)
            configure_windows_signing("${SIGN_WINDOWS_CERT_PATH}" "${SIGN_WINDOWS_CERT_PASSWORD}" "${SIGN_TIMESTAMP_SERVER}")
        endif()

        # macOS code signing
        if(APPLE)
            configure_macos_signing("${SIGN_MACOS_IDENTITY}" "${SIGN_MACOS_KEYCHAIN}")
        endif()

        # Linux package signing
        if(UNIX AND NOT APPLE)
            configure_linux_signing("${SIGN_LINUX_GPG_KEY}")
        endif()

        # Create signature verification script
        if(SIGN_VERIFY_SIGNATURES)
            create_signature_verification_script()
        endif()
    else()
        message(STATUS "Package signing disabled")
    endif()
endfunction()

# Windows-specific signing configuration
function(configure_windows_signing cert_path cert_password timestamp_server)
    if(cert_path AND EXISTS "${cert_path}")
        # Set default timestamp server if not provided
        if(NOT timestamp_server)
            set(timestamp_server "http://timestamp.digicert.com")
        endif()

        # Configure signtool command
        set(SIGNTOOL_COMMAND "signtool sign /f \"${cert_path}\"")

        if(cert_password)
            string(APPEND SIGNTOOL_COMMAND " /p \"${cert_password}\"")
        endif()

        string(APPEND SIGNTOOL_COMMAND " /t \"${timestamp_server}\" /v")

        set(CPACK_NSIS_SIGN_TOOL "${SIGNTOOL_COMMAND}" PARENT_SCOPE)
        set(CPACK_WIX_SIGN_TOOL "${SIGNTOOL_COMMAND}" PARENT_SCOPE)

        message(STATUS "Windows code signing configured with certificate: ${cert_path}")
    else()
        message(WARNING "Windows certificate not found or not specified: ${cert_path}")
    endif()
endfunction()

# macOS-specific signing configuration
function(configure_macos_signing identity keychain)
    if(identity)
        set(CPACK_BUNDLE_APPLE_CERT_APP "${identity}" PARENT_SCOPE)

        if(keychain)
            set(CPACK_BUNDLE_APPLE_KEYCHAIN "${keychain}" PARENT_SCOPE)
        endif()

        # Enable hardened runtime
        set(CPACK_BUNDLE_APPLE_CODESIGN_PARAMETER "--options runtime" PARENT_SCOPE)

        message(STATUS "macOS code signing configured with identity: ${identity}")

        # Check for notarization credentials
        if(DEFINED ENV{APPLE_ID} AND DEFINED ENV{APPLE_PASSWORD})
            set(CPACK_BUNDLE_APPLE_NOTARIZE ON PARENT_SCOPE)
            message(STATUS "macOS notarization enabled")
        endif()
    else()
        message(WARNING "macOS signing identity not specified")
    endif()
endfunction()

# Linux-specific signing configuration
function(configure_linux_signing gpg_key)
    if(gpg_key)
        set(CPACK_RPM_PACKAGE_SIGN ON PARENT_SCOPE)
        set(CPACK_DEBIAN_PACKAGE_SIGN ON PARENT_SCOPE)
        set(CPACK_RPM_GPG_NAME "${gpg_key}" PARENT_SCOPE)
        set(CPACK_DEBIAN_PACKAGE_SIGN_GPG_KEY "${gpg_key}" PARENT_SCOPE)

        message(STATUS "Linux package signing configured with GPG key: ${gpg_key}")
    else()
        message(WARNING "Linux GPG key not specified")
    endif()
endfunction()

# Create signature verification script
function(create_signature_verification_script)
    set(VERIFY_SCRIPT "${CMAKE_BINARY_DIR}/verify_signatures.cmake")

    file(WRITE "${VERIFY_SCRIPT}" "
# Package signature verification script
cmake_minimum_required(VERSION 3.20)

message(STATUS \"Verifying package signatures...\")

# Find all package files
file(GLOB PACKAGE_FILES
    \"${CMAKE_BINARY_DIR}/*.exe\"
    \"${CMAKE_BINARY_DIR}/*.msi\"
    \"${CMAKE_BINARY_DIR}/*.dmg\"
    \"${CMAKE_BINARY_DIR}/*.deb\"
    \"${CMAKE_BINARY_DIR}/*.rpm\"
    \"${CMAKE_BINARY_DIR}/*.AppImage\"
)

foreach(PACKAGE_FILE \${PACKAGE_FILES})
    get_filename_component(PACKAGE_NAME \"\${PACKAGE_FILE}\" NAME)
    message(STATUS \"Checking: \${PACKAGE_NAME}\")

    # Platform-specific verification
    if(\"\${PACKAGE_FILE}\" MATCHES \"\\.(exe|msi)$\")
        # Windows: Check digital signature
        execute_process(
            COMMAND signtool verify /pa \"\${PACKAGE_FILE}\"
            RESULT_VARIABLE VERIFY_RESULT
            OUTPUT_QUIET ERROR_QUIET
        )
        if(VERIFY_RESULT EQUAL 0)
            message(STATUS \"  ✓ Windows signature valid\")
        else()
            message(WARNING \"  ✗ Windows signature invalid or missing\")
        endif()

    elseif(\"\${PACKAGE_FILE}\" MATCHES \"\\.dmg$\")
        # macOS: Check code signature
        execute_process(
            COMMAND codesign --verify --verbose \"\${PACKAGE_FILE}\"
            RESULT_VARIABLE VERIFY_RESULT
            OUTPUT_QUIET ERROR_QUIET
        )
        if(VERIFY_RESULT EQUAL 0)
            message(STATUS \"  ✓ macOS signature valid\")
        else()
            message(WARNING \"  ✗ macOS signature invalid or missing\")
        endif()

    elseif(\"\${PACKAGE_FILE}\" MATCHES \"\\.(deb|rpm)$\")
        # Linux: Check package signature
        if(\"\${PACKAGE_FILE}\" MATCHES \"\\.deb$\")
            execute_process(
                COMMAND dpkg-sig --verify \"\${PACKAGE_FILE}\"
                RESULT_VARIABLE VERIFY_RESULT
                OUTPUT_QUIET ERROR_QUIET
            )
        else()
            execute_process(
                COMMAND rpm --checksig \"\${PACKAGE_FILE}\"
                RESULT_VARIABLE VERIFY_RESULT
                OUTPUT_QUIET ERROR_QUIET
            )
        endif()

        if(VERIFY_RESULT EQUAL 0)
            message(STATUS \"  ✓ Linux package signature valid\")
        else()
            message(WARNING \"  ✗ Linux package signature invalid or missing\")
        endif()
    endif()
endforeach()

message(STATUS \"Signature verification completed\")
")

    # Add custom target for signature verification
    add_custom_target(verify_signatures
        COMMAND ${CMAKE_COMMAND} -P "${VERIFY_SCRIPT}"
        COMMENT "Verifying package signatures"
    )

    message(STATUS "Created signature verification script: ${VERIFY_SCRIPT}")
endfunction()

# Configure Installation
function(configure_installation target_name)
    set(options)
    set(oneValueArgs DESTINATION COMPONENT)
    set(multiValueArgs FILES DIRECTORIES)
    cmake_parse_arguments(INSTALL "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    # Install the target
    if(INSTALL_DESTINATION)
        install(TARGETS ${target_name}
            RUNTIME DESTINATION ${INSTALL_DESTINATION}
            LIBRARY DESTINATION ${INSTALL_DESTINATION}
            ARCHIVE DESTINATION ${INSTALL_DESTINATION}
        )
    else()
        install(TARGETS ${target_name} RUNTIME DESTINATION bin)
    endif()

    # Install additional files
    if(INSTALL_FILES)
        if(INSTALL_DESTINATION)
            install(FILES ${INSTALL_FILES} DESTINATION ${INSTALL_DESTINATION})
        else()
            install(FILES ${INSTALL_FILES} DESTINATION share/${target_name})
        endif()
    endif()

    # Install directories
    if(INSTALL_DIRECTORIES)
        foreach(dir ${INSTALL_DIRECTORIES})
            if(INSTALL_DESTINATION)
                install(DIRECTORY ${dir} DESTINATION ${INSTALL_DESTINATION})
            else()
                install(DIRECTORY ${dir} DESTINATION share/${target_name})
            endif()
        endforeach()
    endif()
endfunction()

# Configure CPack
function(configure_cpack project_name project_version)
    set(options)
    set(oneValueArgs VENDOR DESCRIPTION SUMMARY INSTALL_DIRECTORY)
    set(multiValueArgs)
    cmake_parse_arguments(CPACK "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    # Basic package information
    set(CPACK_PACKAGE_NAME ${project_name})
    set(CPACK_PACKAGE_VERSION ${project_version})

    if(CPACK_VENDOR)
        set(CPACK_PACKAGE_VENDOR ${CPACK_VENDOR})
    else()
        set(CPACK_PACKAGE_VENDOR "${project_name} Team")
    endif()

    if(CPACK_DESCRIPTION)
        set(CPACK_PACKAGE_DESCRIPTION_SUMMARY ${CPACK_DESCRIPTION})
    else()
        set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "C++ Project")
    endif()

    # Parse version components
    string(REPLACE "." ";" VERSION_LIST ${project_version})
    list(GET VERSION_LIST 0 CPACK_PACKAGE_VERSION_MAJOR)
    if(VERSION_LIST)
        list(LENGTH VERSION_LIST VERSION_LIST_LENGTH)
        if(VERSION_LIST_LENGTH GREATER 1)
            list(GET VERSION_LIST 1 CPACK_PACKAGE_VERSION_MINOR)
        else()
            set(CPACK_PACKAGE_VERSION_MINOR 0)
        endif()
        if(VERSION_LIST_LENGTH GREATER 2)
            list(GET VERSION_LIST 2 CPACK_PACKAGE_VERSION_PATCH)
        else()
            set(CPACK_PACKAGE_VERSION_PATCH 0)
        endif()
    endif()

    if(CPACK_INSTALL_DIRECTORY)
        set(CPACK_PACKAGE_INSTALL_DIRECTORY ${CPACK_INSTALL_DIRECTORY})
    else()
        set(CPACK_PACKAGE_INSTALL_DIRECTORY ${project_name})
    endif()

    # Platform-specific packaging options with modern formats
    if(WIN32)
        set(CPACK_GENERATOR "NSIS;ZIP;WIX")

        # Enhanced NSIS configuration
        set(CPACK_NSIS_MODIFY_PATH ON)
        set(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL ON)
        set(CPACK_NSIS_DISPLAY_NAME ${project_name})
        set(CPACK_NSIS_PACKAGE_NAME ${project_name})
        set(CPACK_NSIS_HELP_LINK "https://github.com/cpp-scaffold/${project_name}")
        set(CPACK_NSIS_URL_INFO_ABOUT "https://github.com/cpp-scaffold/${project_name}")
        set(CPACK_NSIS_CONTACT "support@cpp-scaffold.org")
        set(CPACK_NSIS_MUI_ICON "${CMAKE_SOURCE_DIR}/resources/icon.ico")
        set(CPACK_NSIS_MUI_UNIICON "${CMAKE_SOURCE_DIR}/resources/icon.ico")
        set(CPACK_NSIS_INSTALLED_ICON_NAME "bin\\\\${project_name}.exe")
        set(CPACK_NSIS_CREATE_ICONS_EXTRA "CreateShortCut '$SMPROGRAMS\\\\$STARTMENU_FOLDER\\\\${project_name}.lnk' '$INSTDIR\\\\bin\\\\${project_name}.exe'")
        set(CPACK_NSIS_DELETE_ICONS_EXTRA "Delete '$SMPROGRAMS\\\\$START_MENU\\\\${project_name}.lnk'")

        # WiX MSI configuration
        set(CPACK_WIX_UPGRADE_GUID "12345678-1234-1234-1234-123456789012")
        set(CPACK_WIX_PRODUCT_ICON "${CMAKE_SOURCE_DIR}/resources/icon.ico")
        set(CPACK_WIX_UI_BANNER "${CMAKE_SOURCE_DIR}/resources/banner.bmp")
        set(CPACK_WIX_UI_DIALOG "${CMAKE_SOURCE_DIR}/resources/dialog.bmp")

    elseif(APPLE)
        set(CPACK_GENERATOR "DragNDrop;TGZ;Bundle")

        # Enhanced DMG configuration
        set(CPACK_DMG_FORMAT "UDBZ")
        set(CPACK_DMG_VOLUME_NAME ${project_name})
        set(CPACK_DMG_BACKGROUND_IMAGE "${CMAKE_SOURCE_DIR}/resources/dmg_background.png")
        set(CPACK_DMG_DS_STORE_SETUP_SCRIPT "${CMAKE_SOURCE_DIR}/resources/dmg_setup.scpt")
        set(CPACK_SYSTEM_NAME "macOS")

        # Bundle configuration
        set(CPACK_BUNDLE_NAME ${project_name})
        set(CPACK_BUNDLE_ICON "${CMAKE_SOURCE_DIR}/resources/icon.icns")
        set(CPACK_BUNDLE_PLIST "${CMAKE_SOURCE_DIR}/resources/Info.plist")

    else()
        set(CPACK_GENERATOR "DEB;RPM;TGZ")

        # Check for MSYS2 environment
        if(DEFINED ENV{MSYSTEM} OR DEFINED ENV{MINGW_PREFIX})
            list(APPEND CPACK_GENERATOR "External")
            set(CPACK_EXTERNAL_PACKAGE_SCRIPT "${CMAKE_SOURCE_DIR}/cmake/scripts/create_msys2_package.cmake")
            message(STATUS "MSYS2 environment detected, adding MSYS2 packaging support")
        endif()

        # Check for AppImage tools
        find_program(LINUXDEPLOY_EXECUTABLE linuxdeploy)
        if(LINUXDEPLOY_EXECUTABLE)
            list(APPEND CPACK_GENERATOR "External")
            set(CPACK_EXTERNAL_PACKAGE_SCRIPT "${CMAKE_SOURCE_DIR}/cmake/scripts/create_appimage.cmake")
        endif()

        # Enhanced Debian package configuration
        set(CPACK_DEBIAN_PACKAGE_MAINTAINER "CPP-Scaffold Team <support@cpp-scaffold.org>")
        set(CPACK_DEBIAN_PACKAGE_SECTION "devel")
        set(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")
        set(CPACK_DEBIAN_PACKAGE_HOMEPAGE "https://github.com/cpp-scaffold/${project_name}")
        set(CPACK_DEBIAN_PACKAGE_DEPENDS "libc6 (>= 2.17), libstdc++6 (>= 4.8)")
        set(CPACK_DEBIAN_PACKAGE_RECOMMENDS "cmake (>= 3.14), ninja-build")
        set(CPACK_DEBIAN_PACKAGE_SUGGESTS "git, build-essential")
        set(CPACK_DEBIAN_PACKAGE_CONTROL_EXTRA "${CMAKE_SOURCE_DIR}/packaging/debian/postinst;${CMAKE_SOURCE_DIR}/packaging/debian/prerm")

        # Enhanced RPM package configuration
        set(CPACK_RPM_PACKAGE_GROUP "Development/Tools")
        set(CPACK_RPM_PACKAGE_LICENSE "GPL-3.0")
        set(CPACK_RPM_PACKAGE_URL "https://github.com/cpp-scaffold/${project_name}")
        set(CPACK_RPM_PACKAGE_REQUIRES "glibc >= 2.17, libstdc++ >= 4.8")
        set(CPACK_RPM_PACKAGE_SUGGESTS "cmake >= 3.14, ninja-build")
        set(CPACK_RPM_POST_INSTALL_SCRIPT_FILE "${CMAKE_SOURCE_DIR}/packaging/rpm/postinstall.sh")
        set(CPACK_RPM_PRE_UNINSTALL_SCRIPT_FILE "${CMAKE_SOURCE_DIR}/packaging/rpm/preuninstall.sh")
    endif()

    include(CPack)
endfunction()

# Setup Standard Installation
function(setup_standard_installation project_name)
    # Install main executable
    configure_installation(${project_name}
        DESTINATION bin
    )

    # Install documentation
    configure_installation(${project_name}
        FILES README.md LICENSE CHANGELOG.md
        DESTINATION share/${project_name}
    )
endfunction()

# Create Development Package
function(create_development_package target_name)
    set(options)
    set(oneValueArgs NAMESPACE)
    set(multiValueArgs HEADERS INCLUDE_DIRS)
    cmake_parse_arguments(DEV "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    # Install headers
    if(DEV_HEADERS)
        install(FILES ${DEV_HEADERS} DESTINATION include/${target_name})
    endif()

    # Install include directories
    if(DEV_INCLUDE_DIRS)
        foreach(include_dir ${DEV_INCLUDE_DIRS})
            install(DIRECTORY ${include_dir}/ DESTINATION include/${target_name}
                FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp" PATTERN "*.hxx"
            )
        endforeach()
    endif()

    # Create and install CMake config files
    set(CONFIG_INSTALL_DIR lib/cmake/${target_name})

    # Generate config file
    configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Config.cmake.in"
        "${CMAKE_CURRENT_BINARY_DIR}/${target_name}Config.cmake"
        @ONLY
    )

    # Generate version file
    write_basic_package_version_file(
        "${CMAKE_CURRENT_BINARY_DIR}/${target_name}ConfigVersion.cmake"
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY SameMajorVersion
    )

    # Install config files
    install(FILES
        "${CMAKE_CURRENT_BINARY_DIR}/${target_name}Config.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/${target_name}ConfigVersion.cmake"
        DESTINATION ${CONFIG_INSTALL_DIR}
    )

    # Export targets
    if(DEV_NAMESPACE)
        set(EXPORT_NAMESPACE ${DEV_NAMESPACE})
    else()
        set(EXPORT_NAMESPACE ${target_name})
    endif()

    install(TARGETS ${target_name}
        EXPORT ${target_name}Targets
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin
        INCLUDES DESTINATION include
    )

    install(EXPORT ${target_name}Targets
        FILE ${target_name}Targets.cmake
        NAMESPACE ${EXPORT_NAMESPACE}::
        DESTINATION ${CONFIG_INSTALL_DIR}
    )
endfunction()

# Create AppImage package (Linux only)
function(create_appimage_package target_name)
    if(NOT UNIX OR APPLE)
        return()
    endif()

    find_program(LINUXDEPLOY_EXECUTABLE linuxdeploy)
    if(NOT LINUXDEPLOY_EXECUTABLE)
        message(WARNING "linuxdeploy not found, AppImage creation will be skipped")
        return()
    endif()

    set(APPIMAGE_DIR "${CMAKE_BINARY_DIR}/AppImage")
    set(APPDIR "${APPIMAGE_DIR}/${target_name}.AppDir")

    # Create AppDir structure
    file(MAKE_DIRECTORY "${APPDIR}")
    file(MAKE_DIRECTORY "${APPDIR}/usr/bin")
    file(MAKE_DIRECTORY "${APPDIR}/usr/share/applications")
    file(MAKE_DIRECTORY "${APPDIR}/usr/share/icons/hicolor/256x256/apps")

    # Install executable
    install(TARGETS ${target_name} DESTINATION "${APPDIR}/usr/bin")

    # Create desktop file
    configure_file(
        "${CMAKE_SOURCE_DIR}/resources/${target_name}.desktop.in"
        "${APPDIR}/${target_name}.desktop"
        @ONLY
    )

    # Copy icon
    if(EXISTS "${CMAKE_SOURCE_DIR}/resources/icon.png")
        configure_file(
            "${CMAKE_SOURCE_DIR}/resources/icon.png"
            "${APPDIR}/usr/share/icons/hicolor/256x256/apps/${target_name}.png"
            COPYONLY
        )
        configure_file(
            "${CMAKE_SOURCE_DIR}/resources/icon.png"
            "${APPDIR}/${target_name}.png"
            COPYONLY
        )
    endif()

    # Create AppRun script
    file(WRITE "${APPDIR}/AppRun" "#!/bin/bash\ncd \"$(dirname \"$0\")\"\nexec ./usr/bin/${target_name} \"$@\"\n")
    execute_process(COMMAND chmod +x "${APPDIR}/AppRun")

    # Add custom target to create AppImage
    add_custom_target(appimage
        COMMAND ${LINUXDEPLOY_EXECUTABLE} --appdir "${APPDIR}" --output appimage
        WORKING_DIRECTORY "${APPIMAGE_DIR}"
        DEPENDS ${target_name}
        COMMENT "Creating AppImage for ${target_name}"
    )
endfunction()

# Configure package metadata and dependencies
function(configure_package_metadata project_name)
    set(options)
    set(oneValueArgs DESCRIPTION SUMMARY HOMEPAGE MAINTAINER LICENSE)
    set(multiValueArgs KEYWORDS CATEGORIES)
    cmake_parse_arguments(META "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    # Set comprehensive package metadata
    if(META_DESCRIPTION)
        set(CPACK_PACKAGE_DESCRIPTION "${META_DESCRIPTION}" PARENT_SCOPE)
        set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "${META_DESCRIPTION}" PARENT_SCOPE)
    endif()

    if(META_HOMEPAGE)
        set(CPACK_PACKAGE_HOMEPAGE_URL "${META_HOMEPAGE}" PARENT_SCOPE)
        set(CPACK_NSIS_URL_INFO_ABOUT "${META_HOMEPAGE}" PARENT_SCOPE)
        set(CPACK_DEBIAN_PACKAGE_HOMEPAGE "${META_HOMEPAGE}" PARENT_SCOPE)
        set(CPACK_RPM_PACKAGE_URL "${META_HOMEPAGE}" PARENT_SCOPE)
    endif()

    if(META_MAINTAINER)
        set(CPACK_PACKAGE_CONTACT "${META_MAINTAINER}" PARENT_SCOPE)
        set(CPACK_DEBIAN_PACKAGE_MAINTAINER "${META_MAINTAINER}" PARENT_SCOPE)
        set(CPACK_NSIS_CONTACT "${META_MAINTAINER}" PARENT_SCOPE)
    endif()

    if(META_LICENSE)
        set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/LICENSE" PARENT_SCOPE)
        set(CPACK_RPM_PACKAGE_LICENSE "${META_LICENSE}" PARENT_SCOPE)
    endif()

    # Set keywords for package managers
    if(META_KEYWORDS)
        string(JOIN "," KEYWORDS_STRING ${META_KEYWORDS})
        set(CPACK_DEBIAN_PACKAGE_KEYWORDS "${KEYWORDS_STRING}" PARENT_SCOPE)
    endif()
endfunction()

# Setup Complete Packaging with Advanced Features
function(setup_complete_packaging project_name project_version)
    setup_standard_installation(${project_name})

    # Configure comprehensive metadata
    configure_package_metadata(${project_name}
        DESCRIPTION "A powerful, modern C++ project scaffolding tool that helps developers quickly create well-structured C++ projects with industry best practices"
        HOMEPAGE "https://github.com/cpp-scaffold/cpp-scaffold"
        MAINTAINER "CPP-Scaffold Team <support@cpp-scaffold.org>"
        LICENSE "GPL-3.0"
        KEYWORDS "cpp" "cmake" "scaffolding" "development" "build-tools"
    )

    configure_cpack(${project_name} ${project_version}
        VENDOR "CPP-Scaffold Team"
        DESCRIPTION "C++ Project Scaffolding Tool"
        INSTALL_DIRECTORY ${project_name}
    )

    # Create AppImage for Linux
    create_appimage_package(${project_name})

    # Enable package signing if configured
    if(DEFINED ENV{ENABLE_PACKAGE_SIGNING})
        configure_package_signing(ENABLE_SIGNING
            WINDOWS_CERT_PATH "$ENV{WINDOWS_CERT_PATH}"
            WINDOWS_CERT_PASSWORD "$ENV{WINDOWS_CERT_PASSWORD}"
            MACOS_IDENTITY "$ENV{MACOS_IDENTITY}"
            LINUX_GPG_KEY "$ENV{LINUX_GPG_KEY}"
        )
    endif()

    # Configure platform-specific optimizations
    configure_platform_optimizations(${project_name})

    # Enable package testing if requested
    if(ENABLE_PACKAGE_TESTING OR CMAKE_BUILD_TYPE STREQUAL "Debug")
        include(PackageTesting)
        create_package_test_suite(TIMEOUT 600)
    endif()
endfunction()

# Configure platform-specific optimizations and features
function(configure_platform_optimizations project_name)
    if(WIN32)
        configure_windows_packaging(${project_name})
    elseif(APPLE)
        configure_macos_packaging(${project_name})
    else()
        configure_linux_packaging(${project_name})

        # Check for MSYS2 environment and add MSYS2 packaging
        if(DEFINED ENV{MSYSTEM} OR DEFINED ENV{MINGW_PREFIX})
            configure_msys2_packaging(${project_name})
        endif()
    endif()
endfunction()

# Windows-specific packaging configuration
function(configure_windows_packaging project_name)
    # Enhanced NSIS configuration
    set(CPACK_NSIS_EXECUTABLES_DIRECTORY "bin" PARENT_SCOPE)
    set(CPACK_NSIS_MUI_FINISHPAGE_RUN "${project_name}.exe" PARENT_SCOPE)
    set(CPACK_NSIS_COMPRESSOR "/SOLID lzma" PARENT_SCOPE)

    # Registry entries for uninstaller
    set(CPACK_NSIS_EXTRA_INSTALL_COMMANDS "
        WriteRegStr HKLM 'Software\\\\Microsoft\\\\Windows\\\\CurrentVersion\\\\Uninstall\\\\${project_name}' 'DisplayName' '${project_name}'
        WriteRegStr HKLM 'Software\\\\Microsoft\\\\Windows\\\\CurrentVersion\\\\Uninstall\\\\${project_name}' 'UninstallString' '$INSTDIR\\\\Uninstall.exe'
        WriteRegStr HKLM 'Software\\\\Microsoft\\\\Windows\\\\CurrentVersion\\\\Uninstall\\\\${project_name}' 'InstallLocation' '$INSTDIR'
        WriteRegStr HKLM 'Software\\\\Microsoft\\\\Windows\\\\CurrentVersion\\\\Uninstall\\\\${project_name}' 'DisplayVersion' '${PROJECT_VERSION}'
        WriteRegStr HKLM 'Software\\\\Microsoft\\\\Windows\\\\CurrentVersion\\\\Uninstall\\\\${project_name}' 'Publisher' 'CPP-Scaffold Team'
        WriteRegStr HKLM 'Software\\\\Microsoft\\\\Windows\\\\CurrentVersion\\\\Uninstall\\\\${project_name}' 'HelpLink' 'https://github.com/cpp-scaffold/cpp-scaffold'
        WriteRegStr HKLM 'Software\\\\Microsoft\\\\Windows\\\\CurrentVersion\\\\Uninstall\\\\${project_name}' 'URLInfoAbout' 'https://github.com/cpp-scaffold/cpp-scaffold'
        WriteRegDWORD HKLM 'Software\\\\Microsoft\\\\Windows\\\\CurrentVersion\\\\Uninstall\\\\${project_name}' 'NoModify' 1
        WriteRegDWORD HKLM 'Software\\\\Microsoft\\\\Windows\\\\CurrentVersion\\\\Uninstall\\\\${project_name}' 'NoRepair' 1
        " PARENT_SCOPE)

    set(CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS "
        DeleteRegKey HKLM 'Software\\\\Microsoft\\\\Windows\\\\CurrentVersion\\\\Uninstall\\\\${project_name}'
        " PARENT_SCOPE)

    # Add to PATH option
    set(CPACK_NSIS_MODIFY_PATH ON PARENT_SCOPE)

    # File associations
    set(CPACK_NSIS_DEFINES "
        !define MUI_FINISHPAGE_SHOWREADME_TEXT 'Create Desktop Shortcut'
        !define MUI_FINISHPAGE_SHOWREADME_FUNCTION CreateDesktopShortcut
        " PARENT_SCOPE)

    # WiX MSI specific settings
    set(CPACK_WIX_PROPERTY_ARPURLINFOABOUT "https://github.com/cpp-scaffold/cpp-scaffold" PARENT_SCOPE)
    set(CPACK_WIX_PROPERTY_ARPHELPLINK "https://github.com/cpp-scaffold/cpp-scaffold/issues" PARENT_SCOPE)
    set(CPACK_WIX_PROPERTY_ARPCONTACT "support@cpp-scaffold.org" PARENT_SCOPE)
endfunction()

# macOS-specific packaging configuration
function(configure_macos_packaging project_name)
    # Bundle configuration
    set(CPACK_BUNDLE_STARTUP_COMMAND "${project_name}" PARENT_SCOPE)

    # DMG configuration
    set(CPACK_DMG_SLA_USE_RESOURCE_FILE_LICENSE ON PARENT_SCOPE)
    set(CPACK_DMG_SLA_LANGUAGES "en" PARENT_SCOPE)

    # Code signing configuration (if available)
    if(DEFINED ENV{MACOS_IDENTITY})
        set(CPACK_BUNDLE_APPLE_CERT_APP "$ENV{MACOS_IDENTITY}" PARENT_SCOPE)
        set(CPACK_BUNDLE_APPLE_ENTITLEMENTS "${CMAKE_SOURCE_DIR}/resources/entitlements.plist" PARENT_SCOPE)
    endif()

    # Notarization (if configured)
    if(DEFINED ENV{APPLE_ID} AND DEFINED ENV{APPLE_PASSWORD})
        set(CPACK_BUNDLE_APPLE_NOTARIZE ON PARENT_SCOPE)
    endif()
endfunction()

# Linux-specific packaging configuration
function(configure_linux_packaging project_name)
    # Enhanced Debian configuration
    set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "amd64" PARENT_SCOPE)
    set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON PARENT_SCOPE)
    set(CPACK_DEBIAN_PACKAGE_GENERATE_SHLIBS ON PARENT_SCOPE)

    # Enhanced RPM configuration
    set(CPACK_RPM_PACKAGE_ARCHITECTURE "x86_64" PARENT_SCOPE)
    set(CPACK_RPM_PACKAGE_AUTOREQ ON PARENT_SCOPE)
    set(CPACK_RPM_PACKAGE_AUTOPROV ON PARENT_SCOPE)

    # Desktop integration
    set(CPACK_DEBIAN_PACKAGE_SECTION "devel" PARENT_SCOPE)
    set(CPACK_RPM_PACKAGE_GROUP "Development/Tools" PARENT_SCOPE)

    # Install desktop file and icon
    install(FILES "${CMAKE_SOURCE_DIR}/resources/${project_name}.desktop.in"
        DESTINATION share/applications
        RENAME "${project_name}.desktop"
    )

    if(EXISTS "${CMAKE_SOURCE_DIR}/resources/icon.png")
        install(FILES "${CMAKE_SOURCE_DIR}/resources/icon.png"
            DESTINATION share/icons/hicolor/256x256/apps
            RENAME "${project_name}.png"
        )
    endif()

    # Man page installation
    if(EXISTS "${CMAKE_SOURCE_DIR}/docs/${project_name}.1")
        install(FILES "${CMAKE_SOURCE_DIR}/docs/${project_name}.1"
            DESTINATION share/man/man1
        )
    endif()
endfunction()

# MSYS2-specific packaging configuration
function(configure_msys2_packaging project_name)
    # MSYS2 package configuration
    set(CPACK_MSYS2_PACKAGE_NAME "mingw-w64-${project_name}" PARENT_SCOPE)
    set(CPACK_MSYS2_PACKAGE_ARCHITECTURE "any" PARENT_SCOPE)
    set(CPACK_MSYS2_MINGW_ARCH "mingw32;mingw64;ucrt64;clang64;clangarm64" PARENT_SCOPE)

    # Set MSYS2-specific metadata
    set(CPACK_MSYS2_PACKAGE_MAINTAINER "Your Name <your.email@example.com>" PARENT_SCOPE)
    set(CPACK_MSYS2_PACKAGE_LICENSE "MIT" PARENT_SCOPE)
    set(CPACK_MSYS2_PACKAGE_URL "https://github.com/yourname/${project_name}" PARENT_SCOPE)

    # MSYS2 dependencies
    set(CPACK_MSYS2_PACKAGE_DEPENDS "\${MINGW_PACKAGE_PREFIX}-gcc-libs" PARENT_SCOPE)
    set(CPACK_MSYS2_PACKAGE_MAKEDEPENDS "\${MINGW_PACKAGE_PREFIX}-cc;\${MINGW_PACKAGE_PREFIX}-cmake;\${MINGW_PACKAGE_PREFIX}-ninja" PARENT_SCOPE)

    # Create MSYS2 package creation script
    if(EXISTS "${CMAKE_SOURCE_DIR}/cmake/templates/create_msys2_package.cmake.in")
        configure_file(
            "${CMAKE_SOURCE_DIR}/cmake/templates/create_msys2_package.cmake.in"
            "${CMAKE_BINARY_DIR}/cmake/scripts/create_msys2_package.cmake"
            @ONLY
        )
    endif()

    message(STATUS "MSYS2 packaging configured for ${project_name}")
endfunction()

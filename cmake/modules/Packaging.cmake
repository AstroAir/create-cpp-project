# Packaging.cmake
# Package configuration and installation rules

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

    # Platform-specific packaging options
    if(WIN32)
        set(CPACK_GENERATOR "NSIS;ZIP")
        set(CPACK_NSIS_MODIFY_PATH ON)
        set(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL ON)
        set(CPACK_NSIS_DISPLAY_NAME ${project_name})
        set(CPACK_NSIS_PACKAGE_NAME ${project_name})
        set(CPACK_NSIS_HELP_LINK "https://github.com/your-org/${project_name}")
        set(CPACK_NSIS_URL_INFO_ABOUT "https://github.com/your-org/${project_name}")
        set(CPACK_NSIS_CONTACT "your-email@example.com")
    elseif(APPLE)
        set(CPACK_GENERATOR "DragNDrop;TGZ")
        set(CPACK_DMG_FORMAT "UDBZ")
        set(CPACK_DMG_VOLUME_NAME ${project_name})
        set(CPACK_SYSTEM_NAME "macOS")
    else()
        set(CPACK_GENERATOR "DEB;RPM;TGZ")
        
        # Debian package configuration
        set(CPACK_DEBIAN_PACKAGE_MAINTAINER "${CPACK_PACKAGE_VENDOR}")
        set(CPACK_DEBIAN_PACKAGE_SECTION "devel")
        set(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")
        set(CPACK_DEBIAN_PACKAGE_HOMEPAGE "https://github.com/your-org/${project_name}")
        
        # RPM package configuration
        set(CPACK_RPM_PACKAGE_GROUP "Development/Tools")
        set(CPACK_RPM_PACKAGE_LICENSE "MIT")
        set(CPACK_RPM_PACKAGE_URL "https://github.com/your-org/${project_name}")
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

# Setup Complete Packaging
function(setup_complete_packaging project_name project_version)
    setup_standard_installation(${project_name})
    
    configure_cpack(${project_name} ${project_version}
        VENDOR "${project_name} Team"
        DESCRIPTION "C++ Project Scaffolding Tool"
        INSTALL_DIRECTORY ${project_name}
    )
endfunction()

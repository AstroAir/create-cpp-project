# PlatformSupport.cmake
# Cross-platform compilation support with better platform detection and configuration

# Platform detection and configuration
function(detect_platform)
    # Set platform variables
    if(WIN32)
        set(PLATFORM_WINDOWS TRUE PARENT_SCOPE)
        set(PLATFORM_NAME "Windows" PARENT_SCOPE)
        if(CMAKE_SIZEOF_VOID_P EQUAL 8)
            set(PLATFORM_ARCH "x64" PARENT_SCOPE)
        else()
            set(PLATFORM_ARCH "x86" PARENT_SCOPE)
        endif()
    elseif(APPLE)
        set(PLATFORM_APPLE TRUE PARENT_SCOPE)
        if(IOS)
            set(PLATFORM_NAME "iOS" PARENT_SCOPE)
            set(PLATFORM_MOBILE TRUE PARENT_SCOPE)
        else()
            set(PLATFORM_NAME "macOS" PARENT_SCOPE)
        endif()
        execute_process(
            COMMAND uname -m
            OUTPUT_VARIABLE ARCH_OUTPUT
            OUTPUT_STRIP_TRAILING_WHITESPACE
        )
        if(ARCH_OUTPUT MATCHES "arm64")
            set(PLATFORM_ARCH "arm64" PARENT_SCOPE)
        else()
            set(PLATFORM_ARCH "x64" PARENT_SCOPE)
        endif()
    elseif(UNIX)
        set(PLATFORM_LINUX TRUE PARENT_SCOPE)
        set(PLATFORM_NAME "Linux" PARENT_SCOPE)
        execute_process(
            COMMAND uname -m
            OUTPUT_VARIABLE ARCH_OUTPUT
            OUTPUT_STRIP_TRAILING_WHITESPACE
        )
        if(ARCH_OUTPUT MATCHES "x86_64")
            set(PLATFORM_ARCH "x64" PARENT_SCOPE)
        elseif(ARCH_OUTPUT MATCHES "i[3-6]86")
            set(PLATFORM_ARCH "x86" PARENT_SCOPE)
        elseif(ARCH_OUTPUT MATCHES "aarch64")
            set(PLATFORM_ARCH "arm64" PARENT_SCOPE)
        elseif(ARCH_OUTPUT MATCHES "armv7l")
            set(PLATFORM_ARCH "arm" PARENT_SCOPE)
        else()
            set(PLATFORM_ARCH "unknown" PARENT_SCOPE)
        endif()
    else()
        set(PLATFORM_NAME "Unknown" PARENT_SCOPE)
        set(PLATFORM_ARCH "unknown" PARENT_SCOPE)
    endif()
endfunction()

# Configure platform-specific compiler settings
function(configure_platform_compiler_settings target)
    if(PLATFORM_WINDOWS)
        # Windows-specific settings
        target_compile_definitions(${target} PRIVATE
            WIN32_LEAN_AND_MEAN
            NOMINMAX
            UNICODE
            _UNICODE
            _WIN32_WINNT=0x0601  # Windows 7 minimum
        )

        if(MSVC)
            target_compile_options(${target} PRIVATE
                /utf-8              # UTF-8 source and execution character sets
                /permissive-        # Disable non-conforming code
                /Zc:__cplusplus     # Enable correct __cplusplus macro
                /Zc:preprocessor    # Enable conforming preprocessor (VS 2019+)
            )
        endif()

    elseif(PLATFORM_APPLE)
        # macOS/iOS-specific settings
        if(NOT IOS)
            # macOS minimum version
            set(CMAKE_OSX_DEPLOYMENT_TARGET "10.14" PARENT_SCOPE)
        endif()

        target_compile_definitions(${target} PRIVATE
            _DARWIN_C_SOURCE
        )

    elseif(PLATFORM_LINUX)
        # Linux-specific settings
        target_compile_definitions(${target} PRIVATE
            _GNU_SOURCE
            _DEFAULT_SOURCE
        )

        # Enable position independent code for shared libraries
        set_target_properties(${target} PROPERTIES
            POSITION_INDEPENDENT_CODE ON
        )
    endif()
endfunction()

# Configure platform-specific libraries
function(link_platform_libraries target)
    if(PLATFORM_WINDOWS)
        # Windows system libraries
        target_link_libraries(${target} PRIVATE
            shlwapi     # Shell API
            ws2_32      # Winsock
            iphlpapi    # IP Helper API
            userenv     # User environment
        )

    elseif(PLATFORM_APPLE)
        # macOS/iOS frameworks
        find_library(FOUNDATION_FRAMEWORK Foundation)
        find_library(CORESERVICES_FRAMEWORK CoreServices)

        if(FOUNDATION_FRAMEWORK)
            target_link_libraries(${target} PRIVATE ${FOUNDATION_FRAMEWORK})
        endif()
        if(CORESERVICES_FRAMEWORK)
            target_link_libraries(${target} PRIVATE ${CORESERVICES_FRAMEWORK})
        endif()

    elseif(PLATFORM_LINUX)
        # Linux system libraries
        find_package(Threads REQUIRED)
        target_link_libraries(${target} PRIVATE Threads::Threads)

        # Check for additional libraries
        find_library(DL_LIBRARY dl)
        if(DL_LIBRARY)
            target_link_libraries(${target} PRIVATE ${DL_LIBRARY})
        endif()

        find_library(RT_LIBRARY rt)
        if(RT_LIBRARY)
            target_link_libraries(${target} PRIVATE ${RT_LIBRARY})
        endif()
    endif()
endfunction()

# Configure platform-specific output directories
function(configure_platform_output_dirs target)
    # Set output directories based on platform conventions
    if(PLATFORM_WINDOWS)
        set_target_properties(${target} PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
            LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
            ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
        )
    else()
        set_target_properties(${target} PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
            LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
            ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
        )
    endif()

    # Create platform-specific subdirectories for multi-config generators
    if(CMAKE_CONFIGURATION_TYPES)
        foreach(config ${CMAKE_CONFIGURATION_TYPES})
            string(TOUPPER ${config} config_upper)
            set_target_properties(${target} PROPERTIES
                RUNTIME_OUTPUT_DIRECTORY_${config_upper} "${CMAKE_BINARY_DIR}/bin/${config}"
                LIBRARY_OUTPUT_DIRECTORY_${config_upper} "${CMAKE_BINARY_DIR}/lib/${config}"
                ARCHIVE_OUTPUT_DIRECTORY_${config_upper} "${CMAKE_BINARY_DIR}/lib/${config}"
            )
        endforeach()
    endif()
endfunction()

# Setup platform-specific installation paths
function(setup_platform_install_paths)
    if(PLATFORM_WINDOWS)
        set(CMAKE_INSTALL_BINDIR "bin" PARENT_SCOPE)
        set(CMAKE_INSTALL_LIBDIR "lib" PARENT_SCOPE)
        set(CMAKE_INSTALL_INCLUDEDIR "include" PARENT_SCOPE)
        set(CMAKE_INSTALL_DATADIR "share" PARENT_SCOPE)
    else()
        include(GNUInstallDirs)
        # GNUInstallDirs sets the standard variables
    endif()
endfunction()

# Configure platform-specific resource handling
function(configure_platform_resources target)
    set(options)
    set(oneValueArgs ICON_FILE)
    set(multiValueArgs RESOURCE_FILES)
    cmake_parse_arguments(RES "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if(PLATFORM_WINDOWS AND RES_ICON_FILE)
        # Windows resource file
        set(RC_FILE "${CMAKE_CURRENT_BINARY_DIR}/${target}.rc")
        file(WRITE ${RC_FILE}
            "IDI_ICON1 ICON DISCARDABLE \"${RES_ICON_FILE}\"\n"
        )
        target_sources(${target} PRIVATE ${RC_FILE})
    endif()

    if(RES_RESOURCE_FILES)
        add_resources(${target} FILES ${RES_RESOURCE_FILES})
    endif()
endfunction()

# Check for platform-specific features
function(check_platform_features)
    include(CheckSymbolExists)
    include(CheckIncludeFileCXX)
    include(CheckFunctionExists)

    # Check for common POSIX functions
    if(NOT PLATFORM_WINDOWS)
        check_symbol_exists(pthread_create "pthread.h" HAVE_PTHREAD)
        check_symbol_exists(dlopen "dlfcn.h" HAVE_DLOPEN)
        check_function_exists(mmap HAVE_MMAP)
        check_include_file_cxx(unistd.h HAVE_UNISTD_H)
    endif()

    # Check for Windows-specific features
    if(PLATFORM_WINDOWS)
        check_include_file_cxx(windows.h HAVE_WINDOWS_H)
        check_include_file_cxx(shlwapi.h HAVE_SHLWAPI_H)
    endif()

    # Check for C++17 filesystem
    include(CheckCXXSourceCompiles)
    check_cxx_source_compiles(
        "#include <filesystem>\nint main() { std::filesystem::current_path(); return 0; }"
        HAVE_STD_FILESYSTEM
    )

    if(HAVE_STD_FILESYSTEM)
        message(STATUS "std::filesystem is available")
    else()
        message(STATUS "std::filesystem is not available, checking for alternatives")
        # Check for experimental filesystem
        check_cxx_source_compiles(
            "#include <experimental/filesystem>\nint main() { std::experimental::filesystem::current_path(); return 0; }"
            HAVE_EXPERIMENTAL_FILESYSTEM
        )
        if(HAVE_EXPERIMENTAL_FILESYSTEM)
            message(STATUS "std::experimental::filesystem is available")
        endif()
    endif()
endfunction()

# Apply all platform configurations to a target
function(apply_platform_config target)
    configure_platform_compiler_settings(${target})
    link_platform_libraries(${target})
    configure_platform_output_dirs(${target})
endfunction()

# Setup complete platform support
function(setup_platform_support)
    detect_platform()
    setup_platform_install_paths()
    check_platform_features()

    message(STATUS "Platform: ${PLATFORM_NAME} (${PLATFORM_ARCH})")
    if(PLATFORM_WINDOWS)
        message(STATUS "Windows-specific configuration applied")
    elseif(PLATFORM_APPLE)
        message(STATUS "Apple-specific configuration applied")
        if(CMAKE_OSX_DEPLOYMENT_TARGET)
            message(STATUS "macOS deployment target: ${CMAKE_OSX_DEPLOYMENT_TARGET}")
        endif()
    elseif(PLATFORM_LINUX)
        message(STATUS "Linux-specific configuration applied")
    endif()
endfunction()

# PlatformSupport.cmake
# Cross-platform compilation support with better platform detection and configuration

# Enhanced platform detection and configuration
function(detect_platform)
    # Initialize platform variables
    set(PLATFORM_WINDOWS FALSE PARENT_SCOPE)
    set(PLATFORM_APPLE FALSE PARENT_SCOPE)
    set(PLATFORM_LINUX FALSE PARENT_SCOPE)
    set(PLATFORM_ANDROID FALSE PARENT_SCOPE)
    set(PLATFORM_IOS FALSE PARENT_SCOPE)
    set(PLATFORM_WASM FALSE PARENT_SCOPE)
    set(PLATFORM_EMBEDDED FALSE PARENT_SCOPE)
    set(PLATFORM_MOBILE FALSE PARENT_SCOPE)
    set(PLATFORM_MSYS2 FALSE PARENT_SCOPE)

    # Detect WebAssembly/Emscripten
    if(CMAKE_SYSTEM_NAME STREQUAL "Emscripten")
        set(PLATFORM_WASM TRUE PARENT_SCOPE)
        set(PLATFORM_NAME "WebAssembly" PARENT_SCOPE)
        set(PLATFORM_ARCH "wasm32" PARENT_SCOPE)
        message(STATUS "Detected WebAssembly/Emscripten platform")

    # Detect Android
    elseif(CMAKE_SYSTEM_NAME STREQUAL "Android")
        set(PLATFORM_ANDROID TRUE PARENT_SCOPE)
        set(PLATFORM_MOBILE TRUE PARENT_SCOPE)
        set(PLATFORM_NAME "Android" PARENT_SCOPE)

        # Detect Android architecture
        if(ANDROID_ABI STREQUAL "arm64-v8a")
            set(PLATFORM_ARCH "arm64" PARENT_SCOPE)
        elseif(ANDROID_ABI STREQUAL "armeabi-v7a")
            set(PLATFORM_ARCH "arm" PARENT_SCOPE)
        elseif(ANDROID_ABI STREQUAL "x86_64")
            set(PLATFORM_ARCH "x64" PARENT_SCOPE)
        elseif(ANDROID_ABI STREQUAL "x86")
            set(PLATFORM_ARCH "x86" PARENT_SCOPE)
        else()
            set(PLATFORM_ARCH "unknown" PARENT_SCOPE)
        endif()
        message(STATUS "Detected Android platform: ${ANDROID_ABI}")

    # Detect embedded/bare-metal systems
    elseif(CMAKE_SYSTEM_NAME STREQUAL "Generic")
        set(PLATFORM_EMBEDDED TRUE PARENT_SCOPE)
        set(PLATFORM_NAME "Embedded" PARENT_SCOPE)

        # Detect embedded architecture
        if(CMAKE_SYSTEM_PROCESSOR MATCHES "arm")
            set(PLATFORM_ARCH "arm" PARENT_SCOPE)
            if(CMAKE_SYSTEM_PROCESSOR MATCHES "cortex-m")
                set(PLATFORM_ARCH "arm-cortex-m" PARENT_SCOPE)
            endif()
        elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "riscv")
            set(PLATFORM_ARCH "riscv" PARENT_SCOPE)
        else()
            set(PLATFORM_ARCH "${CMAKE_SYSTEM_PROCESSOR}" PARENT_SCOPE)
        endif()
        message(STATUS "Detected embedded platform: ${CMAKE_SYSTEM_PROCESSOR}")

    # Detect Windows
    elseif(WIN32)
        set(PLATFORM_WINDOWS TRUE PARENT_SCOPE)
        set(PLATFORM_NAME "Windows" PARENT_SCOPE)

        # Check for MSYS2 environment
        if(DEFINED ENV{MSYSTEM})
            set(PLATFORM_MSYS2 TRUE PARENT_SCOPE)
            set(PLATFORM_NAME "Windows-MSYS2" PARENT_SCOPE)
            message(STATUS "Detected MSYS2 environment: $ENV{MSYSTEM}")
        endif()

        # Detect Windows architecture
        if(CMAKE_SIZEOF_VOID_P EQUAL 8)
            set(PLATFORM_ARCH "x64" PARENT_SCOPE)
        else()
            set(PLATFORM_ARCH "x86" PARENT_SCOPE)
        endif()

    # Detect Apple platforms
    elseif(APPLE)
        set(PLATFORM_APPLE TRUE PARENT_SCOPE)

        # Detect iOS
        if(CMAKE_SYSTEM_NAME STREQUAL "iOS")
            set(PLATFORM_IOS TRUE PARENT_SCOPE)
            set(PLATFORM_MOBILE TRUE PARENT_SCOPE)
            set(PLATFORM_NAME "iOS" PARENT_SCOPE)
            message(STATUS "Detected iOS platform")
        else()
            set(PLATFORM_NAME "macOS" PARENT_SCOPE)
        endif()

        # Detect Apple architecture
        execute_process(
            COMMAND uname -m
            OUTPUT_VARIABLE ARCH_OUTPUT
            OUTPUT_STRIP_TRAILING_WHITESPACE
            ERROR_QUIET
        )
        if(ARCH_OUTPUT MATCHES "arm64")
            set(PLATFORM_ARCH "arm64" PARENT_SCOPE)
        elseif(ARCH_OUTPUT MATCHES "x86_64")
            set(PLATFORM_ARCH "x64" PARENT_SCOPE)
        else()
            set(PLATFORM_ARCH "x64" PARENT_SCOPE)  # Default fallback
        endif()

    # Detect Linux and other Unix systems
    elseif(UNIX)
        set(PLATFORM_LINUX TRUE PARENT_SCOPE)
        set(PLATFORM_NAME "Linux" PARENT_SCOPE)

        # Enhanced Linux architecture detection
        execute_process(
            COMMAND uname -m
            OUTPUT_VARIABLE ARCH_OUTPUT
            OUTPUT_STRIP_TRAILING_WHITESPACE
            ERROR_QUIET
        )
        if(ARCH_OUTPUT MATCHES "x86_64")
            set(PLATFORM_ARCH "x64" PARENT_SCOPE)
        elseif(ARCH_OUTPUT MATCHES "i[3-6]86")
            set(PLATFORM_ARCH "x86" PARENT_SCOPE)
        elseif(ARCH_OUTPUT MATCHES "aarch64")
            set(PLATFORM_ARCH "arm64" PARENT_SCOPE)
        elseif(ARCH_OUTPUT MATCHES "armv7l")
            set(PLATFORM_ARCH "arm" PARENT_SCOPE)
        elseif(ARCH_OUTPUT MATCHES "armv6l")
            set(PLATFORM_ARCH "armv6" PARENT_SCOPE)
        elseif(ARCH_OUTPUT MATCHES "riscv64")
            set(PLATFORM_ARCH "riscv64" PARENT_SCOPE)
        elseif(ARCH_OUTPUT MATCHES "riscv32")
            set(PLATFORM_ARCH "riscv32" PARENT_SCOPE)
        elseif(ARCH_OUTPUT MATCHES "mips64")
            set(PLATFORM_ARCH "mips64" PARENT_SCOPE)
        elseif(ARCH_OUTPUT MATCHES "mips")
            set(PLATFORM_ARCH "mips" PARENT_SCOPE)
        elseif(ARCH_OUTPUT MATCHES "ppc64le")
            set(PLATFORM_ARCH "ppc64le" PARENT_SCOPE)
        elseif(ARCH_OUTPUT MATCHES "ppc64")
            set(PLATFORM_ARCH "ppc64" PARENT_SCOPE)
        elseif(ARCH_OUTPUT MATCHES "s390x")
            set(PLATFORM_ARCH "s390x" PARENT_SCOPE)
        else()
            set(PLATFORM_ARCH "unknown" PARENT_SCOPE)
        endif()

    else()
        set(PLATFORM_NAME "Unknown" PARENT_SCOPE)
        set(PLATFORM_ARCH "unknown" PARENT_SCOPE)
    endif()

    # Set additional platform characteristics
    if(PLATFORM_ANDROID OR PLATFORM_IOS)
        set(PLATFORM_MOBILE TRUE PARENT_SCOPE)
    endif()

    if(PLATFORM_EMBEDDED OR PLATFORM_WASM)
        set(PLATFORM_CONSTRAINED TRUE PARENT_SCOPE)
    endif()
endfunction()

# Enhanced platform-specific compiler settings
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
        elseif(PLATFORM_MSYS2)
            # MSYS2-specific settings
            target_compile_options(${target} PRIVATE
                -fno-strict-aliasing
                -fwrapv
            )
        endif()

    elseif(PLATFORM_ANDROID)
        # Android-specific settings
        target_compile_definitions(${target} PRIVATE
            ANDROID
            __ANDROID_API__=${ANDROID_PLATFORM_LEVEL}
        )

        # Android-specific compiler flags
        target_compile_options(${target} PRIVATE
            -fPIC
            -ffunction-sections
            -fdata-sections
        )

        # Android-specific linker flags
        target_link_options(${target} PRIVATE
            -Wl,--gc-sections
            -Wl,--as-needed
        )

    elseif(PLATFORM_IOS)
        # iOS-specific settings
        target_compile_definitions(${target} PRIVATE
            IOS
            _DARWIN_C_SOURCE
        )

        # iOS deployment target
        set(CMAKE_OSX_DEPLOYMENT_TARGET "12.0" PARENT_SCOPE)

        # iOS-specific compiler flags
        target_compile_options(${target} PRIVATE
            -fno-common
            -fvisibility=hidden
        )

    elseif(PLATFORM_APPLE)
        # macOS-specific settings
        set(CMAKE_OSX_DEPLOYMENT_TARGET "10.14" PARENT_SCOPE)

        target_compile_definitions(${target} PRIVATE
            _DARWIN_C_SOURCE
        )

        # macOS-specific compiler flags
        target_compile_options(${target} PRIVATE
            -fno-common
        )

    elseif(PLATFORM_WASM)
        # WebAssembly/Emscripten-specific settings
        target_compile_definitions(${target} PRIVATE
            EMSCRIPTEN
            __EMSCRIPTEN__
        )

        # WebAssembly-specific compiler flags
        target_compile_options(${target} PRIVATE
            -fno-exceptions  # Exceptions are expensive in WASM
            -fno-rtti       # RTTI is expensive in WASM
        )

        # WebAssembly-specific linker flags
        target_link_options(${target} PRIVATE
            -s WASM=1
            -s ALLOW_MEMORY_GROWTH=1
        )

    elseif(PLATFORM_EMBEDDED)
        # Embedded/bare-metal settings
        target_compile_definitions(${target} PRIVATE
            EMBEDDED
            NO_STDLIB
        )

        # Embedded-specific compiler flags
        target_compile_options(${target} PRIVATE
            -fno-exceptions
            -fno-rtti
            -fno-unwind-tables
            -ffunction-sections
            -fdata-sections
            -Os  # Optimize for size
        )

        # Embedded-specific linker flags
        target_link_options(${target} PRIVATE
            -Wl,--gc-sections
            -Wl,--print-memory-usage
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

        # Linux-specific compiler flags for better security
        target_compile_options(${target} PRIVATE
            -fstack-protector-strong
            -D_FORTIFY_SOURCE=2
        )

        # Linux-specific linker flags
        target_link_options(${target} PRIVATE
            -Wl,-z,relro
            -Wl,-z,now
        )
    endif()

    # Common settings for mobile platforms
    if(PLATFORM_MOBILE)
        target_compile_definitions(${target} PRIVATE
            MOBILE_PLATFORM
        )

        # Mobile-optimized flags
        target_compile_options(${target} PRIVATE
            -ffast-math
            -fno-math-errno
        )
    endif()

    # Common settings for constrained platforms
    if(PLATFORM_CONSTRAINED)
        target_compile_definitions(${target} PRIVATE
            CONSTRAINED_PLATFORM
            NO_DYNAMIC_ALLOCATION
        )
    endif()
endfunction()

# Enhanced platform-specific library linking
function(link_platform_libraries target)
    if(PLATFORM_WINDOWS)
        # Windows system libraries
        target_link_libraries(${target} PRIVATE
            shlwapi     # Shell API
            ws2_32      # Winsock
            iphlpapi    # IP Helper API
            userenv     # User environment
        )

        if(PLATFORM_MSYS2)
            # Additional MSYS2 libraries
            find_library(PTHREAD_LIBRARY pthread)
            if(PTHREAD_LIBRARY)
                target_link_libraries(${target} PRIVATE ${PTHREAD_LIBRARY})
            endif()
        endif()

    elseif(PLATFORM_ANDROID)
        # Android system libraries
        target_link_libraries(${target} PRIVATE
            log         # Android logging
            android     # Android native API
        )

        # Optional Android libraries
        find_library(GLESV2_LIBRARY GLESv2)
        if(GLESV2_LIBRARY)
            target_link_libraries(${target} PRIVATE ${GLESV2_LIBRARY})
        endif()

        find_library(EGL_LIBRARY EGL)
        if(EGL_LIBRARY)
            target_link_libraries(${target} PRIVATE ${EGL_LIBRARY})
        endif()

    elseif(PLATFORM_IOS)
        # iOS frameworks
        find_library(FOUNDATION_FRAMEWORK Foundation)
        find_library(UIKIT_FRAMEWORK UIKit)
        find_library(COREGRAPHICS_FRAMEWORK CoreGraphics)
        find_library(QUARTZCORE_FRAMEWORK QuartzCore)

        if(FOUNDATION_FRAMEWORK)
            target_link_libraries(${target} PRIVATE ${FOUNDATION_FRAMEWORK})
        endif()
        if(UIKIT_FRAMEWORK)
            target_link_libraries(${target} PRIVATE ${UIKIT_FRAMEWORK})
        endif()
        if(COREGRAPHICS_FRAMEWORK)
            target_link_libraries(${target} PRIVATE ${COREGRAPHICS_FRAMEWORK})
        endif()
        if(QUARTZCORE_FRAMEWORK)
            target_link_libraries(${target} PRIVATE ${QUARTZCORE_FRAMEWORK})
        endif()

    elseif(PLATFORM_APPLE)
        # macOS frameworks
        find_library(FOUNDATION_FRAMEWORK Foundation)
        find_library(CORESERVICES_FRAMEWORK CoreServices)
        find_library(COCOA_FRAMEWORK Cocoa)
        find_library(IOKIT_FRAMEWORK IOKit)

        if(FOUNDATION_FRAMEWORK)
            target_link_libraries(${target} PRIVATE ${FOUNDATION_FRAMEWORK})
        endif()
        if(CORESERVICES_FRAMEWORK)
            target_link_libraries(${target} PRIVATE ${CORESERVICES_FRAMEWORK})
        endif()
        if(COCOA_FRAMEWORK)
            target_link_libraries(${target} PRIVATE ${COCOA_FRAMEWORK})
        endif()
        if(IOKIT_FRAMEWORK)
            target_link_libraries(${target} PRIVATE ${IOKIT_FRAMEWORK})
        endif()

    elseif(PLATFORM_WASM)
        # WebAssembly doesn't need traditional system libraries
        # but may need specific Emscripten libraries
        target_link_options(${target} PRIVATE
            -s USE_PTHREADS=0  # Disable pthreads by default
            -s FILESYSTEM=1    # Enable filesystem support
        )

    elseif(PLATFORM_EMBEDDED)
        # Embedded systems typically don't link standard libraries
        # but may need specific runtime libraries
        if(CMAKE_SYSTEM_PROCESSOR MATCHES "arm")
            # ARM-specific libraries
            find_library(NOSYS_LIBRARY nosys)
            if(NOSYS_LIBRARY)
                target_link_libraries(${target} PRIVATE ${NOSYS_LIBRARY})
            endif()
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

        find_library(M_LIBRARY m)
        if(M_LIBRARY)
            target_link_libraries(${target} PRIVATE ${M_LIBRARY})
        endif()

        # Additional Linux libraries for specific architectures
        if(PLATFORM_ARCH MATCHES "arm")
            find_library(ATOMIC_LIBRARY atomic)
            if(ATOMIC_LIBRARY)
                target_link_libraries(${target} PRIVATE ${ATOMIC_LIBRARY})
            endif()
        endif()
    endif()

    # Common libraries for POSIX systems
    if(PLATFORM_LINUX OR PLATFORM_APPLE OR PLATFORM_ANDROID)
        find_package(Threads REQUIRED)
        target_link_libraries(${target} PRIVATE Threads::Threads)
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

# Enhanced platform-specific feature detection
function(check_platform_features)
    include(CheckSymbolExists)
    include(CheckIncludeFileCXX)
    include(CheckFunctionExists)
    include(CheckCXXSourceCompiles)

    # Check for common POSIX functions
    if(NOT PLATFORM_WINDOWS AND NOT PLATFORM_WASM AND NOT PLATFORM_EMBEDDED)
        check_symbol_exists(pthread_create "pthread.h" HAVE_PTHREAD)
        check_symbol_exists(dlopen "dlfcn.h" HAVE_DLOPEN)
        check_function_exists(mmap HAVE_MMAP)
        check_include_file_cxx(unistd.h HAVE_UNISTD_H)
        check_function_exists(fork HAVE_FORK)
        check_function_exists(execv HAVE_EXECV)
    endif()

    # Check for Windows-specific features
    if(PLATFORM_WINDOWS)
        check_include_file_cxx(windows.h HAVE_WINDOWS_H)
        check_include_file_cxx(shlwapi.h HAVE_SHLWAPI_H)
        check_include_file_cxx(winsock2.h HAVE_WINSOCK2_H)

        if(PLATFORM_MSYS2)
            check_include_file_cxx(sys/cygwin.h HAVE_CYGWIN_H)
        endif()
    endif()

    # Check for Android-specific features
    if(PLATFORM_ANDROID)
        check_include_file_cxx(android/log.h HAVE_ANDROID_LOG_H)
        check_include_file_cxx(android/native_activity.h HAVE_ANDROID_NATIVE_ACTIVITY_H)
        check_include_file_cxx(jni.h HAVE_JNI_H)
    endif()

    # Check for iOS/macOS-specific features
    if(PLATFORM_APPLE OR PLATFORM_IOS)
        check_include_file_cxx(TargetConditionals.h HAVE_TARGET_CONDITIONALS_H)
        check_include_file_cxx(CoreFoundation/CoreFoundation.h HAVE_COREFOUNDATION_H)

        if(PLATFORM_IOS)
            check_include_file_cxx(UIKit/UIKit.h HAVE_UIKIT_H)
        else()
            check_include_file_cxx(Cocoa/Cocoa.h HAVE_COCOA_H)
        endif()
    endif()

    # Check for WebAssembly-specific features
    if(PLATFORM_WASM)
        check_include_file_cxx(emscripten.h HAVE_EMSCRIPTEN_H)
        check_include_file_cxx(emscripten/html5.h HAVE_EMSCRIPTEN_HTML5_H)
    endif()

    # Check for embedded-specific features
    if(PLATFORM_EMBEDDED)
        check_include_file_cxx(stdint.h HAVE_STDINT_H)
        check_function_exists(malloc HAVE_MALLOC)
        check_function_exists(free HAVE_FREE)

        # Check for newlib (common embedded C library)
        check_include_file_cxx(newlib.h HAVE_NEWLIB_H)
    endif()

    # Check for C++17 filesystem
    if(NOT PLATFORM_EMBEDDED AND NOT PLATFORM_WASM)
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
    endif()

    # Check for threading support
    if(NOT PLATFORM_EMBEDDED)
        check_cxx_source_compiles(
            "#include <thread>\nint main() { std::thread t; return 0; }"
            HAVE_STD_THREAD
        )

        if(HAVE_STD_THREAD)
            message(STATUS "std::thread is available")
        endif()
    endif()

    # Check for atomic operations
    check_cxx_source_compiles(
        "#include <atomic>\nint main() { std::atomic<int> a{0}; return 0; }"
        HAVE_STD_ATOMIC
    )

    if(HAVE_STD_ATOMIC)
        message(STATUS "std::atomic is available")
    endif()

    # Check for memory mapping
    if(PLATFORM_LINUX OR PLATFORM_APPLE)
        check_function_exists(mmap HAVE_MMAP)
        check_function_exists(munmap HAVE_MUNMAP)
    endif()

    # Check for networking capabilities
    if(NOT PLATFORM_EMBEDDED)
        if(PLATFORM_WINDOWS)
            check_symbol_exists(WSAStartup "winsock2.h" HAVE_WINSOCK)
        else()
            check_include_file_cxx(sys/socket.h HAVE_SYS_SOCKET_H)
            check_function_exists(socket HAVE_SOCKET)
        endif()
    endif()
endfunction()

# Apply all platform configurations to a target
function(apply_platform_config target)
    configure_platform_compiler_settings(${target})
    link_platform_libraries(${target})
    configure_platform_output_dirs(${target})
endfunction()

# Enhanced setup complete platform support
function(setup_platform_support)
    detect_platform()
    setup_platform_install_paths()
    check_platform_features()

    message(STATUS "=== Platform Configuration ===")
    message(STATUS "Platform: ${PLATFORM_NAME} (${PLATFORM_ARCH})")

    if(PLATFORM_WINDOWS)
        message(STATUS "Windows-specific configuration applied")
        if(PLATFORM_MSYS2)
            message(STATUS "MSYS2 environment detected: $ENV{MSYSTEM}")
        endif()

    elseif(PLATFORM_ANDROID)
        message(STATUS "Android-specific configuration applied")
        if(ANDROID_ABI)
            message(STATUS "Android ABI: ${ANDROID_ABI}")
        endif()
        if(ANDROID_PLATFORM)
            message(STATUS "Android API Level: ${ANDROID_PLATFORM}")
        endif()

    elseif(PLATFORM_IOS)
        message(STATUS "iOS-specific configuration applied")
        if(CMAKE_OSX_DEPLOYMENT_TARGET)
            message(STATUS "iOS deployment target: ${CMAKE_OSX_DEPLOYMENT_TARGET}")
        endif()

    elseif(PLATFORM_APPLE)
        message(STATUS "macOS-specific configuration applied")
        if(CMAKE_OSX_DEPLOYMENT_TARGET)
            message(STATUS "macOS deployment target: ${CMAKE_OSX_DEPLOYMENT_TARGET}")
        endif()

    elseif(PLATFORM_WASM)
        message(STATUS "WebAssembly/Emscripten configuration applied")
        if(EMSCRIPTEN_VERSION)
            message(STATUS "Emscripten version: ${EMSCRIPTEN_VERSION}")
        endif()

    elseif(PLATFORM_EMBEDDED)
        message(STATUS "Embedded/bare-metal configuration applied")
        if(CMAKE_SYSTEM_PROCESSOR)
            message(STATUS "Target processor: ${CMAKE_SYSTEM_PROCESSOR}")
        endif()

    elseif(PLATFORM_LINUX)
        message(STATUS "Linux-specific configuration applied")
    endif()

    # Report platform characteristics
    if(PLATFORM_MOBILE)
        message(STATUS "Mobile platform optimizations enabled")
    endif()

    if(PLATFORM_CONSTRAINED)
        message(STATUS "Constrained platform optimizations enabled")
    endif()

    message(STATUS "==============================")
endfunction()

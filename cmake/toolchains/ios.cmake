# iOS toolchain file for CMake
# This toolchain file configures CMake for iOS cross-compilation

cmake_minimum_required(VERSION 3.20)

# Set the system name
set(CMAKE_SYSTEM_NAME iOS)
set(CMAKE_SYSTEM_VERSION 12.0)

# iOS SDK configuration
if(NOT IOS_PLATFORM)
    set(IOS_PLATFORM "OS") # OS, SIMULATOR, WATCHOS, TVOS
endif()

if(NOT IOS_DEPLOYMENT_TARGET)
    set(IOS_DEPLOYMENT_TARGET "12.0")
endif()

# Set deployment target
set(CMAKE_OSX_DEPLOYMENT_TARGET ${IOS_DEPLOYMENT_TARGET})

# Determine SDK and architecture based on platform
if(IOS_PLATFORM STREQUAL "OS")
    set(IOS_SDK_NAME "iphoneos")
    if(NOT CMAKE_OSX_ARCHITECTURES)
        set(CMAKE_OSX_ARCHITECTURES "arm64")
    endif()
    set(CMAKE_SYSTEM_PROCESSOR arm64)
    
elseif(IOS_PLATFORM STREQUAL "SIMULATOR")
    set(IOS_SDK_NAME "iphonesimulator")
    if(NOT CMAKE_OSX_ARCHITECTURES)
        # Support both Intel and Apple Silicon simulators
        set(CMAKE_OSX_ARCHITECTURES "x86_64;arm64")
    endif()
    set(CMAKE_SYSTEM_PROCESSOR x86_64)
    
elseif(IOS_PLATFORM STREQUAL "WATCHOS")
    set(IOS_SDK_NAME "watchos")
    if(NOT CMAKE_OSX_ARCHITECTURES)
        set(CMAKE_OSX_ARCHITECTURES "armv7k;arm64_32")
    endif()
    set(CMAKE_SYSTEM_PROCESSOR arm64)
    set(CMAKE_SYSTEM_NAME watchOS)
    
elseif(IOS_PLATFORM STREQUAL "TVOS")
    set(IOS_SDK_NAME "appletvos")
    if(NOT CMAKE_OSX_ARCHITECTURES)
        set(CMAKE_OSX_ARCHITECTURES "arm64")
    endif()
    set(CMAKE_SYSTEM_PROCESSOR arm64)
    set(CMAKE_SYSTEM_NAME tvOS)
    
else()
    message(FATAL_ERROR "Unsupported iOS platform: ${IOS_PLATFORM}")
endif()

# Find Xcode and SDK
execute_process(
    COMMAND xcode-select -print-path
    OUTPUT_VARIABLE XCODE_DEVELOPER_DIR
    OUTPUT_STRIP_TRAILING_WHITESPACE
    ERROR_QUIET
)

if(NOT XCODE_DEVELOPER_DIR)
    message(FATAL_ERROR "Xcode not found. Please install Xcode and run 'xcode-select --install'")
endif()

# Find SDK path
execute_process(
    COMMAND xcrun --sdk ${IOS_SDK_NAME} --show-sdk-path
    OUTPUT_VARIABLE CMAKE_OSX_SYSROOT
    OUTPUT_STRIP_TRAILING_WHITESPACE
    ERROR_QUIET
)

if(NOT CMAKE_OSX_SYSROOT OR NOT EXISTS "${CMAKE_OSX_SYSROOT}")
    message(FATAL_ERROR "iOS SDK not found for ${IOS_SDK_NAME}")
endif()

# Find SDK version
execute_process(
    COMMAND xcrun --sdk ${IOS_SDK_NAME} --show-sdk-version
    OUTPUT_VARIABLE IOS_SDK_VERSION
    OUTPUT_STRIP_TRAILING_WHITESPACE
    ERROR_QUIET
)

# Set compilers
execute_process(
    COMMAND xcrun --sdk ${IOS_SDK_NAME} --find clang
    OUTPUT_VARIABLE CMAKE_C_COMPILER
    OUTPUT_STRIP_TRAILING_WHITESPACE
    ERROR_QUIET
)

execute_process(
    COMMAND xcrun --sdk ${IOS_SDK_NAME} --find clang++
    OUTPUT_VARIABLE CMAKE_CXX_COMPILER
    OUTPUT_STRIP_TRAILING_WHITESPACE
    ERROR_QUIET
)

# Set additional tools
execute_process(
    COMMAND xcrun --sdk ${IOS_SDK_NAME} --find ar
    OUTPUT_VARIABLE CMAKE_AR
    OUTPUT_STRIP_TRAILING_WHITESPACE
    ERROR_QUIET
)

execute_process(
    COMMAND xcrun --sdk ${IOS_SDK_NAME} --find ranlib
    OUTPUT_VARIABLE CMAKE_RANLIB
    OUTPUT_STRIP_TRAILING_WHITESPACE
    ERROR_QUIET
)

execute_process(
    COMMAND xcrun --sdk ${IOS_SDK_NAME} --find strip
    OUTPUT_VARIABLE CMAKE_STRIP
    OUTPUT_STRIP_TRAILING_WHITESPACE
    ERROR_QUIET
)

# Verify tools exist
if(NOT EXISTS "${CMAKE_C_COMPILER}")
    message(FATAL_ERROR "iOS C compiler not found: ${CMAKE_C_COMPILER}")
endif()

if(NOT EXISTS "${CMAKE_CXX_COMPILER}")
    message(FATAL_ERROR "iOS C++ compiler not found: ${CMAKE_CXX_COMPILER}")
endif()

# Set find root path
set(CMAKE_FIND_ROOT_PATH "${CMAKE_OSX_SYSROOT}")
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

# iOS-specific compiler flags
set(IOS_COMPILER_FLAGS "")
set(IOS_COMPILER_FLAGS "${IOS_COMPILER_FLAGS} -fvisibility=hidden")
set(IOS_COMPILER_FLAGS "${IOS_COMPILER_FLAGS} -fvisibility-inlines-hidden")
set(IOS_COMPILER_FLAGS "${IOS_COMPILER_FLAGS} -fno-common")
set(IOS_COMPILER_FLAGS "${IOS_COMPILER_FLAGS} -fembed-bitcode")

# Platform-specific flags
if(IOS_PLATFORM STREQUAL "OS")
    set(IOS_COMPILER_FLAGS "${IOS_COMPILER_FLAGS} -miphoneos-version-min=${IOS_DEPLOYMENT_TARGET}")
elseif(IOS_PLATFORM STREQUAL "SIMULATOR")
    set(IOS_COMPILER_FLAGS "${IOS_COMPILER_FLAGS} -mios-simulator-version-min=${IOS_DEPLOYMENT_TARGET}")
elseif(IOS_PLATFORM STREQUAL "WATCHOS")
    set(IOS_COMPILER_FLAGS "${IOS_COMPILER_FLAGS} -mwatchos-version-min=${IOS_DEPLOYMENT_TARGET}")
elseif(IOS_PLATFORM STREQUAL "TVOS")
    set(IOS_COMPILER_FLAGS "${IOS_COMPILER_FLAGS} -mtvos-version-min=${IOS_DEPLOYMENT_TARGET}")
endif()

# Set compiler flags
set(CMAKE_C_FLAGS_INIT "${IOS_COMPILER_FLAGS}")
set(CMAKE_CXX_FLAGS_INIT "${IOS_COMPILER_FLAGS}")

# iOS-specific linker flags
set(IOS_LINKER_FLAGS "")
set(IOS_LINKER_FLAGS "${IOS_LINKER_FLAGS} -Wl,-dead_strip")
set(IOS_LINKER_FLAGS "${IOS_LINKER_FLAGS} -Wl,-dead_strip_dylibs")

# Set linker flags
set(CMAKE_EXE_LINKER_FLAGS_INIT "${IOS_LINKER_FLAGS}")
set(CMAKE_SHARED_LINKER_FLAGS_INIT "${IOS_LINKER_FLAGS}")

# Set build type specific flags
set(CMAKE_C_FLAGS_DEBUG_INIT "-g -O0")
set(CMAKE_CXX_FLAGS_DEBUG_INIT "-g -O0")
set(CMAKE_C_FLAGS_RELEASE_INIT "-O2 -DNDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE_INIT "-O2 -DNDEBUG")

# iOS-specific definitions
add_definitions(-DIOS)
add_definitions(-DTARGET_OS_IPHONE=1)

if(IOS_PLATFORM STREQUAL "SIMULATOR")
    add_definitions(-DTARGET_IPHONE_SIMULATOR=1)
endif()

# Set properties for iOS build
set(IOS TRUE)
set(CMAKE_CROSSCOMPILING TRUE)
set(CMAKE_MACOSX_BUNDLE YES)

# Configure for iOS app bundle
set(CMAKE_XCODE_ATTRIBUTE_CODE_SIGNING_REQUIRED "NO")
set(CMAKE_XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY "")
set(CMAKE_XCODE_ATTRIBUTE_DEVELOPMENT_TEAM "")
set(CMAKE_XCODE_ATTRIBUTE_TARGETED_DEVICE_FAMILY "1,2") # iPhone and iPad

# Set minimum iOS version
set(CMAKE_XCODE_ATTRIBUTE_IPHONEOS_DEPLOYMENT_TARGET ${IOS_DEPLOYMENT_TARGET})

# Enable ARC by default
set(CMAKE_XCODE_ATTRIBUTE_CLANG_ENABLE_OBJC_ARC YES)

# Modern Objective-C features
set(CMAKE_XCODE_ATTRIBUTE_CLANG_ENABLE_OBJC_WEAK YES)
set(CMAKE_XCODE_ATTRIBUTE_CLANG_ENABLE_MODULES YES)

# Bitcode settings
if(IOS_PLATFORM STREQUAL "OS")
    set(CMAKE_XCODE_ATTRIBUTE_ENABLE_BITCODE YES)
    set(CMAKE_XCODE_ATTRIBUTE_BITCODE_GENERATION_MODE bitcode)
else()
    set(CMAKE_XCODE_ATTRIBUTE_ENABLE_BITCODE NO)
endif()

# Skip install rules for iOS
set(CMAKE_SKIP_INSTALL_RULES YES)

# Function to create iOS app bundle
function(create_ios_app target_name)
    set_target_properties(${target_name} PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_GUI_IDENTIFIER "com.example.${target_name}"
        MACOSX_BUNDLE_BUNDLE_NAME "${target_name}"
        MACOSX_BUNDLE_BUNDLE_VERSION "1.0"
        MACOSX_BUNDLE_SHORT_VERSION_STRING "1.0"
        XCODE_ATTRIBUTE_PRODUCT_BUNDLE_IDENTIFIER "com.example.${target_name}"
    )
endfunction()

message(STATUS "iOS toolchain configured:")
message(STATUS "  Platform: ${IOS_PLATFORM}")
message(STATUS "  SDK: ${IOS_SDK_NAME} (${IOS_SDK_VERSION})")
message(STATUS "  Deployment Target: ${IOS_DEPLOYMENT_TARGET}")
message(STATUS "  Architectures: ${CMAKE_OSX_ARCHITECTURES}")
message(STATUS "  SDK Path: ${CMAKE_OSX_SYSROOT}")
message(STATUS "  Xcode: ${XCODE_DEVELOPER_DIR}")

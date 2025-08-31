# Android toolchain file for CMake
# This toolchain file configures CMake for Android cross-compilation

cmake_minimum_required(VERSION 3.20)

# Set the system name and processor
set(CMAKE_SYSTEM_NAME Android)
set(CMAKE_SYSTEM_VERSION 21) # Minimum API level

# Android NDK configuration
if(NOT ANDROID_NDK)
    if(DEFINED ENV{ANDROID_NDK_ROOT})
        set(ANDROID_NDK $ENV{ANDROID_NDK_ROOT})
    elseif(DEFINED ENV{ANDROID_NDK})
        set(ANDROID_NDK $ENV{ANDROID_NDK})
    else()
        message(FATAL_ERROR "ANDROID_NDK must be set to the Android NDK root directory")
    endif()
endif()

# Validate NDK path
if(NOT EXISTS "${ANDROID_NDK}")
    message(FATAL_ERROR "Android NDK not found at: ${ANDROID_NDK}")
endif()

# Set default values for Android build parameters
if(NOT ANDROID_ABI)
    set(ANDROID_ABI "arm64-v8a")
endif()

if(NOT ANDROID_PLATFORM)
    set(ANDROID_PLATFORM "android-21")
endif()

if(NOT ANDROID_STL)
    set(ANDROID_STL "c++_shared")
endif()

# Extract API level from platform
string(REGEX MATCH "[0-9]+" ANDROID_PLATFORM_LEVEL "${ANDROID_PLATFORM}")
set(CMAKE_SYSTEM_VERSION ${ANDROID_PLATFORM_LEVEL})

# Set architecture-specific variables
if(ANDROID_ABI STREQUAL "arm64-v8a")
    set(CMAKE_SYSTEM_PROCESSOR aarch64)
    set(ANDROID_ARCH_NAME arm64)
    set(ANDROID_LLVM_TRIPLE aarch64-linux-android)
elseif(ANDROID_ABI STREQUAL "armeabi-v7a")
    set(CMAKE_SYSTEM_PROCESSOR arm)
    set(ANDROID_ARCH_NAME arm)
    set(ANDROID_LLVM_TRIPLE armv7-linux-androideabi)
elseif(ANDROID_ABI STREQUAL "x86_64")
    set(CMAKE_SYSTEM_PROCESSOR x86_64)
    set(ANDROID_ARCH_NAME x86_64)
    set(ANDROID_LLVM_TRIPLE x86_64-linux-android)
elseif(ANDROID_ABI STREQUAL "x86")
    set(CMAKE_SYSTEM_PROCESSOR i686)
    set(ANDROID_ARCH_NAME x86)
    set(ANDROID_LLVM_TRIPLE i686-linux-android)
else()
    message(FATAL_ERROR "Unsupported Android ABI: ${ANDROID_ABI}")
endif()

# Find the NDK toolchain
set(ANDROID_TOOLCHAIN_ROOT "${ANDROID_NDK}/toolchains/llvm/prebuilt")

# Detect host system for toolchain path
if(CMAKE_HOST_SYSTEM_NAME STREQUAL "Linux")
    set(ANDROID_HOST_TAG "linux-x86_64")
elseif(CMAKE_HOST_SYSTEM_NAME STREQUAL "Darwin")
    set(ANDROID_HOST_TAG "darwin-x86_64")
elseif(CMAKE_HOST_SYSTEM_NAME STREQUAL "Windows")
    set(ANDROID_HOST_TAG "windows-x86_64")
else()
    message(FATAL_ERROR "Unsupported host system: ${CMAKE_HOST_SYSTEM_NAME}")
endif()

set(ANDROID_TOOLCHAIN_ROOT "${ANDROID_TOOLCHAIN_ROOT}/${ANDROID_HOST_TAG}")

if(NOT EXISTS "${ANDROID_TOOLCHAIN_ROOT}")
    message(FATAL_ERROR "Android toolchain not found at: ${ANDROID_TOOLCHAIN_ROOT}")
endif()

# Set compiler paths
set(CMAKE_C_COMPILER "${ANDROID_TOOLCHAIN_ROOT}/bin/${ANDROID_LLVM_TRIPLE}${ANDROID_PLATFORM_LEVEL}-clang")
set(CMAKE_CXX_COMPILER "${ANDROID_TOOLCHAIN_ROOT}/bin/${ANDROID_LLVM_TRIPLE}${ANDROID_PLATFORM_LEVEL}-clang++")

# Set additional tools
set(CMAKE_AR "${ANDROID_TOOLCHAIN_ROOT}/bin/llvm-ar")
set(CMAKE_RANLIB "${ANDROID_TOOLCHAIN_ROOT}/bin/llvm-ranlib")
set(CMAKE_STRIP "${ANDROID_TOOLCHAIN_ROOT}/bin/llvm-strip")

# Verify compiler exists
if(NOT EXISTS "${CMAKE_C_COMPILER}")
    message(FATAL_ERROR "Android C compiler not found: ${CMAKE_C_COMPILER}")
endif()

if(NOT EXISTS "${CMAKE_CXX_COMPILER}")
    message(FATAL_ERROR "Android C++ compiler not found: ${CMAKE_CXX_COMPILER}")
endif()

# Set sysroot
set(CMAKE_SYSROOT "${ANDROID_TOOLCHAIN_ROOT}/sysroot")

# Set find root path
set(CMAKE_FIND_ROOT_PATH "${CMAKE_SYSROOT}")
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

# Android-specific compiler flags
set(ANDROID_COMPILER_FLAGS "-fPIC")
set(ANDROID_COMPILER_FLAGS "${ANDROID_COMPILER_FLAGS} -ffunction-sections")
set(ANDROID_COMPILER_FLAGS "${ANDROID_COMPILER_FLAGS} -fdata-sections")
set(ANDROID_COMPILER_FLAGS "${ANDROID_COMPILER_FLAGS} -funwind-tables")
set(ANDROID_COMPILER_FLAGS "${ANDROID_COMPILER_FLAGS} -fstack-protector-strong")
set(ANDROID_COMPILER_FLAGS "${ANDROID_COMPILER_FLAGS} -no-canonical-prefixes")

# Architecture-specific flags
if(ANDROID_ABI STREQUAL "armeabi-v7a")
    set(ANDROID_COMPILER_FLAGS "${ANDROID_COMPILER_FLAGS} -march=armv7-a")
    set(ANDROID_COMPILER_FLAGS "${ANDROID_COMPILER_FLAGS} -mfloat-abi=softfp")
    set(ANDROID_COMPILER_FLAGS "${ANDROID_COMPILER_FLAGS} -mfpu=vfpv3-d16")
endif()

# Set compiler flags
set(CMAKE_C_FLAGS_INIT "${ANDROID_COMPILER_FLAGS}")
set(CMAKE_CXX_FLAGS_INIT "${ANDROID_COMPILER_FLAGS}")

# Android-specific linker flags
set(ANDROID_LINKER_FLAGS "-Wl,--build-id=sha1")
set(ANDROID_LINKER_FLAGS "${ANDROID_LINKER_FLAGS} -Wl,--no-rosegment")
set(ANDROID_LINKER_FLAGS "${ANDROID_LINKER_FLAGS} -Wl,--fatal-warnings")
set(ANDROID_LINKER_FLAGS "${ANDROID_LINKER_FLAGS} -Wl,--gc-sections")
set(ANDROID_LINKER_FLAGS "${ANDROID_LINKER_FLAGS} -Wl,--no-undefined")

# Set linker flags
set(CMAKE_EXE_LINKER_FLAGS_INIT "${ANDROID_LINKER_FLAGS}")
set(CMAKE_SHARED_LINKER_FLAGS_INIT "${ANDROID_LINKER_FLAGS} -Wl,-soname,")

# STL configuration
if(ANDROID_STL STREQUAL "c++_shared")
    set(CMAKE_CXX_FLAGS_INIT "${CMAKE_CXX_FLAGS_INIT} -stdlib=libc++")
    set(CMAKE_EXE_LINKER_FLAGS_INIT "${CMAKE_EXE_LINKER_FLAGS_INIT} -lc++_shared")
    set(CMAKE_SHARED_LINKER_FLAGS_INIT "${CMAKE_SHARED_LINKER_FLAGS_INIT} -lc++_shared")
elseif(ANDROID_STL STREQUAL "c++_static")
    set(CMAKE_CXX_FLAGS_INIT "${CMAKE_CXX_FLAGS_INIT} -stdlib=libc++")
    set(CMAKE_EXE_LINKER_FLAGS_INIT "${CMAKE_EXE_LINKER_FLAGS_INIT} -static-libstdc++")
    set(CMAKE_SHARED_LINKER_FLAGS_INIT "${CMAKE_SHARED_LINKER_FLAGS_INIT} -static-libstdc++")
endif()

# Set build type specific flags
set(CMAKE_C_FLAGS_DEBUG_INIT "-g -O0 -fno-limit-debug-info")
set(CMAKE_CXX_FLAGS_DEBUG_INIT "-g -O0 -fno-limit-debug-info")
set(CMAKE_C_FLAGS_RELEASE_INIT "-O2 -DNDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE_INIT "-O2 -DNDEBUG")

# Android-specific definitions
add_definitions(-DANDROID)
add_definitions(-D__ANDROID_API__=${ANDROID_PLATFORM_LEVEL})

# Set properties for Android build
set(ANDROID TRUE)
set(CMAKE_CROSSCOMPILING TRUE)

# Configure pkg-config for cross-compilation
set(ENV{PKG_CONFIG_LIBDIR} "${CMAKE_SYSROOT}/usr/lib/pkgconfig:${CMAKE_SYSROOT}/usr/share/pkgconfig")
set(ENV{PKG_CONFIG_SYSROOT_DIR} "${CMAKE_SYSROOT}")

message(STATUS "Android toolchain configured:")
message(STATUS "  NDK: ${ANDROID_NDK}")
message(STATUS "  ABI: ${ANDROID_ABI}")
message(STATUS "  Platform: ${ANDROID_PLATFORM}")
message(STATUS "  STL: ${ANDROID_STL}")
message(STATUS "  Toolchain: ${ANDROID_TOOLCHAIN_ROOT}")

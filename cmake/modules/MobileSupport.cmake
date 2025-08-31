# MobileSupport.cmake
# Comprehensive mobile platform development support for Android and iOS

# Function to detect Android NDK
function(detect_android_ndk)
    # Try to find Android NDK in various locations
    set(ANDROID_NDK_SEARCH_PATHS
        "$ENV{ANDROID_NDK_ROOT}"
        "$ENV{ANDROID_NDK}"
        "$ENV{NDK_ROOT}"
        "$ENV{ANDROID_HOME}/ndk-bundle"
        "$ENV{ANDROID_SDK_ROOT}/ndk-bundle"
        "/opt/android-ndk"
        "/usr/local/android-ndk"
        "C:/Android/Sdk/ndk-bundle"
        "C:/android-ndk"
    )

    foreach(SEARCH_PATH ${ANDROID_NDK_SEARCH_PATHS})
        if(EXISTS "${SEARCH_PATH}/build/cmake/android.toolchain.cmake")
            set(ANDROID_NDK_ROOT "${SEARCH_PATH}" PARENT_SCOPE)
            message(STATUS "Found Android NDK at: ${SEARCH_PATH}")
            return()
        endif()
    endforeach()

    message(WARNING "Android NDK not found. Android builds will not be available.")
    set(ANDROID_NDK_AVAILABLE FALSE PARENT_SCOPE)
endfunction()

# Function to detect Xcode and iOS SDK
function(detect_ios_sdk)
    if(NOT APPLE)
        message(WARNING "iOS development is only supported on macOS")
        set(IOS_SDK_AVAILABLE FALSE PARENT_SCOPE)
        return()
    endif()

    # Check if Xcode is installed
    execute_process(
        COMMAND xcode-select -print-path
        OUTPUT_VARIABLE XCODE_DEVELOPER_DIR
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET
    )

    if(NOT XCODE_DEVELOPER_DIR OR NOT EXISTS "${XCODE_DEVELOPER_DIR}")
        message(WARNING "Xcode not found. iOS builds will not be available.")
        set(IOS_SDK_AVAILABLE FALSE PARENT_SCOPE)
        return()
    endif()

    # Check for iOS SDK
    execute_process(
        COMMAND xcrun --sdk iphoneos --show-sdk-path
        OUTPUT_VARIABLE IOS_SDK_PATH
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET
    )

    if(NOT IOS_SDK_PATH OR NOT EXISTS "${IOS_SDK_PATH}")
        message(WARNING "iOS SDK not found. iOS builds will not be available.")
        set(IOS_SDK_AVAILABLE FALSE PARENT_SCOPE)
        return()
    endif()

    set(XCODE_DEVELOPER_DIR "${XCODE_DEVELOPER_DIR}" PARENT_SCOPE)
    set(IOS_SDK_PATH "${IOS_SDK_PATH}" PARENT_SCOPE)
    set(IOS_SDK_AVAILABLE TRUE PARENT_SCOPE)
    message(STATUS "Found iOS SDK at: ${IOS_SDK_PATH}")
endfunction()

# Function to configure Android target
function(configure_android_target target_name)
    set(options SHARED_STL STATIC_STL)
    set(oneValueArgs 
        ABI 
        API_LEVEL 
        STL_TYPE 
        APP_PLATFORM
        PACKAGE_NAME
        VERSION_CODE
        VERSION_NAME
    )
    set(multiValueArgs 
        PERMISSIONS 
        FEATURES 
        NATIVE_LIBS
        ASSETS
        RESOURCES
    )
    cmake_parse_arguments(ANDROID "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if(NOT CMAKE_SYSTEM_NAME STREQUAL "Android")
        message(WARNING "configure_android_target called but not building for Android")
        return()
    endif()

    # Set default values
    if(NOT ANDROID_ABI)
        set(ANDROID_ABI "arm64-v8a")
    endif()

    if(NOT ANDROID_API_LEVEL)
        set(ANDROID_API_LEVEL "21")
    endif()

    if(NOT ANDROID_STL_TYPE)
        set(ANDROID_STL_TYPE "c++_shared")
    endif()

    if(NOT ANDROID_PACKAGE_NAME)
        set(ANDROID_PACKAGE_NAME "com.example.${target_name}")
    endif()

    if(NOT ANDROID_VERSION_CODE)
        set(ANDROID_VERSION_CODE "1")
    endif()

    if(NOT ANDROID_VERSION_NAME)
        set(ANDROID_VERSION_NAME "1.0")
    endif()

    # Configure target properties
    set_target_properties(${target_name} PROPERTIES
        ANDROID_ABI ${ANDROID_ABI}
        ANDROID_API_LEVEL ${ANDROID_API_LEVEL}
        ANDROID_STL_TYPE ${ANDROID_STL_TYPE}
    )

    # Add Android-specific definitions
    target_compile_definitions(${target_name} PRIVATE
        ANDROID
        __ANDROID_API__=${ANDROID_API_LEVEL}
        ANDROID_ABI_${ANDROID_ABI}
    )

    # Add Android-specific libraries
    target_link_libraries(${target_name} PRIVATE
        log
        android
    )

    # Configure STL
    if(ANDROID_SHARED_STL OR ANDROID_STL_TYPE STREQUAL "c++_shared")
        target_link_libraries(${target_name} PRIVATE c++_shared)
    elseif(ANDROID_STATIC_STL OR ANDROID_STL_TYPE STREQUAL "c++_static")
        target_link_libraries(${target_name} PRIVATE c++_static)
    endif()

    message(STATUS "Configured Android target: ${target_name} (${ANDROID_ABI}, API ${ANDROID_API_LEVEL})")
endfunction()

# Function to configure iOS target
function(configure_ios_target target_name)
    set(options DEVICE SIMULATOR UNIVERSAL)
    set(oneValueArgs 
        DEPLOYMENT_TARGET 
        BUNDLE_IDENTIFIER 
        VERSION 
        BUILD_NUMBER
        TEAM_ID
        PROVISIONING_PROFILE
    )
    set(multiValueArgs 
        FRAMEWORKS 
        WEAK_FRAMEWORKS 
        LIBRARIES
        ARCHITECTURES
        CAPABILITIES
    )
    cmake_parse_arguments(IOS "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if(NOT CMAKE_SYSTEM_NAME STREQUAL "iOS")
        message(WARNING "configure_ios_target called but not building for iOS")
        return()
    endif()

    # Set default values
    if(NOT IOS_DEPLOYMENT_TARGET)
        set(IOS_DEPLOYMENT_TARGET "12.0")
    endif()

    if(NOT IOS_BUNDLE_IDENTIFIER)
        set(IOS_BUNDLE_IDENTIFIER "com.example.${target_name}")
    endif()

    if(NOT IOS_VERSION)
        set(IOS_VERSION "1.0")
    endif()

    if(NOT IOS_BUILD_NUMBER)
        set(IOS_BUILD_NUMBER "1")
    endif()

    # Configure target properties
    set_target_properties(${target_name} PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_GUI_IDENTIFIER ${IOS_BUNDLE_IDENTIFIER}
        MACOSX_BUNDLE_BUNDLE_NAME ${target_name}
        MACOSX_BUNDLE_BUNDLE_VERSION ${IOS_BUILD_NUMBER}
        MACOSX_BUNDLE_SHORT_VERSION_STRING ${IOS_VERSION}
        XCODE_ATTRIBUTE_IPHONEOS_DEPLOYMENT_TARGET ${IOS_DEPLOYMENT_TARGET}
        XCODE_ATTRIBUTE_PRODUCT_BUNDLE_IDENTIFIER ${IOS_BUNDLE_IDENTIFIER}
        XCODE_ATTRIBUTE_TARGETED_DEVICE_FAMILY "1,2"  # iPhone and iPad
    )

    # Set platform-specific properties
    if(IOS_DEVICE)
        set_target_properties(${target_name} PROPERTIES
            XCODE_ATTRIBUTE_ONLY_ACTIVE_ARCH NO
        )
    elseif(IOS_SIMULATOR)
        set_target_properties(${target_name} PROPERTIES
            XCODE_ATTRIBUTE_ONLY_ACTIVE_ARCH YES
        )
    endif()

    # Configure code signing
    if(IOS_TEAM_ID)
        set_target_properties(${target_name} PROPERTIES
            XCODE_ATTRIBUTE_DEVELOPMENT_TEAM ${IOS_TEAM_ID}
            XCODE_ATTRIBUTE_CODE_SIGNING_REQUIRED YES
        )
    else()
        set_target_properties(${target_name} PROPERTIES
            XCODE_ATTRIBUTE_CODE_SIGNING_REQUIRED NO
            XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY ""
        )
    endif()

    if(IOS_PROVISIONING_PROFILE)
        set_target_properties(${target_name} PROPERTIES
            XCODE_ATTRIBUTE_PROVISIONING_PROFILE_SPECIFIER ${IOS_PROVISIONING_PROFILE}
        )
    endif()

    # Add frameworks
    if(IOS_FRAMEWORKS)
        foreach(FRAMEWORK ${IOS_FRAMEWORKS})
            find_library(${FRAMEWORK}_FRAMEWORK ${FRAMEWORK})
            if(${FRAMEWORK}_FRAMEWORK)
                target_link_libraries(${target_name} PRIVATE ${${FRAMEWORK}_FRAMEWORK})
            endif()
        endforeach()
    endif()

    # Add weak frameworks
    if(IOS_WEAK_FRAMEWORKS)
        foreach(FRAMEWORK ${IOS_WEAK_FRAMEWORKS})
            find_library(${FRAMEWORK}_FRAMEWORK ${FRAMEWORK})
            if(${FRAMEWORK}_FRAMEWORK)
                target_link_libraries(${target_name} PRIVATE "-weak_framework ${FRAMEWORK}")
            endif()
        endforeach()
    endif()

    # Add libraries
    if(IOS_LIBRARIES)
        target_link_libraries(${target_name} PRIVATE ${IOS_LIBRARIES})
    endif()

    # Enable modern Objective-C features
    set_target_properties(${target_name} PROPERTIES
        XCODE_ATTRIBUTE_CLANG_ENABLE_OBJC_ARC YES
        XCODE_ATTRIBUTE_CLANG_ENABLE_OBJC_WEAK YES
        XCODE_ATTRIBUTE_CLANG_ENABLE_MODULES YES
    )

    message(STATUS "Configured iOS target: ${target_name} (${IOS_DEPLOYMENT_TARGET})")
endfunction()

# Function to create Android APK
function(create_android_apk target_name)
    set(options)
    set(oneValueArgs 
        PACKAGE_NAME 
        VERSION_CODE 
        VERSION_NAME 
        MIN_SDK_VERSION 
        TARGET_SDK_VERSION
        KEYSTORE_PATH
        KEYSTORE_PASSWORD
        KEY_ALIAS
        KEY_PASSWORD
    )
    set(multiValueArgs PERMISSIONS FEATURES)
    cmake_parse_arguments(APK "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if(NOT CMAKE_SYSTEM_NAME STREQUAL "Android")
        return()
    endif()

    # Find Android SDK tools
    find_program(AAPT_EXECUTABLE aapt)
    find_program(DX_EXECUTABLE dx)
    find_program(ZIPALIGN_EXECUTABLE zipalign)
    find_program(APKSIGNER_EXECUTABLE apksigner)

    if(NOT AAPT_EXECUTABLE)
        message(WARNING "Android SDK build tools not found. APK creation will not be available.")
        return()
    endif()

    # Set default values
    if(NOT APK_PACKAGE_NAME)
        set(APK_PACKAGE_NAME "com.example.${target_name}")
    endif()

    if(NOT APK_VERSION_CODE)
        set(APK_VERSION_CODE "1")
    endif()

    if(NOT APK_VERSION_NAME)
        set(APK_VERSION_NAME "1.0")
    endif()

    if(NOT APK_MIN_SDK_VERSION)
        set(APK_MIN_SDK_VERSION "21")
    endif()

    if(NOT APK_TARGET_SDK_VERSION)
        set(APK_TARGET_SDK_VERSION "30")
    endif()

    # Create APK build target
    add_custom_target(apk-${target_name}
        DEPENDS ${target_name}
        COMMENT "Creating APK for ${target_name}"
    )

    message(STATUS "Created APK target: apk-${target_name}")
endfunction()

# Function to create iOS app bundle
function(create_ios_app target_name)
    set(options)
    set(oneValueArgs 
        BUNDLE_IDENTIFIER 
        VERSION 
        BUILD_NUMBER 
        DISPLAY_NAME
        ICON_FILE
        LAUNCH_SCREEN
    )
    set(multiValueArgs SUPPORTED_ORIENTATIONS REQUIRED_CAPABILITIES)
    cmake_parse_arguments(APP "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if(NOT CMAKE_SYSTEM_NAME STREQUAL "iOS")
        return()
    endif()

    # Set default values
    if(NOT APP_BUNDLE_IDENTIFIER)
        set(APP_BUNDLE_IDENTIFIER "com.example.${target_name}")
    endif()

    if(NOT APP_VERSION)
        set(APP_VERSION "1.0")
    endif()

    if(NOT APP_BUILD_NUMBER)
        set(APP_BUILD_NUMBER "1")
    endif()

    if(NOT APP_DISPLAY_NAME)
        set(APP_DISPLAY_NAME "${target_name}")
    endif()

    # Configure app bundle properties
    set_target_properties(${target_name} PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_GUI_IDENTIFIER ${APP_BUNDLE_IDENTIFIER}
        MACOSX_BUNDLE_BUNDLE_NAME ${APP_DISPLAY_NAME}
        MACOSX_BUNDLE_BUNDLE_VERSION ${APP_BUILD_NUMBER}
        MACOSX_BUNDLE_SHORT_VERSION_STRING ${APP_VERSION}
        MACOSX_BUNDLE_LONG_VERSION_STRING "${APP_VERSION}.${APP_BUILD_NUMBER}"
    )

    # Add Info.plist configuration
    if(APP_SUPPORTED_ORIENTATIONS)
        string(REPLACE ";" "," ORIENTATIONS "${APP_SUPPORTED_ORIENTATIONS}")
        set_target_properties(${target_name} PROPERTIES
            XCODE_ATTRIBUTE_SUPPORTED_INTERFACE_ORIENTATIONS "${ORIENTATIONS}"
        )
    endif()

    if(APP_REQUIRED_CAPABILITIES)
        string(REPLACE ";" "," CAPABILITIES "${APP_REQUIRED_CAPABILITIES}")
        set_target_properties(${target_name} PROPERTIES
            XCODE_ATTRIBUTE_REQUIRED_DEVICE_CAPABILITIES "${CAPABILITIES}"
        )
    endif()

    message(STATUS "Created iOS app: ${target_name}")
endfunction()

# Function to setup mobile development environment
function(setup_mobile_environment)
    detect_android_ndk()
    detect_ios_sdk()

    if(ANDROID_NDK_AVAILABLE)
        message(STATUS "Android development environment available")
        set(ENABLE_ANDROID_BUILDS TRUE PARENT_SCOPE)
    else()
        set(ENABLE_ANDROID_BUILDS FALSE PARENT_SCOPE)
    endif()

    if(IOS_SDK_AVAILABLE)
        message(STATUS "iOS development environment available")
        set(ENABLE_IOS_BUILDS TRUE PARENT_SCOPE)
    else()
        set(ENABLE_IOS_BUILDS FALSE PARENT_SCOPE)
    endif()
endfunction()

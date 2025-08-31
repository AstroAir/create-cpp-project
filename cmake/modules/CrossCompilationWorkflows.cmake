# CrossCompilationWorkflows.cmake
# Advanced cross-compilation workflows with dependency management and testing

# Function to setup cross-compilation environment
function(setup_cross_compilation_environment)
    set(options VERBOSE)
    set(oneValueArgs TARGET_PLATFORM HOST_PLATFORM)
    set(multiValueArgs REQUIRED_TOOLS)
    cmake_parse_arguments(CROSS "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if(NOT CROSS_TARGET_PLATFORM)
        set(CROSS_TARGET_PLATFORM "${CMAKE_SYSTEM_NAME}")
    endif()

    if(NOT CROSS_HOST_PLATFORM)
        set(CROSS_HOST_PLATFORM "${CMAKE_HOST_SYSTEM_NAME}")
    endif()

    message(STATUS "Setting up cross-compilation: ${CROSS_HOST_PLATFORM} -> ${CROSS_TARGET_PLATFORM}")

    # Check if we're actually cross-compiling
    if(CMAKE_CROSSCOMPILING)
        set(IS_CROSS_COMPILING TRUE PARENT_SCOPE)
        
        # Validate required tools
        if(CROSS_REQUIRED_TOOLS)
            foreach(TOOL ${CROSS_REQUIRED_TOOLS})
                find_program(${TOOL}_EXECUTABLE ${TOOL})
                if(NOT ${TOOL}_EXECUTABLE)
                    message(FATAL_ERROR "Required cross-compilation tool not found: ${TOOL}")
                endif()
                if(CROSS_VERBOSE)
                    message(STATUS "Found cross-compilation tool: ${TOOL} at ${${TOOL}_EXECUTABLE}")
                endif()
            endforeach()
        endif()

        # Set up cross-compilation specific variables
        set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER PARENT_SCOPE)
        set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY PARENT_SCOPE)
        set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY PARENT_SCOPE)
        set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY PARENT_SCOPE)

        message(STATUS "Cross-compilation environment configured")
    else()
        set(IS_CROSS_COMPILING FALSE PARENT_SCOPE)
        message(STATUS "Native compilation detected")
    endif()
endfunction()

# Function to manage cross-compilation dependencies
function(manage_cross_dependencies target_name)
    set(options SYSTEM_DEPS VCPKG_DEPS CONAN_DEPS)
    set(oneValueArgs VCPKG_TRIPLET CONAN_PROFILE)
    set(multiValueArgs SYSTEM_LIBRARIES VCPKG_PACKAGES CONAN_PACKAGES)
    cmake_parse_arguments(DEPS "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if(NOT CMAKE_CROSSCOMPILING)
        message(STATUS "Not cross-compiling, using standard dependency management")
        return()
    endif()

    message(STATUS "Managing cross-compilation dependencies for: ${target_name}")

    # Handle system dependencies
    if(DEPS_SYSTEM_DEPS AND DEPS_SYSTEM_LIBRARIES)
        foreach(LIB ${DEPS_SYSTEM_LIBRARIES})
            find_library(${LIB}_LIBRARY ${LIB})
            if(${LIB}_LIBRARY)
                target_link_libraries(${target_name} PRIVATE ${${LIB}_LIBRARY})
                message(STATUS "Found system library: ${LIB}")
            else()
                message(WARNING "System library not found: ${LIB}")
            endif()
        endforeach()
    endif()

    # Handle vcpkg dependencies
    if(DEPS_VCPKG_DEPS AND DEPS_VCPKG_PACKAGES)
        if(NOT DEPS_VCPKG_TRIPLET)
            # Auto-detect vcpkg triplet based on target platform
            if(CMAKE_SYSTEM_NAME STREQUAL "Android")
                if(ANDROID_ABI STREQUAL "arm64-v8a")
                    set(DEPS_VCPKG_TRIPLET "arm64-android")
                elseif(ANDROID_ABI STREQUAL "armeabi-v7a")
                    set(DEPS_VCPKG_TRIPLET "arm-android")
                elseif(ANDROID_ABI STREQUAL "x86_64")
                    set(DEPS_VCPKG_TRIPLET "x64-android")
                endif()
            elseif(CMAKE_SYSTEM_NAME STREQUAL "iOS")
                set(DEPS_VCPKG_TRIPLET "arm64-ios")
            elseif(CMAKE_SYSTEM_NAME STREQUAL "Emscripten")
                set(DEPS_VCPKG_TRIPLET "wasm32-emscripten")
            endif()
        endif()

        if(DEPS_VCPKG_TRIPLET)
            set(VCPKG_TARGET_TRIPLET ${DEPS_VCPKG_TRIPLET} PARENT_SCOPE)
            message(STATUS "Using vcpkg triplet: ${DEPS_VCPKG_TRIPLET}")

            foreach(PACKAGE ${DEPS_VCPKG_PACKAGES})
                find_package(${PACKAGE} REQUIRED)
                if(TARGET ${PACKAGE}::${PACKAGE})
                    target_link_libraries(${target_name} PRIVATE ${PACKAGE}::${PACKAGE})
                endif()
            endforeach()
        endif()
    endif()

    # Handle Conan dependencies
    if(DEPS_CONAN_DEPS AND DEPS_CONAN_PACKAGES)
        if(NOT DEPS_CONAN_PROFILE)
            # Auto-detect Conan profile based on target platform
            if(CMAKE_SYSTEM_NAME STREQUAL "Android")
                set(DEPS_CONAN_PROFILE "android")
            elseif(CMAKE_SYSTEM_NAME STREQUAL "iOS")
                set(DEPS_CONAN_PROFILE "ios")
            endif()
        endif()

        if(DEPS_CONAN_PROFILE)
            message(STATUS "Using Conan profile: ${DEPS_CONAN_PROFILE}")
            # Note: Conan integration would require additional setup
        endif()
    endif()
endfunction()

# Function to create cross-compilation test
function(create_cross_compilation_test test_name target_name)
    set(options EMULATOR_TEST DEVICE_TEST QEMU_TEST)
    set(oneValueArgs EMULATOR_PATH DEVICE_ID QEMU_SYSTEM)
    set(multiValueArgs TEST_ARGS EMULATOR_ARGS QEMU_ARGS)
    cmake_parse_arguments(TEST "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if(NOT CMAKE_CROSSCOMPILING)
        # Native test
        add_test(NAME ${test_name} COMMAND $<TARGET_FILE:${target_name}> ${TEST_TEST_ARGS})
        return()
    endif()

    message(STATUS "Creating cross-compilation test: ${test_name}")

    # Android emulator test
    if(CMAKE_SYSTEM_NAME STREQUAL "Android" AND TEST_EMULATOR_TEST)
        find_program(ADB_EXECUTABLE adb)
        if(ADB_EXECUTABLE)
            # Push executable to device/emulator and run
            add_test(NAME ${test_name}
                COMMAND ${CMAKE_COMMAND} -E env
                ${ADB_EXECUTABLE} push $<TARGET_FILE:${target_name}> /data/local/tmp/
                COMMAND ${ADB_EXECUTABLE} shell chmod 755 /data/local/tmp/$<TARGET_FILE_NAME:${target_name}>
                COMMAND ${ADB_EXECUTABLE} shell /data/local/tmp/$<TARGET_FILE_NAME:${target_name}> ${TEST_TEST_ARGS}
            )
            message(STATUS "Created Android emulator test: ${test_name}")
        else()
            message(WARNING "ADB not found. Android tests will not be available.")
        endif()
    endif()

    # iOS simulator test
    if(CMAKE_SYSTEM_NAME STREQUAL "iOS" AND TEST_EMULATOR_TEST)
        find_program(XCRUN_EXECUTABLE xcrun)
        if(XCRUN_EXECUTABLE)
            add_test(NAME ${test_name}
                COMMAND ${XCRUN_EXECUTABLE} simctl spawn booted $<TARGET_FILE:${target_name}> ${TEST_TEST_ARGS}
            )
            message(STATUS "Created iOS simulator test: ${test_name}")
        else()
            message(WARNING "xcrun not found. iOS tests will not be available.")
        endif()
    endif()

    # QEMU test for embedded systems
    if(CMAKE_SYSTEM_NAME STREQUAL "Generic" AND TEST_QEMU_TEST)
        if(NOT TEST_QEMU_SYSTEM)
            # Auto-detect QEMU system based on processor
            if(CMAKE_SYSTEM_PROCESSOR MATCHES "arm")
                set(TEST_QEMU_SYSTEM "qemu-arm")
            elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64")
                set(TEST_QEMU_SYSTEM "qemu-aarch64")
            endif()
        endif()

        if(TEST_QEMU_SYSTEM)
            find_program(QEMU_EXECUTABLE ${TEST_QEMU_SYSTEM})
            if(QEMU_EXECUTABLE)
                add_test(NAME ${test_name}
                    COMMAND ${QEMU_EXECUTABLE} ${TEST_QEMU_ARGS} $<TARGET_FILE:${target_name}> ${TEST_TEST_ARGS}
                )
                message(STATUS "Created QEMU test: ${test_name}")
            else()
                message(WARNING "${TEST_QEMU_SYSTEM} not found. QEMU tests will not be available.")
            endif()
        endif()
    endif()

    # WebAssembly test with Node.js
    if(CMAKE_SYSTEM_NAME STREQUAL "Emscripten")
        find_program(NODE_EXECUTABLE node)
        if(NODE_EXECUTABLE)
            add_test(NAME ${test_name}
                COMMAND ${NODE_EXECUTABLE} $<TARGET_FILE:${target_name}> ${TEST_TEST_ARGS}
            )
            message(STATUS "Created WebAssembly test: ${test_name}")
        else()
            message(WARNING "Node.js not found. WebAssembly tests will not be available.")
        endif()
    endif()
endfunction()

# Function to create cross-compilation workflow
function(create_cross_compilation_workflow workflow_name)
    set(options PARALLEL_BUILD STAGED_BUILD)
    set(oneValueArgs)
    set(multiValueArgs TARGETS PLATFORMS CONFIGURATIONS)
    cmake_parse_arguments(WORKFLOW "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if(NOT WORKFLOW_TARGETS)
        message(FATAL_ERROR "TARGETS is required for create_cross_compilation_workflow")
    endif()

    if(NOT WORKFLOW_PLATFORMS)
        set(WORKFLOW_PLATFORMS "windows" "linux" "macos" "android" "ios")
    endif()

    if(NOT WORKFLOW_CONFIGURATIONS)
        set(WORKFLOW_CONFIGURATIONS "Debug" "Release")
    endif()

    message(STATUS "Creating cross-compilation workflow: ${workflow_name}")

    # Create workflow target
    add_custom_target(${workflow_name}
        COMMENT "Running cross-compilation workflow: ${workflow_name}"
    )

    # Create platform-specific targets
    foreach(PLATFORM ${WORKFLOW_PLATFORMS})
        foreach(CONFIG ${WORKFLOW_CONFIGURATIONS})
            set(WORKFLOW_TARGET "${workflow_name}-${PLATFORM}-${CONFIG}")
            
            add_custom_target(${WORKFLOW_TARGET}
                COMMENT "Building for ${PLATFORM} (${CONFIG})"
            )
            
            add_dependencies(${workflow_name} ${WORKFLOW_TARGET})
            
            # Add actual build commands based on platform
            if(PLATFORM STREQUAL "android")
                add_custom_command(TARGET ${WORKFLOW_TARGET}
                    COMMAND ${CMAKE_COMMAND} --build . --preset android-${CONFIG}
                    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
                )
            elseif(PLATFORM STREQUAL "ios")
                add_custom_command(TARGET ${WORKFLOW_TARGET}
                    COMMAND ${CMAKE_COMMAND} --build . --preset ios-${CONFIG}
                    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
                )
            elseif(PLATFORM STREQUAL "wasm")
                add_custom_command(TARGET ${WORKFLOW_TARGET}
                    COMMAND ${CMAKE_COMMAND} --build . --preset wasm-${CONFIG}
                    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
                )
            endif()
        endforeach()
    endforeach()

    message(STATUS "Created cross-compilation workflow: ${workflow_name}")
endfunction()

# Function to validate cross-compilation setup
function(validate_cross_compilation_setup)
    set(options VERBOSE)
    set(oneValueArgs)
    set(multiValueArgs REQUIRED_PLATFORMS)
    cmake_parse_arguments(VALIDATE "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    message(STATUS "Validating cross-compilation setup...")

    # Check if we're cross-compiling
    if(CMAKE_CROSSCOMPILING)
        message(STATUS "✓ Cross-compilation detected")
        message(STATUS "  Host system: ${CMAKE_HOST_SYSTEM_NAME}")
        message(STATUS "  Target system: ${CMAKE_SYSTEM_NAME}")
        message(STATUS "  Target processor: ${CMAKE_SYSTEM_PROCESSOR}")
    else()
        message(STATUS "ℹ Native compilation (not cross-compiling)")
    endif()

    # Validate toolchain
    if(CMAKE_TOOLCHAIN_FILE)
        message(STATUS "✓ Toolchain file: ${CMAKE_TOOLCHAIN_FILE}")
    else()
        message(STATUS "ℹ No toolchain file specified")
    endif()

    # Validate compilers
    if(CMAKE_C_COMPILER)
        message(STATUS "✓ C compiler: ${CMAKE_C_COMPILER}")
    endif()
    
    if(CMAKE_CXX_COMPILER)
        message(STATUS "✓ C++ compiler: ${CMAKE_CXX_COMPILER}")
    endif()

    # Platform-specific validations
    if(CMAKE_SYSTEM_NAME STREQUAL "Android")
        if(ANDROID_NDK)
            message(STATUS "✓ Android NDK: ${ANDROID_NDK}")
        else()
            message(WARNING "✗ Android NDK not found")
        endif()
        
        if(ANDROID_ABI)
            message(STATUS "✓ Android ABI: ${ANDROID_ABI}")
        endif()
    endif()

    if(CMAKE_SYSTEM_NAME STREQUAL "iOS")
        if(CMAKE_OSX_SYSROOT)
            message(STATUS "✓ iOS SDK: ${CMAKE_OSX_SYSROOT}")
        else()
            message(WARNING "✗ iOS SDK not found")
        endif()
    endif()

    if(CMAKE_SYSTEM_NAME STREQUAL "Emscripten")
        if(EMSCRIPTEN_ROOT_PATH)
            message(STATUS "✓ Emscripten: ${EMSCRIPTEN_ROOT_PATH}")
        else()
            message(WARNING "✗ Emscripten not found")
        endif()
    endif()

    message(STATUS "Cross-compilation setup validation completed")
endfunction()

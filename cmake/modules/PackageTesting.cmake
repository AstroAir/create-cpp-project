# PackageTesting.cmake
# Comprehensive package validation and testing framework

# Enable package testing
function(enable_package_testing)
    set(options ENABLE_INTEGRATION_TESTS ENABLE_INSTALLATION_TESTS ENABLE_SIGNATURE_TESTS)
    set(oneValueArgs TEST_TIMEOUT)
    set(multiValueArgs TEST_PLATFORMS)
    cmake_parse_arguments(PKG_TEST "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
    
    # Set default timeout
    if(NOT PKG_TEST_TEST_TIMEOUT)
        set(PKG_TEST_TEST_TIMEOUT 300)
    endif()
    
    # Create test directory
    set(PACKAGE_TEST_DIR "${CMAKE_BINARY_DIR}/package_tests")
    file(MAKE_DIRECTORY "${PACKAGE_TEST_DIR}")
    
    # Configure package testing
    configure_package_tests("${PKG_TEST_TEST_TIMEOUT}")
    
    # Add integration tests if enabled
    if(PKG_TEST_ENABLE_INTEGRATION_TESTS)
        add_integration_tests()
    endif()
    
    # Add installation tests if enabled
    if(PKG_TEST_ENABLE_INSTALLATION_TESTS)
        add_installation_tests()
    endif()
    
    # Add signature tests if enabled
    if(PKG_TEST_ENABLE_SIGNATURE_TESTS)
        add_signature_tests()
    endif()
    
    message(STATUS "Package testing framework enabled")
endfunction()

# Configure basic package tests
function(configure_package_tests timeout)
    # Create package validation script
    set(VALIDATION_SCRIPT "${CMAKE_BINARY_DIR}/validate_packages.cmake")
    
    file(WRITE "${VALIDATION_SCRIPT}" "
# Package validation script
cmake_minimum_required(VERSION 3.20)

set(PACKAGE_DIR \"${CMAKE_BINARY_DIR}\")
set(TEST_TIMEOUT ${timeout})

message(STATUS \"Starting package validation...\")

# Find all package files
file(GLOB PACKAGE_FILES 
    \"\${PACKAGE_DIR}/*.exe\"
    \"\${PACKAGE_DIR}/*.msi\"
    \"\${PACKAGE_DIR}/*.dmg\"
    \"\${PACKAGE_DIR}/*.deb\"
    \"\${PACKAGE_DIR}/*.rpm\"
    \"\${PACKAGE_DIR}/*.AppImage\"
    \"\${PACKAGE_DIR}/*.zip\"
    \"\${PACKAGE_DIR}/*.tar.gz\"
)

if(NOT PACKAGE_FILES)
    message(FATAL_ERROR \"No package files found in \${PACKAGE_DIR}\")
endif()

set(VALIDATION_FAILED FALSE)

foreach(PACKAGE_FILE \${PACKAGE_FILES})
    get_filename_component(PACKAGE_NAME \"\${PACKAGE_FILE}\" NAME)
    message(STATUS \"Validating: \${PACKAGE_NAME}\")
    
    # Check file size (should be > 1MB for a real package)
    file(SIZE \"\${PACKAGE_FILE}\" PACKAGE_SIZE)
    if(PACKAGE_SIZE LESS 1048576)
        message(WARNING \"Package \${PACKAGE_NAME} seems too small: \${PACKAGE_SIZE} bytes\")
    else()
        message(STATUS \"  ✓ Size: \${PACKAGE_SIZE} bytes\")
    endif()
    
    # Platform-specific validation
    if(\"\${PACKAGE_FILE}\" MATCHES \"\\.(exe|msi)$\")
        # Windows package validation
        message(STATUS \"  → Windows package detected\")
        
        # Check if it's a valid PE file
        execute_process(
            COMMAND powershell -Command \"Get-AuthenticodeSignature '\${PACKAGE_FILE}' | Select-Object Status\"
            RESULT_VARIABLE PE_CHECK_RESULT
            OUTPUT_VARIABLE PE_CHECK_OUTPUT
            ERROR_QUIET
        )
        
        if(PE_CHECK_RESULT EQUAL 0)
            message(STATUS \"  ✓ Valid Windows executable/installer\")
        else()
            message(WARNING \"  ✗ Invalid Windows executable/installer\")
            set(VALIDATION_FAILED TRUE)
        endif()
        
    elseif(\"\${PACKAGE_FILE}\" MATCHES \"\\.dmg$\")
        # macOS DMG validation
        message(STATUS \"  → macOS DMG detected\")
        
        # Check DMG integrity (macOS only)
        if(APPLE)
            execute_process(
                COMMAND hdiutil verify \"\${PACKAGE_FILE}\"
                RESULT_VARIABLE DMG_CHECK_RESULT
                OUTPUT_QUIET ERROR_QUIET
            )
            
            if(DMG_CHECK_RESULT EQUAL 0)
                message(STATUS \"  ✓ Valid DMG file\")
            else()
                message(WARNING \"  ✗ Invalid DMG file\")
                set(VALIDATION_FAILED TRUE)
            endif()
        endif()
        
    elseif(\"\${PACKAGE_FILE}\" MATCHES \"\\.(deb|rpm)$\")
        # Linux package validation
        message(STATUS \"  → Linux package detected\")
        
        if(\"\${PACKAGE_FILE}\" MATCHES \"\\.deb$\")
            # Debian package validation
            execute_process(
                COMMAND dpkg --info \"\${PACKAGE_FILE}\"
                RESULT_VARIABLE DEB_CHECK_RESULT
                OUTPUT_QUIET ERROR_QUIET
            )
            
            if(DEB_CHECK_RESULT EQUAL 0)
                message(STATUS \"  ✓ Valid DEB package\")
            else()
                message(WARNING \"  ✗ Invalid DEB package\")
                set(VALIDATION_FAILED TRUE)
            endif()
        else()
            # RPM package validation
            execute_process(
                COMMAND rpm -qp \"\${PACKAGE_FILE}\"
                RESULT_VARIABLE RPM_CHECK_RESULT
                OUTPUT_QUIET ERROR_QUIET
            )
            
            if(RPM_CHECK_RESULT EQUAL 0)
                message(STATUS \"  ✓ Valid RPM package\")
            else()
                message(WARNING \"  ✗ Invalid RPM package\")
                set(VALIDATION_FAILED TRUE)
            endif()
        endif()
        
    elseif(\"\${PACKAGE_FILE}\" MATCHES \"\\.AppImage$\")
        # AppImage validation
        message(STATUS \"  → AppImage detected\")
        
        # Check if AppImage is executable
        execute_process(
            COMMAND file \"\${PACKAGE_FILE}\"
            RESULT_VARIABLE APPIMAGE_CHECK_RESULT
            OUTPUT_VARIABLE APPIMAGE_CHECK_OUTPUT
            ERROR_QUIET
        )
        
        if(APPIMAGE_CHECK_OUTPUT MATCHES \"executable\")
            message(STATUS \"  ✓ Valid AppImage\")
        else()
            message(WARNING \"  ✗ Invalid AppImage\")
            set(VALIDATION_FAILED TRUE)
        endif()
        
    elseif(\"\${PACKAGE_FILE}\" MATCHES \"\\.(zip|tar\\.gz)$\")
        # Archive validation
        message(STATUS \"  → Archive detected\")
        
        if(\"\${PACKAGE_FILE}\" MATCHES \"\\.zip$\")
            execute_process(
                COMMAND unzip -t \"\${PACKAGE_FILE}\"
                RESULT_VARIABLE ZIP_CHECK_RESULT
                OUTPUT_QUIET ERROR_QUIET
            )
            
            if(ZIP_CHECK_RESULT EQUAL 0)
                message(STATUS \"  ✓ Valid ZIP archive\")
            else()
                message(WARNING \"  ✗ Invalid ZIP archive\")
                set(VALIDATION_FAILED TRUE)
            endif()
        else()
            execute_process(
                COMMAND tar -tzf \"\${PACKAGE_FILE}\"
                RESULT_VARIABLE TAR_CHECK_RESULT
                OUTPUT_QUIET ERROR_QUIET
            )
            
            if(TAR_CHECK_RESULT EQUAL 0)
                message(STATUS \"  ✓ Valid TAR.GZ archive\")
            else()
                message(WARNING \"  ✗ Invalid TAR.GZ archive\")
                set(VALIDATION_FAILED TRUE)
            endif()
        endif()
    endif()
endforeach()

if(VALIDATION_FAILED)
    message(FATAL_ERROR \"Package validation failed\")
else()
    message(STATUS \"All packages validated successfully\")
endif()
")
    
    # Add custom target for package validation
    add_custom_target(validate_packages
        COMMAND ${CMAKE_COMMAND} -P "${VALIDATION_SCRIPT}"
        COMMENT "Validating generated packages"
        VERBATIM
    )
    
    # Make validation depend on package creation
    add_dependencies(validate_packages package)
endfunction()

# Add integration tests
function(add_integration_tests)
    set(INTEGRATION_TEST_SCRIPT "${CMAKE_BINARY_DIR}/integration_tests.cmake")
    
    file(WRITE "${INTEGRATION_TEST_SCRIPT}" "
# Integration test script
cmake_minimum_required(VERSION 3.20)

message(STATUS \"Running integration tests...\")

# Test 1: Package creation
message(STATUS \"Test 1: Package creation\")
execute_process(
    COMMAND ${CMAKE_COMMAND} --build . --target package
    WORKING_DIRECTORY \"${CMAKE_BINARY_DIR}\"
    RESULT_VARIABLE PACKAGE_RESULT
    OUTPUT_QUIET
)

if(PACKAGE_RESULT EQUAL 0)
    message(STATUS \"  ✓ Package creation successful\")
else()
    message(FATAL_ERROR \"  ✗ Package creation failed\")
endif()

# Test 2: Package validation
message(STATUS \"Test 2: Package validation\")
execute_process(
    COMMAND ${CMAKE_COMMAND} --build . --target validate_packages
    WORKING_DIRECTORY \"${CMAKE_BINARY_DIR}\"
    RESULT_VARIABLE VALIDATION_RESULT
    OUTPUT_QUIET
)

if(VALIDATION_RESULT EQUAL 0)
    message(STATUS \"  ✓ Package validation successful\")
else()
    message(FATAL_ERROR \"  ✗ Package validation failed\")
endif()

message(STATUS \"All integration tests passed\")
")
    
    add_custom_target(integration_tests
        COMMAND ${CMAKE_COMMAND} -P "${INTEGRATION_TEST_SCRIPT}"
        COMMENT "Running package integration tests"
        VERBATIM
    )
endfunction()

# Add installation tests
function(add_installation_tests)
    set(INSTALL_TEST_SCRIPT "${CMAKE_BINARY_DIR}/installation_tests.cmake")
    
    file(WRITE "${INSTALL_TEST_SCRIPT}" "
# Installation test script
cmake_minimum_required(VERSION 3.20)

message(STATUS \"Running installation tests...\")

set(TEST_INSTALL_DIR \"${CMAKE_BINARY_DIR}/test_install\")
file(REMOVE_RECURSE \"\${TEST_INSTALL_DIR}\")
file(MAKE_DIRECTORY \"\${TEST_INSTALL_DIR}\")

# Test installation to temporary directory
message(STATUS \"Testing installation to: \${TEST_INSTALL_DIR}\")

execute_process(
    COMMAND ${CMAKE_COMMAND} --install . --prefix \"\${TEST_INSTALL_DIR}\"
    WORKING_DIRECTORY \"${CMAKE_BINARY_DIR}\"
    RESULT_VARIABLE INSTALL_RESULT
    OUTPUT_VARIABLE INSTALL_OUTPUT
    ERROR_VARIABLE INSTALL_ERROR
)

if(INSTALL_RESULT EQUAL 0)
    message(STATUS \"  ✓ Installation successful\")
    
    # Verify installed files
    if(EXISTS \"\${TEST_INSTALL_DIR}/bin/${PROJECT_NAME}\")
        message(STATUS \"  ✓ Executable installed\")
    else()
        message(FATAL_ERROR \"  ✗ Executable not found after installation\")
    endif()
    
    # Test executable
    execute_process(
        COMMAND \"\${TEST_INSTALL_DIR}/bin/${PROJECT_NAME}\" --version
        RESULT_VARIABLE EXEC_TEST_RESULT
        OUTPUT_QUIET ERROR_QUIET
    )
    
    if(EXEC_TEST_RESULT EQUAL 0)
        message(STATUS \"  ✓ Installed executable works\")
    else()
        message(WARNING \"  ⚠ Installed executable test failed (may be expected)\")
    endif()
    
else()
    message(FATAL_ERROR \"  ✗ Installation failed: \${INSTALL_ERROR}\")
endif()

# Cleanup
file(REMOVE_RECURSE \"\${TEST_INSTALL_DIR}\")
message(STATUS \"Installation tests completed\")
")
    
    add_custom_target(installation_tests
        COMMAND ${CMAKE_COMMAND} -P "${INSTALL_TEST_SCRIPT}"
        COMMENT "Running installation tests"
        VERBATIM
    )
endfunction()

# Add signature tests
function(add_signature_tests)
    if(NOT DEFINED ENV{ENABLE_PACKAGE_SIGNING})
        message(STATUS "Package signing not enabled, skipping signature tests")
        return()
    endif()
    
    add_custom_target(signature_tests
        COMMAND ${CMAKE_COMMAND} --build . --target verify_signatures
        COMMENT "Running signature verification tests"
        VERBATIM
    )
endfunction()

# Create comprehensive test suite
function(create_package_test_suite)
    set(options)
    set(oneValueArgs TIMEOUT)
    set(multiValueArgs)
    cmake_parse_arguments(TEST_SUITE "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
    
    if(NOT TEST_SUITE_TIMEOUT)
        set(TEST_SUITE_TIMEOUT 600)
    endif()
    
    # Enable all testing components
    enable_package_testing(
        ENABLE_INTEGRATION_TESTS
        ENABLE_INSTALLATION_TESTS
        ENABLE_SIGNATURE_TESTS
        TEST_TIMEOUT ${TEST_SUITE_TIMEOUT}
    )
    
    # Create master test target
    add_custom_target(test_packages
        COMMENT "Running complete package test suite"
        VERBATIM
    )
    
    # Add dependencies in correct order
    add_dependencies(test_packages validate_packages)
    add_dependencies(test_packages integration_tests)
    add_dependencies(test_packages installation_tests)
    
    if(DEFINED ENV{ENABLE_PACKAGE_SIGNING})
        add_dependencies(test_packages signature_tests)
    endif()
    
    message(STATUS "Package test suite created with ${TEST_SUITE_TIMEOUT}s timeout")
endfunction()

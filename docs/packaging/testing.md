# Package Testing Guide

This guide explains how to test packages created by the cpp-scaffold build system to ensure they work correctly across different platforms and installation methods.

## Overview

The package testing framework provides comprehensive validation of:
- Package creation and integrity
- Installation procedures
- Executable functionality
- Code signing verification
- Cross-platform compatibility

## Testing Framework Components

### 1. Package Validation
- File integrity checks
- Format-specific validation
- Size and structure verification
- Metadata validation

### 2. Installation Testing
- Installation to test directories
- File placement verification
- Executable functionality testing
- Dependency resolution

### 3. Signature Verification
- Code signing validation
- Certificate chain verification
- Timestamp validation
- Platform-specific security checks

## Running Tests

### Quick Testing
```bash
# Linux/macOS
./scripts/test_packages.sh --platform current --install-test

# Windows
.\scripts\test_packages.ps1 -Platform current -InstallTest
```

### Comprehensive Testing
```bash
# Linux/macOS
./scripts/test_packages.sh \
  --platform current \
  --install-test \
  --signature-test \
  --verbose

# Windows
.\scripts\test_packages.ps1 `
  -Platform current `
  -InstallTest `
  -SignatureTest `
  -Verbose
```

### CMake Integration
```bash
# Build and test packages
cmake --build build --target test_packages

# Individual test components
cmake --build build --target validate_packages
cmake --build build --target integration_tests
cmake --build build --target installation_tests
cmake --build build --target signature_tests
```

## Test Configuration

### Enable Testing in CMake
```cmake
# Enable package testing
set(ENABLE_PACKAGE_TESTING ON)

# Configure testing options
set(PACKAGE_TEST_TIMEOUT 600)  # 10 minutes
```

### Environment Variables
```bash
# Enable comprehensive testing
export ENABLE_PACKAGE_TESTING=1
export ENABLE_PACKAGE_SIGNING=1

# Test configuration
export PACKAGE_TEST_TIMEOUT=600
export PACKAGE_TEST_VERBOSE=1
```

## Platform-Specific Testing

### Windows Testing
Tests Windows-specific package formats and features:

```powershell
# Test Windows packages
.\scripts\test_packages.ps1 -Platform windows -Types "exe,msi"

# Test with signing
$env:ENABLE_PACKAGE_SIGNING = "1"
.\scripts\test_packages.ps1 -SignatureTest
```

**Validated Features:**
- Authenticode signatures
- MSI package integrity
- Registry entries
- PATH modifications
- Uninstaller functionality

### macOS Testing
Tests macOS-specific package formats and features:

```bash
# Test macOS packages
./scripts/test_packages.sh --platform macos --types "dmg,zip"

# Test with notarization
export ENABLE_PACKAGE_SIGNING=1
./scripts/test_packages.sh --signature-test
```

**Validated Features:**
- Code signing and notarization
- DMG integrity and layout
- Bundle structure
- Gatekeeper compatibility
- Keychain integration

### Linux Testing
Tests Linux-specific package formats and features:

```bash
# Test Linux packages
./scripts/test_packages.sh --platform linux --types "deb,rpm,appimage"

# Test package signing
export ENABLE_PACKAGE_SIGNING=1
./scripts/test_packages.sh --signature-test
```

**Validated Features:**
- Package dependencies
- GPG signatures
- Desktop integration
- System service installation
- Repository compatibility

## Test Types

### 1. Package Creation Tests
Verify that packages are created successfully:
- Build system integration
- Package generator functionality
- Output file generation
- Error handling

### 2. Package Integrity Tests
Validate package structure and content:
- File format validation
- Archive integrity
- Metadata verification
- Size and checksum validation

### 3. Installation Tests
Test installation procedures:
- Installation to test directories
- File placement verification
- Permission handling
- Dependency resolution
- Post-installation scripts

### 4. Functionality Tests
Verify installed software works:
- Executable launches
- Basic functionality
- Command-line interface
- Configuration file handling

### 5. Signature Tests
Validate code signing:
- Certificate validation
- Signature integrity
- Timestamp verification
- Trust chain validation

## Continuous Integration

### GitHub Actions Example
```yaml
name: Package Testing

on: [push, pull_request]

jobs:
  test-packages:
    strategy:
      matrix:
        os: [ubuntu-latest, windows-latest, macos-latest]
    
    runs-on: ${{ matrix.os }}
    
    steps:
    - uses: actions/checkout@v3
    
    - name: Configure CMake
      run: cmake --preset default
    
    - name: Build packages
      run: cmake --build --preset default --target package
    
    - name: Test packages
      run: |
        if [ "$RUNNER_OS" == "Windows" ]; then
          .\scripts\test_packages.ps1 -InstallTest
        else
          ./scripts/test_packages.sh --install-test
        fi
      shell: bash
```

### Test Matrix
Configure testing across multiple dimensions:
- **Platforms**: Windows, macOS, Linux
- **Package Types**: Native packages, archives
- **Build Types**: Debug, Release
- **Compilers**: GCC, Clang, MSVC
- **Architectures**: x86_64, ARM64

## Troubleshooting

### Common Issues

#### Package Creation Failures
```bash
# Check build logs
cmake --build build --verbose

# Verify dependencies
cmake --build build --target package --verbose
```

#### Installation Test Failures
```bash
# Check installation logs
cmake --install build --prefix test_install --verbose

# Verify file permissions
ls -la test_install/bin/
```

#### Signature Verification Failures
```bash
# Check signing configuration
./scripts/setup_signing.sh --verify

# Verify certificates
signtool verify /pa package.exe  # Windows
codesign --verify package.app    # macOS
gpg --verify package.deb.sig     # Linux
```

### Debug Mode
Enable verbose testing output:
```bash
# Environment variable
export PACKAGE_TEST_VERBOSE=1

# Command line flag
./scripts/test_packages.sh --verbose
```

### Test Isolation
Run tests in isolated environments:
```bash
# Use containers
docker run --rm -v $(pwd):/src ubuntu:20.04 \
  bash -c "cd /src && ./scripts/test_packages.sh"

# Use virtual machines
vagrant up && vagrant ssh -c "cd /vagrant && ./scripts/test_packages.sh"
```

## Custom Tests

### Adding New Tests
Extend the testing framework:

```cmake
# Add custom test
function(add_custom_package_test test_name)
    add_custom_target(${test_name}
        COMMAND ${CMAKE_COMMAND} -P "${CMAKE_SOURCE_DIR}/tests/${test_name}.cmake"
        COMMENT "Running ${test_name}"
    )
    add_dependencies(test_packages ${test_name})
endfunction()
```

### Test Scripts
Create custom test scripts:
```cmake
# tests/custom_test.cmake
message(STATUS "Running custom package test...")

# Your test logic here
execute_process(
    COMMAND your_test_command
    RESULT_VARIABLE TEST_RESULT
)

if(NOT TEST_RESULT EQUAL 0)
    message(FATAL_ERROR "Custom test failed")
endif()
```

## Best Practices

1. **Test Early and Often**: Run tests during development
2. **Test All Platforms**: Verify cross-platform compatibility
3. **Automate Testing**: Integrate with CI/CD pipelines
4. **Test Real Scenarios**: Use realistic installation environments
5. **Monitor Performance**: Track test execution times
6. **Document Issues**: Record and track test failures
7. **Version Testing**: Test with different dependency versions

## Support

For testing-related issues:
1. Check test logs and output
2. Verify platform requirements
3. Test with minimal configurations
4. Review platform-specific documentation
5. Contact support with detailed test results

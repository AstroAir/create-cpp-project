#!/bin/bash
# Comprehensive package testing script
# Tests package creation, validation, installation, and functionality

set -euo pipefail

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
CYAN='\033[0;36m'
NC='\033[0m' # No Color

# Test results
TESTS_PASSED=0
TESTS_FAILED=0
TESTS_SKIPPED=0

print_status() {
    echo -e "${GREEN}✓ $1${NC}"
}

print_info() {
    echo -e "${CYAN}ℹ $1${NC}"
}

print_warning() {
    echo -e "${YELLOW}⚠ $1${NC}"
}

print_error() {
    echo -e "${RED}✗ $1${NC}"
}

print_test_header() {
    echo -e "\n${CYAN}=== $1 ===${NC}"
}

# Test result tracking
pass_test() {
    ((TESTS_PASSED++))
    print_status "$1"
}

fail_test() {
    ((TESTS_FAILED++))
    print_error "$1"
}

skip_test() {
    ((TESTS_SKIPPED++))
    print_warning "$1 (skipped)"
}

# Get script directory and project root
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"
BUILD_DIR="${PROJECT_ROOT}/build"

# Configuration
PLATFORM=""
PACKAGE_TYPES=""
INSTALL_TEST=false
SIGNATURE_TEST=false
CLEANUP=true
VERBOSE=false

show_help() {
    cat << EOF
Package Testing Script

Usage: $0 [OPTIONS]

OPTIONS:
    -p, --platform PLATFORM    Target platform (windows|macos|linux|current)
    -t, --types TYPES          Package types to test (comma-separated)
    -i, --install-test         Run installation tests
    -s, --signature-test       Run signature verification tests
    --no-cleanup              Don't cleanup test files
    -v, --verbose             Enable verbose output
    -h, --help                Show this help message

EXAMPLES:
    $0 --platform current --install-test
    $0 --platform linux --types "deb,rpm" --signature-test
    $0 --verbose --install-test --signature-test

PACKAGE TYPES:
    Windows: exe, msi, zip
    macOS:   dmg, zip
    Linux:   deb, rpm, appimage, tar.gz

EOF
}

# Parse command line arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        -p|--platform)
            PLATFORM="$2"
            shift 2
            ;;
        -t|--types)
            PACKAGE_TYPES="$2"
            shift 2
            ;;
        -i|--install-test)
            INSTALL_TEST=true
            shift
            ;;
        -s|--signature-test)
            SIGNATURE_TEST=true
            shift
            ;;
        --no-cleanup)
            CLEANUP=false
            shift
            ;;
        -v|--verbose)
            VERBOSE=true
            shift
            ;;
        -h|--help)
            show_help
            exit 0
            ;;
        *)
            print_error "Unknown option: $1"
            show_help
            exit 1
            ;;
    esac
done

# Detect current platform if not specified
if [[ -z "$PLATFORM" || "$PLATFORM" == "current" ]]; then
    if [[ "$OSTYPE" == "msys" || "$OSTYPE" == "cygwin" ]]; then
        PLATFORM="windows"
    elif [[ "$OSTYPE" == "darwin"* ]]; then
        PLATFORM="macos"
    else
        PLATFORM="linux"
    fi
fi

print_info "Testing packages for platform: $PLATFORM"
print_info "Build directory: $BUILD_DIR"

# Change to project root
cd "$PROJECT_ROOT"

# Test 1: Build packages
print_test_header "Package Creation Test"

if [[ ! -d "$BUILD_DIR" ]]; then
    print_info "Build directory not found, configuring..."
    if cmake --preset default; then
        pass_test "CMake configuration successful"
    else
        fail_test "CMake configuration failed"
        exit 1
    fi
fi

print_info "Building packages..."
if cmake --build --preset default --target package; then
    pass_test "Package creation successful"
else
    fail_test "Package creation failed"
    exit 1
fi

# Test 2: Package validation
print_test_header "Package Validation Test"

# Find created packages
PACKAGES=($(find "$BUILD_DIR" -maxdepth 1 -name "*.exe" -o -name "*.msi" -o -name "*.dmg" -o -name "*.deb" -o -name "*.rpm" -o -name "*.AppImage" -o -name "*.zip" -o -name "*.tar.gz" 2>/dev/null))

if [[ ${#PACKAGES[@]} -eq 0 ]]; then
    fail_test "No packages found"
    exit 1
else
    pass_test "Found ${#PACKAGES[@]} package(s)"
fi

# Validate each package
for package in "${PACKAGES[@]}"; do
    package_name=$(basename "$package")
    print_info "Validating: $package_name"
    
    # Check file size
    size=$(stat -f%z "$package" 2>/dev/null || stat -c%s "$package" 2>/dev/null || echo "0")
    if [[ $size -gt 1048576 ]]; then
        pass_test "Package size OK: $size bytes"
    else
        fail_test "Package too small: $size bytes"
        continue
    fi
    
    # Platform-specific validation
    case "$package" in
        *.exe|*.msi)
            if command -v file >/dev/null 2>&1; then
                if file "$package" | grep -q "PE32\|MS Windows"; then
                    pass_test "Valid Windows package: $package_name"
                else
                    fail_test "Invalid Windows package: $package_name"
                fi
            else
                skip_test "Windows package validation (file command not available)"
            fi
            ;;
        *.dmg)
            if [[ "$OSTYPE" == "darwin"* ]] && command -v hdiutil >/dev/null 2>&1; then
                if hdiutil verify "$package" >/dev/null 2>&1; then
                    pass_test "Valid DMG package: $package_name"
                else
                    fail_test "Invalid DMG package: $package_name"
                fi
            else
                skip_test "DMG validation (not on macOS or hdiutil not available)"
            fi
            ;;
        *.deb)
            if command -v dpkg >/dev/null 2>&1; then
                if dpkg --info "$package" >/dev/null 2>&1; then
                    pass_test "Valid DEB package: $package_name"
                else
                    fail_test "Invalid DEB package: $package_name"
                fi
            else
                skip_test "DEB validation (dpkg not available)"
            fi
            ;;
        *.rpm)
            if command -v rpm >/dev/null 2>&1; then
                if rpm -qp "$package" >/dev/null 2>&1; then
                    pass_test "Valid RPM package: $package_name"
                else
                    fail_test "Invalid RPM package: $package_name"
                fi
            else
                skip_test "RPM validation (rpm not available)"
            fi
            ;;
        *.AppImage)
            if file "$package" | grep -q "executable"; then
                pass_test "Valid AppImage: $package_name"
            else
                fail_test "Invalid AppImage: $package_name"
            fi
            ;;
        *.zip)
            if command -v unzip >/dev/null 2>&1; then
                if unzip -t "$package" >/dev/null 2>&1; then
                    pass_test "Valid ZIP archive: $package_name"
                else
                    fail_test "Invalid ZIP archive: $package_name"
                fi
            else
                skip_test "ZIP validation (unzip not available)"
            fi
            ;;
        *.tar.gz)
            if tar -tzf "$package" >/dev/null 2>&1; then
                pass_test "Valid TAR.GZ archive: $package_name"
            else
                fail_test "Invalid TAR.GZ archive: $package_name"
            fi
            ;;
    esac
done

# Test 3: Installation test
if $INSTALL_TEST; then
    print_test_header "Installation Test"
    
    TEST_INSTALL_DIR="$BUILD_DIR/test_install"
    rm -rf "$TEST_INSTALL_DIR"
    mkdir -p "$TEST_INSTALL_DIR"
    
    print_info "Testing installation to: $TEST_INSTALL_DIR"
    
    if cmake --install "$BUILD_DIR" --prefix "$TEST_INSTALL_DIR"; then
        pass_test "Installation successful"
        
        # Check if executable was installed
        if [[ -f "$TEST_INSTALL_DIR/bin/cpp-scaffold" ]]; then
            pass_test "Executable installed correctly"
            
            # Test executable
            if "$TEST_INSTALL_DIR/bin/cpp-scaffold" --version >/dev/null 2>&1; then
                pass_test "Installed executable works"
            else
                fail_test "Installed executable test failed"
            fi
        else
            fail_test "Executable not found after installation"
        fi
    else
        fail_test "Installation failed"
    fi
    
    # Cleanup installation test
    if $CLEANUP; then
        rm -rf "$TEST_INSTALL_DIR"
    fi
fi

# Test 4: Signature verification
if $SIGNATURE_TEST; then
    print_test_header "Signature Verification Test"
    
    if [[ -n "${ENABLE_PACKAGE_SIGNING:-}" ]]; then
        if cmake --build "$BUILD_DIR" --target verify_signatures; then
            pass_test "Signature verification successful"
        else
            fail_test "Signature verification failed"
        fi
    else
        skip_test "Package signing not enabled"
    fi
fi

# Test summary
print_test_header "Test Summary"
echo -e "Tests passed:  ${GREEN}$TESTS_PASSED${NC}"
echo -e "Tests failed:  ${RED}$TESTS_FAILED${NC}"
echo -e "Tests skipped: ${YELLOW}$TESTS_SKIPPED${NC}"

if [[ $TESTS_FAILED -eq 0 ]]; then
    print_status "All tests passed!"
    exit 0
else
    print_error "$TESTS_FAILED test(s) failed"
    exit 1
fi

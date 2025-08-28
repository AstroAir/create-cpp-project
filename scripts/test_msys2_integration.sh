#!/bin/bash

# MSYS2 Integration Test Script
# This script tests MSYS2 integration across all project templates

set -e  # Exit on any error

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Test configuration
TEST_DIR="$(mktemp -d)"
PROJECT_CREATOR="./create-cpp-project"
TOTAL_TESTS=0
PASSED_TESTS=0
FAILED_TESTS=0

# Logging functions
log_info() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

log_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
    ((PASSED_TESTS++))
}

log_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

log_error() {
    echo -e "${RED}[ERROR]${NC} $1"
    ((FAILED_TESTS++))
}

# Test helper functions
run_test() {
    local test_name="$1"
    local test_command="$2"
    
    ((TOTAL_TESTS++))
    log_info "Running test: $test_name"
    
    if eval "$test_command"; then
        log_success "$test_name"
        return 0
    else
        log_error "$test_name"
        return 1
    fi
}

validate_pkgbuild() {
    local project_path="$1"
    local expected_type="$2"
    
    local pkgbuild_path="$project_path/PKGBUILD"
    
    if [[ ! -f "$pkgbuild_path" ]]; then
        log_error "PKGBUILD not found in $project_path"
        return 1
    fi
    
    # Check for required PKGBUILD elements
    local content=$(cat "$pkgbuild_path")
    
    # Required fields
    if ! echo "$content" | grep -q "_realname="; then
        log_error "Missing _realname in PKGBUILD"
        return 1
    fi
    
    if ! echo "$content" | grep -q "pkgname="; then
        log_error "Missing pkgname in PKGBUILD"
        return 1
    fi
    
    if ! echo "$content" | grep -q "pkgver="; then
        log_error "Missing pkgver in PKGBUILD"
        return 1
    fi
    
    if ! echo "$content" | grep -q "pkgdesc="; then
        log_error "Missing pkgdesc in PKGBUILD"
        return 1
    fi
    
    if ! echo "$content" | grep -q "arch="; then
        log_error "Missing arch in PKGBUILD"
        return 1
    fi
    
    if ! echo "$content" | grep -q "license="; then
        log_error "Missing license in PKGBUILD"
        return 1
    fi
    
    # Required functions
    if ! echo "$content" | grep -q "build() {"; then
        log_error "Missing build() function in PKGBUILD"
        return 1
    fi
    
    if ! echo "$content" | grep -q "package() {"; then
        log_error "Missing package() function in PKGBUILD"
        return 1
    fi
    
    # MSYS2-specific elements
    if ! echo "$content" | grep -q "mingw_arch="; then
        log_warning "Missing mingw_arch in PKGBUILD (recommended)"
    fi
    
    if ! echo "$content" | grep -q "MINGW_PREFIX"; then
        log_error "Missing MINGW_PREFIX usage in PKGBUILD"
        return 1
    fi
    
    if ! echo "$content" | grep -q "cmake.exe"; then
        log_error "Missing cmake.exe usage in PKGBUILD"
        return 1
    fi
    
    # Type-specific validation
    case "$expected_type" in
        "library")
            if ! echo "$content" | grep -q "BUILD_SHARED_LIBS"; then
                log_warning "Library PKGBUILD should include BUILD_SHARED_LIBS option"
            fi
            ;;
        "gui")
            if ! echo "$content" | grep -qE "(qt6-base|gtk3|wxwidgets)"; then
                log_error "GUI PKGBUILD should include GUI framework dependencies"
                return 1
            fi
            ;;
    esac
    
    log_success "PKGBUILD validation passed for $expected_type project"
    return 0
}

check_prerequisites() {
    log_info "Checking prerequisites..."
    
    # Check if project creator exists
    if [[ ! -f "$PROJECT_CREATOR" ]]; then
        log_error "Project creator not found: $PROJECT_CREATOR"
        return 1
    fi
    
    # Check if we can create test directory
    if [[ ! -d "$TEST_DIR" ]]; then
        log_error "Cannot create test directory: $TEST_DIR"
        return 1
    fi
    
    log_success "Prerequisites check passed"
    return 0
}

test_console_template() {
    local project_name="msys2_console_test"
    local project_path="$TEST_DIR/$project_name"
    
    cd "$TEST_DIR"
    
    # Create console project with MSYS2
    if ! "$PROJECT_CREATOR" "$project_name" --package msys2 --type console --test-framework gtest --no-wizard; then
        log_error "Failed to create console project with MSYS2"
        return 1
    fi
    
    # Validate generated files
    if ! validate_pkgbuild "$project_path" "console"; then
        return 1
    fi
    
    # Check for test framework dependencies
    if ! grep -q "gtest" "$project_path/PKGBUILD"; then
        log_error "Google Test dependency not found in console PKGBUILD"
        return 1
    fi
    
    log_success "Console template test passed"
    return 0
}

test_library_template() {
    local project_name="msys2_library_test"
    local project_path="$TEST_DIR/$project_name"
    
    cd "$TEST_DIR"
    
    # Create library project with MSYS2
    if ! "$PROJECT_CREATOR" "$project_name" --package msys2 --type library --test-framework catch2 --no-wizard; then
        log_error "Failed to create library project with MSYS2"
        return 1
    fi
    
    # Validate generated files
    if ! validate_pkgbuild "$project_path" "library"; then
        return 1
    fi
    
    # Check for test framework dependencies
    if ! grep -q "catch2" "$project_path/PKGBUILD"; then
        log_error "Catch2 dependency not found in library PKGBUILD"
        return 1
    fi
    
    log_success "Library template test passed"
    return 0
}

test_gui_template() {
    local project_name="msys2_gui_test"
    local project_path="$TEST_DIR/$project_name"
    
    cd "$TEST_DIR"
    
    # Create GUI project with MSYS2 and Qt
    if ! "$PROJECT_CREATOR" "$project_name" --package msys2 --type gui --gui-framework qt --no-wizard; then
        log_error "Failed to create GUI project with MSYS2"
        return 1
    fi
    
    # Validate generated files
    if ! validate_pkgbuild "$project_path" "gui"; then
        return 1
    fi
    
    # Check for Qt dependencies
    if ! grep -q "qt6-base" "$project_path/PKGBUILD"; then
        log_error "Qt6 dependencies not found in GUI PKGBUILD"
        return 1
    fi
    
    log_success "GUI template test passed"
    return 0
}

test_pkgbuild_syntax() {
    log_info "Testing PKGBUILD syntax validation..."
    
    # Create a test project to get a PKGBUILD
    local project_name="syntax_test"
    local project_path="$TEST_DIR/$project_name"
    
    cd "$TEST_DIR"
    
    if ! "$PROJECT_CREATOR" "$project_name" --package msys2 --type console --no-wizard; then
        log_error "Failed to create test project for syntax validation"
        return 1
    fi
    
    # Test with makepkg if available (MSYS2 environment)
    if command -v makepkg >/dev/null 2>&1; then
        cd "$project_path"
        if makepkg --printsrcinfo >/dev/null 2>&1; then
            log_success "PKGBUILD syntax validation with makepkg passed"
        else
            log_error "PKGBUILD syntax validation with makepkg failed"
            return 1
        fi
    else
        log_warning "makepkg not available, skipping syntax validation"
    fi
    
    return 0
}

test_environment_detection() {
    log_info "Testing MSYS2 environment detection..."
    
    # Check environment variables
    if [[ -n "$MSYSTEM" ]]; then
        log_success "MSYSTEM detected: $MSYSTEM"
    else
        log_warning "MSYSTEM not set (not in MSYS2 environment)"
    fi
    
    if [[ -n "$MINGW_PREFIX" ]]; then
        log_success "MINGW_PREFIX detected: $MINGW_PREFIX"
    else
        log_warning "MINGW_PREFIX not set (not in MinGW environment)"
    fi
    
    # Check for pacman
    if command -v pacman >/dev/null 2>&1; then
        log_success "pacman package manager found"
    else
        log_warning "pacman not found (not in MSYS2 environment)"
    fi
    
    return 0
}

cleanup() {
    log_info "Cleaning up test directory: $TEST_DIR"
    rm -rf "$TEST_DIR"
}

main() {
    echo "========================================="
    echo "MSYS2 Integration Test Suite"
    echo "========================================="
    
    # Set up cleanup trap
    trap cleanup EXIT
    
    # Run prerequisite checks
    if ! check_prerequisites; then
        exit 1
    fi
    
    # Run environment detection test
    run_test "Environment Detection" "test_environment_detection"
    
    # Run template tests
    run_test "Console Template" "test_console_template"
    run_test "Library Template" "test_library_template"
    run_test "GUI Template" "test_gui_template"
    
    # Run syntax validation test
    run_test "PKGBUILD Syntax" "test_pkgbuild_syntax"
    
    # Print summary
    echo "========================================="
    echo "Test Summary"
    echo "========================================="
    echo "Total tests: $TOTAL_TESTS"
    echo -e "Passed: ${GREEN}$PASSED_TESTS${NC}"
    echo -e "Failed: ${RED}$FAILED_TESTS${NC}"
    
    if [[ $FAILED_TESTS -eq 0 ]]; then
        echo -e "${GREEN}All tests passed!${NC}"
        exit 0
    else
        echo -e "${RED}Some tests failed!${NC}"
        exit 1
    fi
}

# Run main function
main "$@"

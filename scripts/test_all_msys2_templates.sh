#!/bin/bash

# Comprehensive MSYS2 Template Testing Script
# Tests MSYS2 integration across all supported project templates

set -e  # Exit on any error

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Test configuration
TEST_DIR="$(mktemp -d)"
PROJECT_CREATOR="./build/bin/cpp-scaffold.exe"
TOTAL_TESTS=0
PASSED_TESTS=0
FAILED_TESTS=0

# Template configurations to test
declare -A TEMPLATES=(
    ["console"]="--template console --tests gtest"
    ["library"]="--template lib --tests catch2"
    ["gui-qt"]="--template gui --gui-frameworks qt"
    ["gui-gtk"]="--template gui --gui-frameworks gtk"
    ["gui-wxwidgets"]="--template gui --gui-frameworks wxwidgets"
    ["multi-executable"]="--template multi-executable --tests gtest"
    ["header-only"]="--template header-only-lib --tests doctest"
    ["modules"]="--template modules --std cpp20 --tests gtest"
)

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

validate_msys2_project() {
    local project_path="$1"
    local template_type="$2"
    
    # Check if PKGBUILD exists
    if [[ ! -f "$project_path/PKGBUILD" ]]; then
        log_error "PKGBUILD not found in $project_path"
        return 1
    fi
    
    # Validate PKGBUILD content
    local pkgbuild_content=$(cat "$project_path/PKGBUILD")
    
    # Check required PKGBUILD fields
    if ! echo "$pkgbuild_content" | grep -q "_realname="; then
        log_error "Missing _realname in PKGBUILD"
        return 1
    fi
    
    if ! echo "$pkgbuild_content" | grep -q "pkgname="; then
        log_error "Missing pkgname in PKGBUILD"
        return 1
    fi
    
    if ! echo "$pkgbuild_content" | grep -q "build() {"; then
        log_error "Missing build() function in PKGBUILD"
        return 1
    fi
    
    if ! echo "$pkgbuild_content" | grep -q "package() {"; then
        log_error "Missing package() function in PKGBUILD"
        return 1
    fi
    
    # Check MSYS2-specific elements
    if ! echo "$pkgbuild_content" | grep -q "MINGW_PREFIX"; then
        log_error "Missing MINGW_PREFIX usage in PKGBUILD"
        return 1
    fi
    
    # Template-specific validation
    case "$template_type" in
        "library"|"header-only")
            if ! echo "$pkgbuild_content" | grep -q "BUILD_SHARED_LIBS"; then
                log_warning "Library PKGBUILD should include BUILD_SHARED_LIBS option"
            fi
            ;;
        "gui-qt")
            if ! echo "$pkgbuild_content" | grep -q "qt6-base"; then
                log_error "Qt GUI PKGBUILD should include qt6-base dependency"
                return 1
            fi
            ;;
        "gui-gtk")
            if ! echo "$pkgbuild_content" | grep -q "gtk3"; then
                log_error "GTK GUI PKGBUILD should include gtk3 dependency"
                return 1
            fi
            ;;
        "gui-wxwidgets")
            if ! echo "$pkgbuild_content" | grep -q "wxwidgets"; then
                log_error "wxWidgets GUI PKGBUILD should include wxwidgets dependency"
                return 1
            fi
            ;;
    esac
    
    # Check CMakeLists.txt exists
    if [[ ! -f "$project_path/CMakeLists.txt" ]]; then
        log_error "CMakeLists.txt not found in $project_path"
        return 1
    fi
    
    # Check basic project structure
    if [[ ! -d "$project_path/src" ]]; then
        log_error "src directory not found in $project_path"
        return 1
    fi
    
    log_success "MSYS2 project validation passed for $template_type"
    return 0
}

test_template() {
    local template_name="$1"
    local template_args="$2"
    local project_name="msys2_test_${template_name}"
    local project_path="$TEST_DIR/$project_name"
    
    cd "$TEST_DIR"
    
    log_info "Testing template: $template_name"
    
    # Create project with MSYS2 package manager
    if ! "$PROJECT_CREATOR" create "$project_name" --package msys2 $template_args; then
        log_error "Failed to create $template_name project with MSYS2"
        return 1
    fi
    
    # Validate generated project
    if ! validate_msys2_project "$project_path" "$template_name"; then
        return 1
    fi
    
    # Test PKGBUILD syntax if makepkg is available
    if command -v makepkg >/dev/null 2>&1; then
        cd "$project_path"
        if makepkg --printsrcinfo >/dev/null 2>&1; then
            log_success "PKGBUILD syntax validation passed for $template_name"
        else
            log_error "PKGBUILD syntax validation failed for $template_name"
            return 1
        fi
        cd "$TEST_DIR"
    else
        log_warning "makepkg not available, skipping syntax validation for $template_name"
    fi
    
    log_success "Template test passed: $template_name"
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
    
    # Check MSYS2 environment (optional)
    if [[ -n "$MSYSTEM" ]]; then
        log_success "MSYS2 environment detected: $MSYSTEM"
    else
        log_warning "Not in MSYS2 environment (tests will still run)"
    fi
    
    log_success "Prerequisites check passed"
    return 0
}

cleanup() {
    log_info "Cleaning up test directory: $TEST_DIR"
    rm -rf "$TEST_DIR"
}

main() {
    echo "========================================="
    echo "MSYS2 Template Testing Suite"
    echo "========================================="
    
    # Set up cleanup trap
    trap cleanup EXIT
    
    # Run prerequisite checks
    if ! check_prerequisites; then
        exit 1
    fi
    
    # Test each template
    for template_name in "${!TEMPLATES[@]}"; do
        template_args="${TEMPLATES[$template_name]}"
        run_test "Template: $template_name" "test_template '$template_name' '$template_args'"
    done
    
    # Print summary
    echo "========================================="
    echo "Test Summary"
    echo "========================================="
    echo "Total tests: $TOTAL_TESTS"
    echo -e "Passed: ${GREEN}$PASSED_TESTS${NC}"
    echo -e "Failed: ${RED}$FAILED_TESTS${NC}"
    
    if [[ $FAILED_TESTS -eq 0 ]]; then
        echo -e "${GREEN}All template tests passed!${NC}"
        exit 0
    else
        echo -e "${RED}Some template tests failed!${NC}"
        exit 1
    fi
}

# Run main function
main "$@"

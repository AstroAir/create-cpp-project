#!/bin/bash

# PKGBUILD Validation and Build Process Testing Script
# Validates generated PKGBUILD files and tests build processes in MSYS2

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

# Configuration
VALIDATION_RESULTS=()
BUILD_RESULTS=()
TOTAL_VALIDATIONS=0
PASSED_VALIDATIONS=0
FAILED_VALIDATIONS=0

# Logging functions
log_info() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

log_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
    ((PASSED_VALIDATIONS++))
}

log_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

log_error() {
    echo -e "${RED}[ERROR]${NC} $1"
    ((FAILED_VALIDATIONS++))
}

# PKGBUILD validation functions
validate_pkgbuild_syntax() {
    local pkgbuild_path="$1"
    local project_name="$2"
    
    ((TOTAL_VALIDATIONS++))
    log_info "Validating PKGBUILD syntax for $project_name"
    
    if [[ ! -f "$pkgbuild_path" ]]; then
        log_error "PKGBUILD file not found: $pkgbuild_path"
        return 1
    fi
    
    # Check if makepkg can parse the PKGBUILD
    cd "$(dirname "$pkgbuild_path")"
    
    if makepkg --printsrcinfo >/dev/null 2>&1; then
        log_success "PKGBUILD syntax validation passed for $project_name"
        return 0
    else
        log_error "PKGBUILD syntax validation failed for $project_name"
        return 1
    fi
}

validate_pkgbuild_content() {
    local pkgbuild_path="$1"
    local project_name="$2"
    
    ((TOTAL_VALIDATIONS++))
    log_info "Validating PKGBUILD content for $project_name"
    
    local content=$(cat "$pkgbuild_path")
    local errors=()
    local warnings=()
    
    # Check required fields
    if ! echo "$content" | grep -q "^_realname="; then
        errors+=("Missing _realname field")
    fi
    
    if ! echo "$content" | grep -q "^pkgname="; then
        errors+=("Missing pkgname field")
    fi
    
    if ! echo "$content" | grep -q "^pkgver="; then
        errors+=("Missing pkgver field")
    fi
    
    if ! echo "$content" | grep -q "^pkgrel="; then
        errors+=("Missing pkgrel field")
    fi
    
    if ! echo "$content" | grep -q "^pkgdesc="; then
        errors+=("Missing pkgdesc field")
    fi
    
    if ! echo "$content" | grep -q "^arch="; then
        errors+=("Missing arch field")
    fi
    
    if ! echo "$content" | grep -q "^license="; then
        errors+=("Missing license field")
    fi
    
    # Check required functions
    if ! echo "$content" | grep -q "^build() {"; then
        errors+=("Missing build() function")
    fi
    
    if ! echo "$content" | grep -q "^package() {"; then
        errors+=("Missing package() function")
    fi
    
    # Check MSYS2-specific elements
    if ! echo "$content" | grep -q "mingw_arch="; then
        warnings+=("Missing mingw_arch field (recommended)")
    fi
    
    if ! echo "$content" | grep -q "MINGW_PREFIX"; then
        errors+=("Missing MINGW_PREFIX usage")
    fi
    
    if ! echo "$content" | grep -q "cmake.exe"; then
        warnings+=("Should use cmake.exe for MSYS2 compatibility")
    fi
    
    # Report results
    if [[ ${#errors[@]} -eq 0 ]]; then
        log_success "PKGBUILD content validation passed for $project_name"
        if [[ ${#warnings[@]} -gt 0 ]]; then
            for warning in "${warnings[@]}"; do
                log_warning "$project_name: $warning"
            done
        fi
        return 0
    else
        log_error "PKGBUILD content validation failed for $project_name"
        for error in "${errors[@]}"; do
            log_error "$project_name: $error"
        done
        return 1
    fi
}

test_build_process() {
    local project_path="$1"
    local project_name="$2"
    
    ((TOTAL_VALIDATIONS++))
    log_info "Testing build process for $project_name"
    
    cd "$project_path"
    
    # Check if we can prepare sources
    if makepkg --nobuild --noextract --nodeps >/dev/null 2>&1; then
        log_success "Source preparation passed for $project_name"
    else
        log_error "Source preparation failed for $project_name"
        return 1
    fi
    
    # Test dependency resolution (if in MSYS2 environment)
    if [[ -n "$MSYSTEM" ]]; then
        if makepkg --syncdeps --noconfirm --nobuild >/dev/null 2>&1; then
            log_success "Dependency resolution passed for $project_name"
        else
            log_warning "Dependency resolution had issues for $project_name"
        fi
    else
        log_warning "Not in MSYS2 environment, skipping dependency test for $project_name"
    fi
    
    return 0
}

validate_cmake_integration() {
    local project_path="$1"
    local project_name="$2"
    
    ((TOTAL_VALIDATIONS++))
    log_info "Validating CMake integration for $project_name"
    
    cd "$project_path"
    
    # Check if CMakeLists.txt exists
    if [[ ! -f "CMakeLists.txt" ]]; then
        log_error "CMakeLists.txt not found for $project_name"
        return 1
    fi
    
    # Test CMake configuration
    mkdir -p build-test
    cd build-test
    
    if cmake .. -G Ninja >/dev/null 2>&1; then
        log_success "CMake configuration passed for $project_name"
        
        # Test build (if ninja is available)
        if command -v ninja >/dev/null 2>&1; then
            if ninja >/dev/null 2>&1; then
                log_success "Build test passed for $project_name"
            else
                log_warning "Build test had issues for $project_name"
            fi
        else
            log_warning "Ninja not available, skipping build test for $project_name"
        fi
        
        cd ..
        rm -rf build-test
        return 0
    else
        log_error "CMake configuration failed for $project_name"
        cd ..
        rm -rf build-test
        return 1
    fi
}

validate_project() {
    local project_path="$1"
    local project_name=$(basename "$project_path")
    
    log_info "Starting validation for project: $project_name"
    
    # Validate PKGBUILD syntax
    validate_pkgbuild_syntax "$project_path/PKGBUILD" "$project_name"
    
    # Validate PKGBUILD content
    validate_pkgbuild_content "$project_path/PKGBUILD" "$project_name"
    
    # Test build process
    test_build_process "$project_path" "$project_name"
    
    # Validate CMake integration
    validate_cmake_integration "$project_path" "$project_name"
    
    log_info "Completed validation for project: $project_name"
}

check_environment() {
    log_info "Checking validation environment..."
    
    # Check for makepkg
    if ! command -v makepkg >/dev/null 2>&1; then
        log_error "makepkg not found - please run in MSYS2 environment"
        return 1
    fi
    
    # Check for cmake
    if ! command -v cmake >/dev/null 2>&1; then
        log_warning "cmake not found - CMake validation will be skipped"
    fi
    
    # Check MSYS2 environment
    if [[ -n "$MSYSTEM" ]]; then
        log_success "MSYS2 environment detected: $MSYSTEM"
    else
        log_warning "Not in MSYS2 environment - some tests will be limited"
    fi
    
    return 0
}

usage() {
    echo "Usage: $0 [OPTIONS] PROJECT_PATH..."
    echo ""
    echo "Validate PKGBUILD files and build processes for MSYS2 projects"
    echo ""
    echo "OPTIONS:"
    echo "  -h, --help     Show this help message"
    echo "  -v, --verbose  Enable verbose output"
    echo ""
    echo "EXAMPLES:"
    echo "  $0 ./my_project"
    echo "  $0 ./project1 ./project2 ./project3"
    echo "  $0 --verbose ./my_project"
}

main() {
    local verbose=false
    local projects=()
    
    # Parse arguments
    while [[ $# -gt 0 ]]; do
        case $1 in
            -h|--help)
                usage
                exit 0
                ;;
            -v|--verbose)
                verbose=true
                shift
                ;;
            -*)
                log_error "Unknown option: $1"
                usage
                exit 1
                ;;
            *)
                projects+=("$1")
                shift
                ;;
        esac
    done
    
    # Check if projects were provided
    if [[ ${#projects[@]} -eq 0 ]]; then
        log_error "No project paths provided"
        usage
        exit 1
    fi
    
    echo "========================================="
    echo "PKGBUILD Validation Suite"
    echo "========================================="
    
    # Check environment
    if ! check_environment; then
        exit 1
    fi
    
    # Validate each project
    for project_path in "${projects[@]}"; do
        if [[ ! -d "$project_path" ]]; then
            log_error "Project directory not found: $project_path"
            ((TOTAL_VALIDATIONS++))
            ((FAILED_VALIDATIONS++))
            continue
        fi
        
        validate_project "$project_path"
    done
    
    # Print summary
    echo "========================================="
    echo "Validation Summary"
    echo "========================================="
    echo "Total validations: $TOTAL_VALIDATIONS"
    echo -e "Passed: ${GREEN}$PASSED_VALIDATIONS${NC}"
    echo -e "Failed: ${RED}$FAILED_VALIDATIONS${NC}"
    
    if [[ $FAILED_VALIDATIONS -eq 0 ]]; then
        echo -e "${GREEN}All validations passed!${NC}"
        exit 0
    else
        echo -e "${RED}Some validations failed!${NC}"
        exit 1
    fi
}

# Run main function
main "$@"

#!/bin/bash

# Package Manager Integration Test Script
# Verifies that MSYS2 integration doesn't break existing package managers
# and maintains backward compatibility

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

# Test configuration
TEST_DIR="$(mktemp -d)"
PROJECT_CREATOR="./build/bin/cpp-scaffold.exe"
TOTAL_TESTS=0
PASSED_TESTS=0
FAILED_TESTS=0

# Package managers to test
declare -A PACKAGE_MANAGERS=(
    ["vcpkg"]="vcpkg.json"
    ["conan"]="conanfile.txt"
    ["none"]=""
    ["msys2"]="PKGBUILD"
)

# Template types to test with each package manager
TEMPLATES=("console" "lib" "gui")

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

validate_package_manager_files() {
    local project_path="$1"
    local package_manager="$2"
    local expected_file="${PACKAGE_MANAGERS[$package_manager]}"
    
    if [[ -z "$expected_file" ]]; then
        # For "none" package manager, no specific file is expected
        log_success "No package manager file expected for 'none'"
        return 0
    fi
    
    local file_path="$project_path/$expected_file"
    if [[ ! -f "$file_path" ]]; then
        log_error "Expected package manager file not found: $expected_file"
        return 1
    fi
    
    # Validate file content based on package manager
    case "$package_manager" in
        "vcpkg")
            if ! grep -q '"name"' "$file_path"; then
                log_error "vcpkg.json missing required 'name' field"
                return 1
            fi
            ;;
        "conan")
            if ! grep -q '\[requires\]' "$file_path"; then
                log_error "conanfile.txt missing [requires] section"
                return 1
            fi
            ;;
        "msys2")
            if ! grep -q '_realname=' "$file_path"; then
                log_error "PKGBUILD missing _realname field"
                return 1
            fi
            if ! grep -q 'build() {' "$file_path"; then
                log_error "PKGBUILD missing build() function"
                return 1
            fi
            ;;
    esac
    
    log_success "Package manager file validation passed: $expected_file"
    return 0
}

test_package_manager_compatibility() {
    local package_manager="$1"
    local template="$2"
    local project_name="test_${package_manager}_${template}"
    local project_path="$TEST_DIR/$project_name"
    
    cd "$TEST_DIR"
    
    log_info "Testing $package_manager with $template template"
    
    # Create project with specific package manager
    local create_cmd="$PROJECT_CREATOR create $project_name --template $template --package $package_manager"
    
    if ! $create_cmd >/dev/null 2>&1; then
        log_error "Failed to create project: $project_name"
        return 1
    fi
    
    # Validate project structure
    if [[ ! -d "$project_path" ]]; then
        log_error "Project directory not created: $project_path"
        return 1
    fi
    
    # Check basic files exist
    if [[ ! -f "$project_path/CMakeLists.txt" ]]; then
        log_error "CMakeLists.txt not found in $project_path"
        return 1
    fi
    
    if [[ ! -d "$project_path/src" ]]; then
        log_error "src directory not found in $project_path"
        return 1
    fi
    
    # Validate package manager specific files
    if ! validate_package_manager_files "$project_path" "$package_manager"; then
        return 1
    fi
    
    # Check that no conflicting package manager files exist
    for pm in "${!PACKAGE_MANAGERS[@]}"; do
        if [[ "$pm" != "$package_manager" ]]; then
            local conflicting_file="${PACKAGE_MANAGERS[$pm]}"
            if [[ -n "$conflicting_file" && -f "$project_path/$conflicting_file" ]]; then
                log_error "Conflicting package manager file found: $conflicting_file"
                return 1
            fi
        fi
    done
    
    log_success "Package manager compatibility test passed: $package_manager with $template"
    return 0
}

test_backward_compatibility() {
    log_info "Testing backward compatibility..."
    
    # Test that old commands still work
    local old_commands=(
        "create test_old_vcpkg --template console --package vcpkg"
        "create test_old_conan --template lib --package conan"
        "create test_old_none --template console --package none"
    )
    
    cd "$TEST_DIR"
    
    for cmd in "${old_commands[@]}"; do
        local project_name=$(echo "$cmd" | awk '{print $2}')
        
        if $PROJECT_CREATOR $cmd >/dev/null 2>&1; then
            log_success "Backward compatibility test passed: $cmd"
        else
            log_error "Backward compatibility test failed: $cmd"
            return 1
        fi
        
        # Clean up
        rm -rf "$project_name"
    done
    
    return 0
}

test_msys2_isolation() {
    log_info "Testing MSYS2 isolation from other package managers..."
    
    cd "$TEST_DIR"
    
    # Create projects with different package managers
    local projects=(
        "test_vcpkg_isolation:vcpkg:console"
        "test_conan_isolation:conan:lib"
        "test_msys2_isolation:msys2:console"
    )
    
    for project_info in "${projects[@]}"; do
        IFS=':' read -r project_name package_manager template <<< "$project_info"
        
        if ! $PROJECT_CREATOR create "$project_name" --template "$template" --package "$package_manager" >/dev/null 2>&1; then
            log_error "Failed to create isolation test project: $project_name"
            return 1
        fi
    done
    
    # Verify each project has only its expected package manager files
    if [[ -f "test_vcpkg_isolation/vcpkg.json" && ! -f "test_vcpkg_isolation/PKGBUILD" && ! -f "test_vcpkg_isolation/conanfile.txt" ]]; then
        log_success "vcpkg isolation test passed"
    else
        log_error "vcpkg isolation test failed"
        return 1
    fi
    
    if [[ -f "test_conan_isolation/conanfile.txt" && ! -f "test_conan_isolation/PKGBUILD" && ! -f "test_conan_isolation/vcpkg.json" ]]; then
        log_success "conan isolation test passed"
    else
        log_error "conan isolation test failed"
        return 1
    fi
    
    if [[ -f "test_msys2_isolation/PKGBUILD" && ! -f "test_msys2_isolation/vcpkg.json" && ! -f "test_msys2_isolation/conanfile.txt" ]]; then
        log_success "msys2 isolation test passed"
    else
        log_error "msys2 isolation test failed"
        return 1
    fi
    
    return 0
}

test_cli_argument_parsing() {
    log_info "Testing CLI argument parsing for all package managers..."
    
    cd "$TEST_DIR"
    
    # Test various argument formats
    local test_cases=(
        "--package vcpkg"
        "-p vcpkg"
        "--package conan"
        "-p conan"
        "--package msys2"
        "-p msys2"
        "--package none"
        "-p none"
    )
    
    for args in "${test_cases[@]}"; do
        local project_name="test_cli_$(echo "$args" | tr ' -' '_')"
        
        if $PROJECT_CREATOR create "$project_name" --template console $args >/dev/null 2>&1; then
            log_success "CLI argument test passed: $args"
            rm -rf "$project_name"
        else
            log_error "CLI argument test failed: $args"
            return 1
        fi
    done
    
    return 0
}

check_prerequisites() {
    log_info "Checking prerequisites..."
    
    if [[ ! -f "$PROJECT_CREATOR" ]]; then
        log_error "Project creator not found: $PROJECT_CREATOR"
        return 1
    fi
    
    if [[ ! -d "$TEST_DIR" ]]; then
        log_error "Cannot create test directory: $TEST_DIR"
        return 1
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
    echo "Package Manager Integration Test Suite"
    echo "========================================="
    
    trap cleanup EXIT
    
    if ! check_prerequisites; then
        exit 1
    fi
    
    # Test backward compatibility
    run_test "Backward Compatibility" "test_backward_compatibility"
    
    # Test CLI argument parsing
    run_test "CLI Argument Parsing" "test_cli_argument_parsing"
    
    # Test package manager isolation
    run_test "Package Manager Isolation" "test_msys2_isolation"
    
    # Test each package manager with each template
    for package_manager in "${!PACKAGE_MANAGERS[@]}"; do
        for template in "${TEMPLATES[@]}"; do
            run_test "Compatibility: $package_manager + $template" \
                "test_package_manager_compatibility '$package_manager' '$template'"
        done
    done
    
    # Print summary
    echo "========================================="
    echo "Integration Test Summary"
    echo "========================================="
    echo "Total tests: $TOTAL_TESTS"
    echo -e "Passed: ${GREEN}$PASSED_TESTS${NC}"
    echo -e "Failed: ${RED}$FAILED_TESTS${NC}"
    
    if [[ $FAILED_TESTS -eq 0 ]]; then
        echo -e "${GREEN}All integration tests passed!${NC}"
        echo -e "${GREEN}MSYS2 integration maintains backward compatibility${NC}"
        exit 0
    else
        echo -e "${RED}Some integration tests failed!${NC}"
        exit 1
    fi
}

main "$@"

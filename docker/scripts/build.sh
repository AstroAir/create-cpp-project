#!/bin/bash
# Docker container build script for C++ projects
# This script provides a standardized way to build C++ projects in containers

set -euo pipefail

# Default values
BUILD_TYPE="Release"
PRESET=""
CLEAN=false
VERBOSE=false
JOBS=$(nproc)
SOURCE_DIR="/workspace"
BUILD_DIR=""

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Print colored output
print_info() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

print_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Show usage information
show_usage() {
    cat << EOF
Usage: build [OPTIONS]

Build C++ project in Docker container

OPTIONS:
    -t, --type TYPE         Build type (Debug, Release, RelWithDebInfo, MinSizeRel)
    -p, --preset PRESET     CMake preset to use
    -c, --clean             Clean build directory before building
    -v, --verbose           Enable verbose output
    -j, --jobs JOBS         Number of parallel jobs (default: $(nproc))
    -s, --source DIR        Source directory (default: /workspace)
    -b, --build DIR         Build directory (default: auto-generated)
    -h, --help              Show this help message

EXAMPLES:
    build                           # Build with default settings
    build -t Debug -v               # Debug build with verbose output
    build -p release --clean        # Use release preset and clean build
    build -j 4 --type Release       # Release build with 4 parallel jobs

EOF
}

# Parse command line arguments
parse_args() {
    while [[ $# -gt 0 ]]; do
        case $1 in
            -t|--type)
                BUILD_TYPE="$2"
                shift 2
                ;;
            -p|--preset)
                PRESET="$2"
                shift 2
                ;;
            -c|--clean)
                CLEAN=true
                shift
                ;;
            -v|--verbose)
                VERBOSE=true
                shift
                ;;
            -j|--jobs)
                JOBS="$2"
                shift 2
                ;;
            -s|--source)
                SOURCE_DIR="$2"
                shift 2
                ;;
            -b|--build)
                BUILD_DIR="$2"
                shift 2
                ;;
            -h|--help)
                show_usage
                exit 0
                ;;
            *)
                print_error "Unknown option: $1"
                show_usage
                exit 1
                ;;
        esac
    done
}

# Validate build environment
validate_environment() {
    print_info "Validating build environment..."
    
    # Check if source directory exists
    if [[ ! -d "$SOURCE_DIR" ]]; then
        print_error "Source directory not found: $SOURCE_DIR"
        exit 1
    fi
    
    # Check if CMakeLists.txt exists
    if [[ ! -f "$SOURCE_DIR/CMakeLists.txt" ]]; then
        print_error "CMakeLists.txt not found in: $SOURCE_DIR"
        exit 1
    fi
    
    # Check required tools
    local tools=("cmake" "ninja")
    for tool in "${tools[@]}"; do
        if ! command -v "$tool" &> /dev/null; then
            print_error "$tool is not installed"
            exit 1
        fi
    done
    
    print_success "Build environment validated"
}

# Set up build directory
setup_build_dir() {
    if [[ -z "$BUILD_DIR" ]]; then
        if [[ -n "$PRESET" ]]; then
            BUILD_DIR="$SOURCE_DIR/build-$PRESET"
        else
            BUILD_DIR="$SOURCE_DIR/build-$(echo "$BUILD_TYPE" | tr '[:upper:]' '[:lower:]')"
        fi
    fi
    
    print_info "Build directory: $BUILD_DIR"
    
    if [[ "$CLEAN" == true ]] && [[ -d "$BUILD_DIR" ]]; then
        print_info "Cleaning build directory..."
        rm -rf "$BUILD_DIR"
    fi
    
    mkdir -p "$BUILD_DIR"
}

# Configure project
configure_project() {
    print_info "Configuring project..."
    
    local cmake_args=()
    cmake_args+=("-S" "$SOURCE_DIR")
    cmake_args+=("-B" "$BUILD_DIR")
    cmake_args+=("-G" "Ninja")
    
    if [[ -n "$PRESET" ]]; then
        cmake_args+=("--preset" "$PRESET")
    else
        cmake_args+=("-DCMAKE_BUILD_TYPE=$BUILD_TYPE")
    fi
    
    # Add common configuration
    cmake_args+=("-DCMAKE_EXPORT_COMPILE_COMMANDS=ON")
    
    if [[ "$VERBOSE" == true ]]; then
        cmake_args+=("-DCMAKE_VERBOSE_MAKEFILE=ON")
    fi
    
    # Enable ccache if available
    if command -v ccache &> /dev/null; then
        cmake_args+=("-DCMAKE_C_COMPILER_LAUNCHER=ccache")
        cmake_args+=("-DCMAKE_CXX_COMPILER_LAUNCHER=ccache")
    fi
    
    print_info "Running: cmake ${cmake_args[*]}"
    cmake "${cmake_args[@]}"
    
    print_success "Project configured successfully"
}

# Build project
build_project() {
    print_info "Building project..."
    
    local build_args=()
    build_args+=("--build" "$BUILD_DIR")
    build_args+=("--parallel" "$JOBS")
    
    if [[ "$VERBOSE" == true ]]; then
        build_args+=("--verbose")
    fi
    
    print_info "Running: cmake ${build_args[*]}"
    cmake "${build_args[@]}"
    
    print_success "Project built successfully"
}

# Show build summary
show_summary() {
    print_info "Build Summary:"
    echo "  Source Directory: $SOURCE_DIR"
    echo "  Build Directory: $BUILD_DIR"
    echo "  Build Type: $BUILD_TYPE"
    if [[ -n "$PRESET" ]]; then
        echo "  Preset: $PRESET"
    fi
    echo "  Jobs: $JOBS"
    echo "  Clean Build: $CLEAN"
    echo "  Verbose: $VERBOSE"
    
    # Show ccache stats if available
    if command -v ccache &> /dev/null; then
        echo ""
        print_info "ccache statistics:"
        ccache -s
    fi
}

# Main function
main() {
    print_info "Starting C++ project build in container..."
    
    parse_args "$@"
    validate_environment
    setup_build_dir
    configure_project
    build_project
    show_summary
    
    print_success "Build completed successfully!"
}

# Run main function with all arguments
main "$@"

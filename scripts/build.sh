#!/bin/bash
# Modern build script for cpp-scaffold project (Linux/macOS)

set -euo pipefail

# Default values
CONFIG="default"
CLEAN=false
TESTS=false
INSTALL=false
VERBOSE=false

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
CYAN='\033[0;36m'
GRAY='\033[0;37m'
NC='\033[0m' # No Color

# Function to print colored output
print_status() {
    echo -e "${GREEN}🚀 $1${NC}"
}

print_info() {
    echo -e "${CYAN}$1${NC}"
}

print_warning() {
    echo -e "${YELLOW}⚠️  $1${NC}"
}

print_error() {
    echo -e "${RED}❌ $1${NC}"
}

print_step() {
    echo -e "${YELLOW}$1${NC}"
}

# Help function
show_help() {
    cat << EOF
Modern build script for cpp-scaffold project

Usage: $0 [OPTIONS]

OPTIONS:
    -c, --config CONFIG     Build configuration (debug|release|fast-build|minimal|default)
    -C, --clean            Clean build directories before building
    -t, --tests            Run tests after building
    -i, --install          Install the project after building
    -v, --verbose          Enable verbose output
    -h, --help             Show this help message

EXAMPLES:
    $0 --config debug --tests
    $0 --config release --clean --install
    $0 -c fast-build -t
EOF
}

# Parse command line arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        -c|--config)
            CONFIG="$2"
            shift 2
            ;;
        -C|--clean)
            CLEAN=true
            shift
            ;;
        -t|--tests)
            TESTS=true
            shift
            ;;
        -i|--install)
            INSTALL=true
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

# Validate config
case $CONFIG in
    debug|release|fast-build|minimal|default)
        ;;
    *)
        print_error "Invalid configuration: $CONFIG"
        print_info "Valid configurations: debug, release, fast-build, minimal, default"
        exit 1
        ;;
esac

# Get script directory and project root
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"

print_status "Building cpp-scaffold project..."
print_info "Configuration: $CONFIG"

# Change to project root
cd "$PROJECT_ROOT"

# Clean if requested
if [ "$CLEAN" = true ]; then
    print_step "🧹 Cleaning build directories..."
    for dir in build build-debug build-release build-fast build-minimal; do
        if [ -d "$dir" ]; then
            rm -rf "$dir"
            echo -e "${GRAY}Cleaned $dir${NC}"
        fi
    done
fi

# Configure
print_step "⚙️  Configuring build..."
configure_args=(--preset "$CONFIG")
if [ "$VERBOSE" = true ]; then
    configure_args+=(--verbose)
fi

cmake "${configure_args[@]}"

# Build
print_step "🔨 Building..."
build_args=(--build --preset "$CONFIG")
if [ "$VERBOSE" = true ]; then
    build_args+=(--verbose)
fi

# Add parallel jobs
if command -v nproc >/dev/null 2>&1; then
    CORES=$(nproc)
elif command -v sysctl >/dev/null 2>&1; then
    CORES=$(sysctl -n hw.ncpu)
else
    CORES=4  # fallback
fi

JOBS=$((CORES > 8 ? 8 : CORES))  # Cap at 8
build_args+=(--parallel "$JOBS")

echo -e "${GRAY}Using $JOBS parallel jobs${NC}"

cmake "${build_args[@]}"

# Run tests if requested
if [ "$TESTS" = true ]; then
    print_step "🧪 Running tests..."
    if ! ctest --preset default --output-on-failure; then
        print_warning "Some tests failed"
    fi
fi

# Install if requested
if [ "$INSTALL" = true ]; then
    print_step "📦 Installing..."
    cmake --install "build-$CONFIG" --config "$CONFIG"
fi

print_status "Build completed successfully!"

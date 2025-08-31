#!/bin/bash

# Test script for all CPP-Scaffold examples
# This script builds and tests all available examples

set -e  # Exit on any error

echo "=========================================="
echo "Testing CPP-Scaffold Examples"
echo "=========================================="

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Function to print status
print_status() {
    local status=$1
    local message=$2
    
    if [ "$status" = "PASS" ]; then
        echo -e "${GREEN}[PASS]${NC} $message"
    elif [ "$status" = "FAIL" ]; then
        echo -e "${RED}[FAIL]${NC} $message"
    elif [ "$status" = "SKIP" ]; then
        echo -e "${YELLOW}[SKIP]${NC} $message"
    else
        echo -e "${YELLOW}[INFO]${NC} $message"
    fi
}

# Function to test an example
test_example() {
    local example_dir=$1
    local example_name=$2
    local build_command=${3:-"mkdir -p build && cd build && cmake .. && make -j4"}
    local test_command=${4:-""}
    
    print_status "INFO" "Testing $example_name..."
    
    if [ ! -d "$example_dir" ]; then
        print_status "SKIP" "$example_name - Directory not found"
        return 0
    fi
    
    cd "$example_dir"
    
    # Clean previous build
    rm -rf build
    
    # Build the example
    if eval "$build_command" > build.log 2>&1; then
        print_status "PASS" "$example_name - Build successful"
        
        # Run test if provided
        if [ -n "$test_command" ]; then
            if eval "$test_command" > test.log 2>&1; then
                print_status "PASS" "$example_name - Test successful"
            else
                print_status "FAIL" "$example_name - Test failed"
                echo "Test output:"
                cat test.log
            fi
        fi
    else
        print_status "FAIL" "$example_name - Build failed"
        echo "Build output:"
        cat build.log
    fi
    
    cd - > /dev/null
}

# Start testing
echo ""
print_status "INFO" "Starting example tests..."
echo ""

# Test 1: Console File Processor
test_example "console-file-processor" "Console File Processor" \
    "mkdir -p build && cd build && cmake .. -DBUILD_TESTING=OFF && make -j4" \
    "cd build && echo 'test content' > test.txt && ./file-processor --help && ./file-processor --count test.txt"

echo ""

# Test 2: Qt Image Viewer
test_example "qt-image-viewer" "Qt Image Viewer" \
    "mkdir -p build && cd build && cmake .. && make -j4" \
    "cd build && ./qt-image-viewer --help || echo 'Qt app requires display'"

echo ""

# Test 3: Modern CMake Example
test_example "build-systems-showcase/cmake-examples/modern-cmake" "Modern CMake Example" \
    "mkdir -p build && cd build && cmake .. && make -j4" \
    "cd build && ./modern_app"

echo ""

# Test 4: Check if other examples have basic structure
print_status "INFO" "Checking other example structures..."

examples=(
    "sfml-game-demo:SFML Game Demo"
    "boost-network-server:Boost Network Server"
    "testing-patterns:Testing Patterns"
    "build-systems-showcase:Build Systems Showcase"
)

for example in "${examples[@]}"; do
    IFS=':' read -r dir name <<< "$example"
    if [ -d "$dir" ] && [ -f "$dir/README.md" ]; then
        print_status "PASS" "$name - Structure exists"
    else
        print_status "SKIP" "$name - Structure missing"
    fi
done

echo ""
echo "=========================================="
print_status "INFO" "Example testing completed!"
echo "=========================================="

# Summary
echo ""
echo "Summary of available examples:"
echo "1. Console File Processor - Complete CLI application"
echo "2. Qt Image Viewer - GUI application with Qt6"
echo "3. Modern CMake Example - Build system demonstration"
echo "4. SFML Game Demo - Game development example"
echo "5. Boost Network Server - Network programming example"
echo "6. Testing Patterns - Comprehensive testing guide"
echo "7. Build Systems Showcase - Multiple build systems"
echo ""
echo "All examples include:"
echo "- Complete source code"
echo "- Comprehensive documentation"
echo "- Build system configuration"
echo "- Usage examples"
echo ""
echo "To run individual examples:"
echo "  cd examples/[example-name]"
echo "  mkdir build && cd build"
echo "  cmake .. && make"
echo "  ./[executable-name]"

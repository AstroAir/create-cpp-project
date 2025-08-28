#!/bin/bash

# Generate C++20 Modules Examples Script
# This script creates all the module examples using cpp-scaffold

set -e

echo "🚀 Generating C++20 Modules Examples with cpp-scaffold"
echo "======================================================="

# Check if cpp-scaffold is available
if ! command -v cpp-scaffold &> /dev/null; then
    echo "❌ cpp-scaffold not found. Please install cpp-scaffold first."
    exit 1
fi

# Create examples directory
mkdir -p generated
cd generated

echo ""
echo "📁 Creating Basic Module Example..."
echo "-----------------------------------"
cpp-scaffold basic-module \
    --template modules \
    --build cmake \
    --std cpp20 \
    --tests \
    --test-framework gtest \
    --package vcpkg \
    --docs \
    --init-git

echo "✅ Basic module example created!"

echo ""
echo "📁 Creating Math Library Example..."
echo "-----------------------------------"
cpp-scaffold math-library \
    --template modules \
    --build cmake \
    --std cpp20 \
    --tests \
    --test-framework gtest \
    --package vcpkg \
    --docs \
    --code-style \
    --init-git

echo "✅ Math library example created!"

echo ""
echo "📁 Creating Header Units Example..."
echo "-----------------------------------"
cpp-scaffold header-units \
    --template modules \
    --build cmake \
    --std cpp20 \
    --tests \
    --test-framework catch2 \
    --package vcpkg \
    --docs \
    --init-git

echo "✅ Header units example created!"

echo ""
echo "📁 Creating Module Partitions Example..."
echo "----------------------------------------"
cpp-scaffold partitions \
    --template modules \
    --build cmake \
    --std cpp23 \
    --tests \
    --test-framework gtest \
    --package vcpkg \
    --docs \
    --code-style \
    --init-git

echo "✅ Module partitions example created!"

echo ""
echo "🎉 All examples generated successfully!"
echo "======================================"
echo ""
echo "📂 Generated examples in: $(pwd)"
echo ""
echo "🔨 To build an example:"
echo "   cd <example-name>"
echo "   mkdir build && cd build"
echo "   cmake .. -DCMAKE_CXX_STANDARD=20  # or 23 for partitions"
echo "   cmake --build ."
echo ""
echo "🧪 To run tests:"
echo "   ctest"
echo ""
echo "📖 Each example includes:"
echo "   - Complete source code"
echo "   - CMake configuration"
echo "   - Documentation"
echo "   - Unit tests"
echo "   - Usage examples"
echo ""
echo "⚠️  Compiler Requirements:"
echo "   - GCC 11+ (basic support)"
echo "   - Clang 15+ (good support)"
echo "   - MSVC 19.28+ (full support)"
echo "   - CMake 3.28+"

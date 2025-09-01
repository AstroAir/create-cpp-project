#!/bin/bash
# Installation script for test-premake-lib

set -e

# Default installation directory
INSTALL_DIR="/usr/local"

# Parse command line arguments
while [[ "$#" -gt 0 ]]; do
  case $1 in
    --prefix=*) INSTALL_DIR="${1#*=}"; shift ;;
    --prefix) INSTALL_DIR="$2"; shift 2 ;;
    *) echo "Unknown parameter: $1"; exit 1 ;;
  esac
done

echo "Installing test-premake-lib to $INSTALL_DIR"

# Create build directory if it doesn't exist
if [ ! -d "build" ]; then
  mkdir -p build
fi

# Configure and build
if command -v cmake &> /dev/null; then
  cd build
  cmake .. -DCMAKE_INSTALL_PREFIX="$INSTALL_DIR"
  make -j$(nproc)
  make install
else
  echo "CMake not found. Please install CMake and try again."
  exit 1
fi

echo "test-premake-lib has been successfully installed to $INSTALL_DIR"

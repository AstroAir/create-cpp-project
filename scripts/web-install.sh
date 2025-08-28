#!/bin/bash
# Web-based installer for cpp-scaffold
# Usage: curl -sSL https://raw.githubusercontent.com/cpp-scaffold/cpp-scaffold/main/scripts/web-install.sh | bash

set -euo pipefail

# Configuration
REPO_URL="https://raw.githubusercontent.com/cpp-scaffold/cpp-scaffold/main"
INSTALLER_SCRIPT="scripts/install.sh"
TEMP_INSTALLER="/tmp/cpp-scaffold-install.sh"

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
CYAN='\033[0;36m'
NC='\033[0m'

print_status() {
    echo -e "${GREEN}✓ $1${NC}"
}

print_info() {
    echo -e "${CYAN}ℹ $1${NC}"
}

print_error() {
    echo -e "${RED}✗ $1${NC}"
}

print_banner() {
    echo -e "${CYAN}"
    echo "╔══════════════════════════════════════════════════════════════╗"
    echo "║                CPP-Scaffold Web Installer                   ║"
    echo "║              Modern C++ Project Scaffolding Tool            ║"
    echo "╚══════════════════════════════════════════════════════════════╝"
    echo -e "${NC}"
}

# Check if running with curl/wget pipe
check_pipe_install() {
    if [[ -t 0 ]]; then
        # Not piped, running directly
        return 1
    else
        # Piped installation
        return 0
    fi
}

# Download and execute installer
main() {
    print_banner
    
    print_info "CPP-Scaffold Web Installer"
    print_info "This will download and run the full installer script"
    
    # Check for required tools
    if command -v curl >/dev/null 2>&1; then
        DOWNLOAD_CMD="curl -sSL"
    elif command -v wget >/dev/null 2>&1; then
        DOWNLOAD_CMD="wget -qO-"
    else
        print_error "Neither curl nor wget found. Please install one of them."
        exit 1
    fi
    
    # Download installer script
    print_info "Downloading installer script..."
    
    if ! $DOWNLOAD_CMD "$REPO_URL/$INSTALLER_SCRIPT" > "$TEMP_INSTALLER"; then
        print_error "Failed to download installer script"
        exit 1
    fi
    
    # Make executable
    chmod +x "$TEMP_INSTALLER"
    
    print_status "Installer downloaded successfully"
    
    # If we're in a pipe, ask for confirmation
    if check_pipe_install; then
        print_info "About to run the cpp-scaffold installer"
        print_info "Press Enter to continue or Ctrl+C to cancel..."
        read -r
    fi
    
    # Execute installer with all arguments
    print_info "Running installer..."
    exec "$TEMP_INSTALLER" "$@"
}

# Cleanup on exit
cleanup() {
    rm -f "$TEMP_INSTALLER" 2>/dev/null || true
}
trap cleanup EXIT

# Run main function
main "$@"

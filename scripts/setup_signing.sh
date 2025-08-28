#!/bin/bash
# Setup script for package signing infrastructure
# This script helps configure code signing for different platforms

set -euo pipefail

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
CYAN='\033[0;36m'
NC='\033[0m' # No Color

print_status() {
    echo -e "${GREEN}✓ $1${NC}"
}

print_info() {
    echo -e "${CYAN}ℹ $1${NC}"
}

print_warning() {
    echo -e "${YELLOW}⚠ $1${NC}"
}

print_error() {
    echo -e "${RED}✗ $1${NC}"
}

# Help function
show_help() {
    cat << EOF
Package Signing Setup Script

Usage: $0 [OPTIONS]

OPTIONS:
    --platform PLATFORM    Target platform (windows|macos|linux|all)
    --windows-cert PATH     Path to Windows certificate (.p12/.pfx)
    --macos-identity ID     macOS signing identity
    --linux-gpg-key KEY     Linux GPG key ID
    --verify               Verify signing setup
    --help                 Show this help message

EXAMPLES:
    $0 --platform windows --windows-cert mycert.p12
    $0 --platform macos --macos-identity "Developer ID Application: My Company"
    $0 --platform linux --linux-gpg-key "12345678"
    $0 --verify

ENVIRONMENT VARIABLES:
    WINDOWS_CERT_PASSWORD   Password for Windows certificate
    APPLE_ID               Apple ID for notarization
    APPLE_PASSWORD         App-specific password for notarization
    LINUX_GPG_PASSPHRASE   GPG key passphrase

EOF
}

# Check if running on correct platform
check_platform() {
    local target_platform="$1"
    local current_platform=""
    
    if [[ "$OSTYPE" == "msys" || "$OSTYPE" == "cygwin" ]]; then
        current_platform="windows"
    elif [[ "$OSTYPE" == "darwin"* ]]; then
        current_platform="macos"
    else
        current_platform="linux"
    fi
    
    if [[ "$target_platform" != "all" && "$target_platform" != "$current_platform" ]]; then
        print_warning "Setting up $target_platform signing on $current_platform platform"
        print_info "Some verification steps may not work cross-platform"
    fi
}

# Setup Windows signing
setup_windows_signing() {
    local cert_path="$1"
    
    print_info "Setting up Windows code signing..."
    
    # Check if signtool is available
    if ! command -v signtool >/dev/null 2>&1; then
        print_error "signtool not found. Please install Windows SDK or Visual Studio"
        return 1
    fi
    
    # Verify certificate
    if [[ -f "$cert_path" ]]; then
        print_status "Certificate found: $cert_path"
        
        # Test certificate (requires password)
        if [[ -n "${WINDOWS_CERT_PASSWORD:-}" ]]; then
            print_info "Testing certificate..."
            # Create a dummy file to test signing
            echo "test" > test_file.txt
            if signtool sign /f "$cert_path" /p "$WINDOWS_CERT_PASSWORD" /t http://timestamp.digicert.com test_file.txt >/dev/null 2>&1; then
                print_status "Certificate test successful"
                rm -f test_file.txt
            else
                print_error "Certificate test failed"
                rm -f test_file.txt
                return 1
            fi
        else
            print_warning "WINDOWS_CERT_PASSWORD not set, skipping certificate test"
        fi
        
        # Export environment variable
        export WINDOWS_CERT_PATH="$cert_path"
        print_status "Windows signing configured"
    else
        print_error "Certificate not found: $cert_path"
        return 1
    fi
}

# Setup macOS signing
setup_macos_signing() {
    local identity="$1"
    
    print_info "Setting up macOS code signing..."
    
    # Check if codesign is available
    if ! command -v codesign >/dev/null 2>&1; then
        print_error "codesign not found. Please install Xcode command line tools"
        return 1
    fi
    
    # Verify identity
    if security find-identity -v -p codesigning | grep -q "$identity"; then
        print_status "Signing identity found: $identity"
        
        # Test signing
        print_info "Testing signing identity..."
        echo "test" > test_file.txt
        if codesign --sign "$identity" test_file.txt >/dev/null 2>&1; then
            print_status "Identity test successful"
            rm -f test_file.txt
        else
            print_error "Identity test failed"
            rm -f test_file.txt
            return 1
        fi
        
        # Check for notarization setup
        if [[ -n "${APPLE_ID:-}" && -n "${APPLE_PASSWORD:-}" ]]; then
            print_status "Notarization credentials configured"
        else
            print_warning "Notarization credentials not set (APPLE_ID, APPLE_PASSWORD)"
        fi
        
        # Export environment variable
        export MACOS_IDENTITY="$identity"
        print_status "macOS signing configured"
    else
        print_error "Signing identity not found: $identity"
        print_info "Available identities:"
        security find-identity -v -p codesigning
        return 1
    fi
}

# Setup Linux signing
setup_linux_signing() {
    local gpg_key="$1"
    
    print_info "Setting up Linux package signing..."
    
    # Check if GPG is available
    if ! command -v gpg >/dev/null 2>&1; then
        print_error "gpg not found. Please install GnuPG"
        return 1
    fi
    
    # Verify GPG key
    if gpg --list-secret-keys "$gpg_key" >/dev/null 2>&1; then
        print_status "GPG key found: $gpg_key"
        
        # Test signing
        print_info "Testing GPG key..."
        echo "test" > test_file.txt
        if gpg --armor --detach-sign --default-key "$gpg_key" test_file.txt >/dev/null 2>&1; then
            print_status "GPG key test successful"
            rm -f test_file.txt test_file.txt.asc
        else
            print_error "GPG key test failed"
            rm -f test_file.txt test_file.txt.asc
            return 1
        fi
        
        # Export environment variable
        export LINUX_GPG_KEY="$gpg_key"
        print_status "Linux signing configured"
    else
        print_error "GPG key not found: $gpg_key"
        print_info "Available keys:"
        gpg --list-secret-keys --keyid-format LONG
        return 1
    fi
}

# Verify signing setup
verify_setup() {
    print_info "Verifying signing setup..."
    
    local all_good=true
    
    # Check Windows
    if [[ -n "${WINDOWS_CERT_PATH:-}" ]]; then
        if [[ -f "$WINDOWS_CERT_PATH" ]]; then
            print_status "Windows certificate: $WINDOWS_CERT_PATH"
        else
            print_error "Windows certificate not found: $WINDOWS_CERT_PATH"
            all_good=false
        fi
    fi
    
    # Check macOS
    if [[ -n "${MACOS_IDENTITY:-}" ]]; then
        if command -v security >/dev/null 2>&1 && security find-identity -v -p codesigning | grep -q "$MACOS_IDENTITY"; then
            print_status "macOS identity: $MACOS_IDENTITY"
        else
            print_error "macOS identity not found: $MACOS_IDENTITY"
            all_good=false
        fi
    fi
    
    # Check Linux
    if [[ -n "${LINUX_GPG_KEY:-}" ]]; then
        if command -v gpg >/dev/null 2>&1 && gpg --list-secret-keys "$LINUX_GPG_KEY" >/dev/null 2>&1; then
            print_status "Linux GPG key: $LINUX_GPG_KEY"
        else
            print_error "Linux GPG key not found: $LINUX_GPG_KEY"
            all_good=false
        fi
    fi
    
    if $all_good; then
        print_status "All signing configurations verified"
        return 0
    else
        print_error "Some signing configurations failed verification"
        return 1
    fi
}

# Main script
main() {
    local platform=""
    local windows_cert=""
    local macos_identity=""
    local linux_gpg_key=""
    local verify_only=false
    
    # Parse arguments
    while [[ $# -gt 0 ]]; do
        case $1 in
            --platform)
                platform="$2"
                shift 2
                ;;
            --windows-cert)
                windows_cert="$2"
                shift 2
                ;;
            --macos-identity)
                macos_identity="$2"
                shift 2
                ;;
            --linux-gpg-key)
                linux_gpg_key="$2"
                shift 2
                ;;
            --verify)
                verify_only=true
                shift
                ;;
            --help)
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
    
    if $verify_only; then
        verify_setup
        exit $?
    fi
    
    if [[ -z "$platform" ]]; then
        print_error "Platform not specified"
        show_help
        exit 1
    fi
    
    check_platform "$platform"
    
    case "$platform" in
        windows)
            if [[ -z "$windows_cert" ]]; then
                print_error "Windows certificate path required"
                exit 1
            fi
            setup_windows_signing "$windows_cert"
            ;;
        macos)
            if [[ -z "$macos_identity" ]]; then
                print_error "macOS identity required"
                exit 1
            fi
            setup_macos_signing "$macos_identity"
            ;;
        linux)
            if [[ -z "$linux_gpg_key" ]]; then
                print_error "Linux GPG key required"
                exit 1
            fi
            setup_linux_signing "$linux_gpg_key"
            ;;
        all)
            [[ -n "$windows_cert" ]] && setup_windows_signing "$windows_cert"
            [[ -n "$macos_identity" ]] && setup_macos_signing "$macos_identity"
            [[ -n "$linux_gpg_key" ]] && setup_linux_signing "$linux_gpg_key"
            ;;
        *)
            print_error "Invalid platform: $platform"
            print_info "Valid platforms: windows, macos, linux, all"
            exit 1
            ;;
    esac
    
    print_status "Signing setup completed for $platform"
}

# Run main function
main "$@"

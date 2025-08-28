#!/bin/bash
# Universal installation script for cpp-scaffold
# Supports multiple installation methods and platforms

set -euo pipefail

# Script metadata
SCRIPT_VERSION="1.0.0"
PROJECT_NAME="cpp-scaffold"
PROJECT_VERSION="1.0.0"
GITHUB_REPO="cpp-scaffold/cpp-scaffold"

# Default configuration
INSTALL_DIR=""
INSTALL_METHOD="auto"
FORCE_INSTALL=false
GUI_MODE=false
VERBOSE=false
DRY_RUN=false
UNINSTALL=false

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
CYAN='\033[0;36m'
BLUE='\033[0;34m'
BOLD='\033[1m'
NC='\033[0m' # No Color

# Platform detection
PLATFORM=""
ARCH=""
PACKAGE_MANAGER=""

# Installation paths
DEFAULT_INSTALL_PREFIX="/usr/local"
USER_INSTALL_PREFIX="$HOME/.local"

print_banner() {
    echo -e "${CYAN}${BOLD}"
    echo "╔══════════════════════════════════════════════════════════════╗"
    echo "║                    CPP-Scaffold Installer                   ║"
    echo "║              Modern C++ Project Scaffolding Tool            ║"
    echo "╚══════════════════════════════════════════════════════════════╝"
    echo -e "${NC}"
}

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

print_step() {
    echo -e "${BLUE}→ $1${NC}"
}

show_help() {
    cat << EOF
CPP-Scaffold Universal Installer

Usage: $0 [OPTIONS]

OPTIONS:
    -d, --dir DIR              Installation directory
    -m, --method METHOD        Installation method (auto|package|binary|source)
    -f, --force               Force installation (overwrite existing)
    -g, --gui                 Use GUI mode (if available)
    -v, --verbose             Enable verbose output
    -n, --dry-run             Show what would be done without executing
    -u, --uninstall           Uninstall cpp-scaffold
    -h, --help                Show this help message

INSTALLATION METHODS:
    auto                      Automatically choose best method
    package                   Use system package manager
    binary                    Download and install binary
    source                    Build from source

EXAMPLES:
    $0                        # Auto-install to default location
    $0 --dir ~/.local         # Install to user directory
    $0 --method package       # Force package manager installation
    $0 --gui                  # Use GUI installer (if available)
    $0 --uninstall            # Uninstall cpp-scaffold

EOF
}

# Detect platform and architecture
detect_platform() {
    case "$(uname -s)" in
        Linux*)     PLATFORM="linux" ;;
        Darwin*)    PLATFORM="macos" ;;
        CYGWIN*|MINGW*|MSYS*) PLATFORM="windows" ;;
        *)          PLATFORM="unknown" ;;
    esac
    
    case "$(uname -m)" in
        x86_64|amd64)   ARCH="x86_64" ;;
        aarch64|arm64)  ARCH="arm64" ;;
        armv7l)         ARCH="armv7" ;;
        i386|i686)      ARCH="i386" ;;
        *)              ARCH="unknown" ;;
    esac
    
    if $VERBOSE; then
        print_info "Detected platform: $PLATFORM ($ARCH)"
    fi
}

# Detect package manager
detect_package_manager() {
    if command -v apt-get >/dev/null 2>&1; then
        PACKAGE_MANAGER="apt"
    elif command -v yum >/dev/null 2>&1; then
        PACKAGE_MANAGER="yum"
    elif command -v dnf >/dev/null 2>&1; then
        PACKAGE_MANAGER="dnf"
    elif command -v pacman >/dev/null 2>&1; then
        PACKAGE_MANAGER="pacman"
    elif command -v zypper >/dev/null 2>&1; then
        PACKAGE_MANAGER="zypper"
    elif command -v brew >/dev/null 2>&1; then
        PACKAGE_MANAGER="brew"
    elif command -v port >/dev/null 2>&1; then
        PACKAGE_MANAGER="macports"
    else
        PACKAGE_MANAGER="none"
    fi
    
    if $VERBOSE; then
        print_info "Detected package manager: $PACKAGE_MANAGER"
    fi
}

# Check if running as root
is_root() {
    [[ $EUID -eq 0 ]]
}

# Check if GUI is available
has_gui() {
    if [[ "$PLATFORM" == "linux" ]]; then
        command -v zenity >/dev/null 2>&1 || command -v dialog >/dev/null 2>&1
    elif [[ "$PLATFORM" == "macos" ]]; then
        command -v osascript >/dev/null 2>&1
    else
        false
    fi
}

# Show GUI dialog
show_gui_dialog() {
    local title="$1"
    local message="$2"
    local type="${3:-info}"
    
    if [[ "$PLATFORM" == "linux" ]]; then
        if command -v zenity >/dev/null 2>&1; then
            case "$type" in
                error)   zenity --error --title "$title" --text "$message" ;;
                warning) zenity --warning --title "$title" --text "$message" ;;
                question) zenity --question --title "$title" --text "$message" ;;
                *)       zenity --info --title "$title" --text "$message" ;;
            esac
        elif command -v dialog >/dev/null 2>&1; then
            dialog --title "$title" --msgbox "$message" 10 60
        fi
    elif [[ "$PLATFORM" == "macos" ]]; then
        local icon="note"
        case "$type" in
            error)   icon="stop" ;;
            warning) icon="caution" ;;
        esac
        osascript -e "display dialog \"$message\" with title \"$title\" with icon $icon"
    fi
}

# Get installation directory
get_install_dir() {
    if [[ -n "$INSTALL_DIR" ]]; then
        echo "$INSTALL_DIR"
        return
    fi
    
    if is_root; then
        echo "$DEFAULT_INSTALL_PREFIX"
    else
        echo "$USER_INSTALL_PREFIX"
    fi
}

# Check dependencies
check_dependencies() {
    local missing_deps=()
    
    # Check for required tools
    local required_tools=("curl" "tar")
    
    for tool in "${required_tools[@]}"; do
        if ! command -v "$tool" >/dev/null 2>&1; then
            missing_deps+=("$tool")
        fi
    done
    
    if [[ ${#missing_deps[@]} -gt 0 ]]; then
        print_error "Missing required dependencies: ${missing_deps[*]}"
        print_info "Please install the missing dependencies and try again"
        return 1
    fi
    
    return 0
}

# Download file with progress
download_file() {
    local url="$1"
    local output="$2"
    
    if $VERBOSE; then
        print_step "Downloading: $url"
    fi
    
    if command -v curl >/dev/null 2>&1; then
        if $VERBOSE; then
            curl -L --progress-bar -o "$output" "$url"
        else
            curl -L -s -o "$output" "$url"
        fi
    elif command -v wget >/dev/null 2>&1; then
        if $VERBOSE; then
            wget --progress=bar -O "$output" "$url"
        else
            wget -q -O "$output" "$url"
        fi
    else
        print_error "Neither curl nor wget found"
        return 1
    fi
}

# Install via package manager
install_via_package_manager() {
    print_step "Installing via package manager ($PACKAGE_MANAGER)..."
    
    case "$PACKAGE_MANAGER" in
        apt)
            if $DRY_RUN; then
                print_info "Would run: apt-get update && apt-get install cpp-scaffold"
                return 0
            fi
            
            print_step "Updating package lists..."
            sudo apt-get update
            
            print_step "Installing cpp-scaffold..."
            sudo apt-get install -y cpp-scaffold
            ;;
        yum|dnf)
            if $DRY_RUN; then
                print_info "Would run: $PACKAGE_MANAGER install cpp-scaffold"
                return 0
            fi
            
            print_step "Installing cpp-scaffold..."
            sudo "$PACKAGE_MANAGER" install -y cpp-scaffold
            ;;
        pacman)
            if $DRY_RUN; then
                print_info "Would run: pacman -S cpp-scaffold"
                return 0
            fi
            
            print_step "Installing cpp-scaffold..."
            sudo pacman -S --noconfirm cpp-scaffold
            ;;
        brew)
            if $DRY_RUN; then
                print_info "Would run: brew install cpp-scaffold"
                return 0
            fi
            
            print_step "Installing cpp-scaffold..."
            brew install cpp-scaffold
            ;;
        *)
            print_warning "Package manager installation not supported for $PACKAGE_MANAGER"
            return 1
            ;;
    esac
    
    return 0
}

# Install from binary
install_from_binary() {
    print_step "Installing from binary..."
    
    local install_dir
    install_dir=$(get_install_dir)
    
    local binary_url="https://github.com/$GITHUB_REPO/releases/latest/download/cpp-scaffold-$PROJECT_VERSION-$PLATFORM-$ARCH.tar.gz"
    local temp_dir
    temp_dir=$(mktemp -d)
    local archive_file="$temp_dir/cpp-scaffold.tar.gz"
    
    if $DRY_RUN; then
        print_info "Would download: $binary_url"
        print_info "Would extract to: $install_dir"
        rm -rf "$temp_dir"
        return 0
    fi
    
    # Download binary archive
    if ! download_file "$binary_url" "$archive_file"; then
        print_error "Failed to download binary"
        rm -rf "$temp_dir"
        return 1
    fi
    
    # Extract archive
    print_step "Extracting archive..."
    if ! tar -xzf "$archive_file" -C "$temp_dir"; then
        print_error "Failed to extract archive"
        rm -rf "$temp_dir"
        return 1
    fi
    
    # Install files
    print_step "Installing to $install_dir..."
    
    # Create directories
    mkdir -p "$install_dir/bin"
    mkdir -p "$install_dir/share/cpp-scaffold"
    
    # Copy files
    if [[ -f "$temp_dir/cpp-scaffold" ]]; then
        cp "$temp_dir/cpp-scaffold" "$install_dir/bin/"
        chmod +x "$install_dir/bin/cpp-scaffold"
    else
        print_error "Binary not found in archive"
        rm -rf "$temp_dir"
        return 1
    fi
    
    # Copy documentation
    if [[ -d "$temp_dir/share" ]]; then
        cp -r "$temp_dir/share/"* "$install_dir/share/cpp-scaffold/"
    fi
    
    # Cleanup
    rm -rf "$temp_dir"
    
    return 0
}

# Install from source
install_from_source() {
    print_step "Installing from source..."
    
    local install_dir
    install_dir=$(get_install_dir)
    
    # Check build dependencies
    local build_deps=("cmake" "ninja" "git")
    local missing_build_deps=()
    
    for dep in "${build_deps[@]}"; do
        if ! command -v "$dep" >/dev/null 2>&1; then
            missing_build_deps+=("$dep")
        fi
    done
    
    if [[ ${#missing_build_deps[@]} -gt 0 ]]; then
        print_error "Missing build dependencies: ${missing_build_deps[*]}"
        return 1
    fi
    
    local temp_dir
    temp_dir=$(mktemp -d)
    
    if $DRY_RUN; then
        print_info "Would clone repository to: $temp_dir"
        print_info "Would build and install to: $install_dir"
        rm -rf "$temp_dir"
        return 0
    fi
    
    # Clone repository
    print_step "Cloning repository..."
    if ! git clone "https://github.com/$GITHUB_REPO.git" "$temp_dir"; then
        print_error "Failed to clone repository"
        rm -rf "$temp_dir"
        return 1
    fi
    
    cd "$temp_dir"
    
    # Configure build
    print_step "Configuring build..."
    if ! cmake --preset release; then
        print_error "Failed to configure build"
        rm -rf "$temp_dir"
        return 1
    fi
    
    # Build
    print_step "Building..."
    if ! cmake --build --preset release; then
        print_error "Failed to build"
        rm -rf "$temp_dir"
        return 1
    fi
    
    # Install
    print_step "Installing to $install_dir..."
    if ! cmake --install build --prefix "$install_dir"; then
        print_error "Failed to install"
        rm -rf "$temp_dir"
        return 1
    fi
    
    # Cleanup
    cd - >/dev/null
    rm -rf "$temp_dir"
    
    return 0
}

# Verify installation
verify_installation() {
    local install_dir
    install_dir=$(get_install_dir)
    
    local executable="$install_dir/bin/cpp-scaffold"
    
    if [[ ! -f "$executable" ]]; then
        print_error "Installation verification failed: executable not found"
        return 1
    fi
    
    if [[ ! -x "$executable" ]]; then
        print_error "Installation verification failed: executable not executable"
        return 1
    fi
    
    # Test executable
    if ! "$executable" --version >/dev/null 2>&1; then
        print_warning "Installation verification: executable test failed (may be expected)"
    fi
    
    print_status "Installation verified successfully"
    return 0
}

# Update PATH
update_path() {
    local install_dir
    install_dir=$(get_install_dir)
    
    local bin_dir="$install_dir/bin"
    
    # Check if already in PATH
    if [[ ":$PATH:" == *":$bin_dir:"* ]]; then
        print_info "Installation directory already in PATH"
        return 0
    fi
    
    # Determine shell configuration file
    local shell_config=""
    if [[ -n "${BASH_VERSION:-}" ]]; then
        shell_config="$HOME/.bashrc"
    elif [[ -n "${ZSH_VERSION:-}" ]]; then
        shell_config="$HOME/.zshrc"
    else
        shell_config="$HOME/.profile"
    fi
    
    if $DRY_RUN; then
        print_info "Would add $bin_dir to PATH in $shell_config"
        return 0
    fi
    
    # Add to PATH
    echo "" >> "$shell_config"
    echo "# Added by cpp-scaffold installer" >> "$shell_config"
    echo "export PATH=\"$bin_dir:\$PATH\"" >> "$shell_config"
    
    print_status "Added $bin_dir to PATH in $shell_config"
    print_info "Please restart your shell or run: source $shell_config"
}

# Uninstall
uninstall_cpp_scaffold() {
    print_step "Uninstalling cpp-scaffold..."
    
    local install_dir
    install_dir=$(get_install_dir)
    
    if $DRY_RUN; then
        print_info "Would remove: $install_dir/bin/cpp-scaffold"
        print_info "Would remove: $install_dir/share/cpp-scaffold"
        return 0
    fi
    
    # Remove executable
    if [[ -f "$install_dir/bin/cpp-scaffold" ]]; then
        rm -f "$install_dir/bin/cpp-scaffold"
        print_status "Removed executable"
    fi
    
    # Remove documentation
    if [[ -d "$install_dir/share/cpp-scaffold" ]]; then
        rm -rf "$install_dir/share/cpp-scaffold"
        print_status "Removed documentation"
    fi
    
    print_status "Uninstallation completed"
}

# Main installation logic
main_install() {
    if [[ "$INSTALL_METHOD" == "auto" ]]; then
        # Try package manager first, then binary, then source
        if [[ "$PACKAGE_MANAGER" != "none" ]]; then
            INSTALL_METHOD="package"
        else
            INSTALL_METHOD="binary"
        fi
    fi
    
    case "$INSTALL_METHOD" in
        package)
            if ! install_via_package_manager; then
                print_warning "Package manager installation failed, trying binary..."
                install_from_binary
            fi
            ;;
        binary)
            if ! install_from_binary; then
                print_warning "Binary installation failed, trying source..."
                install_from_source
            fi
            ;;
        source)
            install_from_source
            ;;
        *)
            print_error "Unknown installation method: $INSTALL_METHOD"
            return 1
            ;;
    esac
}

# Parse command line arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        -d|--dir)
            INSTALL_DIR="$2"
            shift 2
            ;;
        -m|--method)
            INSTALL_METHOD="$2"
            shift 2
            ;;
        -f|--force)
            FORCE_INSTALL=true
            shift
            ;;
        -g|--gui)
            GUI_MODE=true
            shift
            ;;
        -v|--verbose)
            VERBOSE=true
            shift
            ;;
        -n|--dry-run)
            DRY_RUN=true
            shift
            ;;
        -u|--uninstall)
            UNINSTALL=true
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

# Main execution
main() {
    print_banner
    
    # Enable GUI mode if requested and available
    if $GUI_MODE && ! has_gui; then
        print_warning "GUI mode requested but not available, falling back to CLI"
        GUI_MODE=false
    fi
    
    # Detect platform and package manager
    detect_platform
    detect_package_manager
    
    # Check dependencies
    if ! check_dependencies; then
        exit 1
    fi
    
    # Handle uninstall
    if $UNINSTALL; then
        uninstall_cpp_scaffold
        exit 0
    fi
    
    # Check if already installed
    local install_dir
    install_dir=$(get_install_dir)
    
    if [[ -f "$install_dir/bin/cpp-scaffold" ]] && ! $FORCE_INSTALL; then
        print_warning "cpp-scaffold is already installed at $install_dir"
        print_info "Use --force to reinstall or --uninstall to remove"
        exit 1
    fi
    
    # Show GUI confirmation if in GUI mode
    if $GUI_MODE; then
        if ! show_gui_dialog "CPP-Scaffold Installer" "Install cpp-scaffold to $install_dir?" "question"; then
            print_info "Installation cancelled by user"
            exit 0
        fi
    fi
    
    # Perform installation
    print_info "Installing cpp-scaffold to: $install_dir"
    print_info "Installation method: $INSTALL_METHOD"
    
    if $DRY_RUN; then
        print_info "DRY RUN MODE - No changes will be made"
    fi
    
    if main_install; then
        if ! $DRY_RUN; then
            verify_installation
            update_path
        fi
        
        print_status "Installation completed successfully!"
        
        if $GUI_MODE; then
            show_gui_dialog "Installation Complete" "cpp-scaffold has been installed successfully!"
        fi
        
        print_info "Run 'cpp-scaffold --help' to get started"
    else
        print_error "Installation failed"
        
        if $GUI_MODE; then
            show_gui_dialog "Installation Failed" "cpp-scaffold installation failed. Check the terminal for details." "error"
        fi
        
        exit 1
    fi
}

# Run main function
main "$@"

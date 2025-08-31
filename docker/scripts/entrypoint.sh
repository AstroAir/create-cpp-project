#!/bin/bash
# Docker container entrypoint script
# This script sets up the container environment and handles initialization

set -euo pipefail

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Print colored output
print_info() {
    echo -e "${BLUE}[ENTRYPOINT]${NC} $1"
}

print_success() {
    echo -e "${GREEN}[ENTRYPOINT]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[ENTRYPOINT]${NC} $1"
}

print_error() {
    echo -e "${RED}[ENTRYPOINT]${NC} $1"
}

# Initialize container environment
init_environment() {
    print_info "Initializing container environment..."
    
    # Set up user if USER_ID and GROUP_ID are provided
    if [[ -n "${USER_ID:-}" ]] && [[ -n "${GROUP_ID:-}" ]]; then
        print_info "Setting up user with UID=$USER_ID and GID=$GROUP_ID"
        
        # Create group if it doesn't exist
        if ! getent group "$GROUP_ID" > /dev/null 2>&1; then
            groupadd -g "$GROUP_ID" containeruser
        fi
        
        # Create user if it doesn't exist
        if ! getent passwd "$USER_ID" > /dev/null 2>&1; then
            useradd -u "$USER_ID" -g "$GROUP_ID" -m -s /bin/bash containeruser
        fi
        
        # Set up home directory permissions
        chown -R "$USER_ID:$GROUP_ID" /home/containeruser || true
        
        # Switch to the created user for the rest of the execution
        export HOME="/home/containeruser"
        exec gosu "$USER_ID:$GROUP_ID" "$0" "$@"
    fi
    
    # Set up ccache directory permissions
    if [[ -d "/root/.ccache" ]]; then
        chmod 755 /root/.ccache
    fi
    
    # Set up vcpkg permissions
    if [[ -d "/opt/vcpkg" ]]; then
        chmod -R 755 /opt/vcpkg
    fi
    
    print_success "Container environment initialized"
}

# Set up development tools
setup_dev_tools() {
    print_info "Setting up development tools..."
    
    # Configure git if not already configured
    if ! git config --global user.name > /dev/null 2>&1; then
        git config --global user.name "${GIT_USER_NAME:-Container User}"
        git config --global user.email "${GIT_USER_EMAIL:-user@container.local}"
        print_info "Git configured with default user"
    fi
    
    # Set up ccache configuration
    if command -v ccache &> /dev/null; then
        export CCACHE_DIR="${CCACHE_DIR:-/root/.ccache}"
        mkdir -p "$CCACHE_DIR"
        
        # Set ccache configuration if not exists
        if [[ ! -f "$CCACHE_DIR/ccache.conf" ]]; then
            cat > "$CCACHE_DIR/ccache.conf" << EOF
max_size = ${CCACHE_MAX_SIZE:-5.0G}
compression = true
compression_level = 6
hash_dir = false
EOF
            print_info "ccache configured"
        fi
    fi
    
    # Set up Conan profile if not exists
    if command -v conan &> /dev/null; then
        if [[ ! -f "$HOME/.conan2/profiles/default" ]]; then
            conan profile detect --force > /dev/null 2>&1 || true
            print_info "Conan profile created"
        fi
    fi
    
    print_success "Development tools configured"
}

# Show container information
show_container_info() {
    print_info "Container Information:"
    echo "  OS: $(cat /etc/os-release | grep PRETTY_NAME | cut -d'"' -f2)"
    echo "  Architecture: $(uname -m)"
    echo "  Kernel: $(uname -r)"
    echo "  CMake: $(cmake --version | head -n1)"
    echo "  Compiler (C): $(${CC:-gcc} --version | head -n1)"
    echo "  Compiler (C++): $(${CXX:-g++} --version | head -n1)"
    
    if command -v ccache &> /dev/null; then
        echo "  ccache: $(ccache --version | head -n1)"
    fi
    
    if command -v vcpkg &> /dev/null; then
        echo "  vcpkg: Available"
    fi
    
    if command -v conan &> /dev/null; then
        echo "  Conan: $(conan --version)"
    fi
    
    echo "  Working Directory: $(pwd)"
    echo "  User: $(whoami) ($(id))"
}

# Handle special commands
handle_special_commands() {
    case "${1:-}" in
        "info")
            show_container_info
            exit 0
            ;;
        "build")
            shift
            exec build "$@"
            ;;
        "test")
            shift
            exec test "$@"
            ;;
        "shell")
            exec /bin/bash
            ;;
        "help")
            cat << EOF
Container Commands:
  info          Show container information
  build [args]  Build the project using the build script
  test [args]   Run tests
  shell         Start an interactive shell
  help          Show this help message

Any other command will be executed directly.

Examples:
  docker run --rm -it container info
  docker run --rm -v \$(pwd):/workspace container build --type Debug
  docker run --rm -v \$(pwd):/workspace container shell

EOF
            exit 0
            ;;
    esac
}

# Main entrypoint function
main() {
    # Handle special commands first
    handle_special_commands "$@"
    
    # Initialize environment
    init_environment "$@"
    setup_dev_tools
    
    # Show container info if in interactive mode
    if [[ -t 0 ]] && [[ "${SHOW_CONTAINER_INFO:-true}" == "true" ]]; then
        show_container_info
        echo ""
    fi
    
    # Execute the provided command or start bash
    if [[ $# -eq 0 ]]; then
        print_info "Starting interactive shell..."
        exec /bin/bash
    else
        print_info "Executing command: $*"
        exec "$@"
    fi
}

# Run main function with all arguments
main "$@"

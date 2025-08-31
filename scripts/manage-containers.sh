#!/bin/bash
# Container management script for cpp-scaffold
# This script provides utilities for managing Docker containers and images

set -euo pipefail

# Script directory
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"

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
Usage: $0 COMMAND [OPTIONS]

Container management for cpp-scaffold

COMMANDS:
    build [IMAGE]       Build container image(s)
    run IMAGE [CMD]     Run command in container
    shell IMAGE         Start interactive shell in container
    test IMAGE          Run tests in container
    validate IMAGE      Validate container image
    clean               Clean up container images
    list                List available images
    status              Show container status
    help                Show this help message

BUILD OPTIONS:
    --force             Force rebuild even if image exists
    --no-cache          Build without using cache
    --all               Build all images

RUN OPTIONS:
    --interactive       Run in interactive mode
    --remove            Remove container after execution
    --volume PATH       Mount volume (can be used multiple times)
    --env VAR=VALUE     Set environment variable

CLEAN OPTIONS:
    --force             Force removal
    --dangling          Remove dangling images only
    --all               Remove all unused images and containers

EXAMPLES:
    $0 build ubuntu-dev                    # Build Ubuntu development image
    $0 build --all                         # Build all images
    $0 run ubuntu-dev build --type Debug   # Run build in Ubuntu container
    $0 shell alpine-minimal                # Start shell in Alpine container
    $0 clean --dangling                    # Clean up dangling images
    $0 validate ubuntu-dev                 # Validate Ubuntu image

EOF
}

# Check if Docker is available
check_docker() {
    if ! command -v docker &> /dev/null; then
        print_error "Docker is not installed or not in PATH"
        exit 1
    fi

    if ! docker info &> /dev/null; then
        print_error "Docker daemon is not running"
        exit 1
    fi
}

# Build container image
build_image() {
    local image_name="$1"
    local force_rebuild="${2:-false}"
    local no_cache="${3:-false}"

    local dockerfile="$PROJECT_ROOT/docker/${image_name}.Dockerfile"
    local image_tag="cpp-scaffold:${image_name}"

    if [[ ! -f "$dockerfile" ]]; then
        print_error "Dockerfile not found: $dockerfile"
        exit 1
    fi

    # Check if image exists
    if [[ "$force_rebuild" != "true" ]] && docker images -q "$image_tag" &> /dev/null; then
        print_info "Image $image_tag already exists. Use --force to rebuild."
        return 0
    fi

    print_info "Building container image: $image_tag"

    local build_args=()
    build_args+=("-f" "$dockerfile")
    build_args+=("-t" "$image_tag")

    if [[ "$no_cache" == "true" ]]; then
        build_args+=("--no-cache")
    fi

    build_args+=("$PROJECT_ROOT")

    if docker build "${build_args[@]}"; then
        print_success "Successfully built image: $image_tag"
    else
        print_error "Failed to build image: $image_tag"
        exit 1
    fi
}

# Build all images
build_all_images() {
    local force_rebuild="${1:-false}"
    local no_cache="${2:-false}"

    local dockerfiles=("$PROJECT_ROOT"/docker/*.Dockerfile)
    
    for dockerfile in "${dockerfiles[@]}"; do
        if [[ -f "$dockerfile" ]]; then
            local basename=$(basename "$dockerfile" .Dockerfile)
            print_info "Building image: $basename"
            build_image "$basename" "$force_rebuild" "$no_cache"
        fi
    done
}

# Run command in container
run_in_container() {
    local image_name="$1"
    shift
    local command=("$@")

    local image_tag="cpp-scaffold:${image_name}"

    # Check if image exists
    if ! docker images -q "$image_tag" &> /dev/null; then
        print_error "Image not found: $image_tag"
        print_info "Run '$0 build $image_name' to build the image first"
        exit 1
    fi

    local run_args=()
    run_args+=("--rm")
    run_args+=("-v" "$PROJECT_ROOT:/workspace")
    run_args+=("-w" "/workspace")

    # Add user mapping for Linux/macOS
    if [[ "$OSTYPE" != "msys" ]] && [[ "$OSTYPE" != "cygwin" ]]; then
        run_args+=("-e" "USER_ID=$(id -u)")
        run_args+=("-e" "GROUP_ID=$(id -g)")
    fi

    # Add git configuration
    if git config --global user.name &> /dev/null; then
        run_args+=("-e" "GIT_USER_NAME=$(git config --global user.name)")
    fi
    if git config --global user.email &> /dev/null; then
        run_args+=("-e" "GIT_USER_EMAIL=$(git config --global user.email)")
    fi

    run_args+=("$image_tag")

    if [[ ${#command[@]} -eq 0 ]]; then
        command=("/bin/bash")
    fi

    print_info "Running command in container: $image_tag"
    docker run "${run_args[@]}" "${command[@]}"
}

# Start interactive shell
start_shell() {
    local image_name="$1"
    local image_tag="cpp-scaffold:${image_name}"

    print_info "Starting interactive shell in: $image_tag"
    docker run --rm -it \
        -v "$PROJECT_ROOT:/workspace" \
        -w "/workspace" \
        -e "USER_ID=$(id -u 2>/dev/null || echo 1000)" \
        -e "GROUP_ID=$(id -g 2>/dev/null || echo 1000)" \
        "$image_tag" \
        /bin/bash
}

# Validate container image
validate_image() {
    local image_name="$1"
    local image_tag="cpp-scaffold:${image_name}"

    print_info "Validating container image: $image_tag"

    # Check if image exists
    if ! docker images -q "$image_tag" &> /dev/null; then
        print_error "Image not found: $image_tag"
        exit 1
    fi

    # Check required tools
    local required_tools=("cmake" "ninja" "gcc" "g++")
    
    for tool in "${required_tools[@]}"; do
        if docker run --rm "$image_tag" which "$tool" &> /dev/null; then
            print_success "✓ $tool is available"
        else
            print_error "✗ $tool is not available"
            exit 1
        fi
    done

    # Test basic functionality
    if docker run --rm "$image_tag" cmake --version &> /dev/null; then
        print_success "✓ CMake is working"
    else
        print_error "✗ CMake is not working"
        exit 1
    fi

    print_success "Container image validation passed: $image_tag"
}

# Clean up images
clean_images() {
    local force="${1:-false}"
    local dangling_only="${2:-false}"
    local all="${3:-false}"

    print_info "Cleaning up container images..."

    if [[ "$dangling_only" == "true" ]]; then
        docker image prune -f
        print_success "Removed dangling images"
    elif [[ "$all" == "true" ]]; then
        if [[ "$force" == "true" ]]; then
            docker system prune -af
        else
            docker system prune -a
        fi
        print_success "Removed all unused images and containers"
    else
        # Remove cpp-scaffold images
        local images=$(docker images --format "{{.Repository}}:{{.Tag}}" --filter "reference=cpp-scaffold:*")
        if [[ -n "$images" ]]; then
            echo "$images" | while read -r image; do
                if [[ "$force" == "true" ]]; then
                    docker rmi -f "$image" &> /dev/null || true
                else
                    docker rmi "$image" &> /dev/null || true
                fi
                print_info "Removed image: $image"
            done
        fi
        print_success "Cleaned up cpp-scaffold images"
    fi
}

# List available images
list_images() {
    print_info "Available cpp-scaffold images:"
    docker images --format "table {{.Repository}}\t{{.Tag}}\t{{.Size}}\t{{.CreatedAt}}" \
        --filter "reference=cpp-scaffold:*" || true
}

# Show container status
show_status() {
    print_info "Docker system information:"
    docker system df

    echo ""
    print_info "Running containers:"
    docker ps --format "table {{.Names}}\t{{.Image}}\t{{.Status}}\t{{.Ports}}" || true

    echo ""
    list_images
}

# Main function
main() {
    if [[ $# -eq 0 ]]; then
        show_usage
        exit 1
    fi

    check_docker

    local command="$1"
    shift

    case "$command" in
        "build")
            local force_rebuild=false
            local no_cache=false
            local build_all=false
            local image_name=""

            while [[ $# -gt 0 ]]; do
                case $1 in
                    --force)
                        force_rebuild=true
                        shift
                        ;;
                    --no-cache)
                        no_cache=true
                        shift
                        ;;
                    --all)
                        build_all=true
                        shift
                        ;;
                    *)
                        image_name="$1"
                        shift
                        ;;
                esac
            done

            if [[ "$build_all" == "true" ]]; then
                build_all_images "$force_rebuild" "$no_cache"
            elif [[ -n "$image_name" ]]; then
                build_image "$image_name" "$force_rebuild" "$no_cache"
            else
                print_error "Image name required for build command"
                exit 1
            fi
            ;;
        "run")
            if [[ $# -lt 1 ]]; then
                print_error "Image name required for run command"
                exit 1
            fi
            run_in_container "$@"
            ;;
        "shell")
            if [[ $# -lt 1 ]]; then
                print_error "Image name required for shell command"
                exit 1
            fi
            start_shell "$1"
            ;;
        "test")
            if [[ $# -lt 1 ]]; then
                print_error "Image name required for test command"
                exit 1
            fi
            run_in_container "$1" "build" "--type" "Debug" "--preset" "debug"
            ;;
        "validate")
            if [[ $# -lt 1 ]]; then
                print_error "Image name required for validate command"
                exit 1
            fi
            validate_image "$1"
            ;;
        "clean")
            local force=false
            local dangling_only=false
            local all=false

            while [[ $# -gt 0 ]]; do
                case $1 in
                    --force)
                        force=true
                        shift
                        ;;
                    --dangling)
                        dangling_only=true
                        shift
                        ;;
                    --all)
                        all=true
                        shift
                        ;;
                    *)
                        print_error "Unknown option: $1"
                        exit 1
                        ;;
                esac
            done

            clean_images "$force" "$dangling_only" "$all"
            ;;
        "list")
            list_images
            ;;
        "status")
            show_status
            ;;
        "help")
            show_usage
            ;;
        *)
            print_error "Unknown command: $command"
            show_usage
            exit 1
            ;;
    esac
}

# Run main function with all arguments
main "$@"

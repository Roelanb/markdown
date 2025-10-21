#!/bin/bash

# Markdown Editor Build Script
# For Linux and macOS

set -e  # Exit on error

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Script directory
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
BUILD_DIR="${SCRIPT_DIR}/build"

# Print colored message
print_message() {
    local color=$1
    shift
    echo -e "${color}$@${NC}"
}

# Print section header
print_header() {
    echo ""
    print_message "${BLUE}" "================================================"
    print_message "${BLUE}" "$@"
    print_message "${BLUE}" "================================================"
}

# Check if command exists
command_exists() {
    command -v "$1" >/dev/null 2>&1
}

# Detect OS
detect_os() {
    if [[ "$OSTYPE" == "linux-gnu"* ]]; then
        echo "linux"
    elif [[ "$OSTYPE" == "darwin"* ]]; then
        echo "macos"
    else
        echo "unknown"
    fi
}

# Check dependencies
check_dependencies() {
    print_header "Checking Dependencies"

    local missing_deps=0

    # Check CMake
    if command_exists cmake; then
        CMAKE_VERSION=$(cmake --version | head -n1 | cut -d' ' -f3)
        print_message "${GREEN}" "✓ CMake found: $CMAKE_VERSION"
    else
        print_message "${RED}" "✗ CMake not found"
        missing_deps=1
    fi

    # Check C++ compiler
    if command_exists g++ || command_exists clang++; then
        if command_exists g++; then
            CXX_VERSION=$(g++ --version | head -n1)
            print_message "${GREEN}" "✓ C++ compiler found: $CXX_VERSION"
        else
            CXX_VERSION=$(clang++ --version | head -n1)
            print_message "${GREEN}" "✓ C++ compiler found: $CXX_VERSION"
        fi
    else
        print_message "${RED}" "✗ C++ compiler not found"
        missing_deps=1
    fi

    # Check Qt6
    if command_exists qmake6 || command_exists qmake; then
        print_message "${GREEN}" "✓ Qt6 found"
    else
        print_message "${YELLOW}" "⚠ Qt6 qmake not found in PATH"
        print_message "${YELLOW}" "  Build may still succeed if Qt6 is installed"
    fi

    if [ $missing_deps -eq 1 ]; then
        print_message "${RED}" "\nMissing required dependencies!"
        print_message "${YELLOW}" "Please install missing dependencies and try again."

        OS=$(detect_os)
        echo ""
        if [ "$OS" == "linux" ]; then
            print_message "${BLUE}" "For Ubuntu/Debian:"
            echo "  sudo apt update"
            echo "  sudo apt install build-essential cmake qt6-base-dev qt6-webengine-dev"
            echo ""
            print_message "${BLUE}" "For Fedora:"
            echo "  sudo dnf install cmake gcc-c++ qt6-qtbase-devel qt6-qtwebengine-devel"
            echo ""
            print_message "${BLUE}" "For Arch Linux:"
            echo "  sudo pacman -S cmake qt6-base qt6-webengine"
        elif [ "$OS" == "macos" ]; then
            print_message "${BLUE}" "For macOS:"
            echo "  brew install cmake qt@6"
        fi

        exit 1
    fi

    print_message "${GREEN}" "\n✓ All required dependencies found"
}

# Clean build directory
clean_build() {
    print_header "Cleaning Build Directory"

    if [ -d "$BUILD_DIR" ]; then
        print_message "${YELLOW}" "Removing existing build directory..."
        rm -rf "$BUILD_DIR"
        print_message "${GREEN}" "✓ Build directory cleaned"
    else
        print_message "${BLUE}" "No existing build directory found"
    fi
}

# Configure with CMake
configure() {
    print_header "Configuring Project with CMake"

    mkdir -p "$BUILD_DIR"
    cd "$BUILD_DIR"

    # Set Qt path for macOS if using Homebrew
    CMAKE_ARGS=()
    if [[ "$OSTYPE" == "darwin"* ]] && command_exists brew; then
        QT_PATH=$(brew --prefix qt@6 2>/dev/null || echo "")
        if [ -n "$QT_PATH" ]; then
            print_message "${BLUE}" "Detected Qt6 from Homebrew: $QT_PATH"
            CMAKE_ARGS+=("-DCMAKE_PREFIX_PATH=$QT_PATH")
        fi
    fi

    print_message "${BLUE}" "Running CMake..."
    if cmake "${CMAKE_ARGS[@]}" ..; then
        print_message "${GREEN}" "✓ Configuration successful"
    else
        print_message "${RED}" "✗ Configuration failed"
        exit 1
    fi
}

# Build the project
build() {
    print_header "Building Project"

    cd "$BUILD_DIR"

    # Detect number of CPU cores
    if [[ "$OSTYPE" == "darwin"* ]]; then
        CORES=$(sysctl -n hw.ncpu)
    else
        CORES=$(nproc)
    fi

    print_message "${BLUE}" "Building with $CORES cores..."

    if cmake --build . -j"$CORES"; then
        print_message "${GREEN}" "✓ Build successful"
    else
        print_message "${RED}" "✗ Build failed"
        exit 1
    fi
}

# Install the application
install_app() {
    print_header "Installing Application"

    cd "$BUILD_DIR"

    print_message "${BLUE}" "Installing to system..."
    if sudo cmake --install .; then
        print_message "${GREEN}" "✓ Installation successful"
        print_message "${BLUE}" "You can now run: MarkdownEditor"
    else
        print_message "${RED}" "✗ Installation failed"
        exit 1
    fi
}

# Run the application
run_app() {
    print_header "Running Application"

    if [ -f "$BUILD_DIR/MarkdownEditor" ]; then
        print_message "${GREEN}" "Starting Markdown Editor..."
        "$BUILD_DIR/MarkdownEditor"
    else
        print_message "${RED}" "✗ Executable not found"
        print_message "${YELLOW}" "Build may have failed"
        exit 1
    fi
}

# Print usage information
print_usage() {
    echo "Markdown Editor Build Script"
    echo ""
    echo "Usage: $0 [OPTION]"
    echo ""
    echo "Options:"
    echo "  build          Build the project (default)"
    echo "  clean          Clean the build directory"
    echo "  rebuild        Clean and build"
    echo "  install        Install the application system-wide"
    echo "  run            Build and run the application"
    echo "  check          Check dependencies only"
    echo "  help           Show this help message"
    echo ""
    echo "Examples:"
    echo "  $0              # Build the project"
    echo "  $0 rebuild      # Clean and rebuild"
    echo "  $0 run          # Build and run"
    echo "  $0 install      # Install system-wide"
}

# Main script logic
main() {
    local action="${1:-build}"

    case "$action" in
        build)
            check_dependencies
            configure
            build
            print_header "Build Complete!"
            print_message "${GREEN}" "✓ Build successful"
            print_message "${BLUE}" "\nTo run the application:"
            echo "  cd build && ./MarkdownEditor"
            print_message "${BLUE}" "\nOr use:"
            echo "  $0 run"
            ;;

        clean)
            clean_build
            print_message "${GREEN}" "✓ Clean complete"
            ;;

        rebuild)
            check_dependencies
            clean_build
            configure
            build
            print_header "Rebuild Complete!"
            print_message "${GREEN}" "✓ Rebuild successful"
            ;;

        install)
            if [ ! -d "$BUILD_DIR" ]; then
                print_message "${YELLOW}" "No build directory found. Building first..."
                check_dependencies
                configure
                build
            fi
            install_app
            ;;

        run)
            if [ ! -f "$BUILD_DIR/MarkdownEditor" ]; then
                print_message "${YELLOW}" "Executable not found. Building first..."
                check_dependencies
                configure
                build
            fi
            run_app
            ;;

        check)
            check_dependencies
            ;;

        help|--help|-h)
            print_usage
            ;;

        *)
            print_message "${RED}" "Unknown option: $action"
            echo ""
            print_usage
            exit 1
            ;;
    esac
}

# Run main function
main "$@"
